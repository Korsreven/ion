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

std::optional<std::tuple<font::detail::container_type<std::string>, font::detail::container_type<font::GlyphExtents>, int>> prepare_font(
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
	font::detail::container_type<font::GlyphExtents> glyph_extents(glyph_count);
	auto glyph_max_height = 0;
	
	for (auto i = 0; i < glyph_count; ++i)
	{
		//Load glyph, if failed continue to the next glyph
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT) != 0)
			continue;
		
		//Render glyph, if failed continue to next glyph
		if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) != 0)
			continue;

		font::GlyphExtents extents;
		extents.Left = face->glyph->bitmap_left;
		extents.Top = face->glyph->bitmap_top;
		extents.Width = static_cast<int>(face->glyph->bitmap.width);
		extents.Height = static_cast<int>(face->glyph->bitmap.rows);
		extents.ActualWidth = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(extents.Width));
		extents.ActualHeight = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(extents.Height));
		extents.Advance = face->glyph->advance.x / 64;

		//Update max glyph height if higher than current max
		if (glyph_max_height < extents.Height)
			glyph_max_height = extents.Height;

		glyph_extents[i] = extents;
		glyph_data[i].assign(extents.ActualWidth * extents.ActualHeight * 2, '\0');

		//Convert FreeType bitmap data to OpenGL texture data
		for (auto y = 0; y < extents.ActualHeight; ++y)
		{
			for (auto x = 0; x < extents.ActualWidth; ++x)
			{
				glyph_data[i][2 * (x + y * extents.ActualWidth)] = '\xff'; //Anti-aliasing fix

				if (x < extents.Width && y < extents.Height)
					glyph_data[i][2 * (x + y * extents.ActualWidth) + 1] =
						face->glyph->bitmap.buffer[x + extents.Width * y];
			}
		}
	}

	//Use default font size if no visual glyphs found
	if (glyph_max_height == 0)
		glyph_max_height = size;

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	return std::tuple{std::move(glyph_data), std::move(glyph_extents), glyph_max_height};
}

std::pair<std::optional<int>, std::optional<font::detail::container_type<int>>> load_font(
	const font::detail::container_type<std::string> &glyph_data,
	const font::detail::container_type<font::GlyphExtents> &glyph_extents,
	font::GlyphFilter min_filter, font::GlyphFilter mag_filter) noexcept
{
	auto glyph_count = std::ssize(glyph_data);
	auto handle = static_cast<int>(glGenLists(glyph_count));
	font::detail::container_type<int> glyph_handles(glyph_count);

	glGenTextures(glyph_count, reinterpret_cast<unsigned int*>(std::data(glyph_handles)));

	for (auto i = 0; i < glyph_count; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, glyph_handles[i]);

		//Minification filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			min_filter == font::GlyphFilter::NearestNeighbor ? GL_NEAREST : GL_LINEAR);

		//Magnification filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			mag_filter == font::GlyphFilter::NearestNeighbor ? GL_NEAREST : GL_LINEAR);

		//Texture wrap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Upload image to gl (always POT)
		glTexImage2D(GL_TEXTURE_2D, 0,
			GL_RGBA, glyph_extents[i].ActualWidth, glyph_extents[i].ActualHeight, 0, GL_LUMINANCE_ALPHA,
			GL_UNSIGNED_BYTE, std::data(glyph_data[i]));


		//Make display list for this glyph
		glNewList(handle + i, GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, glyph_handles[i]);

		glPushMatrix();
		glTranslatef(static_cast<real>(glyph_extents[i].Left), 0.0f, 0.0f);
		glTranslatef(0.0f, static_cast<real>(glyph_extents[i].Top) - glyph_extents[i].Height, 0.0f);

		//Texture coordinates
		auto s = static_cast<real>(glyph_extents[i].Width) / glyph_extents[i].ActualWidth;
		auto t = static_cast<real>(glyph_extents[i].Height) / glyph_extents[i].ActualHeight;

		//Note:
		//The texture coordinates follows [0, 0] -> [width, height] (GUI coordinate system)
		//The vertices follows [0, height] -> [width, 0] (normal coordinate system)
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);	glVertex2i(0, glyph_extents[i].Height);
		glTexCoord2f(0.0f, t);		glVertex2i(0, 0);
		glTexCoord2f(s, t);			glVertex2i(glyph_extents[i].Width, 0);
		glTexCoord2f(s, 0.0f);		glVertex2i(glyph_extents[i].Width, glyph_extents[i].Height);
		glEnd();

		glPopMatrix();
		glTranslatef(static_cast<real>(glyph_extents[i].Advance), 0, 0); //Translate relative
		glEndList();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return std::pair{handle, glyph_handles};
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

bool FontManager::PrepareResource(Font &font) noexcept
{
	if (FileResourceManager::PrepareResource(font))
	{
		if (auto font_data = detail::prepare_font(*font.FileData(),
			font.Size(), font.FaceIndex(), font.CharacterEncoding()); font_data)
		{
			auto &[glyph_data, glyph_extents, glyph_max_height] = *font_data;
			font.GlyphData(std::move(glyph_data), std::move(glyph_extents), glyph_max_height);
		}

		return font.GlyphData().has_value();
	}
	else
		return false;
}

bool FontManager::LoadResource(Font &font) noexcept
{
	auto &glyph_data = font.GlyphData();
	auto &glyph_extents = font.GlyphExtents();
	auto [glyph_min_filter, glyph_mag_filter] = font.GlyphFilter();

	if (glyph_data && glyph_extents)
	{
		auto [handle, glyph_handles] = detail::load_font(*glyph_data, *glyph_extents, glyph_min_filter, glyph_mag_filter);
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

FontManager::~FontManager() noexcept
{
	UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Fonts
	Creating
*/

Font& FontManager::CreateFont(std::string name, std::string asset_name, int size, int face_index,
	font::CharacterEncoding encoding, font::GlyphFilter min_filter, font::GlyphFilter mag_filter)
{
	return CreateResource(std::move(name), std::move(asset_name), size, face_index, encoding, min_filter, mag_filter);
}

Font& FontManager::CreateFont(std::string name, std::string asset_name, int size,
	font::CharacterEncoding encoding, font::GlyphFilter min_filter, font::GlyphFilter mag_filter)
{
	return CreateResource(std::move(name), std::move(asset_name), size, encoding, min_filter, mag_filter);
}

Font& FontManager::CreateFont(std::string name, std::string asset_name, int size,
	font::CharacterEncoding encoding, font::GlyphFilter filter)
{
	return CreateResource(std::move(name), std::move(asset_name), size, encoding, filter);
}

Font& FontManager::CreateFont(std::string name, std::string asset_name, int size,
	font::CharacterEncoding encoding)
{
	return CreateResource(std::move(name), std::move(asset_name), size, encoding);
}

Font& FontManager::CreateFont(std::string name, std::string asset_name, int size)
{
	return CreateResource(std::move(name), std::move(asset_name), size);
}


Font& FontManager::CreateFont(const Font &font)
{
	return CreateResource(font);
}

Font& FontManager::CreateFont(Font &&font)
{
	return CreateResource(std::move(font));
}


/*
	Fonts
	Retrieving
*/

Font* FontManager::GetFont(std::string_view name) noexcept
{
	return GetResource(name);
}

const Font* FontManager::GetFont(std::string_view name) const noexcept
{
	return GetResource(name);
}


/*
	Fonts
	Removing
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