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

mesh::Vertices rectangle_vertices(const Vector3 &position, const Vector2 &size, const Color &color)
{
	auto [x, y, z] = position.XYZ();
	auto [half_width, half_height] = (size * 0.5_r).XY();

	return {{{x - half_width, y + half_height, z}, vector3::UnitZ, color},
			{{x - half_width, y - half_height, z}, vector3::UnitZ, color},
			{{x + half_width, y - half_height, z}, vector3::UnitZ, color},
			{{x + half_width, y - half_height, z}, vector3::UnitZ, color},
			{{x + half_width, y + half_height, z}, vector3::UnitZ, color},
			{{x - half_width, y + half_height, z}, vector3::UnitZ, color}};
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

	Shape{detail::rectangle_vertices(position, size, color), material, visible},
	
	position_{position},
	size_{size}
{
	//Empty
}

//Public

Rectangle::Rectangle(const Vector2 &size, const Color &color, bool visible) :
	Rectangle{vector3::Zero, size, color, visible}
{
	//Empty
}

Rectangle::Rectangle(const Vector3 &position, const Vector2 &size, const Color &color, bool visible) :

	Shape{detail::rectangle_vertices(position, size, color), visible},

	position_{position},
	size_{size}
{
	//Empty
}

} //ion::graphics::scene::shapes