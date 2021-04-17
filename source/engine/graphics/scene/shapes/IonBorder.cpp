/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonBorder.cpp
-------------------------------------------
*/

#include "IonBorder.h"

#include "graphics/materials/IonMaterial.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{

using namespace border;
using namespace types::type_literals;

namespace border::detail
{

mesh::Vertices border_vertices(
	const Vector3 &position, real rotation, const Vector2 &size, const Vector2 &border_size,
	BorderCornerStyle corner_style, const Color &color,
	const std::optional<Color> &top_side_color, const std::optional<Color> &bottom_side_color,
	const std::optional<Color> &left_side_color, const std::optional<Color> &right_side_color,
	const std::optional<Color> &top_left_corner_color, const std::optional<Color> &top_right_corner_color,
	const std::optional<Color> &bottom_left_corner_color, const std::optional<Color> &bottom_right_corner_color)
{
	auto [half_width, half_height] = (size * 0.5_r).XY();
	auto [border_width, border_height] = border_size.XY();

	auto v1 = position + Vector2{-half_width, half_height};
	auto v2 = position + Vector2{-half_width, -half_height};
	auto v3 = position + Vector2{half_width, -half_height};
	auto v4 = position + Vector2{half_width, half_height};


	//Top side
	auto top_v1 = (v1 + Vector2{0.0_r, border_height}).RotateCopy(rotation, position);
	auto top_v2 = v1.RotateCopy(rotation, position);
	auto top_v3 = v4.RotateCopy(rotation, position);
	auto top_v4 = (v4 + Vector2{0.0_r, border_height}).RotateCopy(rotation, position);

	//Bottom side
	auto bottom_v1 = v2.RotateCopy(rotation, position);
	auto bottom_v2 = (v2 + Vector2{0.0_r, -border_height}).RotateCopy(rotation, position);
	auto bottom_v3 = (v3 + Vector2{0.0_r, -border_height}).RotateCopy(rotation, position);
	auto bottom_v4 = v3.RotateCopy(rotation, position);

	//Left side
	auto left_v1 = (v1 + Vector2{-border_width, 0.0_r}).RotateCopy(rotation, position);
	auto left_v2 = (v2 + Vector2{-border_width, 0.0_r}).RotateCopy(rotation, position);
	auto left_v3 = v2.RotateCopy(rotation, position);
	auto left_v4 = v1.RotateCopy(rotation, position);

	//Right side
	auto right_v1 = v4.RotateCopy(rotation, position);
	auto right_v2 = v3.RotateCopy(rotation, position);
	auto right_v3 = (v3 + Vector2{border_width, 0.0_r}).RotateCopy(rotation, position);
	auto right_v4 = (v4 + Vector2{border_width, 0.0_r}).RotateCopy(rotation, position);


	//Top-left corner
	auto top_left_v1 = (v1 + Vector2{-border_width, border_height}).RotateCopy(rotation, position);
	auto top_left_v2 = (v1 + Vector2{-border_width, 0.0_r}).RotateCopy(rotation, position);
	auto top_left_v3 = v1.RotateCopy(rotation, position);
	auto top_left_v4 = (v1 + Vector2{0.0_r, border_height}).RotateCopy(rotation, position);

	//Top-right corner
	auto top_right_v1 = (v4 + Vector2{0.0_r, border_height}).RotateCopy(rotation, position);
	auto top_right_v2 = v4.RotateCopy(rotation, position);
	auto top_right_v3 = (v4 + Vector2{border_width, 0.0_r}).RotateCopy(rotation, position);
	auto top_right_v4 = (v4 + Vector2{border_width, border_height}).RotateCopy(rotation, position);

	//Bottom-left corner
	auto bottom_left_v1 = (v2 + Vector2{-border_width, 0.0_r}).RotateCopy(rotation, position);
	auto bottom_left_v2 = (v2 + Vector2{-border_width, -border_height}).RotateCopy(rotation, position);
	auto bottom_left_v3 = (v2 + Vector2{0.0_r, -border_height}).RotateCopy(rotation, position);
	auto bottom_left_v4 = v2.RotateCopy(rotation, position);

	//Bottom-right corner
	auto bottom_right_v1 = v3.RotateCopy(rotation, position);
	auto bottom_right_v2 = (v3 + Vector2{0.0_r, -border_height}).RotateCopy(rotation, position);
	auto bottom_right_v3 = (v3 + Vector2{border_width, -border_height}).RotateCopy(rotation, position);
	auto bottom_right_v4 = (v3 + Vector2{border_width, 0.0_r}).RotateCopy(rotation, position);


	//Side colors
	auto top_color = top_side_color.value_or(color);
	auto bottom_color = bottom_side_color.value_or(color);
	auto left_color = left_side_color.value_or(color);
	auto right_color = right_side_color.value_or(color);

	//Corner colors
	auto top_left_color = top_left_corner_color.value_or(color);
	auto top_right_color = top_right_corner_color.value_or(color);
	auto bottom_left_color = bottom_left_corner_color.value_or(color);
	auto bottom_right_color = bottom_right_corner_color.value_or(color);

	mesh::Vertices vertices;
	vertices.reserve(corner_style == BorderCornerStyle::Oblique ? 6 * 4 + 3 * 4 : 6 * 8);

	//Top side
	vertices.push_back({top_v1, vector3::UnitZ, {0.0_r, 1.0_r}, top_color});
	vertices.push_back({top_v2, vector3::UnitZ, {0.0_r, 0.0_r}, top_color});
	vertices.push_back({top_v3, vector3::UnitZ, {1.0_r, 0.0_r}, top_color});
	vertices.push_back({top_v3, vector3::UnitZ, {1.0_r, 0.0_r}, top_color});
	vertices.push_back({top_v4, vector3::UnitZ, {1.0_r, 1.0_r}, top_color});
	vertices.push_back({top_v1, vector3::UnitZ, {0.0_r, 1.0_r}, top_color});
	
	//Bottom side
	vertices.push_back({bottom_v1, vector3::UnitZ, {0.0_r, 1.0_r}, bottom_color});
	vertices.push_back({bottom_v2, vector3::UnitZ, {0.0_r, 0.0_r}, bottom_color});
	vertices.push_back({bottom_v3, vector3::UnitZ, {1.0_r, 0.0_r}, bottom_color});
	vertices.push_back({bottom_v3, vector3::UnitZ, {1.0_r, 0.0_r}, bottom_color});
	vertices.push_back({bottom_v4, vector3::UnitZ, {1.0_r, 1.0_r}, bottom_color});
	vertices.push_back({bottom_v1, vector3::UnitZ, {0.0_r, 1.0_r}, bottom_color});
	
	//Left side
	vertices.push_back({left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, left_color});
	vertices.push_back({left_v2, vector3::UnitZ, {0.0_r, 0.0_r}, left_color});
	vertices.push_back({left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, left_color});
	vertices.push_back({left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, left_color});
	vertices.push_back({left_v4, vector3::UnitZ, {1.0_r, 1.0_r}, left_color});
	vertices.push_back({left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, left_color});
	
	//Right side
	vertices.push_back({right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, right_color});
	vertices.push_back({right_v2, vector3::UnitZ, {0.0_r, 0.0_r}, right_color});
	vertices.push_back({right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, right_color});
	vertices.push_back({right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, right_color});
	vertices.push_back({right_v4, vector3::UnitZ, {1.0_r, 1.0_r}, right_color});
	vertices.push_back({right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, right_color});			


	//Top-left corner
	switch (corner_style)
	{
		case BorderCornerStyle::None:
		vertices.push_back({top_left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, left_color});
		vertices.push_back({top_left_v2, vector3::UnitZ, {0.0_r, 0.0_r}, left_color});
		vertices.push_back({top_left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, left_color});
		vertices.push_back({top_left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, top_color});
		vertices.push_back({top_left_v4, vector3::UnitZ, {1.0_r, 1.0_r}, top_color});
		vertices.push_back({top_left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, top_color});
		break;

		case BorderCornerStyle::Oblique:
		vertices.push_back({top_left_v2, vector3::UnitZ, {0.0_r, 0.0_r}, top_left_color});
		vertices.push_back({top_left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, top_left_color});
		vertices.push_back({top_left_v4, vector3::UnitZ, {1.0_r, 1.0_r}, top_left_color});
		break;

		default:
		vertices.push_back({top_left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, top_left_color});
		vertices.push_back({top_left_v2, vector3::UnitZ, {0.0_r, 0.0_r}, top_left_color});
		vertices.push_back({top_left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, top_left_color});
		vertices.push_back({top_left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, top_left_color});
		vertices.push_back({top_left_v4, vector3::UnitZ, {1.0_r, 1.0_r}, top_left_color});
		vertices.push_back({top_left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, top_left_color});
		break;
	}
	
	//Top-right corner
	switch (corner_style)
	{
		case BorderCornerStyle::None:
		vertices.push_back({top_right_v4, vector3::UnitZ, {1.0_r, 1.0_r}, top_color});
		vertices.push_back({top_right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, top_color});
		vertices.push_back({top_right_v2, vector3::UnitZ, {0.0_r, 0.0_r}, top_color});
		vertices.push_back({top_right_v2, vector3::UnitZ, {0.0_r, 0.0_r}, right_color});
		vertices.push_back({top_right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, right_color});
		vertices.push_back({top_right_v4, vector3::UnitZ, {1.0_r, 1.0_r}, right_color});
		break;

		case BorderCornerStyle::Oblique:
		vertices.push_back({top_right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, top_right_color});
		vertices.push_back({top_right_v2, vector3::UnitZ, {0.0_r, 0.0_r}, top_right_color});
		vertices.push_back({top_right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, top_right_color});
		break;

		default:
		vertices.push_back({top_right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, top_right_color});
		vertices.push_back({top_right_v2, vector3::UnitZ, {0.0_r, 0.0_r}, top_right_color});
		vertices.push_back({top_right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, top_right_color});
		vertices.push_back({top_right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, top_right_color});
		vertices.push_back({top_right_v4, vector3::UnitZ, {1.0_r, 1.0_r}, top_right_color});
		vertices.push_back({top_right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, top_right_color});
		break;
	}
	
	//Bottom-left corner
	switch (corner_style)
	{
		case BorderCornerStyle::None:
		vertices.push_back({bottom_left_v4, vector3::UnitZ, {1.0_r, 1.0_r}, left_color});
		vertices.push_back({bottom_left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, left_color});
		vertices.push_back({bottom_left_v2, vector3::UnitZ, {0.0_r, 0.0_r}, left_color});
		vertices.push_back({bottom_left_v2, vector3::UnitZ, {0.0_r, 0.0_r}, bottom_color});
		vertices.push_back({bottom_left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, bottom_color});
		vertices.push_back({bottom_left_v4, vector3::UnitZ, {1.0_r, 1.0_r}, bottom_color});
		break;

		case BorderCornerStyle::Oblique:
		vertices.push_back({bottom_left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, bottom_left_color});
		vertices.push_back({bottom_left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, bottom_left_color});
		vertices.push_back({bottom_left_v4, vector3::UnitZ, {1.0_r, 1.0_r}, bottom_left_color});
		break;

		default:
		vertices.push_back({bottom_left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, bottom_left_color});
		vertices.push_back({bottom_left_v2, vector3::UnitZ, {0.0_r, 0.0_r}, bottom_left_color});
		vertices.push_back({bottom_left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, bottom_left_color});
		vertices.push_back({bottom_left_v3, vector3::UnitZ, {1.0_r, 0.0_r}, bottom_left_color});
		vertices.push_back({bottom_left_v4, vector3::UnitZ, {1.0_r, 1.0_r}, bottom_left_color});
		vertices.push_back({bottom_left_v1, vector3::UnitZ, {0.0_r, 1.0_r}, bottom_left_color});
		break;
	}
	
	//Bottom-right corner
	switch (corner_style)
	{
		case BorderCornerStyle::None:
		vertices.push_back({bottom_right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, bottom_color});
		vertices.push_back({bottom_right_v2, vector3::UnitZ, {0.0_r, 0.0_r}, bottom_color});
		vertices.push_back({bottom_right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, bottom_color});
		vertices.push_back({bottom_right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, right_color});
		vertices.push_back({bottom_right_v4, vector3::UnitZ, {1.0_r, 1.0_r}, right_color});
		vertices.push_back({bottom_right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, right_color});
		break;

		case BorderCornerStyle::Oblique:
		vertices.push_back({bottom_right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, bottom_right_color});
		vertices.push_back({bottom_right_v2, vector3::UnitZ, {0.0_r, 0.0_r}, bottom_right_color});
		vertices.push_back({bottom_right_v4, vector3::UnitZ, {1.0_r, 1.0_r}, bottom_right_color});
		break;

		default:
		vertices.push_back({bottom_right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, bottom_right_color});
		vertices.push_back({bottom_right_v2, vector3::UnitZ, {0.0_r, 0.0_r}, bottom_right_color});
		vertices.push_back({bottom_right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, bottom_right_color});
		vertices.push_back({bottom_right_v3, vector3::UnitZ, {1.0_r, 0.0_r}, bottom_right_color});
		vertices.push_back({bottom_right_v4, vector3::UnitZ, {1.0_r, 1.0_r}, bottom_right_color});
		vertices.push_back({bottom_right_v1, vector3::UnitZ, {0.0_r, 1.0_r}, bottom_right_color});
		break;
	}

	return vertices;
}

} //border::detail


//Protected

mesh::Vertices Border::GetVertices() const noexcept
{
	return detail::border_vertices(
		position_, rotation_, size_, border_size_,
		corner_style_, color_,
		top_side_color_, bottom_side_color_,
		left_side_color_, right_side_color_,
		top_left_corner_color_, top_right_corner_color_,
		bottom_left_corner_color_, bottom_right_corner_color_);
}


//Public

Border::Border(const Vector2 &size, const Vector2 &border_size, const Color &color, bool visible) :
	Border{vector3::Zero, size, border_size, color, visible}
{
	//Empty
}

Border::Border(const Vector3 &position, const Vector2 &size, const Vector2 &border_size, const Color &color, bool visible) :
	Border{position, 0.0_r, size, border_size, color, visible}
{
	//Empty
}

Border::Border(const Vector3 &position, real rotation, const Vector2 &size, const Vector2 &border_size, const Color &color, bool visible) :
	Border{position, rotation, size, border_size, BorderCornerStyle::None, color, visible}
{
	//Empty
}


Border::Border(const Vector2 &size, const Vector2 &border_size,
	border::BorderCornerStyle corner_style, const Color &color, bool visible) :
	
	Border{vector3::Zero, size, border_size, corner_style, color, visible}
{
	//Empty
}

Border::Border(const Vector3 &position, const Vector2 &size, const Vector2 &border_size,
	border::BorderCornerStyle corner_style, const Color &color, bool visible) :
	
	Border{position, 0.0_r, size, border_size, corner_style, color, visible}
{
	//Empty
}

Border::Border(const Vector3 &position, real rotation, const Vector2 &size, const Vector2 &border_size,
	border::BorderCornerStyle corner_style, const Color &color, bool visible) :
	
	Rectangle{detail::border_vertices(position, rotation, size, border_size, corner_style, color, {}, {}, {}, {}, {}, {}, {}, {}),
			  position, rotation, size, color, visible},

	border_size_{border_size},
	corner_style_{corner_style}
{
	//Empty
}

} //ion::graphics::scene::shapes