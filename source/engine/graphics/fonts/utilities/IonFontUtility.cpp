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
#include "script/utilities/IonParseUtility.h"
#include "types/IonTypes.h"

namespace ion::graphics::fonts::utilities
{

namespace detail
{

std::optional<std::string_view> get_tag(std::string_view str) noexcept
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

std::optional<html_element> parse_opening_tag(std::string_view str) noexcept
{
	auto element =
		[&]()
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


text::TextSectionStyle html_element_to_text_section_style(const html_element &element, text::TextSectionStyle *parent_section) noexcept
{
	std::optional<Color> section_color;
	std::optional<text::TextDecoration> section_decoration;
	std::optional<Color> section_decoration_color;
	std::optional<text::FontStyle> section_font_style;

	//Inherit from parent
	if (parent_section)
	{
		section_color = parent_section->TextColor();
		section_decoration = parent_section->Decoration();
		section_decoration_color = parent_section->DecorationColor();
		section_font_style = parent_section->TextFontStyle();
	}

	//Tags
	//Underline
	if (element.tag == "u" || element.tag == "ins")
		section_decoration = text::TextDecoration::Underline;

	//Line-through
	else if (element.tag == "del")
		section_decoration = text::TextDecoration::LineThrough;

	//Bold
	else if (element.tag == "b" || element.tag == "strong")
		section_font_style =
			[&]() noexcept
			{
				if (section_font_style &&
					(*section_font_style == text::FontStyle::Italic ||
					 *section_font_style == text::FontStyle::BoldItalic))
					return text::FontStyle::BoldItalic;
				else
					return text::FontStyle::Bold;
			}();

	//Italic
	else if (element.tag == "i" || element.tag == "em")
		section_font_style =
			[&]() noexcept
			{
				if (section_font_style &&
					(*section_font_style == text::FontStyle::Bold ||
					 *section_font_style == text::FontStyle::BoldItalic))
					return text::FontStyle::BoldItalic;
				else
					return text::FontStyle::Italic;
			}();


	//Attributes
	if (element.attribute)
	{
		//Parse attribute value (as string literal)
		if (auto value =
			script::utilities::parse::AsString(element.attribute->value); value)
		{
			//Color
			if (is_color_attribute(element.attribute->name))
			{
				if (auto color = script::utilities::parse::AsColor(*value); color)
					section_color = *color;
			}

			//Style
			else if (is_style_attribute(element.attribute->name))
			{
				//TODO
			}
		}
	}

	return {section_color, section_decoration, section_decoration_color, section_font_style};
}

text::TextSections string_to_text_sections(std::string_view str)
{
	html_elements elements;
	text::TextSectionStyles text_section_styles;
	text::TextSections text_sections;
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
				if (auto tag = get_tag(str.substr(off + 2));
					tag && *tag == elements.back().tag) 
				{
					if (!std::empty(content))
						text_sections.emplace_back(std::move(content), text_section_styles.back());

					elements.pop_back();
					text_section_styles.pop_back();

					iter += std::size(*tag) + 2;
					continue;
				}
			}
		}
		//Opening tag
		else if (is_start_of_opening_tag(c))
		{
			if (auto tag = get_tag(str.substr(off + 1)); tag) 
			{
				//Opening tag has been parsed and validated
				if (auto element = parse_opening_tag(*tag); element)
				{
					if (is_empty_tag(element->tag))
					{
						if (is_br_tag(element->tag))
							content += '\n';
					}
					else
					{
						if (!std::empty(content))
						{
							if (!std::empty(text_section_styles))
								text_sections.emplace_back(std::move(content), text_section_styles.back());
							else
								text_sections.emplace_back(std::move(content));
						}

						elements.push_back(std::move(*element));
						text_section_styles.push_back(
							html_element_to_text_section_style(
								elements.back(),
								!std::empty(text_section_styles) ?
								&text_section_styles.back() : nullptr
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
	{
		if (!std::empty(text_section_styles))
			text_sections.emplace_back(std::move(content), text_section_styles.back());
		else
			text_sections.emplace_back(std::move(content));
	}

	return text_sections;
}

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
	Formatting
*/

text::TextSections AsTextSections(std::string_view str)
{
	return detail::string_to_text_sections(str);
}


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