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

TypeFace::TypeFace(Font &regular)
{
	RegularFont(regular);
}

TypeFace::TypeFace(Font &regular, Font &bold, std::nullptr_t)
{
	RegularFont(regular);
	BoldFont(bold);
}

TypeFace::TypeFace(Font &regular, std::nullptr_t, Font &italic)
{
	RegularFont(regular);
	ItalicFont(italic);
}

TypeFace::TypeFace(Font &regular, Font &bold, Font &italic)
{
	RegularFont(regular);
	BoldFont(bold);
	ItalicFont(italic);
}

TypeFace::TypeFace(Font &regular, Font &bold, Font &italic, Font &bold_italic)
{
	RegularFont(regular);
	BoldFont(bold);
	ItalicFont(italic);
	BoldItalicFont(bold_italic);
}


/*
	Modifiers
*/

void TypeFace::RegularFont(Font &font)
{
	regular_font_.Observe(font);
}

void TypeFace::RegularFont(std::nullptr_t) noexcept
{
	regular_font_.Release();
}


void TypeFace::BoldFont(Font &font)
{
	bold_font_.Observe(font);
}

void TypeFace::BoldFont(std::nullptr_t) noexcept
{
	bold_font_.Release();
}


void TypeFace::ItalicFont(Font &font)
{
	italic_font_.Observe(font);
}

void TypeFace::ItalicFont(std::nullptr_t) noexcept
{
	italic_font_.Release();
}


void TypeFace::BoldItalicFont(Font &font)
{
	bold_italic_font_.Observe(font);
}

void TypeFace::BoldItalicFont(std::nullptr_t) noexcept
{
	bold_italic_font_.Release();
}

} //ion::graphics::fonts