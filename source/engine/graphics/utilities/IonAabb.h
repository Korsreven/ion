/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonAabb.h
-------------------------------------------
*/

#ifndef ION_AABB_H
#define ION_AABB_H

#include <cmath>
#include <utility>
#include <vector>

#include "IonColor.h"
#include "IonVector2.h"
#include "types/IonTypes.h"

namespace ion::graphics::utilities
{
	using namespace types::type_literals;
	class Matrix3;

	namespace aabb::detail
	{
		inline auto area_to_minmax(real area) noexcept
		{
			auto minmax = std::sqrt(area) * 0.5_r;
			return std::pair{-minmax, minmax};
		}

		inline auto minmax_to_area(const Vector2 &min, const Vector2 &max) noexcept
		{
			auto size = max - min;
			return size.X() * size.Y();
		}

		inline auto valid_extents(const Vector2 &min, const Vector2 &max) noexcept
		{
			return min.X() <= max.X() && min.Y() <= max.Y();
		}


		std::pair<Vector2, Vector2> minmax_point(const std::vector<Vector2> &points) noexcept;


		/**
			@name Graphics API
			@{
		*/

		void draw_bounds(const Vector2 &min, const Vector2 &max, const Color &color) noexcept;

		///@}
	} //aabb::detail


	///@brief A class representing a two-dimensional axis-aligned bounding box (AABB)
	class Aabb final
	{
		private:

			/*
					max
				x-----x
				|     |
				|     |
				x-----x
				min
			*/

			Vector2 min_;
			Vector2 max_;

		public:

			///@brief Default constructor
			Aabb() = default;

			///@brief Constructs a new aabb from the given min and max vectors
			Aabb(const Vector2 &min, const Vector2 &max) noexcept;


			/**
				@name Static aabb conversions
				@{
			*/

			///@brief Returns a new aabb from the given area and center
			[[nodiscard]] static Aabb Area(real area, const Vector2 &center = vector2::Zero) noexcept;

			///@brief Returns a new aabb from the given points
			///@details The aabb created will be the minimum/smallest bounding box (enclosing box)
			[[nodiscard]] static Aabb Enclose(const std::vector<Vector2> &points) noexcept;

			///@brief Returns a new aabb from the given aabbs
			///@details The aabb created will be the minimum/smallest bounding box (enclosing box)
			[[nodiscard]] static Aabb Enclose(const std::vector<Aabb> &boxes) noexcept;

			///@brief Returns a new aabb from the given half-size and center
			[[nodiscard]] static Aabb HalfSize(const Vector2 &half_size, const Vector2 &center = vector2::Zero) noexcept;

			///@brief Returns a new aabb from the given size and center
			[[nodiscard]] static Aabb Size(const Vector2 &size, const Vector2 &center = vector2::Zero) noexcept;

			///@}

			/**
				@name Operators
				@{
			*/

			///@brief Checks if two aabbs are equal (both min and max are equal)
			[[nodiscard]] inline auto operator==(const Aabb &rhs) const noexcept
			{
				return min_ == rhs.min_ && max_ == rhs.max_;
			}

			///@brief Checks if two aabbs are different (min or max are different)
			[[nodiscard]] inline auto operator!=(const Aabb &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the max vector (top right point) to the given value
			inline void Max(const Vector2 &max) noexcept
			{
				if (aabb::detail::valid_extents(min_, max))
					max_ = max;
			}

			///@brief Sets the min vector (bottom left point) to the given value
			inline void Min(const Vector2 &min) noexcept
			{
				if (aabb::detail::valid_extents(min, max_))
					min_ = min;
			}


			///@brief Sets the center of the aabb to the given value
			void Center(const Vector2 &center) noexcept;

			///@brief Sets the extents of the aabb to the given min and max values
			///@details This is faster than modify via min and then max
			void Extents(const Vector2 &min, const Vector2 &max) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the max vector (top right point)
			[[nodiscard]] inline auto& Max() const noexcept
			{
				return max_;
			}

			///@brief Returns the min vector (bottom left point)
			[[nodiscard]] inline auto& Min() const noexcept
			{
				return min_;
			}

			///@brief Returns the min and max vector
			[[nodiscard]] inline auto MinMax() const noexcept
			{
				return std::pair{min_, max_};
			}


			///@brief Returns the center of the aabb
			[[nodiscard]] Vector2 Center() const noexcept;

			///@brief Returns true if this aabb is empty
			///@details Meaning that the size is zero
			[[nodiscard]] bool Empty() const noexcept;

			///@}

			/**
				@name Aabb conversions
				@{
			*/

			///@brief Returns the area of the aabb
			[[nodiscard]] real ToArea() const noexcept;

			///@brief Returns the half-size of the aabb
			[[nodiscard]] Vector2 ToHalfSize() const noexcept;

			///@brief Returns the size of the aabb
			[[nodiscard]] Vector2 ToSize() const noexcept;

			///@}

			/**
				@name Containing
				@{
			*/

			///@brief Returns true if this aabb contains the given aabb
			[[nodiscard]] bool Contains(const Aabb &aabb) const noexcept;

			///@brief Returns true if this aabb contains the given point
			[[nodiscard]] bool Contains(const Vector2 &point) const noexcept;

			///@}

			/**
				@name Intersecting
				@{
			*/

			///@brief Returns true if this aabb intersects the given aabb
			[[nodiscard]] bool Intersects(const Aabb &aabb) const noexcept;

			///@brief Returns true if this aabb intersects the given point
			[[nodiscard]] bool Intersects(const Vector2 &point) const noexcept;


			///@brief Returns the intersection of this aabb and the given aabb
			[[nodiscard]] Aabb Intersection(const Aabb &aabb) const noexcept;

			///@}

			/**
				@name Merging
				@{
			*/

			///@brief Merge this aabb with the given aabb
			Aabb& Merge(const Aabb &aabb) noexcept;

			///@brief Merge this aabb with the given point
			Aabb& Merge(const Vector2 &point) noexcept;


			///@brief Merge this aabb with the given aabb
			///@details Returns the result as a copy
			[[nodiscard]] Aabb MergeCopy(const Aabb &aabb) const noexcept;

			///@brief Merge this aabb with the given point
			///@details Returns the result as a copy
			[[nodiscard]] Aabb MergeCopy(const Vector2 &point) const noexcept;

			///@}

			/**
				@name Rotating
				@{
			*/

			///@brief Rotates aabb by the given angle (radians)
			Aabb& Rotate(real angle) noexcept;

			///@brief Rotates aabb by the given angle (radians)
			///@details Returns the result as a copy
			[[nodiscard]] Aabb RotateCopy(real angle) const noexcept;

			///@}

			/**
				@name Scaling
				@{
			*/

			///@brief Scales aabb by the given vector
			Aabb& Scale(const Vector2 &vector) noexcept;

			///@brief Scales aabb by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Aabb ScaleCopy(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Transforming
				@{
			*/

			///@brief Transforms aabb based on the given matrix
			Aabb& Transform(const Matrix3 &matrix) noexcept;

			///@brief Transforms aabb based on the given matrix
			///@details Returns the result as a copy
			[[nodiscard]] Aabb TransformCopy(const Matrix3 &matrix) const noexcept;

			///@}

			/**
				@name Translating
				@{
			*/

			///@brief Translates aabb by the given vector
			Aabb& Translate(const Vector2 &vector) noexcept;

			///@brief Translates aabb by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Aabb TranslateCopy(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Drawing
				@{
			*/

			///@brief Draws the bounds of this aabb with the given color
			void Draw(const Color &color) const noexcept;

			///@}
	};


	namespace aabb
	{
		/**
			@name Predefined constant aabbs
			@{
		*/

		inline const auto Zero = Aabb{vector2::Zero, vector2::Zero};
		inline const auto Unit = Aabb{-0.5_r, 0.5_r};

		///@}
	} //aabb
} //ion::graphics::utilities

#endif