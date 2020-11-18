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

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "graphics/fonts/IonFont.h"
#include "graphics/fonts/IonText.h"
#include "graphics/fonts/IonTypeFace.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::fonts::utilities
{
	using graphics::utilities::Color;
	using graphics::utilities::Vector2;

	namespace detail
	{
		/*
			Glyph rope
		*/

		struct glyph_string final
		{
			std::string &value;
			const font::detail::container_type<font::GlyphExtents> &extents;
		};

		using glyph_strings = std::vector<glyph_string>;

		class glyph_rope final
		{
			private:

				glyph_strings strings_;

				mutable size_t size_ = 0;
				mutable std::pair<size_t, size_t> range_;

				std::pair<size_t, size_t> get_offsets(size_t off) const noexcept;

			public:

				glyph_rope(glyph_string str);
				glyph_rope(glyph_strings strings);

				char& operator[](size_t off) noexcept;
				const char& operator[](size_t off) const noexcept;
				const font::detail::container_type<font::GlyphExtents>& glyph(size_t off) const noexcept;

				std::string& insert(size_t off, size_t count, char ch);

				bool empty() const noexcept;
				size_t size() const noexcept;
		};

		glyph_rope make_glyph_rope(text::TextSections &text_sections,
			const font::detail::container_type<font::GlyphExtents> &regular_extents,
			const font::detail::container_type<font::GlyphExtents> *bold_extents,
			const font::detail::container_type<font::GlyphExtents> *italic_extents,
			const font::detail::container_type<font::GlyphExtents> *bold_italic_extents);


		/*
			Formatting
		*/

		struct html_attribute final
		{
			std::string_view name;
			std::string_view value;
		};

		struct html_element final
		{
			std::string_view tag;
			std::optional<html_attribute> attribute;
		};

		using html_elements = std::vector<html_element>;


		constexpr auto is_start_of_opening_tag(char c) noexcept
		{
			return c == '<';
		}

		constexpr auto is_start_of_closing_tag(char c, char next_c) noexcept
		{
			return c == '<' && next_c == '/';
		}

		constexpr auto is_end_of_tag(char c) noexcept
		{
			return c == '>';
		}


		constexpr auto is_html_tag(std::string_view str) noexcept
		{
			return str == "p" ||
				   str == "br" ||

				   str == "b" ||
				   str == "i" ||
				   str == "strong" ||
				   str == "em" ||

				   str == "u" ||
				   str == "ins" ||
				   str == "del" ||
				   
				   str == "font";
		}

		constexpr auto is_empty_tag(std::string_view str) noexcept
		{
			return str == "br";
		}

		constexpr auto is_br_tag(std::string_view str) noexcept
		{
			return str == "br";
		}

		constexpr auto is_font_tag(std::string_view str) noexcept
		{
			return str == "font";
		}


		constexpr auto is_html_attribute(std::string_view str) noexcept
		{
			return str == "color" ||
				   str == "style";
		}	

		constexpr auto is_color_attribute(std::string_view str) noexcept
		{
			return str == "color";
		}

		constexpr auto is_style_attribute(std::string_view str) noexcept
		{
			return str == "style";
		}


		void append_text_section(std::string content, text::TextSections &text_sections, const text::TextSectionStyles &text_section_styles);

		std::optional<std::string_view> get_html_tag(std::string_view str) noexcept;
		std::optional<html_element> parse_html_opening_tag(std::string_view str) noexcept;
		text::TextSectionStyle html_element_to_text_section_style(const html_element &element, text::TextSectionStyle *parent_section) noexcept;

		text::TextSections html_to_text_sections(std::string_view str);
		text::TextLines text_sections_to_text_lines(text::TextSections text_sections);
		std::string text_sections_to_string(const text::TextSections &text_sections);


		/*
			Measuring
		*/

		const font::detail::container_type<font::GlyphExtents>* get_glyph_extents(Font &font);

		inline auto& get_text_section_extents(
			const text::TextSection &section,
			const font::detail::container_type<font::GlyphExtents> &regular_extents,
			const font::detail::container_type<font::GlyphExtents> *bold_extents,
			const font::detail::container_type<font::GlyphExtents> *italic_extents,
			const font::detail::container_type<font::GlyphExtents> *bold_italic_extents) noexcept
		{
			auto extents =
				[&]() noexcept -> decltype(&regular_extents)
				{
					if (section.DefaultFontStyle())
					{
						switch (*section.DefaultFontStyle())
						{
							case text::FontStyle::Bold:
							return bold_extents;

							case text::FontStyle::Italic:
							return italic_extents;

							case text::FontStyle::BoldItalic:
							return bold_italic_extents;
						}
					}

					return nullptr;
				}();

			return extents ? *extents : regular_extents;
		}


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

		std::pair<int,int> text_sections_size_in_pixels(const text::TextSections &text_sections,
			const font::detail::container_type<font::GlyphExtents> &regular_extents,
			const font::detail::container_type<font::GlyphExtents> *bold_extents,
			const font::detail::container_type<font::GlyphExtents> *italic_extents,
			const font::detail::container_type<font::GlyphExtents> *bold_italic_extents) noexcept;


		/*
			Truncating
		*/

		std::string truncate_string(std::string str, int max_width, std::string suffix,
			const font::detail::container_type<font::GlyphExtents> &extents);


		/*
			Word wrapping
		*/

		std::string word_wrap(std::string str, int max_width,
			const font::detail::container_type<font::GlyphExtents> &extents);
		void word_wrap(glyph_rope str, int max_width);
	} //detail


	/*
		Formatting
	*/

	//Returns text sections, by parsing all HTML elements found in the given string
	[[nodiscard]] text::TextSections HTMLToTextSections(std::string_view str);

	//Returns a plain string, by parsing and removing all HTML tags found in the given string
	[[nodiscard]] std::string HTMLToString(std::string_view str);


	//Returns text lines, by splitting up text sections into lines when a '\n' character is found
	[[nodiscard]] text::TextLines SplitTextSections(text::TextSections text_sections);


	/*
		Measuring
	*/

	//Returns the size, in pixels, of the given character when rendered with the given font
	//Returns nullopt if font could not be loaded properly
	[[nodiscard]] std::optional<Vector2> MeasureCharacter(char c, Font &font) noexcept;

	//Returns the size, in pixels, of the given string when rendered with the given font
	//Returns nullopt if font could not be loaded properly
	[[nodiscard]] std::optional<Vector2> MeasureString(std::string_view str, Font &font) noexcept;

	//Returns the size, in pixels, of the given text sections when rendered with the given type face
	//Returns nullopt if type face fonts could not be loaded properly
	[[nodiscard]] std::optional<Vector2> MeasureTextSections(const text::TextSections &text_sections, TypeFace &type_face) noexcept;


	/*
		Truncating
	*/

	//Truncates the given string if wider than max width, in pixels, when rendered with the given font
	//If string has been truncated, then ... is appended to the result
	//Returns nullopt if font could not be loaded properly
	[[nodiscard]] std::optional<std::string> TruncateString(std::string str, int max_width, Font &font);

	//Truncates the given string if wider than max width, in pixels, when rendered with the given font
	//If string has been truncated, then the given suffix is appended to the result
	//Returns nullopt if font could not be loaded properly
	[[nodiscard]] std::optional<std::string> TruncateString(std::string str, int max_width, std::string suffix, Font &font);


	/*
		Word wrapping
	*/

	//Word wraps the given string if wider than max width, in pixels, when rendered with the given font
	//Replaces a ' ' character in between words, with a '\n' character where the line needs to be broken
	//Does only cut words if one word is wider than max width, then a '\n' character is inserted
	//Returns nullopt if font could not be loaded properly
	[[nodiscard]] std::optional<std::string> WordWrap(std::string str, int max_width, Font &font);

	//Word wraps the given text sections if wider than max width, in pixels, when rendered with the given type face
	//Replaces a ' ' character in between words, with a '\n' character where the line needs to be broken
	//Does only cut words if one word is wider than max width, then a '\n' character is inserted
	//Returns nullopt if type face fonts could not be loaded properly
	[[nodiscard]] std::optional<text::TextSections> WordWrap(text::TextSections text_sections, int max_width, TypeFace &type_face);
} //ion::graphics::fonts::utilities

#endif