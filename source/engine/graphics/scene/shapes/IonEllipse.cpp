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
			(position + Vector2{half_width * math::Cos(angle), half_height * math::Sin(angle)}).
			RotateCopy(rotation, position);

		vertices.push_back({v, vector3::UnitZ, color});
	}

	return vertices;
}

} //ellipse::detail


//Protected

mesh::Vertices Ellipse::GetVertices() const noexcept
{
	return detail::ellipse_vertices(position_, rotation_, size_, color_, sides_);
}


//Public

Ellipse::Ellipse(std::optional<std::string> name, const Vector2 &size,
	const Color &color, bool visible) :

	Ellipse{std::move(name), vector3::Zero, size, color, visible}
{
	//Empty
}

Ellipse::Ellipse(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
	const Color &color, bool visible) :

	Ellipse{std::move(name), position, 0.0_r, size, color, visible}
{
	//Empty
}

Ellipse::Ellipse(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
	const Color &color, bool visible) :

	Shape{std::move(name), vertex::vertex_batch::VertexDrawMode::TriangleFan,
		  detail::ellipse_vertices(position, rotation, size, color, ellipse::detail::default_ellipse_sides), color, visible},

	position_{position},
	rotation_{rotation},
	size_{size}
{
	//Empty
}


Ellipse::Ellipse(std::optional<std::string> name, const Vector2 &size,
	const Color &color, int sides, bool visible) :

	Ellipse{std::move(name), vector3::Zero, size, color, sides, visible}
{
	//Empty
}

Ellipse::Ellipse(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
	const Color &color, int sides, bool visible) :

	Ellipse{std::move(name), position, 0.0_r, size, color, sides, visible}
{
	//Empty
}

Ellipse::Ellipse(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
	const Color &color, int sides, bool visible) :

	Shape{std::move(name), vertex::vertex_batch::VertexDrawMode::TriangleFan,
		  detail::ellipse_vertices(position, rotation, size, color, detail::ellipse_sides(sides)), color, visible},

	position_{position},
	rotation_{rotation},
	size_{size},
	sides_{detail::ellipse_sides(sides)}
{
	//Empty
}

} //ion::graphics::scene::shapes