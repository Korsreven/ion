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

#include <type_traits>

#include "types/IonTypes.h"

namespace ion::graphics::fonts
{

using namespace text_manager;

namespace text_manager::detail
{

std::string truncate_string(std::string str, int max_width, std::string suffix,
	const font::detail::container_type<font::GlyphExtents> &extents) noexcept
{
	//Truncate
	if (auto [width, height] = detail::string_size_in_pixels(str, extents);
		width > max_width)
	{
		auto [suffix_width, suffix_height] = detail::string_size_in_pixels(suffix, extents);

		if (suffix_width > max_width)
			return "";
		else if (suffix_width == max_width)
			return suffix;

		width += suffix_width;

		while (!std::empty(str))
		{
			auto [c_width, c_height] = character_size_in_pixels(str.back(), extents);
			str.pop_back();

			if ((width -= c_width) <= max_width)
				break;
		}

		str += std::move(suffix);
	}

	return str;
}

std::string word_wrap(std::string str, int max_width,
	const font::detail::container_type<font::GlyphExtents> &extents) noexcept
{
	return str;
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
	{
		if (auto &extents = font.GlyphExtents(); extents)
		{
			auto [width, height] = detail::character_size_in_pixels(c, *extents);
			return Vector2{static_cast<real>(width), static_cast<real>(height)};
		}
	}
	
	return {};
}

std::optional<Vector2> TextManager::MeasureString(std::string_view str, Font &font) noexcept
{
	if (font.IsLoaded() || (font.Owner() && font.Owner()->Load(font)))
	{
		if (auto &extents = font.GlyphExtents(); extents)
		{
			auto [width, height] = detail::string_size_in_pixels(str, *extents);
			return Vector2{static_cast<real>(width), static_cast<real>(height)};
		}
	}
	
	return {};
}


/*
	Truncating
*/

std::optional<std::string> TextManager::TruncateString(std::string str, int max_width, Font &font)
{
	return TruncateString(std::move(str), max_width, "...", font);
}

std::optional<std::string> TextManager::TruncateString(std::string str, int max_width, std::string suffix, Font &font)
{
	if (font.IsLoaded() || (font.Owner() && font.Owner()->Load(font)))
	{
		if (auto &extents = font.GlyphExtents(); extents)
			return detail::truncate_string(std::move(str), max_width, std::move(suffix), *extents);
	}

	return {};
}


/*
	Word wrapping
*/

std::optional<std::string> TextManager::WordWrap(std::string str, int max_width, Font &font)
{
	if (font.IsLoaded() || (font.Owner() && font.Owner()->Load(font)))
	{
		if (auto &extents = font.GlyphExtents(); extents)
			return detail::word_wrap(std::move(str), max_width, *extents);
	}

	return {};
}

} //ion::graphics::fonts