/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonTriangle.cpp
-------------------------------------------
*/

#include "IonTriangle.h"

namespace ion::graphics::scene::shapes
{

using namespace triangle;

namespace triangle::detail
{

mesh::Vertices triangle_vertices(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Color &color)
{
	return {{a, vector3::UnitZ, color},
			{b, vector3::UnitZ, color},
			{c, vector3::UnitZ, color}};
}

} //triangle::detail


//Protected

mesh::Vertices Triangle::GetVertices() const noexcept
{
	return detail::triangle_vertices(a_, b_, c_, color_);
}


//Public

Triangle::Triangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Color &color, bool visible) :

	Shape{detail::triangle_vertices(a, b, c, color), color, visible},

	a_{a},
	b_{b},
	c_{c}
{
	//Empty
}

} //ion::graphics::scene::shapes