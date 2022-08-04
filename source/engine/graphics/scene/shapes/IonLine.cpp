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

#include "graphics/IonGraphicsAPI.h"

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


/*
	Graphics API
*/

void set_line_width(real thickness) noexcept
{
	glLineWidth(static_cast<float>(thickness));
}

} //line::detail


//Protected

mesh::Vertices Line::GetVertices() const noexcept
{
	return detail::line_vertices(a_, b_, color_);
}


//Public

Line::Line(const Vector3 &a, const Vector3 &b, const Color &color, bool visible) :

	Shape{vertex::vertex_batch::VertexDrawMode::Lines, detail::line_vertices(a, b, color), color, visible},

	a_{a},
	b_{b}
{
	//Empty
}

Line::Line(const Vector3 &a, const Vector3 &b, const Color &color, real thickness, bool visible) :

	Shape{vertex::vertex_batch::VertexDrawMode::Lines, detail::line_vertices(a, b, color), color, visible},

	a_{a},
	b_{b},
	thickness_{thickness}
{
	//Empty
}


/*
	Drawing
*/

void Line::DrawStarted() noexcept
{
	detail::set_line_width(thickness_);
	Shape::DrawStarted();
}

void Line::DrawEnded() noexcept
{
	Shape::DrawEnded();
	detail::set_line_width(1.0_r);
}

} //ion::graphics::scene::shapes