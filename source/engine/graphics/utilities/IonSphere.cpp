/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonSphere.cpp
-------------------------------------------
*/

#include "IonSphere.h"

#include "IonMatrix3.h"
#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::utilities
{

using namespace sphere;
using namespace ion::utilities;

namespace sphere::detail
{

/*
	Graphics API
*/

void draw_bounds(real radius, const Vector2 &center, const Color &color, int steps) noexcept
{
	#if defined(ION_DOUBLE_PRECISION) || defined(ION_EXTENDED_PRECISION)
	glColor4dv(color.Channels());
	#else
	glColor4fv(color.Channels());
	#endif

	auto angle = 0.0_r;
	auto delta_angle = math::TwoPi / steps;

	glBegin(GL_LINE_STRIP);

	for (auto i = 0; i < steps; ++i, angle += delta_angle)
	{
		auto v =
			center + Vector2{radius * math::Cos(angle), radius * math::Sin(angle)};

		#if defined(ION_DOUBLE_PRECISION) || defined(ION_EXTENDED_PRECISION)
		glVertex2dv(v.Components());
		#else
		glVertex2fv(v.Components());
		#endif
	}

	glEnd();
}

} //sphere::detail


//Sphere

Sphere::Sphere(real radius, const Vector2 &center) noexcept :
	radius_{radius}, center_{center}
{
	//Empty
}


/*
	Static sphere conversions
*/

Sphere Sphere::Area(real area, const Vector2 &center) noexcept
{
	return {detail::area_to_radius(area), center};
}

Sphere Sphere::Circumference(real circumference, const Vector2 &center) noexcept
{
	return {detail::circumference_to_radius(circumference), center};
}

Sphere Sphere::Diameter(real diameter, const Vector2 &center) noexcept
{
	return {detail::diameter_to_radius(diameter), center};
}

Sphere Sphere::SurfaceArea(real surface_area, const Vector2 &center) noexcept
{
	return {detail::surface_area_to_radius(surface_area), center};
}

Sphere Sphere::Volume(real volume, const Vector2 &center) noexcept
{
	return {detail::volume_to_radius(volume), center};
}


/*
	Sphere conversions
*/

real Sphere::ToArea() const noexcept
{
	return detail::radius_to_area(radius_);
}

real Sphere::ToCircumference() const noexcept
{
	return detail::radius_to_circumference(radius_);
}

real Sphere::ToDiameter() const noexcept
{
	return detail::radius_to_diameter(radius_);
}

real Sphere::ToSurfaceArea() const noexcept
{
	return detail::radius_to_surface_area(radius_);
}

real Sphere::ToVolume() const noexcept
{
	return detail::radius_to_volume(radius_);
}


/*
	Containing
*/

bool Sphere::Contains(const Sphere &sphere) const noexcept
{
	return (sphere.center_ - center_).SquaredLength() +
		sphere.radius_ * sphere.radius_ < radius_ * radius_;
}

bool Sphere::Contains(const Vector2 &point) const noexcept
{
	return Intersects(point);
}


/*
	Intersecting
*/

bool Sphere::Intersects(const Sphere &sphere) const noexcept
{
	auto radii = radius_ + sphere.radius_;
	return (sphere.center_ - center_).SquaredLength() <= radii * radii;
}

bool Sphere::Intersects(const Vector2 &point) const noexcept
{
	return (point - center_).SquaredLength() <= radius_ * radius_;
}


/*
	Merging
*/

Sphere& Sphere::Merge(const Sphere &sphere) noexcept
{
	return *this = MergeCopy(sphere);
}

Sphere& Sphere::Merge(const Vector2 &point) noexcept
{
	return *this = MergeCopy(point);
}

Sphere Sphere::MergeCopy(const Sphere &sphere) const noexcept
{
	auto center_diff = sphere.center_ - center_;
	auto length_squared = center_diff.SquaredLength();

	auto radius_diff = sphere.radius_ - radius_;
	auto radius_diff_squared = radius_diff * radius_diff;

	if (radius_diff_squared >= length_squared)
		return radius_diff_squared >= 0.0_r ?
			sphere :
			*this;
	else
	{
		auto length = std::sqrt(length_squared);
		auto t = (length + radius_diff) / (2.0_r * length);
		return {(radius_ + sphere.radius_ + length) / 2.0_r,
				 center_ + center_diff + t};
	}
}

Sphere Sphere::MergeCopy(const Vector2 &point) const noexcept
{
	return MergeCopy(Sphere{0.0_r, point});
}


/*
	Scaling
*/

Sphere& Sphere::Scale(real factor) noexcept
{
	return *this = ScaleCopy(factor);
}

Sphere Sphere::ScaleCopy(real factor) const noexcept
{
	return {radius_ * factor, center_};
}


/*
	Transforming
*/

Sphere& Sphere::Transform(const Matrix3 &matrix) noexcept
{
	return *this = TransformCopy(matrix);
}

Sphere Sphere::TransformCopy(const Matrix3 &matrix) const noexcept
{
	return {radius_ * matrix.ToScaling().Max(),
			matrix.TransformPoint(center_)};
}


/*
	Translating
*/

Sphere& Sphere::Translate(const Vector2 &vector) noexcept
{
	return *this = TranslateCopy(vector);
}

Sphere Sphere::TranslateCopy(const Vector2 &vector) const noexcept
{
	return {radius_, center_ + vector};
}


/*
	Drawing
*/

void Sphere::Draw(const Color &color, int steps) noexcept
{
	detail::draw_bounds(radius_, center_, color, steps);
}

} //ion::graphics::utilities