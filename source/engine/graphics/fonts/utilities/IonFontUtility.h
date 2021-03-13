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
#include "types/IonTypes.h"

#undef max

namespace ion::graphics::fonts::utilities
{
	using graphics::utilities::Color;
	using graphics::utilities::Vector2;
	using namespace types::type_literals;

	namespace detail
	{
		constexpr auto smaller_scale_factor = 5.0_r / 6.0_r; //83.33%
		constexpr auto larger_scale_factor = 1.20_r; //120%

		constexpr auto subscript_translate_factor = 1.0_r / 3.0_r; //33.33%
		constexpr auto superscript_translate_factor = 0.5_r; //50%


		/*
			Glyph rope
		*/

		struct glyph_string final
		{
			std::string &value;
			const font::GlyphMetrices &metrics;
			real scale_factor = 1.0_r;
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
				const glyph_string& glyph_str(size_t off) const noexcept;

				std::string& insert(size_t off, size_t count, char ch);

				bool empty() const noexcept;
				size_t size() const noexcept;
		};

		glyph_rope make_glyph_rope(text::TextBlocks &text_blocks,
			const font::GlyphMetrices &regular_metrics,
			const font::GlyphMetrices *bold_metrics,
			const font::GlyphMetrices *italic_metrics,
			const font::GlyphMetrices *bold_italic_metrics);


		/*
			Formatting
		*/

		struct html_attribute final
		{
			std::string_view name;
			std::string_view value;
		};

		using html_attributes = std::vector<html_attribute>;

		struct html_element final
		{
			std::string_view tag;
			html_attributes attributes;
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
			return str == "br" ||
				   str == "span" ||
				   str == "font" ||

				   //Bold
				   str == "b" ||
				   str == "strong" ||

				   //Italic
				   str == "i" ||
				   str == "em" ||
				   str == "dfn" ||
				   str == "var" ||
				   str == "cite" ||

				   //Underline
				   str == "u" ||
				   str == "ins" ||

				   //Line-through
				   str == "s" ||
				   str == "del" ||
				   str == "strike" ||

				   //Vertical-align
				   str == "sub" ||
				   str == "sup" ||

				   //Font-size
				   str == "small" ||
				   str == "big" ||

				   //Other
				   str == "mark";			   
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


		void append_text_block(std::string content, text::TextBlocks &text_blocks,
			const text::TextBlockStyles &text_block_styles);

		std::optional<std::string_view> get_html_tag(std::string_view str) noexcept;
		html_attributes parse_html_attributes(std::string_view str) noexcept;
		std::optional<html_element> parse_html_element(std::string_view str) noexcept;
		std::optional<html_element> parse_html_opening_tag(std::string_view str) noexcept;
		std::optional<html_element> parse_html_closing_tag(std::string_view str) noexcept;
		
		text::TextBlockStyle html_tag_to_text_block_style(std::string_view tag,
			text::TextBlockStyle *parent_text_block) noexcept;
		text::TextBlockStyle html_attributes_to_text_block_style(const html_attributes &attributes,
			text::TextBlockStyle *parent_text_block) noexcept;
		text::TextBlockStyle html_element_to_text_block_style(const html_element &element,
			text::TextBlockStyle *parent_text_block) noexcept;

		text::TextBlocks html_to_text_blocks(std::string_view str);
		text::TextLines text_blocks_to_text_lines(text::TextBlocks text_blocks);
		std::string text_blocks_to_string(const text::TextBlocks &text_blocks);


		/*
			Measuring
		*/

		const font::GlyphMetrices* get_glyph_metrics(Font &font);

		inline auto& get_text_block_metrics(
			const text::TextBlock &text_block,
			const font::GlyphMetrices &regular_metrics,
			const font::GlyphMetrices *bold_metrics,
			const font::GlyphMetrices *italic_metrics,
			const font::GlyphMetrices *bold_italic_metrics) noexcept
		{
			auto metrics =
				[&]() noexcept -> const font::GlyphMetrices*
				{
					if (text_block.FontStyle)
					{
						switch (*text_block.FontStyle)
						{
							case text::TextFontStyle::Bold:
							return bold_metrics;

							case text::TextFontStyle::Italic:
							return italic_metrics;

							case text::TextFontStyle::BoldItalic:
							return bold_italic_metrics;
						}
					}

					return nullptr;
				}();

			return metrics ? *metrics : regular_metrics;
		}

		inline auto get_text_block_scale_factor(const text::TextBlock &text_block)
		{
			if (text_block.FontSize)
			{
				switch (*text_block.FontSize)
				{
					case text::TextBlockFontSize::Smaller:
					return smaller_scale_factor;

					case text::TextBlockFontSize::Larger:
					return larger_scale_factor;
				}
			}
			
			return 1.0_r;
		}


		inline auto character_size_in_pixels(char c,
			const font::GlyphMetrices &metrics) noexcept
		{
			auto glyph_index = static_cast<unsigned char>(c);

			if (glyph_index < std::size(metrics))
				return std::pair{metrics[glyph_index].Advance, metrics[glyph_index].Height};
			else
				return std::pair{0, 0};
		}

		inline auto string_size_in_pixels(std::string_view str,
			const font::GlyphMetrices &metrics) noexcept
		{
			auto width = 0;
			auto height = 0;

			for (auto c : str)
			{
				auto [c_width, c_height] = character_size_in_pixels(c, metrics);
				width += c_width;
				height = std::max(height, c_height);
			}

			return std::pair{width, height};
		}

		std::pair<int,int> text_blocks_size_in_pixels(const text::TextBlocks &text_blocks,
			const font::GlyphMetrices &regular_metrics,
			const font::GlyphMetrices *bold_metrics,
			const font::GlyphMetrices *italic_metrics,
			const font::GlyphMetrices *bold_italic_metrics) noexcept;


		/*
			Truncating
		*/

		std::string truncate_string(std::string str, int max_width, std::string suffix,
			const font::GlyphMetrices &metrics);


		/*
			Word wrapping
		*/

		std::string word_wrap(std::string str, int max_width,
			const font::GlyphMetrices &metrics);
		void word_wrap(glyph_rope str, int max_width);
	} //detail


	/*
		Formatting
	*/

	//Returns text blocks, by parsing all HTML elements found in the given string
	[[nodiscard]] text::TextBlocks HTMLToTextBlocks(std::string_view str);

	//Returns a plain string, by parsing and removing all HTML tags found in the given string
	[[nodiscard]] std::string HTMLToString(std::string_view str);


	//Returns text lines, by splitting up text blocks into lines when a '\n' character is found
	[[nodiscard]] text::TextLines SplitTextBlocks(text::TextBlocks text_blocks);


	/*
		Measuring
	*/

	//Returns the size, in pixels, of the given character when rendered with the given font
	//Returns nullopt if font could not be loaded properly
	[[nodiscard]] std::optional<Vector2> MeasureCharacter(char c, Font &font) noexcept;

	//Returns the size, in pixels, of the given string when rendered with the given font
	//Returns nullopt if font could not be loaded properly
	[[nodiscard]] std::optional<Vector2> MeasureString(std::string_view str, Font &font) noexcept;

	//Returns the size, in pixels, of the given text blocks when rendered with the given type face
	//Returns nullopt if type face fonts could not be loaded properly
	[[nodiscard]] std::optional<Vector2> MeasureTextBlocks(const text::TextBlocks &text_blocks, TypeFace &type_face) noexcept;


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

	//Word wraps the given text blocks if wider than max width, in pixels, when rendered with the given type face
	//Replaces a ' ' character in between words, with a '\n' character where the line needs to be broken
	//Does only cut words if one word is wider than max width, then a '\n' character is inserted
	//Returns nullopt if type face fonts could not be loaded properly
	[[nodiscard]] std::optional<text::TextBlocks> WordWrap(text::TextBlocks text_blocks, int max_width, TypeFace &type_face);
} //ion::graphics::fonts::utilities

#endif