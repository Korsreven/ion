/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonTextManager.cpp
-------------------------------------------
*/

#include "IonTextManager.h"

#include <algorithm>
#include <type_traits>

#include "types/IonTypes.h"

namespace ion::graphics::fonts
{

using namespace text_manager;

namespace text_manager::detail
{

int character_width(char c, Font &font) noexcept
{
	auto glyph_index = static_cast<unsigned char>(c);

	if (auto& extents = font.GlyphExtents(); extents && glyph_index < std::size(*extents))
		return (*extents)[glyph_index].Advance / 64;
	else
		return 0;
}

int character_height(char c, Font &font) noexcept
{
	auto glyph_index = static_cast<unsigned char>(c);

	if (auto& extents = font.GlyphExtents(); extents && glyph_index < std::size(*extents))
		return (*extents)[glyph_index].Height;
	else
		return 0;
}


int string_width(std::string_view str, Font &font) noexcept
{
	auto width = 0;

	if (auto& extents = font.GlyphExtents(); extents)
	{
		for (auto c : str)
		{	
			if (auto glyph_index = static_cast<unsigned char>(c); glyph_index < std::size(*extents))
				width += (*extents)[glyph_index].Advance;
		}
	}
	
	return width / 64;
}

int string_height(std::string_view str, Font &font) noexcept
{
	auto height = 0;

	if (auto& extents = font.GlyphExtents(); extents)
	{
		for (auto c : str)
		{	
			if (auto glyph_index = static_cast<unsigned char>(c); glyph_index < std::size(*extents))
				height = std::max(height, (*extents)[glyph_index].Height);
		}
	}
	
	return height;
}

} //text_manager::detail


//Public


/*
	Texts
	Creating
*/

Text& TextManager::CreateText(std::string name, std::string str, TypeFace &type_face)
{
	return Create(std::move(name), std::move(str), std::ref(type_face));
}


Text& TextManager::CreateText(const Text &text)
{
	return Create(text);
}

Text& TextManager::CreateText(Text &&text)
{
	return Create(std::move(text));
}


/*
	Texts
	Retrieving
*/

Text* TextManager::GetText(std::string_view name) noexcept
{
	return Get(name);
}

const Text* TextManager::GetText(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Texts
	Removing
*/

void TextManager::ClearTexts() noexcept
{
	Clear();
}

bool TextManager::RemoveText(Text &text) noexcept
{
	return Remove(text);
}

bool TextManager::RemoveText(std::string_view name) noexcept
{
	return Remove(name);
}


/*
	Measuring
*/

std::optional<Vector2> TextManager::MeasureCharacter(char c, Font &font) noexcept
{
	if (font.IsLoaded() || (font.Owner() && font.Owner()->Load(font)))
		return Vector2{static_cast<real>(detail::character_width(c, font)),
					   static_cast<real>(detail::character_height(c, font))};
	else
		return {};
}

std::optional<Vector2> TextManager::MeasureString(std::string_view str, Font &font) noexcept
{
	if (font.IsLoaded() || (font.Owner() && font.Owner()->Load(font)))
		return Vector2{static_cast<real>(detail::string_width(str, font)),
					   static_cast<real>(detail::string_height(str, font))};
	else
		return {};
}

} //ion::graphics::fonts