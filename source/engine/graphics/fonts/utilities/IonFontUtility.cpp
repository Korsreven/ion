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
#include "utilities/IonStringUtility.h"

namespace ion::graphics::fonts::utilities
{

namespace detail
{

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

const font::detail::container_type<font::GlyphExtents>& glyph_rope::glyph(size_t off) const noexcept
{
	auto [str_off, ch_off] = get_offsets(off);
	return strings_[str_off].extents;
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


void add_text_section(std::string content, text::TextSections &text_sections, const text::TextSectionStyles &text_section_styles)
{
	//Combine with previous section if equal styles
	if (!std::empty(text_sections) &&
		((!std::empty(text_section_styles) && text_sections.back() == text_section_styles.back()) ||
		(std::empty(text_section_styles) && text_sections.back().IsPlain())))

		text_sections.back().Content() += content;
	
	//Add new section
	else
	{
		if (!std::empty(text_section_styles))
			text_sections.emplace_back(std::move(content), text_section_styles.back());
		else
			text_sections.emplace_back(std::move(content));
	}
}


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
	auto section = parent_section ?
		*parent_section : //Inherit from parent
		text::TextSectionStyle{}; //Plain

	//Tags
	//Underline
	if (element.tag == "u" || element.tag == "ins")
		section.DefaultDecoration(text::TextDecoration::Underline);

	//Line-through
	else if (element.tag == "del")
		section.DefaultDecoration(text::TextDecoration::LineThrough);

	//Bold
	else if (element.tag == "b" || element.tag == "strong")
		section.DefaultFontStyle(
			[&font_style = section.DefaultFontStyle()]() noexcept
			{
				if (font_style &&
					(*font_style == text::FontStyle::Italic ||
					 *font_style == text::FontStyle::BoldItalic))
					return text::FontStyle::BoldItalic;
				else
					return text::FontStyle::Bold;
			}());

	//Italic
	else if (element.tag == "i" || element.tag == "em")
		section.DefaultFontStyle(
			[&font_style = section.DefaultFontStyle()]() noexcept
			{
				if (font_style &&
					(*font_style == text::FontStyle::Bold ||
					 *font_style == text::FontStyle::BoldItalic))
					return text::FontStyle::BoldItalic;
				else
					return text::FontStyle::Italic;
			}());


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
					section.DefaultColor(*color);
			}

			//Style
			else if (is_style_attribute(element.attribute->name))
			{
				//TODO
			}
		}
	}

	return section;
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
						add_text_section(std::move(content), text_sections, text_section_styles);

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
							add_text_section(std::move(content), text_sections, text_section_styles);

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
		add_text_section(std::move(content), text_sections, text_section_styles);

	return text_sections;
}

text::TextLines text_sections_to_text_lines(text::TextSections text_sections)
{
	text::TextLines lines;
	text::TextSections line_sections;

	for (auto &section : text_sections)
	{
		//Content contains one or more new lines characters
		//Split text section into multiple copies
		if (section.Content().find('\n') != std::string::npos)
		{
			auto parts = ion::utilities::string::Split(section.Content(), "\n",
				ion::utilities::string::StringSplitOptions::PreserveEmptyEntries);

			section.Content().clear();
				//Clear content before duplicating text section
			
			for (auto i = 0; auto &part : parts)
			{
				if (!std::empty(part))
				{
					line_sections.push_back(section);
					line_sections.back().Content() = std::move(part);
				}
				
				if (++i < std::ssize(parts))
					lines.emplace_back(std::move(line_sections), 0);
			}
		}
		else
			line_sections.push_back(std::move(section));
	}

	if (!std::empty(line_sections))
		lines.emplace_back(std::move(line_sections), 0);

	return lines;
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
	word_wrap(glyph_string{str, extents}, max_width);
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
				auto [c_width, c_height] = character_size_in_pixels(str[i], str.glyph(i));

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


const font::detail::container_type<font::GlyphExtents>* get_glyph_extents(Font &font)
{
	if (font.IsLoaded() || (font.Owner() && font.Owner()->Load(font)))
	{
		if (auto &extents = font.GlyphExtents(); extents)
			return &*extents;
	}

	return nullptr;
}

glyph_rope make_glyph_rope(text::TextSections &text_sections,
	const font::detail::container_type<font::GlyphExtents> &regular_extents,
	const font::detail::container_type<font::GlyphExtents> *bold_extents,
	const font::detail::container_type<font::GlyphExtents> *italic_extents,
	const font::detail::container_type<font::GlyphExtents> *bold_italic_extents)
{
	glyph_strings strings;

	for (auto &section : text_sections)
	{
		auto extents =
			[&]()
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

				return &regular_extents;
			}();
		
		strings.push_back({section.Content(),
			extents ? *extents : regular_extents});
	}

	return strings;
}

} //detail


/*
	Formatting
*/

text::TextSections AsTextSections(std::string_view str)
{
	return detail::string_to_text_sections(str);
}

text::TextLines SplitTextSections(text::TextSections text_sections)
{
	return detail::text_sections_to_text_lines(std::move(text_sections));
}


/*
	Measuring
*/

std::optional<Vector2> MeasureCharacter(char c, Font &font) noexcept
{
	if (auto extents = detail::get_glyph_extents(font); extents)
	{
		auto [width, height] = detail::character_size_in_pixels(c, *extents);
		return Vector2{static_cast<real>(width), static_cast<real>(height)};
	}
	else
		return {};
}

std::optional<Vector2> MeasureString(std::string_view str, Font &font) noexcept
{
	if (auto extents = detail::get_glyph_extents(font); extents)
	{
		auto [width, height] = detail::string_size_in_pixels(str, *extents);
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
	if (auto extents = detail::get_glyph_extents(font); extents)
		return detail::truncate_string(std::move(str), max_width, std::move(suffix), *extents);
	else
		return {};
}


/*
	Word wrapping
*/

std::optional<std::string> WordWrap(std::string str, int max_width, Font &font)
{
	if (auto extents = detail::get_glyph_extents(font); extents)
		return detail::word_wrap(std::move(str), max_width, *extents);
	else
		return {};
}

std::optional<text::TextSections> WordWrap(text::TextSections text_sections, int max_width, TypeFace &type_face)
{
	if (!type_face.HasRegularFont())
		return {};

	if (auto extents = detail::get_glyph_extents(*type_face.RegularFont()); extents)
	{
		auto bold_extents = type_face.BoldFont() ?
			detail::get_glyph_extents(*type_face.BoldFont()) : nullptr;
		auto italic_extents = type_face.ItalicFont() ?
			detail::get_glyph_extents(*type_face.ItalicFont()) : nullptr;
		auto bold_italic_extents = type_face.BoldItalicFont() ?
			detail::get_glyph_extents(*type_face.BoldItalicFont()) : nullptr;

		detail::word_wrap(
			detail::make_glyph_rope(text_sections,
				*extents, bold_extents, italic_extents, bold_italic_extents),
			max_width);
		return text_sections;
	}
	else
		return {};
}

} //ion::graphics::fonts::utilities