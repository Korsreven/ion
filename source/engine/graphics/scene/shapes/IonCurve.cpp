/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonCurve.cpp
-------------------------------------------
*/

#include "IonCurve.h"

#include <cmath>
#include "utilities/IonMath.h"
#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::scene::shapes
{

using namespace curve;

namespace curve::detail
{

mesh::Vertices curve_vertices(const ControlPoints &control_points, const Color &color, int smoothness)
{
	using namespace ion::utilities;

	if (std::ssize(control_points) > max_control_points)
		return {};

	auto vertex_count =
		std::ssize(control_points) * (smoothness > 1 ? smoothness : 1);

	mesh::Vertices vertices;
	vertices.reserve(vertex_count);

	//Don't use bezier
	if (smoothness == 0 || vertex_count < 2)
	{
		for (auto &p : control_points)
			vertices.push_back({p, vector3::UnitZ, color});

		return vertices;
	}

	//Use bezier
	auto point = vector3::Zero;
	auto percent = 1.0_r;
	auto step = 1.0_r / (vertex_count - 1);

	//Calculate each coordinate in the bezier curve
	for (auto i = 0; i < vertex_count; ++i, percent -= step, point = vector3::Zero)
	{
		for (auto j = 0; auto &p : control_points)
		{
			auto bezier =
				math::Choose(std::ssize(control_points) - 1, j) *
				std::pow(percent, static_cast<real>(std::ssize(control_points) - j - 1)) *
				std::pow(1.0_r - percent, static_cast<real>(j));

			point += p * bezier;
			point.Z(p.Z());
			++j;
		}

		vertices.push_back({point, vector3::UnitZ, color});
	}

	return vertices;
}

} //curve::detail


//Protected

mesh::Vertices Curve::GetVertices() const noexcept
{
	return detail::curve_vertices(control_points_, color_, smoothness_);
}


//Public

Curve::Curve(ControlPoints control_points, const Color &color, bool visible) :

	Shape{vertex::vertex_batch::VertexDrawMode::LineStrip,
		  detail::curve_vertices(control_points, color, detail::default_curve_smoothness), color, visible},
	control_points_{std::move(control_points)}
{
	//Empty
}

Curve::Curve(ControlPoints control_points, const Color &color, real thickness, bool visible) :

	Shape{vertex::vertex_batch::VertexDrawMode::LineStrip,
		  detail::curve_vertices(control_points, color, detail::default_curve_smoothness), color, visible},

	control_points_{std::move(control_points)},
	thickness_{thickness}
{
	//Empty
}


Curve::Curve(ControlPoints control_points, const Color &color, int smoothness, bool visible) :

	Shape{vertex::vertex_batch::VertexDrawMode::LineStrip,
		  detail::curve_vertices(control_points, color, detail::curve_smoothness(smoothness)), color, visible},

	control_points_{std::move(control_points)},
	smoothness_{detail::curve_smoothness(smoothness)}
{
	//Empty
}

Curve::Curve(ControlPoints control_points, const Color &color, real thickness, int smoothness, bool visible) :

	Shape{vertex::vertex_batch::VertexDrawMode::LineStrip,
		  detail::curve_vertices(control_points, color, detail::curve_smoothness(smoothness)), color, visible},

	control_points_{std::move(control_points)},
	thickness_{thickness},
	smoothness_{detail::curve_smoothness(smoothness)}
{
	//Empty
}


/*
	Drawing
*/

void Curve::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	glLineWidth(static_cast<float>(thickness_));
	Mesh::Draw(shader_program);
	glLineWidth(1.0f);
}

} //ion::graphics::scene::shapes