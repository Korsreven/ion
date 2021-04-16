/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonRectangle.cpp
-------------------------------------------
*/

#include "IonRectangle.h"

namespace ion::graphics::scene::shapes
{

using namespace rectangle;

namespace rectangle::detail
{

mesh::Vertices rectangle_vertices(const Vector3 &position, real rotation, const Vector2 &size, const Color &color)
{
	auto [half_width, half_height] = (size * 0.5_r).XY();

	auto v1 = (position + Vector2{-half_width, half_height}).RotateCopy(rotation, position);
	auto v2 = (position + Vector2{-half_width, -half_height}).RotateCopy(rotation, position);
	auto v3 = (position + Vector2{half_width, -half_height}).RotateCopy(rotation, position);
	auto v4 = (position + Vector2{half_width, half_height}).RotateCopy(rotation, position);

	return {{v1, vector3::UnitZ, {0.0_r, 1.0_r}, color},
			{v2, vector3::UnitZ, {0.0_r, 0.0_r}, color},
			{v3, vector3::UnitZ, {1.0_r, 0.0_r}, color},
			{v3, vector3::UnitZ, {1.0_r, 0.0_r}, color},
			{v4, vector3::UnitZ, {1.0_r, 1.0_r}, color},
			{v1, vector3::UnitZ, {0.0_r, 1.0_r}, color}};
}

} //rectangle::detail


//Protected

Rectangle::Rectangle(const mesh::Vertices &vertices, const Vector3 &position, real rotation, const Vector2 &size,
	const Color &color, bool visible) :

	Shape{vertices, color, visible},
	
	position_{position},
	rotation_{rotation},
	size_{size}
{
	//Empty
}

Rectangle::Rectangle(const mesh::Vertices &vertices, const Vector3 &position, real rotation, const Vector2 &size,
	NonOwningPtr<materials::Material> material, const Color &color, bool visible) :

	Shape{vertices, material, color, visible},
	
	position_{position},
	rotation_{rotation},
	size_{size}
{
	Mesh::TexCoordMode(mesh::MeshTexCoordMode::Manual);
}


mesh::Vertices Rectangle::GetVertices() const noexcept
{
	return detail::rectangle_vertices(position_, rotation_, size_, color_);
}


//Public

Rectangle::Rectangle(const Vector2 &size, const Color &color, bool visible) :
	Rectangle{vector3::Zero, size, color, visible}
{
	//Empty
}

Rectangle::Rectangle(const Vector3 &position, const Vector2 &size, const Color &color, bool visible) :
	Rectangle{position, 0.0_r, size, color, visible}
{
	//Empty
}

Rectangle::Rectangle(const Vector3 &position, real rotation, const Vector2 &size, const Color &color, bool visible) :
	Rectangle{detail::rectangle_vertices(position, rotation, size, color), position, rotation, size, color, visible}
{
	//Empty
}

} //ion::graphics::scene::shapes