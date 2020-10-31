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

Text::Text(std::string name, TypeFace &type_face) :

	managed::ManagedObject<TextManager>{std::move(name)},
	type_face_{type_face}
{
	//Empty
}


/*
	Modifiers
*/




/*
	Type face
*/

TypeFace* Text::UnderlyingTypeFace() noexcept
{
	return type_face_.Object();
}

const TypeFace* Text::UnderlyingTypeFace() const noexcept
{
	return type_face_.Object();
}

} //ion::graphics::fonts