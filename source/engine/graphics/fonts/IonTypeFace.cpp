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

TypeFace::TypeFace(std::string name, NonOwningPtr<Font> regular) :

	managed::ManagedObject<TypeFaceManager>{std::move(name)},
	regular_font_{regular}
{
	//Empty
}

TypeFace::TypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic) :

	managed::ManagedObject<TypeFaceManager>{std::move(name)},
	regular_font_{regular}
{
	BoldFont(bold);
	ItalicFont(italic);
}

TypeFace::TypeFace(std::string name, NonOwningPtr<Font> regular, NonOwningPtr<Font> bold, NonOwningPtr<Font> italic,
	NonOwningPtr<Font> bold_italic) :

	managed::ManagedObject<TypeFaceManager>{std::move(name)},
	regular_font_{regular}
{
	BoldFont(bold);
	ItalicFont(italic);
	BoldItalicFont(bold_italic);
}


/*
	Modifiers
*/

void TypeFace::RegularFont(NonOwningPtr<Font> font) noexcept
{
	if (font)
	{
		if (IsEmpty() ||
			detail::is_font_attachable(*regular_font_, *font))

			regular_font_ = font;
	}
	else
	{
		regular_font_ = nullptr;
		bold_font_ = nullptr;
		italic_font_ = nullptr;
		bold_italic_font_ = nullptr;
	}
}

void TypeFace::BoldFont(NonOwningPtr<Font> font) noexcept
{
	if (font)
	{
		if (regular_font_ &&
			detail::is_font_attachable(*regular_font_, *font))

			bold_font_ = font;
	}
	else
	{
		bold_font_ = nullptr;
		bold_italic_font_ = nullptr;
	}
}

void TypeFace::ItalicFont(NonOwningPtr<Font> font) noexcept
{
	if (font)
		{
		if (regular_font_ &&
			detail::is_font_attachable(*regular_font_, *font))

			italic_font_ = font;
	}
	else
	{
		italic_font_ = nullptr;
		bold_italic_font_ = nullptr;
	}
}

void TypeFace::BoldItalicFont(NonOwningPtr<Font> font) noexcept
{
	if (font)
	{
		if (regular_font_ && bold_font_ && italic_font_ &&
			detail::is_font_attachable(*regular_font_, *font))

			bold_italic_font_ = font;
	}
	else
		bold_italic_font_ = nullptr;
}

} //ion::graphics::fonts