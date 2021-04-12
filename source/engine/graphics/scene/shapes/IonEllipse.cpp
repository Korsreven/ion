/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonEllipse.cpp
-------------------------------------------
*/

#include "IonEllipse.h"

#include <cmath>
#include "utilities/IonMath.h"

namespace ion::graphics::scene::shapes
{

using namespace ellipse;

namespace ellipse::detail
{

mesh::Vertices ellipse_vertices(const Vector3 &position, real rotation, const Vector2 &size, const Color &color, int sides)
{
	using namespace ion::utilities;

	auto [half_width, half_height] = (size * 0.5_r).XY();

	auto angle = 0.0_r;
	auto delta_angle = math::TwoPi / sides;

	mesh::Vertices vertices;
	vertices.reserve(sides);

	for (auto i = 0; i < sides; ++i, angle += delta_angle)
	{
		auto v =
			Vector3{half_width * math::Cos(angle), half_height * math::Sin(angle), 0.0_r}.
			RotateCopy(rotation, vector3::Zero) + position;

		vertices.push_back({v, vector3::UnitZ, color});
	}

	return vertices;
}

} //ellipse::detail


Ellipse::Ellipse(const Vector2 &size, const Color &color, bool visible) :
	Ellipse{vector3::Zero, size, color, visible}
{
	//Empty
}

Ellipse::Ellipse(const Vector3 &position, const Vector2 &size, const Color &color, bool visible) :
	Ellipse{position, 0.0_r, size, color, visible}
{
	//Empty
}

Ellipse::Ellipse(const Vector3 &position, real rotation, const Vector2 &size, const Color &color, bool visible) :

	Shape{vertex::vertex_batch::VertexDrawMode::TriangleFan, detail::ellipse_vertices(position, rotation, size, color, ellipse::detail::default_ellipse_sides), visible},

	position_{position},
	rotation_{rotation},
	size_{size}
{
	//Empty
}


Ellipse::Ellipse(const Vector2 &size, const Color &color, int sides, bool visible) :
	Ellipse{vector3::Zero, size, color, sides, visible}
{
	//Empty
}

Ellipse::Ellipse(const Vector3 &position, const Vector2 &size, const Color &color, int sides, bool visible) :
	Ellipse{position, 0.0_r, size, color, sides, visible}
{
	//Empty
}

Ellipse::Ellipse(const Vector3 &position, real rotation, const Vector2 &size, const Color &color, int sides, bool visible) :

	Shape{vertex::vertex_batch::VertexDrawMode::TriangleFan, detail::ellipse_vertices(position, rotation, size, color, detail::ellipse_sides(sides)), visible},

	position_{position},
	rotation_{rotation},
	size_{size},
	sides_{detail::ellipse_sides(sides)}
{
	//Empty
}

} //ion::graphics::scene::shapes