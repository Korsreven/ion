/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonObb.h
-------------------------------------------
*/

#ifndef ION_OBB_H
#define ION_OBB_H

#include <array>

#include "IonAabb.h"
#include "IonColor.h"
#include "IonVector2.h"
#include "types/IonTypes.h"

namespace ion::graphics::utilities
{
	using namespace types::type_literals;
	class Matrix3;

	namespace obb::detail
	{
		/**
			@name Graphics API
			@{
		*/

		void draw_bounds(const std::array<Vector2, 4> &corners, const Color &color) noexcept;

		///@}
	} //obb::detail


	///@brief A class representing a two-dimensional oriented bounding box (OBB)
	class Obb final
	{
		private:

			/*
				3	  2
				x-----x
				|     |
				|     |
				x-----x
				0     1
			*/

			std::array<Vector2, 4> corners_{};


			bool intersects_one_way(const Obb &obb) const noexcept;

		public:

			///@brief Default constructor
			Obb() = default;

			///@brief Constructs a new obb from the given min and max vectors
			Obb(const Vector2 &min, const Vector2 &max) noexcept;

			///@brief Constructs a new obb from the given corners
			Obb(const Vector2 &bottom_left, const Vector2 &bottom_right,
				const Vector2 &top_left, const Vector2 &top_right) noexcept;

			///@brief Constructs a new obb from the given aabb
			Obb(const Aabb &aabb) noexcept;


			/**
				@name Static obb conversions
				@{
			*/

			///@brief Returns a new obb from the given area and center
			[[nodiscard]] static Obb Area(real area, const Vector2 &center = vector2::Zero) noexcept;

			///@brief Returns a new obb from the given half-size and center
			[[nodiscard]] static Obb HalfSize(const Vector2 &half_size, const Vector2 &center = vector2::Zero) noexcept;

			///@brief Returns a new obb from the given size and center
			[[nodiscard]] static Obb Size(const Vector2 &size, const Vector2 &center = vector2::Zero) noexcept;

			///@}

			/**
				@name Operators
				@{
			*/

			///@brief Assign new corner values from the given aabb
			inline auto& operator=(const Aabb &aabb) noexcept
			{
				auto [min, max] = aabb.MinMax();
				corners_[0] = min;
				corners_[1] = {max.X(), min.Y()};
				corners_[2] = max;
				corners_[3] = {min.X(), max.Y()};
				return *this;
			}

			///@brief Checks if two obb are equal (all corners are equal)
			[[nodiscard]] inline auto operator==(const Obb &rhs) const noexcept
			{
				return corners_[0] == rhs.corners_[0] &&
					   corners_[1] == rhs.corners_[1] &&
					   corners_[2] == rhs.corners_[2] &&
					   corners_[3] == rhs.corners_[3];
			}

			///@brief Checks if two obb are different (one or more corners are different)
			[[nodiscard]] inline auto operator!=(const Obb &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			///@}

			/**
				@name Subscripting
				@{
			*/

			///@brief Returns the corner at the given offset
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 4);
				return corners_[off];
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the center of the obb to the given value
			void Center(const Vector2 &center) noexcept;

			///@brief Sets the extents of the obb to the given min and max values
			void Extents(const Vector2 &min, const Vector2 &max) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the bottom left point (corner #0)
			[[nodiscard]] inline auto BottomLeft() const noexcept
			{
				return corners_[0];
			}

			///@brief Returns the bottom right point (corner #1)
			[[nodiscard]] inline auto BottomRight() const noexcept
			{
				return corners_[1];
			}

			///@brief Returns the top left point (corner #3)
			[[nodiscard]] inline auto TopLeft() const noexcept
			{
				return corners_[3];
			}

			///@brief Returns the top right point (corner #2)
			[[nodiscard]] inline auto TopRight() const noexcept
			{
				return corners_[2];
			}

			///@brief Returns direct access to all corners
			[[nodiscard]] inline auto Corners() const noexcept
			{
				return &corners_[0];
			}


			///@brief Returns true if obb is axis aligned
			[[nodiscard]] bool AxisAligned() const noexcept;

			///@brief Returns the center of the obb
			[[nodiscard]] Vector2 Center() const noexcept;

			///@brief Returns true if this obb is empty
			///@details Meaning that the size is zero
			[[nodiscard]] bool Empty() const noexcept;

			///@}

			/**
				@name Obb conversions
				@{
			*/

			///@brief Returns the area of the obb
			[[nodiscard]] real ToArea() const noexcept;

			///@brief Returns the half-size of the obb
			[[nodiscard]] Vector2 ToHalfSize() const noexcept;

			///@brief Returns the size of the obb
			[[nodiscard]] Vector2 ToSize() const noexcept;

			///@}

			/**
				@name Containing
				@{
			*/

			///@brief Returns true if this obb contains the given point
			[[nodiscard]] bool Contains(const Vector2 &point) const noexcept;

			///@}

			/**
				@name Intersecting
				@{
			*/

			///@brief Returns true if this obb intersects the given obb
			[[nodiscard]] bool Intersects(const Obb &obb) const noexcept;

			///@brief Returns true if this obb intersects the given point
			[[nodiscard]] bool Intersects(const Vector2 &point) const noexcept;

			///@}

			/**
				@name Rotating
				@{
			*/

			///@brief Rotates obb by the given angle (radians)
			Obb& Rotate(real angle) noexcept;

			///@brief Rotates obb by the given angle (radians)
			///@details Returns the result as a copy
			[[nodiscard]] Obb RotateCopy(real angle) const noexcept;

			///@}

			/**
				@name Scaling
				@{
			*/

			///@brief Scales obb by the given vector
			Obb& Scale(const Vector2 &vector) noexcept;

			///@brief Scales obb by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Obb ScaleCopy(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Transforming
				@{
			*/

			///@brief Transforms obb based on the given matrix
			Obb& Transform(const Matrix3 &matrix) noexcept;

			///@brief Transforms obb based on the given matrix
			///@details Returns the result as a copy
			[[nodiscard]] Obb TransformCopy(const Matrix3 &matrix) const noexcept;

			///@}

			/**
				@name Translating
				@{
			*/

			///@brief Translates obb by the given vector
			Obb& Translate(const Vector2 &vector) noexcept;

			///@brief Translates obb by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Obb TranslateCopy(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Drawing
				@{
			*/

			///@brief Draws the bounds of this obb with the given color
			void Draw(const Color &color) const noexcept;

			///@}
	};


	namespace obb
	{
		/**
			@name Predefined constant obbs
			@{
		*/

		inline const auto Zero = Obb{vector2::Zero, vector2::Zero};
		inline const auto Unit = Obb{-0.5_r, 0.5_r};

		///@}
	} //obb
} //ion::graphics::utilities

#endif