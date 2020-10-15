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
#include <tuple>

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

int max_texture_size() noexcept
{
	static const auto max_texture_size = gl::MaxTextureSize();
	return max_texture_size;
}


std::optional<std::pair<std::string, texture::TextureExtents>> prepare_texture(
	const std::string &file_data, const std::filesystem::path &file_path,
	texture::TextureFilter min_filter, texture::TextureFilter mag_filter,
	std::optional<NpotScale> npot_scale, std::optional<NpotScaleFit> npot_scale_fit,
	NpotScaleResampling npot_scale_resampling)
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
	if (npot_scale || !has_support_for_non_power_of_two_textures())
	{
		auto width = extents.Width;
		auto height = extents.Height;
		auto [new_width , new_height] =
			power_of_two_adjusted_size(width, height,
				npot_scale.value_or(NpotScale::ToNearest), npot_scale_fit);

		//Rescale is needed (new size is different)
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
							switch (npot_scale_resampling)
							{
								case NpotScaleResampling::Bicubic:
								return FILTER_BICUBIC;

								case NpotScaleResampling::Lanczos3:
								return FILTER_LANCZOS3;

								case NpotScaleResampling::Bilinear:
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

			//Rescale
			{
				auto dst_bitmap = FreeImage_Rescale(bitmap, new_width, new_height, image_filter);
				FreeImage_Unload(bitmap);
				bitmap = dst_bitmap;
			}

			if (!bitmap)
				return {};
		}

		//Enlarge canvas is needed (one dimension is npot)
		if (auto is_width_pot = is_power_of_two(new_width),
				 is_height_pot = is_power_of_two(new_height); !is_width_pot || !is_height_pot)
		{
			auto [left, top, right, bottom] =
				[&]() noexcept
				{
					//Enlarge width
					if (!is_width_pot)
					{
						auto padding_width = static_cast<int>(upper_power_of_two(new_width)) - new_width;
						auto padding_half_width = padding_width / 2;
						return std::tuple{padding_half_width, 0, padding_half_width + padding_width % 2, 0};
					}
					//Enlarge height
					else
					{
						auto padding_height = static_cast<int>(upper_power_of_two(new_height)) - new_height;
						auto padding_half_height = padding_height / 2;
						return std::tuple{0, padding_half_height, 0, padding_half_height + padding_height % 2};
					}
				}();
			
			//Enlarge canvas
			{
				auto color = RGBQUAD{0xFF, 0xFF, 0xFF, 0x00};
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

std::optional<int> load_texture(const std::string &pixel_data, const texture::TextureExtents &extents,
	texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
	texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode) noexcept
{
	auto texture_handle = 0;
	glGenTextures(1, reinterpret_cast<unsigned int*>(&texture_handle));
	glBindTexture(GL_TEXTURE_2D, texture_handle);

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

	//Upload image to gl
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0,
		extents.BitDepth == 32 ? GL_RGBA : GL_RGB, extents.ActualWidth, extents.ActualHeight, 0,
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

void unload_texture(int texture_handle) noexcept
{
	glDeleteTextures(1, reinterpret_cast<unsigned int*>(&texture_handle));
}

} //texture_manager::detail


//Protected

/*
	Events
*/

bool TextureManager::IsResourcesEquivalent(const Texture &lhs, const Texture &rhs) noexcept
{
	return detail::is_textures_equivalent(lhs, rhs);
}


bool TextureManager::PrepareResource(Texture &texture) noexcept
{
	if (this->FileResourceManager::PrepareResource(texture))
	{
		if (auto texture_data =
			detail::prepare_texture(
				*texture.FileData(), *texture.FilePath(),
				texture.MinFilter(), texture.MagFilter(),
				texture_npot_scale_, texture_npot_scale_fit_,
				texture_npot_scale_resampling_); texture_data)
		{
			auto &[pixel_data, extents] = *texture_data;
			texture.PixelData(std::move(pixel_data), extents);
		}

		return texture.PixelData().has_value();
	}
	else
		return false;
}

bool TextureManager::LoadResource(Texture &texture) noexcept
{
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
	detail::max_texture_size();
}

TextureManager::~TextureManager() noexcept
{
	this->UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Textures
	Creating
*/

Texture& TextureManager::CreateTexture(std::string name)
{
	return this->CreateResource(std::move(name));
}

Texture& TextureManager::CreateTexture(std::string name,
	texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
	texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode)
{
	return this->CreateResource(std::move(name), min_filter, mag_filter, mip_filter, s_wrap_mode, t_wrap_mode);
}

Texture& TextureManager::CreateTexture(std::string name,
	texture::TextureFilter filter, texture::MipmapFilter mip_filter,
	texture::TextureWrapMode wrap_mode)
{
	return this->CreateResource(std::move(name), filter, mip_filter, wrap_mode);
}

Texture& TextureManager::CreateTexture(std::string name,
	texture::TextureFilter filter, texture::TextureWrapMode wrap_mode)
{
	return this->CreateResource(std::move(name), filter, wrap_mode);
}


/*
	Textures
	Removing
*/

void TextureManager::ClearTextures() noexcept
{
	this->ClearResources();
}

bool TextureManager::RemoveTexture(Texture &texture) noexcept
{
	return this->RemoveResource(texture);
}

} //ion::graphics::textures