/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonTextManager.h
-------------------------------------------
*/

#ifndef ION_TEXT_MANAGER_H
#define ION_TEXT_MANAGER_H

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IonFont.h"
#include "IonText.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonObjectManager.h"

namespace ion::graphics::fonts
{
	using utilities::Color;
	using utilities::Vector2;

	namespace text_manager::detail
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
			const font::detail::container_type<font::GlyphExtents> &extents) noexcept;

		std::string word_wrap(std::string str, int max_width,
			const font::detail::container_type<font::GlyphExtents> &extents) noexcept;
	} //text_manager::detail


	struct TextManager final :
		managed::ObjectManager<Text, TextManager>
	{
		//Default constructor
		TextManager() = default;

		//Deleted copy constructor
		TextManager(const TextManager&) = delete;

		//Default move constructor
		TextManager(TextManager&&) = default;

		//Destructor
		~TextManager() = default;


		/*
			Operators
		*/

		//Deleted copy assignment
		TextManager& operator=(const TextManager&) = delete;

		//Move assignment
		TextManager& operator=(TextManager&&) = default;


		/*
			Ranges
		*/

		//Returns a mutable range of all texts in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto Texts() noexcept
		{
			return Objects();
		}

		//Returns an immutable range of all texts in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline const auto Texts() const noexcept
		{
			return Objects();
		}


		/*
			Texts
			Creating
		*/

		//Create a text with the given name, string and a type face
		Text& CreateText(std::string name, std::string str, TypeFace &type_face);


		//Create a text as a copy of the given text
		Text& CreateText(const Text &text);

		//Create a text by moving the given text
		Text& CreateText(Text &&text);


		/*
			Texts
			Retrieving
		*/

		//Gets a pointer to a mutable text with the given name
		//Returns nullptr if text could not be found
		[[nodiscard]] Text* GetText(std::string_view name) noexcept;

		//Gets a pointer to an immutable text with the given name
		//Returns nullptr if text could not be found
		[[nodiscard]] const Text* GetText(std::string_view name) const noexcept;


		/*
			Texts
			Removing
		*/

		//Clear all removable texts from this manager
		void ClearTexts() noexcept;

		//Remove a removable text from this manager
		bool RemoveText(Text &text) noexcept;

		//Remove a removable text with the given name from this manager
		bool RemoveText(std::string_view name) noexcept;


		/*
			Measuring
		*/

		//Returns the size, in pixels, of the given character when rendered with the given font
		//Returns nullopt if font could not be loaded properly
		static std::optional<Vector2> MeasureCharacter(char c, Font &font) noexcept;

		//Returns the size, in pixels, of the given string when rendered with the given font
		//Returns nullopt if font could not be loaded properly
		static std::optional<Vector2> MeasureString(std::string_view str, Font &font) noexcept;


		/*
			Truncating
		*/

		//Truncates the given string if wider than max width, in pixels, when rendered with the given font
		//If string has been truncated, then ... is appended to the result
		//Returns nullopt if font could not be loaded properly
		static std::optional<std::string> TruncateString(std::string str, int max_width, Font &font);

		//Truncates the given string if wider than max width, in pixels, when rendered with the given font
		//If string has been truncated, then the given suffix is appended to the result
		//Returns nullopt if font could not be loaded properly
		static std::optional<std::string> TruncateString(std::string str, int max_width, std::string suffix, Font &font);


		/*
			Word wrapping
		*/

		//Word wraps the given string if wider than max width, in pixels, when rendered with the given font
		//Replaces a ' ' character in between words, with a '\n' character where the line needs to be broken
		//Does only cut words if one word is wider than max width, then a '\n' character is inserted
		//Returns nullopt if font could not be loaded properly
		static std::optional<std::string> WordWrap(std::string str, int max_width, Font &font);
	};
} //ion::graphics::fonts

#endif