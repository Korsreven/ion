/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonRay.cpp
-------------------------------------------
*/

#include "IonRay.h"

#include <cmath>
#include "utilities/IonMath.h"

namespace ion::graphics::utilities
{

using namespace ray;
using namespace types::type_literals;

namespace ray::detail
{

} //ray::detail


Ray::Ray(const Vector2 &origin, const Vector2 &direction) noexcept :
	
	origin_{origin},
	direction_{direction}
{
	//Empty
}


/*
	Observers
*/

Vector2 Ray::Point(real unit) noexcept
{
	return origin_ + direction_ * unit;
}


/*
	Intersecting
*/

bool Ray::Intersects(const Aabb &aabb) const noexcept
{
	aabb;
	return false;
}

bool Ray::Intersects(const Sphere &sphere) const noexcept
{
	sphere;
	return false;
}

bool Ray::Intersects(const Vector2 &point) const noexcept
{
	point;
	return false;
}

} //ion::graphics::utilities