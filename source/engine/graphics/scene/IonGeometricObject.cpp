/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonGeometricObject.cpp
-------------------------------------------
*/

#include "IonGeometricObject.h"

namespace ion::graphics::scene
{

using namespace geometric_object;

namespace geometric_object::detail
{
} //geometric_object::detail


GeometricObject::GeometricObject(bool visible) :
	MovableObject{visible}
{
	//Empty
}

GeometricObject::GeometricObject(std::string name, bool visible) :
	MovableObject{std::move(name), visible}
{
	//Empty
}

GeometricObject::GeometricObject(const GeometricObject &rhs) noexcept :
	MovableObject{rhs}
{
	//Empty
}


/*
	Rendering
*/

void GeometricObject::Render(duration time) noexcept
{
	Elapse(time);
	Prepare();
	Draw(); //Todo
}


/*
	Elapse time
*/

void GeometricObject::Elapse([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}

} //ion::graphics::scene