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

#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{

using namespace rectangle;
using namespace types::type_literals;

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

Rectangle::Rectangle(const Vector2 &size, const Color &color, NonOwningPtr<materials::Material> material, bool visible) :
	Rectangle{vector3::Zero, size, color, material, visible}
{
	//Empty
}

Rectangle::Rectangle(const Vector3 &position, const Vector2 &size, const Color &color,
	NonOwningPtr<materials::Material> material, bool visible) :

	Rectangle{position, 0.0_r, size, color, material, visible}
{
	//Empty
}

Rectangle::Rectangle(const Vector3 &position, real rotation, const Vector2 &size, const Color &color,
	NonOwningPtr<materials::Material> material, bool visible) :

	Shape{detail::rectangle_vertices(position, rotation, size, color), material, visible},
	
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

	Shape{detail::rectangle_vertices(position, rotation, size, color), visible},

	position_{position},
	rotation_{rotation},
	size_{size}
{
	//Empty
}

} //ion::graphics::scene::shapes