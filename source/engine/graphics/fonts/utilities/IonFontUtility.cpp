/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts/utilities
File:	IonFontUtility.cpp
-------------------------------------------
*/

#include "IonFontUtility.h"

#include "graphics/fonts/IonFontManager.h"
#include "types/IonTypes.h"

namespace ion::graphics::fonts::utilities
{

namespace detail
{

std::string truncate_string(std::string str, int max_width, std::string suffix,
	const font::detail::container_type<font::GlyphExtents> &extents)
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
	const font::detail::container_type<font::GlyphExtents> &extents)
{
	auto width = 0;
	auto space_off = std::optional<int>{};

	for (auto i = 0; i < std::ssize(str); ++i)
	{
		switch (str[i])
		{
			//New line found
			case '\n':
			break;

			//Space found
			case ' ':
			space_off = i;
			[[fallthrough]];

			default:
			{
				auto [c_width, c_height] = character_size_in_pixels(str[i], extents);			

				//Insert new line
				if (width > 0 && //At least one character
					width + c_width > max_width) //Too  wide
				{
					//Break at last space
					if (space_off)
						str[(i = *space_off)] = '\n';

					//No space found, cut inside word
					else
						str.insert(i, 1, '\n');
				}
				else
				{
					width += c_width;
					continue;
				}
				
				break;
			}
		}

		width = 0;
		space_off = {};
	}

	return str;
}

} //detail


/*
	Measuring
*/

std::optional<Vector2> MeasureCharacter(char c, Font &font) noexcept
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

std::optional<Vector2> MeasureString(std::string_view str, Font &font) noexcept
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

std::optional<std::string> TruncateString(std::string str, int max_width, Font &font)
{
	return TruncateString(std::move(str), max_width, "...", font);
}

std::optional<std::string> TruncateString(std::string str, int max_width, std::string suffix, Font &font)
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

std::optional<std::string> WordWrap(std::string str, int max_width, Font &font)
{
	if (font.IsLoaded() || (font.Owner() && font.Owner()->Load(font)))
	{
		if (auto &extents = font.GlyphExtents(); extents)
			return detail::word_wrap(std::move(str), max_width, *extents);
	}

	return {};
}

} //ion::graphics::fonts::utilities