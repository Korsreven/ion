/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTextureManager.cpp
-------------------------------------------
*/

#include "IonTextureManager.h"

#include <algorithm>

#include "graphics/IonGraphicsAPI.h"
#include "FreeImage/FreeImage.h"

namespace ion::graphics::textures
{

using namespace texture_manager;

namespace texture_manager::detail
{

bool has_support_for_non_power_of_two_textures() noexcept
{
	static const auto has_support_for_non_power_of_two_textures = gl::TextureNonPowerOfTwo_Support() != gl::Extension::None;
	return has_support_for_non_power_of_two_textures;
}

bool has_support_for_array_texture() noexcept
{
	static const auto has_support_for_array_texture = gl::ArrayTexture_Support() != gl::Extension::None;
	return has_support_for_array_texture;
}

int max_texture_size() noexcept
{
	static const auto max_texture_size = gl::MaxTextureSize();
	return max_texture_size;
}

int max_array_texture_layers() noexcept
{
	static const auto max_array_texture_layers = gl::MaxArrayTextureLayers();
	return max_array_texture_layers;
}


std::pair<int, int> power_of_two_adjusted_size(int width, int height,
	NpotSampling npot_sampling, std::optional<NpotResampleFit> npot_resample_fit) noexcept
{
	auto aspect_ratio = static_cast<real>(width) / height;

	//Clamp texture size
	if (width > max_texture_size() || height > max_texture_size())
	{
		if (width > height)
		{
			width = max_texture_size();
			height = static_cast<int>(width / aspect_ratio);
		}
		else
		{
			height = max_texture_size();
			width = static_cast<int>(height * aspect_ratio);
		}
	}
	else
	{
		auto pot_width = static_cast<int>(make_power_of_two(width, npot_sampling));
		auto pot_height = static_cast<int>(make_power_of_two(height, npot_sampling));

		if (!npot_resample_fit)
			npot_resample_fit =
				[&]() noexcept
				{
					//Choose minimum npot->pot difference
					if (std::abs(width - pot_width) < std::abs(height - pot_height))
						return NpotResampleFit::Horizontally;
					else
						return NpotResampleFit::Vertically;
				}();

		switch (*npot_resample_fit)
		{
			case NpotResampleFit::Horizontally:
			{
				width = pot_width;
				height = static_cast<int>(width / aspect_ratio);
				break;
			}

			case NpotResampleFit::Vertically:
			{
				height = pot_height;
				width = static_cast<int>(height * aspect_ratio);
				break;
			}
		}
	}

	return std::pair{width, height};
}

std::tuple<int, int, int, int> padding(int width, int height, int actual_width, int actual_height) noexcept
{
	if (auto padding_width = actual_width - width,
			 padding_height = actual_height - height; padding_width > 0 || padding_height > 0)
	{
		//Pad left and right
		auto padding_half_width = padding_width > 0 ? padding_width / 2 : 0;

		//Pad top and bottom
		auto padding_half_height = padding_height > 0 ? padding_height / 2 : 0;

		return std::tuple{padding_half_width, padding_half_height,
			padding_half_width + padding_width % 2, padding_half_height + padding_height % 2};
	}
	else
		return std::tuple{0, 0, 0, 0};
}

std::tuple<int, int, int, int> power_of_two_padding(int width, int height) noexcept
{
	return padding(width, height, upper_power_of_two(width), upper_power_of_two(height));
}

void enlarge_canvas(std::string &pixel_data, int left, int bottom, const texture::TextureExtents &extents) noexcept
{
	auto color_bytes = extents.BitDepth / 8;

	std::string pixels(std::begin(pixel_data), std::begin(pixel_data) + extents.Width * extents.Height * color_bytes);
	pixel_data.assign(std::size(pixel_data), '\0');

	for (auto from = 0,
		to = extents.ActualWidth * bottom * color_bytes + left * color_bytes,
		size = std::ssize(pixels); from < size;
		from += extents.Width * color_bytes,
		to += extents.ActualWidth * color_bytes)

		std::copy(
			std::begin(pixels) + from,
			std::begin(pixels) + from + extents.Width * color_bytes,
			std::begin(pixel_data) + to
		);
}


std::optional<std::pair<std::string, texture::TextureExtents>> prepare_texture(
	const std::string &file_data, const std::filesystem::path &file_path,
	texture::TextureFilter min_filter, texture::TextureFilter mag_filter,
	std::optional<NpotResizing> npot_resizing, NpotSampling npot_sampling,
	std::optional<NpotResampleFit> npot_resample_fit, NpotResampleFilter npot_resample_filter)
{
	auto stream = FreeImage_OpenMemory(
		reinterpret_cast<BYTE*>(const_cast<char*>(std::data(file_data))),
		static_cast<DWORD>(std::size(file_data)));
	
	auto image_format = FreeImage_GetFileTypeFromMemory(stream);

	if (image_format == FIF_UNKNOWN)
		image_format = FreeImage_GetFIFFromFilename(std::data(file_path.string()));

	auto bitmap = FreeImage_FIFSupportsReading(image_format) ?
		FreeImage_LoadFromMemory(image_format, stream) : nullptr;
	FreeImage_CloseMemory(stream);

	if (!bitmap)
		return {};


	auto bit_depth = FreeImage_GetBPP(bitmap);

	if (bit_depth < 24)
	{
		bitmap = FreeImage_ConvertTo24Bits(bitmap);
		bit_depth = 24;
	}

	texture::TextureExtents extents;
	extents.Width = static_cast<int>(FreeImage_GetWidth(bitmap));
	extents.Height = static_cast<int>(FreeImage_GetHeight(bitmap));
	extents.ActualWidth = extents.Width;
	extents.ActualHeight = extents.Height;
	extents.BitDepth = bit_depth;

	//Make sure texture is power of two
	if (npot_resizing || !has_support_for_non_power_of_two_textures())
	{
		auto width = extents.Width;
		auto height = extents.Height;
		auto [new_width, new_height] =
			npot_resizing == NpotResizing::ResampleImage ?
			power_of_two_adjusted_size(width, height, npot_sampling, npot_resample_fit) :
			std::pair{width, height};

		//Resample is needed (new size is different)
		if (new_width != width || new_height != height)
		{
			auto resampling_filter =
				[&](texture::TextureFilter filter) noexcept
				{
					switch (filter)
					{
						case texture::TextureFilter::NearestNeighbor:
						return FILTER_BOX;

						case texture::TextureFilter::Bilinear:
						default:
						{
							switch (npot_resample_filter)
							{
								case NpotResampleFilter::Box:
								return FILTER_BOX;

								case NpotResampleFilter::Bicubic:
								return FILTER_BICUBIC;

								case NpotResampleFilter::BSpline:
								return FILTER_BSPLINE;

								case NpotResampleFilter::CatmullRom:
								return FILTER_CATMULLROM;

								case NpotResampleFilter::Lanczos3:
								return FILTER_LANCZOS3;

								case NpotResampleFilter::Bilinear:
								default:
								return FILTER_BILINEAR;
							}
						}
					}
				};

			//Resampling
			auto image_filter =
				[&]() noexcept
				{
					//Minification
					if (new_width * new_height < width * height)
						return resampling_filter(min_filter);
					//Magnification
					else
						return resampling_filter(mag_filter);
				}();

			//Resample
			{
				auto dst_bitmap = FreeImage_Rescale(bitmap, new_width, new_height, image_filter);
				FreeImage_Unload(bitmap);
				bitmap = dst_bitmap;
			}

			if (!bitmap)
				return {};
		}

		//Enlarge canvas if one or both dimensions are npot
		if (auto [left, top, right, bottom] = detail::power_of_two_padding(new_width, new_height);
			left + top + right + bottom > 0)
		{
			//Enlarge canvas
			{
				auto color = RGBQUAD{0xFF, 0xFF, 0xFF, 0x00}; //BGRA
				auto dst_bitmap = FreeImage_EnlargeCanvas(bitmap, left, top, right, bottom, &color,
					bit_depth == 32 ? FI_COLOR_IS_RGBA_COLOR : FI_COLOR_IS_RGB_COLOR);
				FreeImage_Unload(bitmap);
				bitmap = dst_bitmap;
			}

			if (!bitmap)
				return {};
		}

		extents.Width = new_width;
		extents.Height = new_height;
		extents.ActualWidth = upper_power_of_two(new_width);
		extents.ActualHeight = upper_power_of_two(new_height);
	}

	auto pixel_data = std::string{
		reinterpret_cast<char*>(FreeImage_GetBits(bitmap)),
		static_cast<size_t>(extents.ActualWidth) * extents.ActualHeight * (bit_depth / 8)};
	FreeImage_Unload(bitmap);

	return std::pair{std::move(pixel_data), extents};
}

std::optional<texture::TextureHandle> load_texture(const std::string &pixel_data, const texture::TextureExtents &extents,
	texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
	texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode) noexcept
{
	if (extents.ActualWidth > max_texture_size() ||
		extents.ActualHeight > max_texture_size())
		return {}; //Max texture limit reached

	texture::TextureHandle texture_handle;
	glGenTextures(1, reinterpret_cast<unsigned int*>(&texture_handle.Id));
	glBindTexture(GL_TEXTURE_2D, texture_handle.Id);

	auto has_latest_generate_mipmap =
		gl::HasGL(gl::Version::v3_0) ||
		gl::FrameBufferObject_Support() != gl::Extension::None;

	//Generate mipmaps
	if (mip_filter)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, number_of_mipmap_levels(extents.ActualWidth, extents.ActualHeight));

		//Must be enabled before glTexImage2D!
		if (!has_latest_generate_mipmap)
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	}

	//Minification filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		[&]() noexcept
		{
			//Has mipmap filter
			if (mip_filter)
			{
				switch (min_filter)
				{
					case texture::TextureFilter::NearestNeighbor:
					{
						switch (*mip_filter)
						{
							case texture::MipmapFilter::ClosestMatch:
							return GL_NEAREST_MIPMAP_NEAREST;

							case texture::MipmapFilter::WeightedAverage:
							default:
							return GL_NEAREST_MIPMAP_LINEAR;
						}
					}

					case texture::TextureFilter::Bilinear:
					default:
					{
						switch (*mip_filter)
						{
							case texture::MipmapFilter::ClosestMatch:
							return GL_LINEAR_MIPMAP_NEAREST;

							case texture::MipmapFilter::WeightedAverage:
							default:
							return GL_LINEAR_MIPMAP_LINEAR;
						}
					}
				}
			}
			else
			{
				switch (min_filter)
				{
					case texture::TextureFilter::NearestNeighbor:
					return GL_NEAREST;

					case texture::TextureFilter::Bilinear:
					default:
					return GL_LINEAR;
				}
			}
		}());

	//Magnification filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		mag_filter == texture::TextureFilter::NearestNeighbor ? GL_NEAREST : GL_LINEAR);

	//Texture wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		s_wrap_mode == texture::TextureWrapMode::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		t_wrap_mode == texture::TextureWrapMode::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);

	//Unpack image from memory to gl
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //May increase transfer speed for NPOT
	glTexImage2D(GL_TEXTURE_2D, 0,
		extents.BitDepth == 32 ? GL_RGBA8 : GL_RGB8, extents.ActualWidth, extents.ActualHeight, 0,
			[&]() noexcept
			{
				if (FreeImage_IsLittleEndian())
					return extents.BitDepth == 32 ? GL_BGRA : GL_BGR;
				else
					return extents.BitDepth == 32 ? GL_RGBA : GL_RGB;
			}(), GL_UNSIGNED_BYTE, std::data(pixel_data));

	//Must be called after glTexImage2D!
	if (has_latest_generate_mipmap)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture_handle;
}

void unload_texture(texture::TextureHandle texture_handle) noexcept
{
	glDeleteTextures(1, reinterpret_cast<unsigned int*>(&texture_handle.Id));
}


/*
	Sub textures
*/

void next_sub_texture_position(std::pair<int, int> &position, int rows, int columns,
	texture_atlas::AtlasSubTextureOrder sub_texture_order) noexcept
{
	switch (sub_texture_order)
	{
		case texture_atlas::AtlasSubTextureOrder::ColumnMajor:
		{
			if (position.first % rows == 0)
			{
				position.first = 1;
				++position.second;
			}
			else
				++position.first;

			break;
		}

		case texture_atlas::AtlasSubTextureOrder::RowMajor:
		default:
		{
			if (position.second % columns == 0)
			{
				position.second = 1;
				++position.first;
			}
			else
				++position.second;

			break;
		}
	}
}

std::optional<std::pair<std::string, texture::TextureExtents>> prepare_sub_texture(
	const TextureAtlas &texture_atlas, const std::pair<int, int> &position,
	std::optional<NpotResizing> npot_resizing) noexcept
{
	auto &atlas_extents = *texture_atlas.Extents();
	auto color_bytes = atlas_extents.BitDepth / 8;

	texture::TextureExtents sub_extents;
	sub_extents.Width = atlas_extents.Width / texture_atlas.Columns();
	sub_extents.Height = atlas_extents.Height / texture_atlas.Rows();

	//Invalid sub extents
	if (sub_extents.Width < 1 || sub_extents.Height < 1)
		return {};

	//Make sure sub texture is power of two
	if (npot_resizing || !has_support_for_non_power_of_two_textures())
	{
		sub_extents.ActualWidth = upper_power_of_two(sub_extents.Width);
		sub_extents.ActualHeight = upper_power_of_two(sub_extents.Height);
	}
	else
	{
		sub_extents.ActualWidth = sub_extents.Width;
		sub_extents.ActualHeight = sub_extents.Height;
	}

	sub_extents.BitDepth = atlas_extents.BitDepth;

	//Allocate required bytes for the sub texture
	std::string sub_pixel_data(sub_extents.ActualWidth * sub_extents.ActualHeight * color_bytes, '\0');

	auto [atlas_left, atlas_top, atlas_right, atlas_bottom] =
		npot_resizing || !has_support_for_non_power_of_two_textures() ?
		detail::power_of_two_padding(atlas_extents.Width, atlas_extents.Height) :
		std::tuple{0, 0, 0, 0};
	auto [sub_left, sub_top, sub_right, sub_bottom] =
		npot_resizing || !has_support_for_non_power_of_two_textures() ?
		detail::power_of_two_padding(sub_extents.Width, sub_extents.Height) :
		std::tuple{0, 0, 0, 0};

	auto x = sub_extents.Width * (position.second - 1) + atlas_left;
	auto y = sub_extents.Height * (texture_atlas.Rows() - position.first) + atlas_bottom +
		(atlas_extents.Height - (sub_extents.Height * texture_atlas.Rows()));

	if (gl::GetTextureSubImage_Support() != gl::Extension::None)
	{
		//Pack image from gl to memory
		glPixelStorei(GL_PACK_ALIGNMENT, 1); //May increase transfer speed for NPOT
		glGetTextureSubImage(texture_atlas.Handle()->Id, 0,
			x, y, 0, sub_extents.Width, sub_extents.Height, 1,
				[&]() noexcept
				{
					if (FreeImage_IsLittleEndian())
						return atlas_extents.BitDepth == 32 ? GL_BGRA : GL_BGR;
					else
						return atlas_extents.BitDepth == 32 ? GL_RGBA : GL_RGB;
				}(),
			GL_UNSIGNED_BYTE, std::size(sub_pixel_data), std::data(sub_pixel_data));

		if (sub_extents.Width * sub_extents.Height <
			sub_extents.ActualWidth * sub_extents.ActualHeight)

			enlarge_canvas(sub_pixel_data, sub_left, sub_bottom, sub_extents);
	}
	else
	{
		//Allocate required bytes for the texture atlas
		std::string atlas_pixel_data(atlas_extents.ActualWidth * atlas_extents.ActualHeight * color_bytes, '\0');

		//Pack image from gl to memory
		glBindTexture(GL_TEXTURE_2D, texture_atlas.Handle()->Id);
		glPixelStorei(GL_PACK_ALIGNMENT, 1); //May increase transfer speed for NPOT
		glGetTexImage(GL_TEXTURE_2D, 0,
				[&]() noexcept
				{
					if (FreeImage_IsLittleEndian())
						return atlas_extents.BitDepth == 32 ? GL_BGRA : GL_BGR;
					else
						return atlas_extents.BitDepth == 32 ? GL_RGBA : GL_RGB;
				}(),
			GL_UNSIGNED_BYTE, std::data(atlas_pixel_data));
		glBindTexture(GL_TEXTURE_2D, 0);

		//Copy sub texture bytes from texture atlas at the given position
		for (auto from = atlas_extents.ActualWidth * y * color_bytes + x * color_bytes,
			to = 0,
			size = sub_extents.Width * sub_extents.Height * color_bytes; to < size;
			from += atlas_extents.ActualWidth * color_bytes,
			to += sub_extents.Width * color_bytes)

			std::copy(
				std::begin(atlas_pixel_data) + from,
				std::begin(atlas_pixel_data) + from + sub_extents.Width * color_bytes,
				std::begin(sub_pixel_data) + to
			);

		if (sub_extents.Width * sub_extents.Height <
			sub_extents.ActualWidth * sub_extents.ActualHeight)

			enlarge_canvas(sub_pixel_data, sub_left, sub_bottom, sub_extents);
	}

	return std::pair{std::move(sub_pixel_data), sub_extents};
}

} //texture_manager::detail


//Protected

/*
	Events
*/

bool TextureManager::PrepareResource(Texture &texture)
{
	//Texture is a sub texture
	if (auto &atlas_region = texture.AtlasRegion(); atlas_region)
		return !!atlas_region->Atlas;

	if (FileResourceManager::PrepareResource(texture))
	{
		if (auto texture_data =
			detail::prepare_texture(
				*texture.FileData(), *texture.FilePath(),
				texture.MinFilter(), texture.MagFilter(),
				texture_npot_resizing_, texture_npot_sampling_,
				texture_npot_resample_fit_, texture_npot_resample_filter_); texture_data)
		{
			auto &[pixel_data, extents] = *texture_data;
			texture.PixelData(std::move(pixel_data), extents);
		}

		return texture.PixelData().has_value();
	}
	else
		return false;
}

Texture* TextureManager::DependentResource(Texture &texture) noexcept
{
	//Texture is a sub texture
	if (auto &atlas_region = texture.AtlasRegion(); atlas_region && atlas_region->Atlas)
	{
		//Make sure texture atlas has been loaded first (and not failed in doing so)
		if (!atlas_region->Atlas->IsLoaded() && !atlas_region->Atlas->HasFailed())
			return atlas_region->Atlas.get();
	}

	return nullptr;
}

bool TextureManager::LoadResource(Texture &texture)
{
	//Texture is a sub texture
	if (auto &atlas_region = texture.AtlasRegion(); atlas_region && atlas_region->Atlas)
	{
		//Make sure texture atlas has been loaded first
		if (atlas_region->Atlas->IsLoaded())
		{
			if (auto texture_data =
				detail::prepare_sub_texture(
					*atlas_region->Atlas, atlas_region->Position,
					texture_npot_resizing_); texture_data)
			{
				auto &[pixel_data, extents] = *texture_data;
				texture.PixelData(std::move(pixel_data), extents);

				if (!texture.PixelData().has_value())
					return false;
			}
		}
		else
			return false;
	}

	auto &pixel_data = texture.PixelData();
	auto &extents = texture.Extents();
	auto [min_filter, mag_filter, mip_filter] = texture.Filter();
	auto [s_wrap_mode, t_wrap_mode] = texture.WrapMode();

	if (pixel_data && extents)
	{
		texture.Handle(detail::load_texture(*pixel_data, *extents, min_filter, mag_filter, mip_filter, s_wrap_mode, t_wrap_mode));
		return texture.Handle().has_value();
	}
	else
		return false;
}

bool TextureManager::UnloadResource(Texture &texture) noexcept
{
	if (auto handle = texture.Handle(); handle)
	{
		detail::unload_texture(*handle);
		texture.Handle({});
		return true;
	}
	else
		return false;
}


void TextureManager::ResourceLoaded(Texture &texture) noexcept
{
	FileResourceManager::ResourceLoaded(texture);
	texture.ResetPixelData();
		//Pixel data not required after texture has been loaded (save memory)
}

void TextureManager::ResourceFailed(Texture &texture) noexcept
{
	FileResourceManager::ResourceFailed(texture);
	texture.ResetPixelData();
		//Pixel data not required after texture has failed (save memory)
}


//Public

TextureManager::TextureManager() noexcept
{
	//Initialize once
	detail::has_support_for_non_power_of_two_textures();
	detail::has_support_for_array_texture();
	detail::max_texture_size();
	detail::max_array_texture_layers();
}

TextureManager::~TextureManager() noexcept
{
	UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Textures - Creating
*/

NonOwningPtr<Texture> TextureManager::CreateTexture(std::string name, std::string asset_name)
{
	return CreateResource(std::move(name), std::move(asset_name));
}

NonOwningPtr<Texture> TextureManager::CreateTexture(std::string name, std::string asset_name,
	texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
	texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode)
{
	return CreateResource(std::move(name), std::move(asset_name), min_filter, mag_filter, mip_filter, s_wrap_mode, t_wrap_mode);
}

NonOwningPtr<Texture> TextureManager::CreateTexture(std::string name, std::string asset_name,
	texture::TextureFilter filter, texture::MipmapFilter mip_filter, texture::TextureWrapMode wrap_mode)
{
	return CreateResource(std::move(name), std::move(asset_name), filter, mip_filter, wrap_mode);
}

NonOwningPtr<Texture> TextureManager::CreateTexture(std::string name, std::string asset_name,
	texture::TextureFilter filter, texture::TextureWrapMode wrap_mode)
{
	return CreateResource(std::move(name), std::move(asset_name), filter, wrap_mode);
}


NonOwningPtr<Texture> TextureManager::CreateTexture(const Texture &texture)
{
	return CreateResource(texture);
}

NonOwningPtr<Texture> TextureManager::CreateTexture(Texture &&texture)
{
	return CreateResource(std::move(texture));
}


/*
	Texture atlases - Creating
*/

NonOwningPtr<TextureAtlas> TextureManager::CreateTextureAtlas(std::string name, std::string asset_name,
	int rows, int columns, std::optional<int> sub_textures, texture_atlas::AtlasSubTextureOrder sub_texture_order)
{
	auto ptr = CreateResource<TextureAtlas>(std::move(name), std::move(asset_name),
		rows, columns, sub_textures, sub_texture_order);
	CreateSubTextures(ptr);
	return ptr;
}

NonOwningPtr<TextureAtlas> TextureManager::CreateTextureAtlas(std::string name, std::string asset_name,
	texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
	texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode,
	int rows, int columns, std::optional<int> sub_textures, texture_atlas::AtlasSubTextureOrder sub_texture_order)
{
	auto ptr = CreateResource<TextureAtlas>(std::move(name), std::move(asset_name),
		min_filter, mag_filter, rows, columns, sub_textures, sub_texture_order);
	CreateSubTextures(ptr, min_filter, mag_filter, mip_filter, s_wrap_mode, t_wrap_mode);
	return ptr;
}

NonOwningPtr<TextureAtlas> TextureManager::CreateTextureAtlas(std::string name, std::string asset_name,
	texture::TextureFilter filter, texture::MipmapFilter mip_filter, texture::TextureWrapMode wrap_mode,
	int rows, int columns, std::optional<int> sub_textures, texture_atlas::AtlasSubTextureOrder sub_texture_order)
{
	auto ptr = CreateResource<TextureAtlas>(std::move(name), std::move(asset_name),
		filter, rows, columns, sub_textures, sub_texture_order);
	CreateSubTextures(ptr, filter, mip_filter, wrap_mode);
	return ptr;
}

NonOwningPtr<TextureAtlas> TextureManager::CreateTextureAtlas(std::string name, std::string asset_name,
	texture::TextureFilter filter, texture::TextureWrapMode wrap_mode,
	int rows, int columns, std::optional<int> sub_textures, texture_atlas::AtlasSubTextureOrder sub_texture_order)
{
	auto ptr = CreateResource<TextureAtlas>(std::move(name), std::move(asset_name),
		filter, rows, columns, sub_textures, sub_texture_order);
	CreateSubTextures(ptr, filter, wrap_mode);
	return ptr;
}


/*
	Textures - Retrieving
*/

NonOwningPtr<Texture> TextureManager::GetTexture(std::string_view name) noexcept
{
	return GetResource(name);
}

NonOwningPtr<const Texture> TextureManager::GetTexture(std::string_view name) const noexcept
{
	return GetResource(name);
}


/*
	Textures - Removing
*/

void TextureManager::ClearTextures() noexcept
{
	ClearResources();
}

bool TextureManager::RemoveTexture(Texture &texture) noexcept
{
	return RemoveResource(texture);
}

bool TextureManager::RemoveTexture(std::string_view name) noexcept
{
	return RemoveResource(name);
}

} //ion::graphics::textures