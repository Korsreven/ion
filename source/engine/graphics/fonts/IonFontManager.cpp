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

std::optional<std::tuple<font::detail::container_type<std::string>, font::detail::container_type<int>, int>> prepare_font(
	const std::string &file_data, int size, int face_index, font::CharacterEncoding encoding)
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

	auto glyph_count = static_cast<int>(encoding);
	font::detail::container_type<std::string> glyph_data(glyph_count);
	font::detail::container_type<int> glyph_advances(glyph_count);
	auto glyph_height = 0;
	
	for (auto i = 0; i < glyph_count; ++i)
	{
		//Load glyph, if failed continue to the next glyph
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT) != 0)
			continue;
		
		//Render glyph, if failed continue to next glyph
		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0)
			continue;

		auto width = static_cast<int>(face->glyph->bitmap.width);
		auto height = static_cast<int>(face->glyph->bitmap.rows);
		auto new_width =
			static_cast<int>(textures::texture_manager::detail::upper_power_of_two(width));
		auto new_height =
			static_cast<int>(textures::texture_manager::detail::upper_power_of_two(height));

		glyph_data[i].assign(new_width * new_height * 2, '\0');

		//Convert FreeType bitmap data to OpenGL texture data
		for (auto y = 0; y < new_height; ++y)
		{
			for (auto x = 0; x < new_width; ++x)
			{
				glyph_data[2 * (x + y * new_width)] = '\xff'; //Anti-aliasing fix

				if (x < width && y < height)
					glyph_data[2 * (x + y * new_width) + 1] =
						face->glyph->bitmap.buffer[x + width * y];
			}
		}

		glyph_advances[i] = face->glyph->advance.x;

		//Update max glyph height if higher than current max
		if (glyph_height < height)
			glyph_height = height;
	}

	//Use default font size if no visual glyphs found
	if (glyph_height == 0)
		glyph_height = size;

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	return std::tuple{std::move(glyph_data), std::move(glyph_advances), glyph_height};
}

std::pair<std::optional<int>, std::optional<font::detail::container_type<int>>> load_font(
	const font::detail::container_type<std::string> &glyph_data,
	font::GlyphFilter min_filter, font::GlyphFilter mag_filter) noexcept
{
	//Make OpenGL textures from glyph data

	return std::pair{0, font::detail::container_type<int>{}};
}

void unload_font(int font_handle, const font::detail::container_type<int> &glyph_handles) noexcept
{
	glDeleteLists(font_handle, std::ssize(glyph_handles));
	glDeleteTextures(std::ssize(glyph_handles), reinterpret_cast<const unsigned int*>(std::data(glyph_handles)));
}

} //font_manager::detail


//Protected

/*
	Events
*/

bool FontManager::IsResourcesEquivalent(const Font &lhs, const Font &rhs) noexcept
{
	return detail::is_fonts_equivalent(lhs, rhs);
}


bool FontManager::PrepareResource(Font &font) noexcept
{
	if (this->FileResourceManager::PrepareResource(font))
	{
		if (auto font_data = detail::prepare_font(*font.FileData(),
			font.Size(), font.FaceIndex(), font.CharacterEncoding()); font_data)
		{
			auto &[glyph_data, glyph_advances, glyph_height] = *font_data;
			font.GlyphData(std::move(glyph_data), std::move(glyph_advances), glyph_height);
		}

		return font.GlyphData().has_value();
	}
	else
		return false;
}

bool FontManager::LoadResource(Font &font) noexcept
{
	auto &glyph_data = font.GlyphData();
	auto [glyph_min_filter, glyph_mag_filter] = font.GlyphFilter();

	if (glyph_data)
	{
		auto [handle, glyph_handles] = detail::load_font(*glyph_data, glyph_min_filter, glyph_mag_filter);
		font.Handle(handle);
		font.GlyphHandles(std::move(glyph_handles));
		return font.Handle().has_value();
	}
	else
		return false;
}

bool FontManager::UnloadResource(Font &font) noexcept
{
	if (auto handle = font.Handle(); handle)
	{
		detail::unload_font(*handle, *font.GlyphHandles());
		font.Handle({});
		font.GlyphHandles({});
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

FontManager::FontManager() noexcept
{
	//Empty
}

FontManager::~FontManager() noexcept
{
	this->UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Fonts
	Creating
*/

Font& FontManager::CreateFont(std::string name, int size, int face_index,
	font::CharacterEncoding encoding, font::GlyphFilter min_filter, font::GlyphFilter mag_filter)
{
	return this->CreateResource(std::move(name), size, face_index, encoding, min_filter, mag_filter);
}

Font& FontManager::CreateFont(std::string name, int size, font::CharacterEncoding encoding,
	font::GlyphFilter min_filter, font::GlyphFilter mag_filter)
{
	return this->CreateResource(std::move(name), size, encoding, min_filter, mag_filter);
}

Font& FontManager::CreateFont(std::string name, int size, font::CharacterEncoding encoding,
	font::GlyphFilter filter)
{
	return this->CreateResource(std::move(name), size, encoding, filter);
}

Font& FontManager::CreateFont(std::string name, int size, font::CharacterEncoding encoding)
{
	return this->CreateResource(std::move(name), size, encoding);
}

Font& FontManager::CreateFont(std::string name, int size)
{
	return this->CreateResource(std::move(name), size);
}


/*
	Fonts
	Removing
*/

void FontManager::ClearFonts() noexcept
{
	this->ClearResources();
}

bool FontManager::RemoveFont(Font &font) noexcept
{
	return this->RemoveResource(font);
}

} //ion::graphics::fonts