/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonAabb.cpp
-------------------------------------------
*/

#include "IonAabb.h"

#include <algorithm>

#include "IonMatrix3.h"
#include "graphics/IonGraphicsAPI.h"
#include "utilities\IonMath.h"

namespace ion::graphics::utilities
{

using namespace aabb;
using namespace ion::utilities;

namespace aabb::detail
{

std::pair<Vector2, Vector2> minmax_point(const std::vector<Vector2> &points) noexcept
{
	if (std::empty(points))
		return {};

	//Left most point (min x)
	//Right most point (max x)
	auto [left_most, right_most] =
		std::minmax_element(std::cbegin(points), std::cend(points),
			[](const Vector2 &first, const Vector2 &second) noexcept
			{
				return first.X() < second.X();
			});

	//Bottom most point (min y)
	//Top most point (max y)
	auto [bottom_most, top_most] =
		std::minmax_element(std::cbegin(points), std::cend(points),
			[](const Vector2 &first, const Vector2 &second) noexcept
			{
				return first.Y() < second.Y();
			});

	return {{left_most->X(), bottom_most->Y()},
			{right_most->X(), top_most->Y()}};
}


/*
	Graphics API
*/

void draw_bounds(const Vector2 &min, const Vector2 &max, const Color &color) noexcept
{
	#if defined(ION_DOUBLE_PRECISION) || defined(ION_EXTENDED_PRECISION)
	glColor4dv(color.Channels());
	#else
	glColor4fv(color.Channels());
	#endif

	auto [min_x, min_y] = min.XY();
	auto [max_x, max_y] = max.XY();

	glBegin(GL_LINE_STRIP);

	#if defined(ION_DOUBLE_PRECISION) || defined(ION_EXTENDED_PRECISION)
	glVertex2d(min_x, max_y);
	glVertex2d(min_x, min_y);
	glVertex2d(max_x, min_y);
	glVertex2d(max_x, max_y);
	glVertex2d(min_x, max_y);
	#else
	glVertex2f(min_x, max_y);
	glVertex2f(min_x, min_y);
	glVertex2f(max_x, min_y);
	glVertex2f(max_x, max_y);
	glVertex2f(min_x, max_y);
	#endif

	glEnd();
}

} //aabb::detail


//Aabb

Aabb::Aabb(const Vector2 &min, const Vector2 &max) noexcept :
	min_{detail::valid_extents(min, max) ? min : 0.0_r},
	max_{detail::valid_extents(min, max) ? max : 0.0_r}
{
	//Empty
}


/*
	Static aabb conversions
*/

Aabb Aabb::Area(real area, const Vector2 &center) noexcept
{
	auto [min, max] = detail::area_to_minmax(area);
	return {min + center, max + center};
}

Aabb Aabb::Enclose(const std::vector<Vector2> &points) noexcept
{
	auto [min, max] = detail::minmax_point(points);
	return {min, max};
}

Aabb Aabb::Enclose(const std::vector<Aabb> &boxes) noexcept
{
	if (std::empty(boxes))
		return {};

	//Left most point (min x)
	auto left_most =
		std::min_element(std::cbegin(boxes), std::cend(boxes),
			[](const Aabb &first, const Aabb &second) noexcept
			{
				return first.min_.X() < second.min_.X();
			});

	//Right most point (max x)
	auto right_most =
		std::max_element(std::cbegin(boxes), std::cend(boxes),
			[](const Aabb &first, const Aabb &second) noexcept
			{
				return first.max_.X() < second.max_.X();
			});

	//Bottom most point (min y)
	auto bottom_most =
		std::min_element(std::cbegin(boxes), std::cend(boxes),
			[](const Aabb &first, const Aabb &second) noexcept
			{
				return first.min_.Y() < second.min_.Y();
			});

	//Top most point (max y)
	auto top_most =
		std::max_element(std::cbegin(boxes), std::cend(boxes),
			[](const Aabb &first, const Aabb &second) noexcept
			{
				return first.max_.Y() < second.max_.Y();
			});

	return {{left_most->min_.X(), bottom_most->min_.Y()},
			{right_most->max_.X(), top_most->max_.Y()}};
}

Aabb Aabb::HalfSize(const Vector2 &half_size, const Vector2 &center) noexcept
{
	return {center - half_size, center + half_size};
}

Aabb Aabb::Size(const Vector2 &size, const Vector2 &center) noexcept
{
	return HalfSize(size * 0.5_r, center);
}


/*
	Modifiers
*/

void Aabb::Center(const Vector2 &center) noexcept
{
	Translate(center - Center());
}

void Aabb::Extents(const Vector2 &min, const Vector2 &max) noexcept
{
	if (detail::valid_extents(min, max))
	{
		min_ = min;
		max_ = max;
	}
}


/*
	Observers
*/

Vector2 Aabb::Center() const noexcept
{
	return min_.Midpoint(max_);
}

bool Aabb::Empty() const noexcept
{
	return min_ == max_;
}


/*
	Aabb conversions
*/

real Aabb::ToArea() const noexcept
{
	return detail::minmax_to_area(min_, max_);
}

Vector2 Aabb::ToHalfSize() const noexcept
{
	return ToSize() * 0.5_r;
}

Vector2 Aabb::ToSize() const noexcept
{
	return max_ - min_;
}


/*
	Containing
*/

bool Aabb::Contains(const Aabb &aabb) const noexcept
{
	return min_.X() <= aabb.min_.X() && min_.Y() <= aabb.min_.Y() &&
		   max_.X() >= aabb.max_.X() && max_.Y() >= aabb.max_.Y();
}

bool Aabb::Contains(const Vector2 &point) const noexcept
{
	return Intersects(point);
}

/*
	Intersecting
*/

bool Aabb::Intersects(const Aabb &aabb) const noexcept
{
	return max_.X() >= aabb.min_.X() &&
		   max_.Y() >= aabb.min_.Y() &&
			 
		   min_.X() <= aabb.max_.X() &&
		   min_.Y() <= aabb.max_.Y();	
}

bool Aabb::Intersects(const Vector2 &point) const noexcept
{
	return point.X() >= min_.X() && point.X() <= max_.X() &&
		   point.Y() >= min_.Y() && point.Y() <= max_.Y();
}


Aabb Aabb::Intersection(const Aabb &aabb) const noexcept
{
	auto intersect_min = min_.CeilCopy(aabb.min_);
	auto intersect_max = max_.FloorCopy(aabb.max_);

	return intersect_min.X() < intersect_max.X() &&
		   intersect_min.Y() < intersect_max.Y() ?	
		//Found intersection
		Aabb{intersect_min, intersect_max} :
		//No intersection
		aabb::Zero;
}


/*
	Merging
*/

Aabb& Aabb::Merge(const Aabb &aabb) noexcept
{
	return *this = MergeCopy(aabb);
}

Aabb& Aabb::Merge(const Vector2 &point) noexcept
{
	return *this = MergeCopy(point);
}


Aabb Aabb::MergeCopy(const Aabb &aabb) const noexcept
{
	if (aabb.Empty())
		return *this;
	else if (Empty())
		return aabb;

	return {min_.FloorCopy(aabb.min_), max_.CeilCopy(aabb.max_)};
}

Aabb Aabb::MergeCopy(const Vector2 &point) const noexcept
{
	return {min_.FloorCopy(point), max_.CeilCopy(point)};
}


/*
	Scaling
*/

Aabb& Aabb::Scale(const Vector2 &vector) noexcept
{
	return *this = ScaleCopy(vector);
}

Aabb Aabb::ScaleCopy(const Vector2 &vector) const noexcept
{
	return {min_ * vector, max_ * vector};
}


/*
	Transforming
*/

Aabb& Aabb::Transform(const Matrix3 &matrix) noexcept
{
	return *this = TransformCopy(matrix);
}

Aabb Aabb::TransformCopy(const Matrix3 &matrix) const noexcept
{
	auto half_size = ToHalfSize();
	auto [x, y] = half_size.XY();

	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
		#ifdef ION_LEFT_HAND_ROTATION
		//Left-hand rotation CW
		half_size = Vector2{math::Abs(matrix.M00()) * x + math::Abs(matrix.M01()) * y,
							math::Abs(matrix.M10()) * x + math::Abs(matrix.M11()) * y};
		#else
		//Right-hand rotation CCW
		half_size = Vector2{math::Abs(matrix.M00()) * x + math::Abs(matrix.M10()) * y,
							math::Abs(matrix.M01()) * x + math::Abs(matrix.M11()) * y};
		#endif
	#else
	//Column-major layout (OpenGL)
		#ifdef ION_LEFT_HAND_ROTATION
		//Left-hand rotation CW
		half_size = Vector2{math::Abs(matrix.M00()) * x + math::Abs(matrix.M10()) * y,
							math::Abs(matrix.M01()) * x + math::Abs(matrix.M11()) * y};
		#else
		//Right-hand rotation CCW
		half_size = Vector2{math::Abs(matrix.M00()) * x + math::Abs(matrix.M01()) * y,
							math::Abs(matrix.M10()) * x + math::Abs(matrix.M11()) * y};
		#endif
	#endif

	return HalfSize(half_size, matrix.TransformPoint(Center()));
}


/*
	Translating
*/

Aabb& Aabb::Translate(const Vector2 &vector) noexcept
{
	return *this = TranslateCopy(vector);
}

Aabb Aabb::TranslateCopy(const Vector2 &vector) const noexcept
{
	return {min_ + vector, max_ + vector};
}


/*
	Drawing
*/

void Aabb::Draw(const Color &color) const noexcept
{
	detail::draw_bounds(min_, max_, color);
}

} //ion::graphics::utilities