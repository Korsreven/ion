/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonDrawableObject.cpp
-------------------------------------------
*/

#include "IonDrawableObject.h"

namespace ion::graphics::scene
{

using namespace drawable_object;

namespace drawable_object::detail
{
} //drawable_object::detail


DrawableObject::DrawableObject(bool visible) :
	MovableObject{visible}
{
	//Empty
}

DrawableObject::DrawableObject(std::string name, bool visible) :
	MovableObject{std::move(name), visible}
{
	//Empty
}

DrawableObject::DrawableObject(const DrawableObject &rhs) noexcept :
	MovableObject{rhs}
{
	//Empty
}


/*
	Rendering
*/

void DrawableObject::Render(duration time) noexcept
{
	Elapse(time);
	Prepare();
	Draw(); //Todo
}


/*
	Elapse time
*/

void DrawableObject::Elapse([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}

} //ion::graphics::scene