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

Text::Text(std::string name, std::string str, TypeFace &type_face) :

	managed::ManagedObject<TextManager>{std::move(name)},

	unformatted_{std::move(str)},
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
	Type face
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
	Formatting
*/

std::string Text::FormattedStr() const noexcept
{
	return "";
}

} //ion::graphics::fonts