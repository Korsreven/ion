/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonFontManager.cpp
-------------------------------------------
*/

#include "IonFontManager.h"

#include <algorithm>

#include "graphics/IonGraphicsAPI.h"
#include "FreeType/ft2build.h"
#include FT_FREETYPE_H

#include "graphics/textures/IonTextureManager.h"

#undef CreateFont

namespace ion::graphics::fonts
{

using namespace font_manager;

namespace font_manager::detail
{

std::optional<std::tuple<font::GlyphBitmapData, font::GlyphMetrices, font::GlyphMaxMetric>> prepare_font(
	const std::string &file_data, int size, int face_index,
	int character_spacing, font::FontCharacterSet character_set)
{
	FT_Library library = nullptr;
	if (FT_Init_FreeType(&library) != 0)
		return {}; //Not supported

	FT_Face face = nullptr;
	if (FT_New_Memory_Face(library, (const FT_Byte*)std::data(file_data),
		std::ssize(file_data), face_index, &face) != 0)
	{
		FT_Done_FreeType(library);
		return {}; //Could not load face
	}

	FT_Set_Char_Size(face, size * 64, size * 64, 96, 96);

	auto glyph_count = static_cast<int>(character_set);
	font::GlyphBitmapData glyph_data(glyph_count);
	font::GlyphMetrices glyph_metrics(glyph_count);
	font::GlyphMaxMetric glyph_max_metrics;
	
	for (auto i = 0; i < glyph_count; ++i)
	{
		//Load glyph, if failed continue to the next glyph
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT) != 0)
			continue;
		
		//Render glyph, if failed continue to next glyph
		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0)
			continue;

		font::GlyphMetric metric;
		metric.Left = face->glyph->bitmap_left;
		metric.Top = face->glyph->bitmap_top;
		metric.Width = static_cast<int>(face->glyph->bitmap.width);
		metric.Height = static_cast<int>(face->glyph->bitmap.rows);
		metric.ActualWidth = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(metric.Width));
		metric.ActualHeight = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(metric.Height));
		metric.Advance = face->glyph->advance.x / 64 + character_spacing;

		glyph_metrics[i] = metric;

		//Update max glyph metrics if higher than current max
		if (glyph_max_metrics.Width < metric.Width)
			glyph_max_metrics.Width = metric.Width;
		if (glyph_max_metrics.Height < metric.Height)
			glyph_max_metrics.Height = metric.Height;

		if (glyph_max_metrics.ActualWidth < metric.ActualWidth)
			glyph_max_metrics.ActualWidth = metric.ActualWidth;
		if (glyph_max_metrics.ActualHeight < metric.ActualHeight)
			glyph_max_metrics.ActualHeight = metric.ActualHeight;

		glyph_data[i].assign(metric.ActualWidth * metric.ActualHeight * 2, '\0');

		//Convert FreeType bitmap data to OpenGL texture data
		for (auto y = 0; y < metric.ActualHeight; ++y)
		{
			for (auto x = 0; x < metric.ActualWidth; ++x)
			{
				glyph_data[i][2 * (x + y * metric.ActualWidth)] = '\xff'; //Anti-aliasing fix

				if (x < metric.Width && y < metric.Height)
				{
					auto color_component = face->glyph->bitmap.buffer[x + metric.Width * y];
					glyph_data[i][2 * (x + y * metric.ActualWidth) + 1] =
						static_cast<unsigned char>(std::clamp(color_component + color_component / 2, 0, 255));
							//Combine color components to better define each glyph
				}
			}
		}
	}

	//Use default font size if no visual glyphs found
	if (glyph_max_metrics.Width == 0)
		glyph_max_metrics.Width = size;
	if (glyph_max_metrics.Height == 0)
		glyph_max_metrics.Height = size;

	if (glyph_max_metrics.ActualWidth == 0)
		glyph_max_metrics.ActualWidth = size;
	if (glyph_max_metrics.ActualHeight == 0)
		glyph_max_metrics.ActualHeight = size;

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	return std::tuple{std::move(glyph_data), std::move(glyph_metrics), std::move(glyph_max_metrics)};
}

std::optional<font::GlyphTextureHandle> load_font(
	const font::GlyphBitmapData &glyph_data,
	const font::GlyphMetrices &glyph_metrics,
	const font::GlyphMaxMetric &glyph_max_metrics,
	font::FontGlyphFilter glyph_min_filter, font::FontGlyphFilter glyph_mag_filter,
	GlyphTextureType glyph_texture_type)
{
	if (!textures::texture_manager::detail::has_support_for_array_texture())
		glyph_texture_type = GlyphTextureType::Texture2D;

	auto glyph_count = std::ssize(glyph_data);
	font::GlyphTextureHandle glyph_handle;
	glyph_handle.Ids.assign(
		glyph_texture_type == GlyphTextureType::Texture2D ?
		glyph_count : 1, 0);

	switch (glyph_texture_type)
	{
		case GlyphTextureType::ArrayTexture2D:
		glyph_handle.Type = textures::texture::TextureType::ArrayTexture2D;
		break;

		case GlyphTextureType::Texture2D:
		default:
		glyph_handle.Type = textures::texture::TextureType::Texture2D;
		break;
	}

	glGenTextures(std::ssize(glyph_handle.Ids), reinterpret_cast<unsigned int*>(std::data(glyph_handle.Ids)));

	if (glyph_texture_type == GlyphTextureType::ArrayTexture2D)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, glyph_handle.Ids.front());

		//Minification filter
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER,
			glyph_min_filter == font::FontGlyphFilter::NearestNeighbor ? GL_NEAREST : GL_LINEAR);

		//Magnification filter
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER,
			glyph_mag_filter == font::FontGlyphFilter::NearestNeighbor ? GL_NEAREST : GL_LINEAR);

		//Texture wrap
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (auto max_glyphs = textures::texture_manager::detail::max_array_texture_layers(); glyph_count > max_glyphs)
			glyph_count = max_glyphs;

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0,
			GL_RGBA8, glyph_max_metrics.ActualWidth, glyph_max_metrics.ActualHeight, glyph_count,
			0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, nullptr);
		
		for (auto i = 0; i < glyph_count; ++i)
		{
			//Upload image to gl (always POT)
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
				glyph_metrics[i].ActualWidth, glyph_metrics[i].ActualHeight, 1,
				GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, std::data(glyph_data[i]));
		}

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}
	else
	{
		for (auto i = 0; i < glyph_count; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, glyph_handle.Ids[i]);

			//Minification filter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				glyph_min_filter == font::FontGlyphFilter::NearestNeighbor ? GL_NEAREST : GL_LINEAR);

			//Magnification filter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
				glyph_mag_filter == font::FontGlyphFilter::NearestNeighbor ? GL_NEAREST : GL_LINEAR);

			//Texture wrap
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			//Upload image to gl (always POT)
			glTexImage2D(GL_TEXTURE_2D, 0,
				GL_RGBA8, glyph_metrics[i].ActualWidth, glyph_metrics[i].ActualHeight,
				0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, std::data(glyph_data[i]));

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	return std::move(glyph_handle);
}

void unload_font(const font::GlyphTextureHandle &glyph_handle) noexcept
{
	glDeleteTextures(std::ssize(glyph_handle.Ids), reinterpret_cast<const unsigned int*>(std::data(glyph_handle.Ids)));
}

} //font_manager::detail


//Protected

/*
	Events
*/

bool FontManager::PrepareResource(Font &font)
{
	if (FileResourceManager::PrepareResource(font))
	{
		if (auto font_data = detail::prepare_font(*font.FileData(),
			font.Size(), font.FaceIndex(), font.CharacterSpacing(), font.CharacterSet()); font_data)
		{
			auto &[glyph_data, glyph_metrics, glyph_max_metrics] = *font_data;
			font.GlyphData(std::move(glyph_data), std::move(glyph_metrics), std::move(glyph_max_metrics));
		}

		return font.GlyphData().has_value();
	}
	else
		return false;
}

bool FontManager::LoadResource(Font &font)
{
	auto &glyph_data = font.GlyphData();
	auto &glyph_metrics = font.GlyphMetrics();
	auto &glyph_max_metrics = font.GlyphMaxMetrics();

	if (glyph_data && glyph_metrics && glyph_max_metrics)
	{
		auto [glyph_min_filter, glyph_mag_filter] = font.GlyphFilter();

		if (auto glyph_handle = detail::load_font(*glyph_data,
			*glyph_metrics, *glyph_max_metrics, glyph_min_filter, glyph_mag_filter, glyph_texture_type_); glyph_handle)
		{
			//Update glyph metrics with glyph max metrics
			if (glyph_handle->Type == textures::texture::TextureType::ArrayTexture2D)
			{
				auto new_glyph_metrics = *glyph_metrics; //Make copy
				for (auto &glyph_metric : new_glyph_metrics)
				{
					glyph_metric.ActualWidth = glyph_max_metrics->ActualWidth;
					glyph_metric.ActualHeight = glyph_max_metrics->ActualHeight;
				}

				font.GlyphMetrics(std::move(new_glyph_metrics));
			}

			font.GlyphHandle(std::move(glyph_handle));
		}

		return font.GlyphHandle().has_value();
	}
	else
		return false;
}

bool FontManager::UnloadResource(Font &font) noexcept
{
	if (auto &glyph_handle = font.GlyphHandle(); glyph_handle)
	{
		detail::unload_font(*glyph_handle);
		font.GlyphHandle({});
		return true;
	}
	else
		return false;
}


void FontManager::ResourceLoaded(Font &font) noexcept
{
	FileResourceManager::ResourceLoaded(font);
	font.ResetGlyphData();
		//Glyph data not required after font has been loaded (save memory)
}

void FontManager::ResourceFailed(Font &font) noexcept
{
	FileResourceManager::ResourceFailed(font);
	font.ResetGlyphData();
		//Glyph data not required after font has failed (save memory)
}


//Public

FontManager::~FontManager() noexcept
{
	UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Fonts - Creating
*/

NonOwningPtr<Font> FontManager::CreateFont(std::string name, std::string asset_name, int size, int face_index,
	int character_spacing, font::FontCharacterSet character_set, font::FontGlyphFilter min_filter, font::FontGlyphFilter mag_filter)
{
	return CreateResource(std::move(name), std::move(asset_name), size, face_index,
		character_spacing, character_set, min_filter, mag_filter);
}

NonOwningPtr<Font> FontManager::CreateFont(std::string name, std::string asset_name, int size,
	int character_spacing, font::FontCharacterSet character_set, font::FontGlyphFilter min_filter, font::FontGlyphFilter mag_filter)
{
	return CreateResource(std::move(name), std::move(asset_name), size,
		character_spacing, character_set, min_filter, mag_filter);
}

NonOwningPtr<Font> FontManager::CreateFont(std::string name, std::string asset_name, int size,
	int character_spacing, font::FontCharacterSet character_set, font::FontGlyphFilter filter)
{
	return CreateResource(std::move(name), std::move(asset_name), size,
		character_spacing, character_set, filter);
}

NonOwningPtr<Font> FontManager::CreateFont(std::string name, std::string asset_name, int size,
	int character_spacing, font::FontCharacterSet character_set)
{
	return CreateResource(std::move(name), std::move(asset_name), size,
		character_spacing, character_set);
}

NonOwningPtr<Font> FontManager::CreateFont(std::string name, std::string asset_name, int size,
	int character_spacing)
{
	return CreateResource(std::move(name), std::move(asset_name), size,
		character_spacing);
}


NonOwningPtr<Font> FontManager::CreateFont(const Font &font)
{
	return CreateResource(font);
}

NonOwningPtr<Font> FontManager::CreateFont(Font &&font)
{
	return CreateResource(std::move(font));
}


/*
	Fonts - Retrieving
*/

NonOwningPtr<Font> FontManager::GetFont(std::string_view name) noexcept
{
	return GetResource(name);
}

NonOwningPtr<const Font> FontManager::GetFont(std::string_view name) const noexcept
{
	return GetResource(name);
}


/*
	Fonts - Removing
*/

void FontManager::ClearFonts() noexcept
{
	ClearResources();
}

bool FontManager::RemoveFont(Font &font) noexcept
{
	return RemoveResource(font);
}

bool FontManager::RemoveFont(std::string_view name) noexcept
{
	return RemoveResource(name);
}

} //ion::graphics::fonts