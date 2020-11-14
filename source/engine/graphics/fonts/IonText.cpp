/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonText.cpp
-------------------------------------------
*/

#include "IonText.h"

namespace ion::graphics::fonts
{

using namespace text;


TextSectionStyle::TextSectionStyle(std::optional<Color> color,
	std::optional<TextDecoration> decoration, std::optional<Color> decoration_color,
	std::optional<FontStyle> font_style) noexcept :

	default_color_{color},
	default_decoration_{decoration},
	default_decoration_color_{decoration_color},
	default_font_style_{font_style}
{
	//Empty
}


TextSection::TextSection(std::string content) :
	content_{std::move(content)}
{
	//Empty
}

TextSection::TextSection(std::string content, TextSectionStyle text_section_style) noexcept :

	TextSectionStyle{std::move(text_section_style)},
	content_{std::move(content)}
{
	//Empty
}


TextLine::TextLine(TextSections sections, int width) :

	sections_{std::move(sections)},
	width_{width}
{
	//Empty
}


/*
	Content
*/

std::string TextLine::Content() const
{
	std::string content;

	for (auto &section : Sections())
		content += section.Content();

	return content;
}


/*
	Text
*/

Text::Text(std::string name, std::string str, TypeFace &type_face) :

	managed::ManagedObject<TextManager>{std::move(name)},

	unformatted_str_{std::move(str)},
	type_face_{type_face}
{
	//Empty
}


/*
	Modifiers
*/

void Text::Lettering(TypeFace &type_face)
{
	type_face_.Observe(type_face);
}

void Text::Lettering(std::nullptr_t) noexcept
{
	type_face_.Release();
}


/*
	Observers
*/

TypeFace* Text::Lettering() noexcept
{
	return type_face_.Object();
}

const TypeFace* Text::Lettering() const noexcept
{
	return type_face_.Object();
}


/*
	Unformatted
*/

void Text::AppendFront(std::string_view str)
{
	unformatted_str_.insert(0, str);
}

void Text::AppendBack(std::string_view str)
{
	unformatted_str_ += str;
}


/*
	Formatted
*/

std::string Text::FormattedStr() const noexcept
{
	std::string str;

	//First
	if (!std::empty(formatted_lines_))
	{
		str += formatted_lines_.front().Content();

		//Rest
		for (auto iter = std::begin(formatted_lines_) + 1,
			end = std::end(formatted_lines_); iter != end; ++iter)
			str += "\n" + iter->Content();
	}

	return str;
}

} //ion::graphics::fonts