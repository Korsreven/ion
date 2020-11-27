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

#include <algorithm>
#include <functional>

#include "graphics/fonts/IonFontManager.h"
#include "graphics/utilities/IonColor.h"
#include "script/utilities/IonParseUtility.h"
#include "types/IonTypes.h"
#include "utilities/IonStringUtility.h"

namespace ion::graphics::fonts::utilities
{

using namespace graphics::utilities;

namespace detail
{

/*
	Glyph rope
*/

std::pair<size_t, size_t> glyph_rope::get_offsets(size_t off) const noexcept
{
	//One string in rope (optimization)
	if (std::size(strings_) == 1)
		return {0, off};

	auto &[str_off, total_size] = range_;

	//Above or inside range
	if (off >= total_size)
	{
		for (; str_off < std::size(strings_); ++str_off)
		{
			//Offset is inside string
			if (off - total_size < std::size(strings_[str_off].value))
				break;
			else
				total_size += std::size(strings_[str_off].value);
		}
	}

	//Below range
	else
	{
		while (str_off > 0)
		{
			total_size -= std::size(strings_[--str_off].value);

			//Offset is inside string
			if (off >= total_size)
				break;
		}
	}

	return {str_off, off - total_size};
}

glyph_rope::glyph_rope(glyph_string str)  :
	strings_{std::move(str)}
{
	//Empty
}

glyph_rope::glyph_rope(glyph_strings strings) :
	strings_{std::move(strings)}
{
	//Empty
}


char& glyph_rope::operator[](size_t off) noexcept
{
	auto [str_off, ch_off] = get_offsets(off);
	auto &str = strings_[str_off].value;
	return str[ch_off];
}

const char& glyph_rope::operator[](size_t off) const noexcept
{
	auto [str_off, ch_off] = get_offsets(off);
	auto &str = strings_[str_off].value;
	return str[ch_off];
}

const font::GlyphMetrices& glyph_rope::glyph_metrics(size_t off) const noexcept
{
	auto [str_off, ch_off] = get_offsets(off);
	return strings_[str_off].metrics;
}

std::string& glyph_rope::insert(size_t off, size_t count, char ch)
{
	size_ = 0;
	range_ = {};

	auto [str_off, ch_off] = get_offsets(off);
	auto &str = strings_[str_off].value;
	return str.insert(ch_off, count, ch);
}

bool glyph_rope::empty() const noexcept
{
	return size() == 0;
}

size_t glyph_rope::size() const noexcept
{
	if (size_ == 0)
	{
		for (auto &str : strings_)
			size_ += std::size(str.value);
	}

	return size_;
}

glyph_rope make_glyph_rope(text::TextBlocks &text_blocks,
	const font::GlyphMetrices &regular_metrics,
	const font::GlyphMetrices *bold_metrics,
	const font::GlyphMetrices *italic_metrics,
	const font::GlyphMetrices *bold_italic_metrics)
{
	glyph_strings strings;

	for (auto &text_block : text_blocks)
	{
		auto &metrics = get_text_block_metrics(text_block, regular_metrics, bold_metrics, italic_metrics, bold_italic_metrics);
		strings.push_back({text_block.Content, metrics});
	}

	return strings;
}


/*
	Formatting
*/

void append_front_text_block(std::string content, text::TextBlocks &text_blocks,
	const text::TextBlockStyles &text_block_styles)
{
	//Combine with first text block if equal styles
	if (!std::empty(text_blocks) &&
		((!std::empty(text_block_styles) && text_blocks.front() == text_block_styles.back()) ||
		(std::empty(text_block_styles) && text_blocks.front().IsPlain())))

		text_blocks.front().Content += content;
	
	//Add new text_block
	else
	{
		if (!std::empty(text_block_styles))
			text_blocks.insert(std::begin(text_blocks), {text_block_styles.back(), std::move(content)});
		else
			text_blocks.insert(std::begin(text_blocks), {{}, std::move(content)});
	}
}

void append_back_text_block(std::string content, text::TextBlocks &text_blocks,
	const text::TextBlockStyles &text_block_styles)
{
	//Combine with last text block if equal styles
	if (!std::empty(text_blocks) &&
		((!std::empty(text_block_styles) && text_blocks.back() == text_block_styles.back()) ||
		(std::empty(text_block_styles) && text_blocks.back().IsPlain())))

		text_blocks.back().Content += content;
	
	//Add new text_block
	else
	{
		if (!std::empty(text_block_styles))
			text_blocks.push_back({text_block_styles.back(), std::move(content)});
		else
			text_blocks.push_back({{}, std::move(content)});
	}
}


std::optional<std::string_view> get_html_tag(std::string_view str) noexcept
{
	if (auto iter = std::find_if(std::begin(str), std::end(str), is_end_of_tag);
		iter != std::end(str))
	{
		str.remove_suffix(std::size(str) - (iter - std::begin(str)));
		return str;
	}
	else
		return {};
}

std::optional<html_element> parse_html_opening_tag(std::string_view str) noexcept
{
	auto element =
		[&]() noexcept
		{
			//Check if opening tag has attribute
			if (auto off = str.find(' ', 1);
				off != std::string_view::npos)
			{
				//Attribute name and value found
				if (auto off2 = str.find("=\"", off + 1);
					off2 != std::string_view::npos && str.back() == '\"')
				{
					return html_element{
						str.substr(0, off),
						html_attribute{str.substr(off + 1, off2 - (off + 1)), str.substr(off2 + 1)}
					};
				}
			}

			return html_element{str};
		}();

	auto valid = is_html_tag(element.tag);

	//Has attribute
	if (element.attribute)
	{
		valid &= !is_empty_tag(element.attribute->name) &&
				  is_html_attribute(element.attribute->name);

		//Is attribute supported
		if (is_style_attribute(element.attribute->name))
			valid &= !is_font_tag(element.tag);
		else if (is_color_attribute(element.attribute->name))
			valid &= is_font_tag(element.tag);
	}

	if (valid)
		return element;
	else
		return {};
}

text::TextBlockStyle html_element_to_text_block_style(const html_element &element,
	text::TextBlockStyle *parent_text_block) noexcept
{
	auto text_block = parent_text_block ?
		*parent_text_block : //Inherit from parent
		text::TextBlockStyle{}; //Plain

	//Tags

	//Background color
	//Mark
	if (element.tag == "mark")
		text_block.BackgroundColor = color::Yellow;

	//Font style
	//Bold
	else if (element.tag == "b" || element.tag == "strong")
		text_block.FontStyle =
			[&font_style = text_block.FontStyle]() noexcept
			{
				if (font_style &&
					(*font_style == text::TextFontStyle::Italic ||
					 *font_style == text::TextFontStyle::BoldItalic))
					return text::TextFontStyle::BoldItalic;
				else
					return text::TextFontStyle::Bold;
			}();

	//Font style
	//Italic
	else if (element.tag == "i" || element.tag == "em")
		text_block.FontStyle =
			[&font_style = text_block.FontStyle]() noexcept
			{
				if (font_style &&
					(*font_style == text::TextFontStyle::Bold ||
					 *font_style == text::TextFontStyle::BoldItalic))
					return text::TextFontStyle::BoldItalic;
				else
					return text::TextFontStyle::Italic;
			}();

	//Decoration
	//Underline
	else if (element.tag == "u" || element.tag == "ins")
		text_block.Decoration = text::TextDecoration::Underline;

	//Decoration
	//Line-through
	else if (element.tag == "del")
		text_block.Decoration = text::TextDecoration::LineThrough;

	//Vertical alignment
	//Subscript
	else if (element.tag == "sub")
		text_block.VerticalAlignment = text::TextBlockVerticalAlignment::Subscript;

	//Vertical alignment
	//Superscript
	else if (element.tag == "sup")
		text_block.VerticalAlignment = text::TextBlockVerticalAlignment::Superscript;


	//Attributes
	if (element.attribute)
	{
		//Parse attribute value (as string literal)
		if (auto value =
			script::utilities::parse::AsString(element.attribute->value); value)
		{
			//Foreground color
			if (is_color_attribute(element.attribute->name))
			{
				if (auto color = script::utilities::parse::AsColor(*value); color)
					text_block.ForegroundColor = *color;
			}

			//Style
			else if (is_style_attribute(element.attribute->name))
			{
				//TODO
			}
		}
	}

	return text_block;
}


text::TextBlocks html_to_text_blocks(std::string_view str)
{
	html_elements elements;
	text::TextBlockStyles text_block_styles;
	text::TextBlocks text_blocks;
	std::string content;

	for (auto iter = std::begin(str), end = std::end(str); iter != end; ++iter)
	{
		auto c = *iter;
		auto next_c = iter + 1 != end ? *(iter + 1) : '\0';
		auto off = iter - std::begin(str);

		
		//Closing tag, must be checked before opening tag
		if (is_start_of_closing_tag(c, next_c))
		{
			//Something to close
			if (!std::empty(elements))
			{
				//Tag matches opening tag
				if (auto tag = get_html_tag(str.substr(off + 2));
					tag && *tag == elements.back().tag) 
				{
					if (!std::empty(content))
						append_back_text_block(std::move(content), text_blocks, text_block_styles);

					elements.pop_back();
					text_block_styles.pop_back();

					iter += std::size(*tag) + 2;
					continue;
				}
			}
		}
		//Opening tag
		else if (is_start_of_opening_tag(c))
		{
			if (auto tag = get_html_tag(str.substr(off + 1)); tag) 
			{
				//Opening tag has been parsed and validated
				if (auto element = parse_html_opening_tag(*tag); element)
				{
					if (is_empty_tag(element->tag))
					{
						if (is_br_tag(element->tag))
							content += '\n';
					}
					else
					{
						if (!std::empty(content))
							append_back_text_block(std::move(content), text_blocks, text_block_styles);

						elements.push_back(std::move(*element));
						text_block_styles.push_back(
							html_element_to_text_block_style(
								elements.back(),
								!std::empty(text_block_styles) ?
								&text_block_styles.back() : nullptr
							));
					}

					iter += std::size(*tag) + 1;
					continue;
				}
			}
		}

		content += c;
	}

	if (!std::empty(content))
		append_back_text_block(std::move(content), text_blocks, text_block_styles);

	return text_blocks;
}

text::TextLines text_blocks_to_text_lines(text::TextBlocks text_blocks)
{
	text::TextLines lines;
	text::TextBlocks line_text_blocks;

	for (auto &text_block : text_blocks)
	{
		//Content contains one or more new lines characters
		//Split text block into multiple copies
		if (text_block.Content.find('\n') != std::string::npos)
		{
			auto parts = ion::utilities::string::Split(text_block.Content, "\n",
				ion::utilities::string::StringSplitOptions::PreserveEmptyEntries);

			text_block.Content.clear();
				//Clear content before duplicating text text_block
			
			for (auto i = 0; auto &part : parts)
			{
				if (!std::empty(part))
				{
					line_text_blocks.push_back(text_block);
					line_text_blocks.back().Content = std::move(part);
				}
				
				if (++i < std::ssize(parts))
					lines.push_back({std::move(line_text_blocks)});
			}
		}
		else
			line_text_blocks.push_back(std::move(text_block));
	}

	if (!std::empty(line_text_blocks))
		lines.push_back({std::move(line_text_blocks)});

	return lines;
}

std::string text_blocks_to_string(const text::TextBlocks &text_blocks)
{
	std::string str;

	for (auto &text_block : text_blocks)
		str += text_block.Content;

	return str;
}


/*
	Measuring
*/

const font::GlyphMetrices* get_glyph_metrics(Font &font)
{
	if (font.IsLoaded() || (font.Owner() && font.Owner()->Load(font)))
	{
		if (auto &metrics = font.GlyphMetrics(); metrics)
			return &*metrics;
	}

	return nullptr;
}

std::pair<int,int> text_blocks_size_in_pixels(const text::TextBlocks &text_blocks,
	const font::GlyphMetrices &regular_metrics,
	const font::GlyphMetrices *bold_metrics,
	const font::GlyphMetrices *italic_metrics,
	const font::GlyphMetrices *bold_italic_metrics) noexcept
{
	auto width = 0;
	auto height = 0;

	for (auto &text_block : text_blocks)
	{
		auto &metrics = get_text_block_metrics(text_block, regular_metrics, bold_metrics, italic_metrics, bold_italic_metrics);
		auto [str_width, str_height] = string_size_in_pixels(text_block.Content, metrics);
		width += str_width;
		height = std::max(height, str_height);
	}

	return {width, height};
}


/*
	Truncating
*/

std::string truncate_string(std::string str, int max_width, std::string suffix,
	const font::GlyphMetrices &metrics)
{
	//Truncate
	if (auto [width, height] = detail::string_size_in_pixels(str, metrics);
		width > max_width)
	{
		auto [suffix_width, suffix_height] = detail::string_size_in_pixels(suffix, metrics);

		if (suffix_width > max_width)
			return "";
		else if (suffix_width == max_width)
			return suffix;

		width += suffix_width;

		while (!std::empty(str))
		{
			auto [c_width, c_height] = character_size_in_pixels(str.back(), metrics);
			str.pop_back();

			if ((width -= c_width) <= max_width)
				break;
		}

		str += std::move(suffix);
	}

	return str;
}


/*
	Word wrapping
*/

std::string word_wrap(std::string str, int max_width,
	const font::GlyphMetrices &metrics)
{
	word_wrap(glyph_string{str, metrics}, max_width);
	return str;
}

void word_wrap(glyph_rope str, int max_width)
{
	auto width = 0;
	auto line_break_off = std::optional<int>{};

	for (auto i = 0; i < std::ssize(str); ++i)
	{
		switch (str[i])
		{
			//New line found
			case '\n':
			break;

			//Space found
			case ' ':
			line_break_off = i;
			[[fallthrough]];

			default:
			{
				auto [c_width, c_height] = character_size_in_pixels(str[i], str.glyph_metrics(i));

				//Insert new line
				if (width > 0 && //At least one character
					width + c_width > max_width) //Too wide
				{
					//Break at last line break candidate
					if (line_break_off)
					{
						switch (str[*line_break_off])
						{
							//Space
							case ' ':
							str[(i = *line_break_off)] = '\n'; //Replace space with new line
							break;

							//Hyphen
							case '-':
							str.insert((i = *line_break_off + 1), 1, '\n'); //Insert new line after hyphen
							break;
						}
					}

					//No space found, cut word
					else
						str.insert(i, 1, '\n');
				}
				else
				{
					//Hyphen found (that fits)
					if (width > 0 && str[i] == '-')
						line_break_off = i;

					width += c_width;
					continue;
				}
				
				break;
			}
		}

		width = 0;
		line_break_off = {};
	}
}

} //detail


/*
	Formatting
*/

text::TextBlocks HTMLToTextBlocks(std::string_view str)
{
	return detail::html_to_text_blocks(str);
}

std::string HTMLToString(std::string_view str)
{
	return detail::text_blocks_to_string(detail::html_to_text_blocks(str));
}


text::TextLines SplitTextBlocks(text::TextBlocks text_blocks)
{
	return detail::text_blocks_to_text_lines(std::move(text_blocks));
}


/*
	Measuring
*/

std::optional<Vector2> MeasureCharacter(char c, Font &font) noexcept
{
	if (auto metrics = detail::get_glyph_metrics(font); metrics)
	{
		auto [width, height] = detail::character_size_in_pixels(c, *metrics);
		return Vector2{static_cast<real>(width), static_cast<real>(height)};
	}
	else
		return {};
}

std::optional<Vector2> MeasureString(std::string_view str, Font &font) noexcept
{
	if (auto metrics = detail::get_glyph_metrics(font); metrics)
	{
		auto [width, height] = detail::string_size_in_pixels(str, *metrics);
		return Vector2{static_cast<real>(width), static_cast<real>(height)};
	}
	else
		return {};
}

std::optional<Vector2> MeasureTextBlocks(const text::TextBlocks &text_blocks, TypeFace &type_face) noexcept
{
	if (!type_face.HasRegularFont())
		return {};

	if (auto metrics = detail::get_glyph_metrics(*type_face.RegularFont()); metrics)
	{
		auto bold_metrics = type_face.BoldFont() ?
			detail::get_glyph_metrics(*type_face.BoldFont()) : nullptr;
		auto italic_metrics = type_face.ItalicFont() ?
			detail::get_glyph_metrics(*type_face.ItalicFont()) : nullptr;
		auto bold_italic_metrics = type_face.BoldItalicFont() ?
			detail::get_glyph_metrics(*type_face.BoldItalicFont()) : nullptr;

		auto [width, height] = detail::text_blocks_size_in_pixels(
			text_blocks, *metrics, bold_metrics, italic_metrics, bold_italic_metrics);
		return Vector2{static_cast<real>(width), static_cast<real>(height)};
	}
	else
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
	if (auto metrics = detail::get_glyph_metrics(font); metrics)
		return detail::truncate_string(std::move(str), max_width, std::move(suffix), *metrics);
	else
		return {};
}


/*
	Word wrapping
*/

std::optional<std::string> WordWrap(std::string str, int max_width, Font &font)
{
	if (auto metrics = detail::get_glyph_metrics(font); metrics)
		return detail::word_wrap(std::move(str), max_width, *metrics);
	else
		return {};
}

std::optional<text::TextBlocks> WordWrap(text::TextBlocks text_blocks, int max_width, TypeFace &type_face)
{
	if (!type_face.HasRegularFont())
		return {};

	if (auto metrics = detail::get_glyph_metrics(*type_face.RegularFont()); metrics)
	{
		auto bold_metrics = type_face.BoldFont() ?
			detail::get_glyph_metrics(*type_face.BoldFont()) : nullptr;
		auto italic_metrics = type_face.ItalicFont() ?
			detail::get_glyph_metrics(*type_face.ItalicFont()) : nullptr;
		auto bold_italic_metrics = type_face.BoldItalicFont() ?
			detail::get_glyph_metrics(*type_face.BoldItalicFont()) : nullptr;

		detail::word_wrap(
			detail::make_glyph_rope(text_blocks,
				*metrics, bold_metrics, italic_metrics, bold_italic_metrics),
			max_width);
		return text_blocks;
	}
	else
		return {};
}

} //ion::graphics::fonts::utilities