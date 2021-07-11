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
using namespace ion::utilities;
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

std::pair<bool, real> Ray::Intersects(const Aabb &aabb) const noexcept
{
	if (aabb.Empty())
		return {false, 0.0_r};

	auto [x, y] = origin_.XY();
	auto [min_x, min_y] = aabb.Min().XY();
	auto [max_x, max_y] = aabb.Max().XY();

	//Ray origin is already inside aabb
	if (x > min_x && y > min_y &&
		x < max_x && y < max_y)

		return {true, 0.0_r};

	auto [dir_x, dir_y] = direction_.XY();
	auto hit = false;
	auto min_units = 0.0_r;

	//Min x
	if (x <= min_x && dir_x > 0.0_r)
	{
		if (auto units = (min_x - x) / dir_x; units >= 0.0)
		{
			auto hit_point = origin_ + direction_ * units;
			auto [hit_x, hit_y] = hit_point.XY();

			if (hit_y >= min_y && hit_y <= max_y &&
				(!hit || units < min_units))
			{
				hit = true;
				min_units = units; //Closest intersection (so far)
			}
		}
	}

	//Max x
	if (x >= max_x && dir_x < 0.0_r)
	{
		if (auto units = (max_x - x) / dir_x; units >= 0.0)
		{
			auto hit_point = origin_ + direction_ * units;
			auto [hit_x, hit_y] = hit_point.XY();

			if (hit_y >= min_y && hit_y <= max_y &&
				(!hit || units < min_units))
			{
				hit = true;
				min_units = units; //Closest intersection (so far)
			}
		}
	}

	//Min y
	if (y <= min_y && dir_y > 0.0_r)
	{
		if (auto units = (min_y - y) / dir_y; units >= 0.0)
		{
			auto hit_point = origin_ + direction_ * units;
			auto [hit_x, hit_y] = hit_point.XY();

			if (hit_x >= min_x && hit_x <= max_x &&
				(!hit || units < min_units))
			{
				hit = true;
				min_units = units; //Closest intersection (so far)
			}
		}
	}

	//Max y
	if (y >= max_y && dir_y < 0.0_r)
	{
		if (auto units = (max_y - y) / dir_y; units >= 0.0)
		{
			auto hit_point = origin_ + direction_ * units;
			auto [hit_x, hit_y] = hit_point.XY();

			if (hit_x >= min_x && hit_x <= max_x &&
				(!hit || units < min_units))
			{
				hit = true;
				min_units = units; //Closest intersection (so far)
			}
		}
	}

	return {hit, min_units};
}

std::pair<bool, real> Ray::Intersects(const Obb &obb) const noexcept
{
	if (obb.Empty())
		return {false, 0.0_r};

	auto c0 = obb.Corners()[0];
	auto c1 = obb.Corners()[1];
	auto c2 = obb.Corners()[2];
	
	//Rotate obb to align with axis
	auto angle = vector2::UnitX.SignedAngleBetween(c1 - c0);
	auto center = obb.Center();
	c0.Rotate(-angle, center); //Min
	c2.Rotate(-angle, center); //Max

	//Reduce problem to ray-aabb intersection, by rotating ray correspondingly
	return Ray{origin_.RotateCopy(angle, center), direction_.Deviant(angle)}.
		Intersects(Aabb{c0, c2});
}

std::pair<bool, real> Ray::Intersects(const Sphere &sphere) const noexcept
{
	if (sphere.Empty())
		return {false, 0.0_r};

	auto ray_origin = origin_ - sphere.Center();
	auto radius = sphere.Radius();

	//Ray origin is already inside sphere
	if (ray_origin.SquaredLength() <= radius * radius)
		return {true, 0.0_r};

	//Quadratics
	auto a = direction_.DotProduct(direction_);
	auto b = ray_origin.DotProduct(direction_) * 2.0_r;
	auto c = ray_origin.DotProduct(ray_origin) - radius * radius;

	//Check determinant
	if (auto det = b * b - 4 * a * c; det < 0.0_r)
		return {false, 0.0_r}; //No intersection

	else //Intersection
	{
		auto det_sqrt = std::sqrt(det);
		auto units = (-b - det_sqrt) / (2.0_r * a);

		if (units < 0.0_r)
			units = (-b + det_sqrt) / (2.0_r * a);

		return {true, units}; //Closest intersection
	}
}

std::pair<bool, real> Ray::Intersects(const Vector2 &point) const noexcept
{
	return Intersects(Aabb{point - math::Epsilon, point + math::Epsilon});
}

} //ion::graphics::utilities