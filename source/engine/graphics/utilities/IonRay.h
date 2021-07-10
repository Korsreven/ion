/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonRay.h
-------------------------------------------
*/

#ifndef ION_RAY_H
#define ION_RAY_H

#include <utility>

#include "IonAabb.h"
#include "IonSphere.h"
#include "IonVector2.h"
#include "types/IonTypes.h"

namespace ion::graphics::utilities
{
	namespace ray::detail
	{
	} //ray::detail


	class Ray final
	{
		private:

			Vector2 origin_;
			Vector2 direction_;

		public:

			Ray() = default;

			//Constructs a new ray from the given origin and direction
			Ray(const Vector2 &origin, const Vector2 &direction) noexcept;


			/*
				Operators
			*/

			//Checks if two rays are equal (both directions are equal)
			[[nodiscard]] inline auto operator==(const Ray &rhs) const noexcept
			{
				return direction_ == rhs.direction_;
			}

			//Checks if two rays are different (both directions are different)
			[[nodiscard]] inline auto operator!=(const Ray &rhs) const noexcept
			{
				return !(*this == rhs);
			}


			/*
				Modifiers
			*/

			//Sets the origin of the ray to the given value
			inline void Origin(const Vector2 &origin) noexcept
			{
				origin_ = origin;
			}

			//Sets the direction of the ray to the given value
			inline void Direction(const Vector2 &direction) noexcept
			{
				direction_ = direction;
			}


			/*
				Observers
			*/

			//Returns the origin of the ray
			[[nodiscard]] inline auto Origin() const noexcept
			{
				return origin_;
			}

			//Returns the direction of the ray
			[[nodiscard]] inline auto Direction() const noexcept
			{
				return direction_;
			}


			//Returns the position of a point at the given unit along the ray
			Vector2 Point(real unit) noexcept;


			/*
				Intersecting
			*/

			//Returns true if this ray intersects the given aabb
			[[nodiscard]] std::pair<bool, real> Intersects(const Aabb &aabb) const noexcept;

			//Returns true if this ray intersects the given sphere
			[[nodiscard]] std::pair<bool, real> Intersects(const Sphere &sphere) const noexcept;

			//Returns true if this ray intersects the given point
			[[nodiscard]] std::pair<bool, real> Intersects(const Vector2 &point) const noexcept;
	};


	namespace ray
	{
		/*
			Predefined constant rays
		*/

		inline const auto Zero = Ray{0.0_r, vector2::Zero};
		inline const auto Unit = Ray{0.0_r, vector2::UnitScale};
	} //ray
} //ion::graphics::utilities

#endif