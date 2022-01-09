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
#include <cmath>
#include <functional>

#include "graphics/fonts/IonFontManager.h"
#include "graphics/utilities/IonColor.h"
#include "script/IonScriptCompiler.h"
#include "script/utilities/IonParseUtility.h"
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

const glyph_string& glyph_rope::glyph_str(size_t off) const noexcept
{
	auto [str_off, ch_off] = get_offsets(off);
	return strings_[str_off];
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
		strings.push_back({text_block.Content, metrics, get_text_block_scale_factor(text_block)});
	}

	return strings;
}


/*
	Formatting
*/

void append_text_block(std::string content, text::TextBlocks &text_blocks,
	const text::TextBlockStyles &text_block_styles)
{
	//Combine with last text block if equal styles
	if (!std::empty(text_blocks) && !text_blocks.back().HardBreak &&
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

html_attributes parse_html_attributes(std::string_view str) noexcept
{
	html_attributes attributes;
	auto has_style_attribute = false;

	for (size_t off = 0;
		(off = str.find_first_not_of(' ', off)) != std::string_view::npos;)
	{
		if (script::script_compiler::detail::is_start_of_identifier(
			str[off], off + 1 < std::size(str) ? str[off + 1] : '\0'))
		{
			auto name =
				script::script_compiler::detail::get_identifer_lexeme(str.substr(off));

			if (off = str.find_first_not_of(' ', str.find_first_of("= ", off + std::size(name)));
				off != std::string_view::npos && str[off] == '=')
			{
				if (off = str.find_first_not_of(' ', off + 1);
					off != std::string_view::npos &&
					script::script_compiler::detail::is_start_of_string_literal(str[off]))
				{
					auto [value, line_breaks] =
						script::script_compiler::detail::get_string_literal_lexeme(str.substr(off));

					//Add only unique attributes (discard duplicates) while keeping original order
					//There is only a few attributes per tag (usually one or two)
					//Because N is small, find_if is one of the simplest and fastest solutions
					if (std::find_if(std::begin(attributes), std::end(attributes),
						[&](auto &attribute) noexcept
						{
							return attribute.name == name;
						}) == std::end(attributes)) //Unique
					{
						//Keep style attribute at the end (special case)
						if (has_style_attribute)
							attributes.emplace(std::end(attributes) - 1, name, value);
						else
						{
							attributes.emplace_back(name, value);
							has_style_attribute = is_style_attribute(name);
						}
					}

					off += std::size(value);
				}
			}
		}
	}
	
	return attributes;
}

std::optional<html_element> parse_html_element(std::string_view str) noexcept
{
	if (!std::empty(str) &&
		script::script_compiler::detail::is_start_of_identifier(
			str[0], std::size(str) > 1 ? str[1] : '\0'))
	{
		auto name = script::script_compiler::detail::get_identifer_lexeme(str);
		auto attributes = parse_html_attributes(str.substr(std::size(name)));
		return html_element{name, std::move(attributes)};
	}
	else
		return {};
}

std::optional<html_element> parse_html_opening_tag(std::string_view str) noexcept
{
	if (auto element = parse_html_element(str); element)
	{
		auto valid = is_html_tag(element->tag);

		//Check validity of each attribute
		for (auto &attribute : element->attributes)
		{
			valid &= !is_empty_tag(attribute.name) &&
					  is_html_attribute(attribute.name);

			//Is attribute supported
			if (is_color_attribute(attribute.name))
				valid &= is_font_tag(element->tag);
		}

		if (valid)
			return element;
	}
	
	return {};
}

std::optional<html_element> parse_html_closing_tag(std::string_view str) noexcept
{
	str.remove_suffix(std::size(str) - str.find_last_not_of(' ') - 1);

	if (!std::empty(str))
		return html_element{str};
	else
		return {};
}


text::TextBlockStyle html_tag_to_text_block_style(std::string_view tag,
	text::TextBlockStyle *parent_text_block) noexcept
{
	auto text_block = parent_text_block ?
		*parent_text_block : //Inherit from parent
		text::TextBlockStyle{}; //Plain

	//Bold
	 if (tag == "b" || tag == "strong")
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

	//Italic
	else if (tag == "i" || tag == "em" || tag == "dfn" || tag == "var" || tag == "cite")
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

	//Underline
	else if (tag == "u" || tag == "ins")
		text_block.Decoration = text::TextDecoration::Underline;

	//Line-through
	else if (tag == "s" || tag == "del" || tag == "strike")
		text_block.Decoration = text::TextDecoration::LineThrough;

	//Subscript
	else if (tag == "sub")
	{
		text_block.VerticalAlign = text::TextBlockVerticalAlign::Subscript;
		text_block.FontSize = text::TextBlockFontSize::Smaller;
	}

	//Superscript
	else if (tag == "sup")
	{
		text_block.VerticalAlign = text::TextBlockVerticalAlign::Superscript;
		text_block.FontSize = text::TextBlockFontSize::Smaller;
	}

	//Small
	else if (tag == "small")
	{
		if (text_block.FontSize &&
			*text_block.FontSize == text::TextBlockFontSize::Larger)
			text_block.FontSize = {};
		else
			text_block.FontSize = text::TextBlockFontSize::Smaller;
	}

	//Big
	else if (tag == "big")
	{
		if (text_block.FontSize &&
			*text_block.FontSize == text::TextBlockFontSize::Smaller)
			text_block.FontSize = {};
		else
			text_block.FontSize = text::TextBlockFontSize::Larger;
	}

	//Mark
	else if (tag == "mark")
	{
		text_block.ForegroundColor = color::Black;
		text_block.BackgroundColor = color::Yellow;
	}

	return text_block;
}

text::TextBlockStyle html_attributes_to_text_block_style(const html_attributes &attributes,
	text::TextBlockStyle *parent_text_block) noexcept
{
	auto text_block = parent_text_block ?
		*parent_text_block : //Inherit from parent
		text::TextBlockStyle{}; //Plain

	//For each attribute
	for (auto &attribute : attributes)
	{
		//Parse attribute value (as string literal)
		if (auto value =
			script::utilities::parse::AsString(attribute.value); value)
		{
			//Color
			if (is_color_attribute(attribute.name))
			{
				if (auto color = script::utilities::parse::AsColor(*value); color)
					text_block.ForegroundColor = *color;
			}

			//Style
			else if (is_style_attribute(attribute.name))
			{
				script::ScriptCompiler compiler;
				script::CompileError error;

				if (auto tree = compiler.CompileString("style{" + *value + "}", error);
					!error && tree)
				{
					//For each CSS property
					for (auto &object = tree->Find("style");
						auto &property : object.Properties())
					{
						//Foreground color
						if (property.Name() == "color")
						{
							if (auto color = property[0].Get<script::ScriptType::Color>(); color)
								text_block.ForegroundColor = color->Get();
						}

						//Background color
						else if (property.Name() == "background-color")
						{
							if (auto color = property[0].Get<script::ScriptType::Color>(); color)
								text_block.BackgroundColor = color->Get();
						}

						//Font weight
						else if (property.Name() == "font-weight")
						{
							auto weight =
								property[0].Get<script::ScriptType::Enumerable>().value_or(
									[&]()
									{
										//100-900
										if (auto weight_value = property[0].Get<script::ScriptType::Integer>();
											weight_value &&
											weight_value->Get() >= 1 && weight_value->Get() <= 1000)
										{
											if (weight_value->Get() < 550) //Cutoff
												return script::ScriptType::Enumerable{"normal"};
											else
												return script::ScriptType::Enumerable{"bold"};
										}
										else
											return script::ScriptType::Enumerable{""};
									}());

							if (weight.Get() == "normal")
								text_block.FontStyle =
									[&font_style = text_block.FontStyle]() noexcept
										-> std::optional<text::TextFontStyle>
									{
										if (font_style && *font_style == text::TextFontStyle::Bold)
											return {};
										else if (font_style && *font_style == text::TextFontStyle::BoldItalic)
											return text::TextFontStyle::Italic;
										else
											return font_style;
									}();
							else if (weight.Get() == "bold")
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
						}

						//Font style
						else if (property.Name() == "font-style")
						{
							if (auto style = property[0].Get<script::ScriptType::Enumerable>(); style)
							{
								if (style->Get() == "normal")
									text_block.FontStyle =
										[&font_style = text_block.FontStyle]() noexcept
											-> std::optional<text::TextFontStyle>
										{
											if (font_style && *font_style == text::TextFontStyle::Italic)
												return {};
											else if (font_style && *font_style == text::TextFontStyle::BoldItalic)
												return text::TextFontStyle::Bold;
											else
												return font_style;
										}();
								else if (style->Get() == "italic")
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
							}
						}

						//Text decoration
						else if (property.Name() == "text-decoration" || property.Name() == "text-decoration-line")
						{
							if (auto decoration = property[0].Get<script::ScriptType::Enumerable>(); decoration)
							{
								if (decoration->Get() == "none")
									text_block.Decoration = {};
								else if (decoration->Get() == "overline")
									text_block.Decoration = text::TextDecoration::Overline;
								else if (decoration->Get() == "line-through")
									text_block.Decoration = text::TextDecoration::LineThrough;
								else if (decoration->Get() == "underline")
									text_block.Decoration = text::TextDecoration::Underline;
							}
						}

						//Text decoration color
						else if (property.Name() == "text-decoration-color")
						{
							if (auto color = property[0].Get<script::ScriptType::Color>(); color)
								text_block.DecorationColor = color->Get();
						}

						//Vertical align
						else if (property.Name() == "vertical-align")
						{
							if (auto align = property[0].Get<script::ScriptType::Enumerable>(); align)
							{
								if (align->Get() == "baseline")
									text_block.VerticalAlign = {};
								else if (align->Get() == "sub")
								{
									text_block.VerticalAlign = text::TextBlockVerticalAlign::Subscript;
									text_block.FontSize = text::TextBlockFontSize::Smaller;
								}
								else if (align->Get() == "super")
								{
									text_block.VerticalAlign = text::TextBlockVerticalAlign::Superscript;
									text_block.FontSize = text::TextBlockFontSize::Smaller;
								}
							}
						}

						//Font-size
						else if (property.Name() == "font-size")
						{
							if (auto size = property[0].Get<script::ScriptType::Enumerable>(); size)
							{
								if (size->Get() == "medium")
									text_block.FontSize = {};
								else if (size->Get() == "smaller")
								{
									if (text_block.FontSize &&
										*text_block.FontSize == text::TextBlockFontSize::Larger)
										text_block.FontSize = {};
									else
										text_block.FontSize = text::TextBlockFontSize::Smaller;
								}
								else if (size->Get() == "larger")
								{
									if (text_block.FontSize &&
										*text_block.FontSize == text::TextBlockFontSize::Smaller)
										text_block.FontSize = {};
									else
										text_block.FontSize = text::TextBlockFontSize::Larger;
								}
							}
						}
					}
				}
			}
		}
	}

	return text_block;
}

text::TextBlockStyle html_element_to_text_block_style(const html_element &element,
	text::TextBlockStyle *parent_text_block) noexcept
{
	//Tag
	auto text_block = html_tag_to_text_block_style(element.tag, parent_text_block);

	//Attributes
	if (!std::empty(element.attributes))
		return html_attributes_to_text_block_style(element.attributes, &text_block);

	else
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
			if (auto tag = get_html_tag(str.substr(off + 2));
				tag && !std::empty(elements))
			{
				//Closing tag has been parsed and validated
				if (auto element = parse_html_closing_tag(*tag);
					element && element->tag == elements.back().tag)
						//Tag matches opening tag
				{
					iter += std::size(*tag) + 2;

					if (!std::empty(content))
						append_text_block(std::move(content), text_blocks, text_block_styles);

					elements.pop_back();
					text_block_styles.pop_back();
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
					iter += std::size(*tag) + 1;

					if (is_empty_tag(element->tag))
					{
						if (is_br_tag(element->tag))
							c = '\n';
					}
					else
					{
						if (!std::empty(content))
							append_text_block(std::move(content), text_blocks, text_block_styles);

						elements.push_back(std::move(*element));
						text_block_styles.push_back(
							html_element_to_text_block_style(
								elements.back(),
								!std::empty(text_block_styles) ?
								&text_block_styles.back() : nullptr
							));

						continue;
					}
				}
			}
		}

		//Split block if new line
		if (c == '\n')
		{
			if (!std::empty(content))
				append_text_block(std::move(content), text_blocks, text_block_styles);

			text_blocks.push_back({{}, "\n", true});
		}
		else
			content += c;
	}

	if (!std::empty(content))
		append_text_block(std::move(content), text_blocks, text_block_styles);

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
					lines.push_back({std::move(line_text_blocks), text_block.HardBreak});
			}
		}
		else
			line_text_blocks.push_back(std::move(text_block));
	}

	if (!std::empty(line_text_blocks))
		lines.push_back({std::move(line_text_blocks), true});

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

std::pair<int,int> text_block_size_in_pixels(const text::TextBlock &text_block,
	const font::GlyphMetrices &regular_metrics,
	const font::GlyphMetrices *bold_metrics,
	const font::GlyphMetrices *italic_metrics,
	const font::GlyphMetrices *bold_italic_metrics) noexcept
{
	auto &metrics = get_text_block_metrics(text_block, regular_metrics, bold_metrics, italic_metrics, bold_italic_metrics);
	auto [string_width, string_height] = string_size_in_pixels(text_block.Content, metrics);

	auto scale_factor = get_text_block_scale_factor(text_block);
	auto width = string_width * scale_factor;
	auto height = string_height * scale_factor;

	return {static_cast<int>(std::ceil(width)), static_cast<int>(std::ceil(height))};
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
		auto [block_width, block_height] =
			text_block_size_in_pixels(text_block, regular_metrics, bold_metrics, italic_metrics, bold_italic_metrics);

		width += block_width;
		height = std::max(height, block_height);
	}

	return {width, height};
}


/*
	Truncating
*/

std::string truncate_string(std::string str, int max_width, std::string_view suffix,
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
			return std::string{suffix};

		width += suffix_width;

		while (!std::empty(str))
		{
			auto [c_width, c_height] = character_size_in_pixels(str.back(), metrics);
			str.pop_back();

			if ((width -= c_width) <= max_width)
				break;
		}

		str += suffix;
	}

	return str;
}

text::TextBlocks truncate_text_blocks(text::TextBlocks text_blocks, int max_width, std::string_view suffix,
	const font::GlyphMetrices &regular_metrics,
	const font::GlyphMetrices *bold_metrics,
	const font::GlyphMetrices *italic_metrics,
	const font::GlyphMetrices *bold_italic_metrics)
{
	auto width = 0;

	for (auto iter = std::begin(text_blocks); iter != std::end(text_blocks);)
	{
		//Hard break, reset width
		if (iter->HardBreak)
		{
			width = 0;
			++iter;
			continue;
		}

		//Measure text block
		auto [block_width, block_height] =
			text_block_size_in_pixels(*iter, regular_metrics, bold_metrics, italic_metrics, bold_italic_metrics);

		if (width + block_width >= max_width)
		{
			//Remove content from last block
			if (width + block_width > max_width)
			{
				auto &metrics = get_text_block_metrics(*iter, regular_metrics, bold_metrics, italic_metrics, bold_italic_metrics);
				iter->Content = truncate_string(std::move(iter->Content), max_width - width, suffix, metrics);
			}
			
			//Search for hard break
			auto hard_iter =
				std::find_if(iter, std::end(text_blocks), [](auto &text_block) { return text_block.HardBreak; });
			iter = text_blocks.erase(iter + !std::empty(iter->Content), hard_iter);
		}
		else
		{
			width += block_width;
			++iter;
		}
	}

	return text_blocks;
}


/*
	Wrapping
*/

std::string wrap(std::string str, int max_width,
	const font::GlyphMetrices &metrics)
{
	wrap(glyph_string{str, metrics}, max_width);
	return str;
}

text::TextBlocks wrap(text::TextBlocks text_blocks, int max_width,
	const font::GlyphMetrices &regular_metrics,
	const font::GlyphMetrices *bold_metrics,
	const font::GlyphMetrices *italic_metrics,
	const font::GlyphMetrices *bold_italic_metrics)
{
	wrap(
		make_glyph_rope(text_blocks,
			regular_metrics, bold_metrics, italic_metrics, bold_italic_metrics),
		max_width);
	return text_blocks;
}

void wrap(glyph_rope str, int max_width)
{
	auto width = 0.0_r;

	for (auto i = 0; i < std::ssize(str); ++i)
	{
		switch (str[i])
		{
			//New line found
			case '\n':
			break;

			default:
			{
				auto [char_width, char_height] = character_size_in_pixels(str[i], str.glyph_str(i).metrics);
				auto c_width = char_width * str.glyph_str(i).scale_factor;

				//Insert new line
				if (width > 0.0_r && //At least one character
					static_cast<int>(std::ceil(width + c_width)) > max_width) //Too wide
				{
					switch (str[i])
					{
						//Space
						case ' ':
						str[i] = '\n'; //Replace space with new line
						break;

						default:
						str.insert(i, 1, '\n'); //Insert new line
						break;
					}
				}
				else
				{
					width += c_width;
					continue;
				}
				
				break;
			}
		}

		width = 0.0_r;
	}
}


/*
	Word truncating
*/

std::string word_truncate(std::string str, int max_width,
	const font::GlyphMetrices &metrics)
{
	word_wrap(glyph_string{str, metrics}, max_width);
	return str.substr(0, str.find('\n'));
}

text::TextBlocks word_truncate(text::TextBlocks text_blocks, int max_width,
	const font::GlyphMetrices &regular_metrics,
	const font::GlyphMetrices *bold_metrics,
	const font::GlyphMetrices *italic_metrics,
	const font::GlyphMetrices *bold_italic_metrics)
{
	word_wrap(
		make_glyph_rope(text_blocks,
			regular_metrics, bold_metrics, italic_metrics, bold_italic_metrics),
		max_width);

	//Remove soft wrapped lines
	if (auto text_lines = text_blocks_to_text_lines(std::move(text_blocks)); std::size(text_lines) > 1)
	{
		for (auto iter = std::begin(text_lines); iter != std::end(text_lines);)
		{
			//Head, search for tail
			if (!iter->Tail)
			{
				auto tail_iter =
					std::find_if(iter, std::end(text_lines), [](auto &text_line) { return text_line.Tail; });
				iter = text_lines.erase(iter + 1, tail_iter + 1);
			}
			else
				++iter;
		}

		//Re-build text blocks
		for (auto i = 0; auto &text_line : text_lines)
		{
			std::move(std::begin(text_line.Blocks), std::end(text_line.Blocks),
				std::back_inserter(text_blocks));

			if (++i < std::ssize(text_lines))
				text_blocks.push_back({{}, "\n", true});
		}
	}

	return text_blocks;
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

text::TextBlocks word_wrap(text::TextBlocks text_blocks, int max_width,
	const font::GlyphMetrices &regular_metrics,
	const font::GlyphMetrices *bold_metrics,
	const font::GlyphMetrices *italic_metrics,
	const font::GlyphMetrices *bold_italic_metrics)
{
	word_wrap(
		make_glyph_rope(text_blocks,
			regular_metrics, bold_metrics, italic_metrics, bold_italic_metrics),
		max_width);
	return text_blocks;
}

void word_wrap(glyph_rope str, int max_width)
{
	auto width = 0.0_r;
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
				auto [char_width, char_height] = character_size_in_pixels(str[i], str.glyph_str(i).metrics);
				auto c_width = char_width * str.glyph_str(i).scale_factor;

				//Insert new line
				if (width > 0.0_r && //At least one character
					static_cast<int>(std::ceil(width + c_width)) > max_width) //Too wide
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
					if (width > 0.0_r && str[i] == '-')
						line_break_off = i;

					width += c_width;
					continue;
				}
				
				break;
			}
		}

		width = 0.0_r;
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

std::optional<Vector2> MeasureTextBlock(const text::TextBlock &text_block, TypeFace &type_face) noexcept
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

		auto [width, height] = detail::text_block_size_in_pixels(
			text_block, *metrics, bold_metrics, italic_metrics, bold_italic_metrics);
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

std::optional<std::string> TruncateString(std::string str, int max_width, std::string_view suffix, Font &font)
{
	if (auto metrics = detail::get_glyph_metrics(font); metrics)
		return detail::truncate_string(std::move(str), max_width, suffix, *metrics);
	else
		return {};
}

std::optional<text::TextBlocks> TruncateTextBlocks(text::TextBlocks text_blocks, int max_width, TypeFace &type_face)
{
	return TruncateTextBlocks(std::move(text_blocks), max_width, "...", type_face);
}

std::optional<text::TextBlocks> TruncateTextBlocks(text::TextBlocks text_blocks, int max_width, std::string_view suffix, TypeFace &type_face)
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

		return detail::truncate_text_blocks(std::move(text_blocks), max_width, suffix,
			*metrics, bold_metrics, italic_metrics, bold_italic_metrics);
	}
	else
		return {};
}


/*
	Wrapping
*/

std::optional<std::string> Wrap(std::string str, int max_width, Font &font)
{
	if (auto metrics = detail::get_glyph_metrics(font); metrics)
		return detail::wrap(std::move(str), max_width, *metrics);
	else
		return {};
}

std::optional<text::TextBlocks> Wrap(text::TextBlocks text_blocks, int max_width, TypeFace &type_face)
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

		return detail::wrap(std::move(text_blocks), max_width,
			*metrics, bold_metrics, italic_metrics, bold_italic_metrics);
	}
	else
		return {};
}


/*
	Word truncating
*/

std::optional<std::string> WordTruncate(std::string str, int max_width, Font &font)
{
	if (auto metrics = detail::get_glyph_metrics(font); metrics)
		return detail::word_truncate(std::move(str), max_width, *metrics);
	else
		return {};
}

std::optional<text::TextBlocks> WordTruncate(text::TextBlocks text_blocks, int max_width, TypeFace &type_face)
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

		return detail::word_truncate(std::move(text_blocks), max_width,
			*metrics, bold_metrics, italic_metrics, bold_italic_metrics);
	}
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

		return detail::word_wrap(std::move(text_blocks), max_width,
			*metrics, bold_metrics, italic_metrics, bold_italic_metrics);
	}
	else
		return {};
}

} //ion::graphics::fonts::utilities