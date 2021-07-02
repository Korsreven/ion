/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonObb.cpp
-------------------------------------------
*/

#include "IonObb.h"

#include <limits>

#include "IonMatrix3.h"
#include "graphics/IonGraphicsAPI.h"

#undef max

namespace ion::graphics::utilities
{

using namespace obb;

namespace obb::detail
{

/*
	Graphics API
*/

void draw_bounds(const std::array<Vector2, 4> &corners, const Color &color) noexcept
{
	#if defined(ION_DOUBLE_PRECISION) || defined(ION_EXTENDED_PRECISION)
	glColor4dv(color.Channels());
	#else
	glColor4fv(color.Channels());
	#endif

	glBegin(GL_LINE_STRIP);

	#if defined(ION_DOUBLE_PRECISION) || defined(ION_EXTENDED_PRECISION)
	glVertex2dv(corners[3].Components());
	glVertex2dv(corners[0].Components());
	glVertex2dv(corners[1].Components());
	glVertex2dv(corners[2].Components());
	glVertex2dv(corners[3].Components());
	#else
	glVertex2fv(corners[3].Components());
	glVertex2fv(corners[0].Components());
	glVertex2fv(corners[1].Components());
	glVertex2fv(corners[2].Components());
	glVertex2fv(corners[3].Components());
	#endif

	glEnd();
}

} //obb::detail


//Obb
//Private

bool Obb::intersects_one_way(const Obb &obb) const noexcept
{
	std::array axes{corners_[1] - corners_[0],
					corners_[3] - corners_[0]};

	//Axes
	for (auto &axis : axes)
	{
		axis /= axis.SquaredLength();

		auto dot_min = std::numeric_limits<real>::max(); //max value
		auto dot_max = -std::numeric_limits<real>::max(); //min value

		//Corners
		for (auto &corner : obb.corners_)
		{
			auto dot = corner.DotProduct(axis);
			
			if (dot < dot_min)
				dot_min = dot;
			else if (dot > dot_max)
				dot_max = dot;
		}

		auto origin = corners_[0].DotProduct(axis);

		//Substract off the origin and see if [tmin, tmax] intersects [0, 1]
		if (dot_min > 1.0_r + origin || dot_max < origin)
			//There was no intersection along this dimension
            //The boxes cannot intersect
			return false;
	}

	//There was no dimension along which there is no intersection
	//Therefore the boxes overlap
	return true;
}


//Public

Obb::Obb(const Vector2 &min, const Vector2 &max) noexcept :
	corners_{min, {max.X(), min.Y()},
			 max, {min.X(), max.Y()}}
{

}

Obb::Obb(const Aabb &aabb) noexcept :
	Obb{aabb.Min(), aabb.Max()}
{
	//Empty
}


/*
	Static obb conversions
*/

Obb Obb::Area(real area, const Vector2 &center) noexcept
{
	auto [min, max] = aabb::detail::area_to_minmax(area);
	return {min + center, max + center};
}

Obb Obb::HalfSize(const Vector2 &half_size, const Vector2 &center) noexcept
{
	return {center - half_size, center + half_size};
}

Obb Obb::Size(const Vector2 &size, const Vector2 &center) noexcept
{
	return HalfSize(size * 0.5_r, center);
}


/*
	Modifiers
*/

void Obb::Center(const Vector2 &center) noexcept
{
	Translate(center - Center());
}


void Obb::Extents(const Vector2 &min, const Vector2 &max) noexcept
{
	*this = Aabb{min, max};
}


/*
	Observers
*/

bool Obb::AxisAligned() const noexcept
{
	auto axis = corners_[1] - corners_[0];
	return axis.X() == 0.0_r || axis.Y() == 0.0_r;
}

Vector2 Obb::Center() const noexcept
{
	return (corners_[0] + corners_[1] + corners_[2] + corners_[3]) * 0.25_r;
}

bool Obb::Empty() const noexcept
{
	return corners_[0] == corners_[1] &&
		   corners_[0] == corners_[3];
}


/*
	Obb conversions
*/

real Obb::ToArea() const noexcept
{
	return aabb::detail::minmax_to_area(corners_[0], corners_[0] + ToSize());
}

Vector2 Obb::ToHalfSize() const noexcept
{
	return ToSize() * 0.5_r;
}

Vector2 Obb::ToSize() const noexcept
{
	return (corners_[1] - corners_[0] -
		    corners_[3] - corners_[0]).Length();
}


/*
	Containing
*/

bool Obb::Contains(const Vector2 &point) const noexcept
{
	return Intersects(point);
}


/*
	Intersecting
*/

bool Obb::Intersects(const Obb &obb) const noexcept
{
	return intersects_one_way(obb) && obb.intersects_one_way(*this);
}

bool Obb::Intersects(const Vector2 &point) const noexcept
{
	Obb obb{point, point + 1.0_r};
	return intersects_one_way(obb) && obb.intersects_one_way(*this);
}


/*
	Rotating
*/

Obb& Obb::Rotate(real angle) noexcept
{
	auto center = Center();
	corners_[0].Rotate(angle, center);
	corners_[1].Rotate(angle, center);
	corners_[2].Rotate(angle, center);
	corners_[3].Rotate(angle, center);
	return *this;
}

Obb Obb::RotateCopy(real angle) const noexcept
{
	return Obb(*this).Rotate(angle);
}


/*
	Scaling
*/

Obb& Obb::Scale(const Vector2 &vector) noexcept
{
	corners_[0] *= vector;
	corners_[1] *= vector;
	corners_[2] *= vector;
	corners_[3] *= vector;
	return *this;
}

Obb Obb::ScaleCopy(const Vector2 &vector) const noexcept
{
	return Obb(*this).Scale(vector);
}


/*
	Transforming
*/

Obb& Obb::Transform(const Matrix3 &matrix) noexcept
{
	return *this = TransformCopy(matrix);
}

Obb Obb::TransformCopy(const Matrix3 &matrix) const noexcept
{
	auto half_size = ToHalfSize();
	auto [x, y] = half_size.XY();

	#ifdef ION_LEFT_HAND_ROTATION
	//Left-hand rotation CW (Direct3D)
	return HalfSize({matrix.M00() * x + matrix.M10() * y,
					 matrix.M01() * x + matrix.M11() * y},
					 matrix.TransformPoint(Center()));
	#else
	//Right-hand rotation CCW (OpenGL)
	return HalfSize({matrix.M00() * x + matrix.M01() * y,
					 matrix.M10() * x + matrix.M11() * y},
					 matrix.TransformPoint(Center()));
	#endif
}


/*
	Translating
*/

Obb& Obb::Translate(const Vector2 &vector) noexcept
{
	corners_[0] += vector;
	corners_[1] += vector;
	corners_[2] += vector;
	corners_[3] += vector;
	return *this;
}

Obb Obb::TranslateCopy(const Vector2 &vector) const noexcept
{
	return Obb(*this).Translate(vector);
}


/*
	Drawing
*/

void Obb::Draw(const Color &color) noexcept
{
	detail::draw_bounds(corners_, color);
}

} //ion::graphics::utilities