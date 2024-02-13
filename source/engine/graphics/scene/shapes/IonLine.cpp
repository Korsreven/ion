/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonLine.cpp
-------------------------------------------
*/

#include "IonLine.h"

namespace ion::graphics::scene::shapes
{

using namespace line;

namespace line::detail
{

mesh::Vertices line_vertices(const Vector3 &a, const Vector3 &b, const Color &color)
{
	return {{a, vector3::UnitZ, color},
			{b, vector3::UnitZ, color}};
}

} //line::detail


//Protected

mesh::Vertices Line::GetVertices() const noexcept
{
	return detail::line_vertices(a_, b_, color_);
}


//Public

Line::Line(std::optional<std::string> name, const Vector3 &a, const Vector3 &b,
	const Color &color, bool visible) :

	Shape{std::move(name), vertex::vertex_batch::VertexDrawMode::Lines, detail::line_vertices(a, b, color), color, visible},

	a_{a},
	b_{b}
{
	//Empty
}

Line::Line(std::optional<std::string> name, const Vector3 &a, const Vector3 &b,
	const Color &color, real thickness, bool visible) :

	Shape{std::move(name), vertex::vertex_batch::VertexDrawMode::Lines, detail::line_vertices(a, b, color), color, visible},

	a_{a},
	b_{b}
{
	Thickness(thickness);
}

} //ion::graphics::scene::shapes