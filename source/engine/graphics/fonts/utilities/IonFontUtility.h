/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts/utilities
File:	IonFontUtility.h
-------------------------------------------
*/

#ifndef ION_FONT_UTILITY_H
#define ION_FONT_UTILITY_H

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "graphics/fonts/IonFont.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::fonts::utilities
{
	using graphics::utilities::Color;
	using graphics::utilities::Vector2;

	namespace detail
	{
		inline auto character_size_in_pixels(char c,
			const font::detail::container_type<font::GlyphExtents> &extents) noexcept
		{
			auto glyph_index = static_cast<unsigned char>(c);

			if (glyph_index < std::size(extents))
				return std::pair{extents[glyph_index].Advance, extents[glyph_index].Height};
			else
				return std::pair{0, 0};
		}

		inline auto string_size_in_pixels(std::string_view str,
			const font::detail::container_type<font::GlyphExtents> &extents) noexcept
		{
			auto width = 0;
			auto height = 0;

			for (auto c : str)
			{
				auto [c_width, c_height] = character_size_in_pixels(c, extents);
				width += c_width;
				height = std::max(height, c_height);
			}

			return std::pair{width, height};
		}


		std::string truncate_string(std::string str, int max_width, std::string suffix,
			const font::detail::container_type<font::GlyphExtents> &extents);

		std::string word_wrap(std::string str, int max_width,
			const font::detail::container_type<font::GlyphExtents> &extents);
	} //detail


	/*
		Measuring
	*/

	//Returns the size, in pixels, of the given character when rendered with the given font
	//Returns nullopt if font could not be loaded properly
	std::optional<Vector2> MeasureCharacter(char c, Font &font) noexcept;

	//Returns the size, in pixels, of the given string when rendered with the given font
	//Returns nullopt if font could not be loaded properly
	std::optional<Vector2> MeasureString(std::string_view str, Font &font) noexcept;


	/*
		Truncating
	*/

	//Truncates the given string if wider than max width, in pixels, when rendered with the given font
	//If string has been truncated, then ... is appended to the result
	//Returns nullopt if font could not be loaded properly
	std::optional<std::string> TruncateString(std::string str, int max_width, Font &font);

	//Truncates the given string if wider than max width, in pixels, when rendered with the given font
	//If string has been truncated, then the given suffix is appended to the result
	//Returns nullopt if font could not be loaded properly
	std::optional<std::string> TruncateString(std::string str, int max_width, std::string suffix, Font &font);


	/*
		Word wrapping
	*/

	//Word wraps the given string if wider than max width, in pixels, when rendered with the given font
	//Replaces a ' ' character in between words, with a '\n' character where the line needs to be broken
	//Does only cut words if one word is wider than max width, then a '\n' character is inserted
	//Returns nullopt if font could not be loaded properly
	std::optional<std::string> WordWrap(std::string str, int max_width, Font &font);
} //ion::graphics::fonts::utilities

#endif