/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonTypeFace.cpp
-------------------------------------------
*/

#include "IonTypeFace.h"

namespace ion::graphics::fonts
{

using namespace type_face;

TypeFace::TypeFace(Font &regular) :
	regular_font_{regular}
{
	//Empty
}

TypeFace::TypeFace(Font &regular, Font &bold, std::nullptr_t) :
	regular_font_{regular}
{
	BoldFont(bold);
}

TypeFace::TypeFace(Font &regular, std::nullptr_t, Font &italic) :
	regular_font_{regular}
{
	ItalicFont(italic);
}

TypeFace::TypeFace(Font &regular, Font &bold, Font &italic) :
	regular_font_{regular}
{
	BoldFont(bold);
	ItalicFont(italic);
}

TypeFace::TypeFace(Font &regular, Font &bold, Font &italic, Font &bold_italic) :
	regular_font_{regular}
{
	BoldFont(bold);
	ItalicFont(italic);
	BoldItalicFont(bold_italic);
}


/*
	Modifiers
*/

void TypeFace::RegularFont(Font &font)
{
	if (IsEmpty() ||
		detail::is_font_attachable(*regular_font_.Object(), font))

		regular_font_.Observe(font);
}

void TypeFace::RegularFont(std::nullptr_t) noexcept
{
	if (regular_font_.Release())
	{
		BoldFont(nullptr);
		ItalicFont(nullptr);
		BoldItalicFont(nullptr);
	}
}


void TypeFace::BoldFont(Font &font)
{
	if (regular_font_ &&
		detail::is_font_attachable(*regular_font_.Object(), font))

		bold_font_.Observe(font);
}

void TypeFace::BoldFont(std::nullptr_t) noexcept
{
	if (bold_font_.Release())
		BoldItalicFont(nullptr);
}


void TypeFace::ItalicFont(Font &font)
{
	if (regular_font_ &&
		detail::is_font_attachable(*regular_font_.Object(), font))

		italic_font_.Observe(font);
}

void TypeFace::ItalicFont(std::nullptr_t) noexcept
{
	if (italic_font_.Release())
		BoldItalicFont(nullptr);
}


void TypeFace::BoldItalicFont(Font &font)
{
	if (regular_font_ && bold_font_ && italic_font_ &&
		detail::is_font_attachable(*regular_font_.Object(), font))

		bold_italic_font_.Observe(font);
}

void TypeFace::BoldItalicFont(std::nullptr_t) noexcept
{
	bold_italic_font_.Release();
}

} //ion::graphics::fonts