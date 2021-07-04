/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonSphere.h
-------------------------------------------
*/

#ifndef ION_SPHERE_H
#define ION_SPHERE_H

#include <cmath>

#include "IonColor.h"
#include "IonVector2.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::utilities
{
	using namespace types::type_literals;
	class Matrix3;

	namespace sphere::detail
	{
		constexpr auto shape_factor = 4.0_r / 3.0_r * ion::utilities::math::Pi;
		constexpr auto four_pi = ion::utilities::math::TwoPi * 2.0_r;


		inline auto area_to_radius(real area) noexcept
		{
			return std::sqrt(area / ion::utilities::math::Pi);
		}

		constexpr auto circumference_to_radius(real circumference) noexcept
		{
			return circumference / ion::utilities::math::TwoPi;
		}

		constexpr auto diameter_to_radius(real diameter) noexcept
		{
			return diameter * 0.5_r;
		}

		inline auto surface_area_to_radius(real area) noexcept
		{
			return std::sqrt(area / four_pi);
		}

		inline auto volume_to_radius(real volume) noexcept
		{
			return std::cbrt(volume / shape_factor); //Cube root
		}


		constexpr auto radius_to_area(real radius) noexcept
		{
			return radius * radius * ion::utilities::math::Pi;
		}

		constexpr auto radius_to_circumference(real radius) noexcept
		{
			return radius * ion::utilities::math::TwoPi;
		}

		constexpr auto radius_to_diameter(real radius) noexcept
		{
			return radius * 2.0_r;
		}

		constexpr auto radius_to_surface_area(real radius) noexcept
		{
			return radius * radius * four_pi;
		}

		constexpr auto radius_to_volume(real radius) noexcept
		{
			return radius * radius * radius * shape_factor;
		}


		/*
			Graphics API
		*/

		void draw_bounds(real radius, const Vector2 &center, const Color &color, int steps) noexcept;
	} //sphere::detail


	class Sphere final
	{
		private:

			real radius_ = 0.0_r;
			Vector2 center_;

		public:

			Sphere() = default;

			//Constructs a new sphere from the given radius and center
			Sphere(real radius, const Vector2 &center = vector2::Zero) noexcept;


			/*
				Static sphere conversions
			*/

			//Returns a new sphere from the given (circle) area and center
			[[nodiscard]] static Sphere Area(real area, const Vector2 &center = vector2::Zero) noexcept;

			//Returns a new sphere from the given circumference/perimeter and center
			[[nodiscard]] static Sphere Circumference(real circumference, const Vector2 &center = vector2::Zero) noexcept;

			//Returns a new sphere from the given diameter and center
			[[nodiscard]] static Sphere Diameter(real diameter, const Vector2 &center = vector2::Zero) noexcept;

			//Returns a new sphere from the given surface area and center
			[[nodiscard]] static Sphere SurfaceArea(real surface_area, const Vector2 &center = vector2::Zero) noexcept;

			//Returns a new sphere from the given volume and center
			[[nodiscard]] static Sphere Volume(real volume, const Vector2 &center = vector2::Zero) noexcept;


			/*
				Operators
			*/

			//Checks if two spheres are equal (both radii are equal)
			[[nodiscard]] inline auto operator==(const Sphere &rhs) const noexcept
			{
				return radius_ == rhs.radius_;
			}

			//Checks if two spheres are different (both radii are different)
			[[nodiscard]] inline auto operator!=(const Sphere &rhs) const noexcept
			{
				return !(*this == rhs);
			}


			/*
				Modifiers
			*/

			//Sets the center of the sphere to the given value
			inline void Center(const Vector2 &center) noexcept
			{
				center_ = center;
			}

			//Sets the radius of the sphere to the given value
			inline void Radius(real radius) noexcept
			{
				radius_ = radius;
			}


			/*
				Observers
			*/

			//Returns the radius of the sphere
			[[nodiscard]] inline auto Center() const noexcept
			{
				return center_;
			}

			//Returns the center of the sphere
			[[nodiscard]] inline auto Radius() const noexcept
			{
				return radius_;
			}


			/*
				Sphere conversions
			*/

			//Returns the (circle) area of the sphere
			[[nodiscard]] real ToArea() const noexcept;

			//Returns the circumference/perimeter of the sphere
			[[nodiscard]] real ToCircumference() const noexcept;

			//Returns the diameter of the sphere
			[[nodiscard]] real ToDiameter() const noexcept;

			//Returns the surface area of the sphere
			[[nodiscard]] real ToSurfaceArea() const noexcept;

			//Returns the volume of the sphere
			[[nodiscard]] real ToVolume() const noexcept;


			/*
				Containing
			*/

			//Returns true if this sphere contains the given sphere
			[[nodiscard]] bool Contains(const Sphere &sphere) const noexcept;

			//Returns true if this sphere contains the given point
			[[nodiscard]] bool Contains(const Vector2 &point) const noexcept;


			/*
				Intersecting
			*/

			//Returns true if this sphere intersects the given sphere
			[[nodiscard]] bool Intersects(const Sphere &sphere) const noexcept;

			//Returns true if this sphere intersects the given point
			[[nodiscard]] bool Intersects(const Vector2 &point) const noexcept;


			/*
				Merging
			*/

			//Merge this sphere with the given sphere
			Sphere& Merge(const Sphere &sphere) noexcept;

			//Merge this sphere with the given point
			Sphere& Merge(const Vector2 &point) noexcept;


			//Merge this sphere with the given sphere
			//Returns the result as a copy
			[[nodiscard]] Sphere MergeCopy(const Sphere &sphere) const noexcept;

			//Merge this sphere with the given point
			//Returns the result as a copy
			[[nodiscard]] Sphere MergeCopy(const Vector2 &point) const noexcept;


			/*
				Scaling
			*/

			//Scale sphere by the given factor
			Sphere& Scale(real factor) noexcept;

			//Scale sphere by the given factor
			//Returns the result as a copy
			[[nodiscard]] Sphere ScaleCopy(real factor) const noexcept;


			/*
				Transforming
			*/

			//Transform sphere based on the given matrix
			Sphere& Transform(const Matrix3 &matrix) noexcept;

			//Transform sphere based on the given matrix
			//Returns the result as a copy
			[[nodiscard]] Sphere TransformCopy(const Matrix3 &matrix) const noexcept;


			/*
				Translating
			*/

			//Translate sphere by the given vector
			Sphere& Translate(const Vector2 &vector) noexcept;

			//Translate sphere by the given vector
			//Returns the result as a copy
			[[nodiscard]] Sphere TranslateCopy(const Vector2 &vector) const noexcept;


			/*
				Drawing
			*/

			//Draw the bounds of this sphere with the given color and steps
			void Draw(const Color &color, int steps = 32) const noexcept;
	};


	namespace sphere
	{
		/*
			Predefined constant spheres
		*/

		inline const auto Unit = Sphere{1.0_r};
	} //sphere
} //ion::graphics::utilities

#endif