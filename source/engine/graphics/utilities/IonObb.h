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
		/*
			Graphics API
		*/

		void draw_bounds(const std::array<Vector2, 4> &corners, const Color &color) noexcept;
	} //obb::detail


	//Oriented bounding box (OBB)
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

			Obb() = default;

			//Constructs a new obb from the given min and max vectors
			Obb(const Vector2 &min, const Vector2 &max) noexcept;

			//Constructs a new obb from the given aabb
			Obb(const Aabb &aabb) noexcept;


			/*
				Static obb conversions
			*/

			//Returns a new obb from the given area and center
			[[nodiscard]] static Obb Area(real area, const Vector2 &center = vector2::Zero) noexcept;

			//Returns a new obb from the given half-size and center
			[[nodiscard]] static Obb HalfSize(const Vector2 &half_size, const Vector2 &center = vector2::Zero) noexcept;

			//Returns a new obb from the given size and center
			[[nodiscard]] static Obb Size(const Vector2 &size, const Vector2 &center = vector2::Zero) noexcept;


			/*
				Operators
			*/

			//Assign new corner values from the given aabb
			inline auto& operator=(const Aabb &aabb) noexcept
			{
				auto [min, max] = aabb.MinMax();
				corners_[0] = min;
				corners_[1] = {max.X(), min.Y()};
				corners_[2] = max;
				corners_[3] = {min.X(), max.Y()};
				return *this;
			}

			//Checks if two obb are equal (all corners are equal)
			[[nodiscard]] inline auto operator==(const Obb &rhs) const noexcept
			{
				return corners_[0] == rhs.corners_[0] &&
					   corners_[1] == rhs.corners_[1] &&
					   corners_[2] == rhs.corners_[2] &&
					   corners_[3] == rhs.corners_[3];
			}

			//Checks if two obb are different (one or more corners are different)
			[[nodiscard]] inline auto operator!=(const Obb &rhs) const noexcept
			{
				return !(*this == rhs);
			}


			/*
				Subscripting
			*/

			//Returns the corner at the given offset
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 4);
				return corners_[off];
			}


			/*
				Modifiers
			*/

			//Sets the center of the obb to the given value
			void Center(const Vector2 &center) noexcept;

			//Sets the extents of the obb to the given min and max values
			void Extents(const Vector2 &min, const Vector2 &max) noexcept;


			/*
				Observers
			*/

			//Returns the bottom left point (corner #0)
			[[nodiscard]] inline auto BottomLeft() const noexcept
			{
				return corners_[0];
			}

			//Returns the bottom right point (corner #1)
			[[nodiscard]] inline auto BottomRight() const noexcept
			{
				return corners_[1];
			}

			//Returns the top left point (corner #3)
			[[nodiscard]] inline auto TopLeft() const noexcept
			{
				return corners_[3];
			}

			//Returns the top right point (corner #2)
			[[nodiscard]] inline auto TopRight() const noexcept
			{
				return corners_[2];
			}

			//Returns direct access to all corners
			[[nodiscard]] inline auto Corners() const noexcept
			{
				return &corners_[0];
			}


			//Returns true if obb is axis aligned
			[[nodiscard]] bool AxisAligned() const noexcept;

			//Returns the center of the obb
			[[nodiscard]] Vector2 Center() const noexcept;

			//Returns true if this obb is empty
			//Meaning that the size is zero
			[[nodiscard]] bool Empty() const noexcept;


			/*
				Obb conversions
			*/

			//Returns the area of the obb
			[[nodiscard]] real ToArea() const noexcept;

			//Returns the half-size of the obb
			[[nodiscard]] Vector2 ToHalfSize() const noexcept;

			//Returns the size of the obb
			[[nodiscard]] Vector2 ToSize() const noexcept;


			/*
				Containing
			*/

			//Returns true if this obb contains the given point
			[[nodiscard]] bool Contains(const Vector2 &point) const noexcept;


			/*
				Intersecting
			*/

			//Returns true if this obb intersects the given obb
			[[nodiscard]] bool Intersects(const Obb &obb) const noexcept;

			//Returns true if this obb intersects the given point
			[[nodiscard]] bool Intersects(const Vector2 &point) const noexcept;


			/*
				Rotating
			*/

			//Rotate obb by the given angle (radians)
			Obb& Rotate(real angle) noexcept;

			//Rotate obb by the given angle (radians)
			//Returns the result as a copy
			[[nodiscard]] Obb RotateCopy(real angle) const noexcept;


			/*
				Scaling
			*/

			//Scale obb by the given vector
			Obb& Scale(const Vector2 &vector) noexcept;

			//Scale obb by the given vector
			//Returns the result as a copy
			[[nodiscard]] Obb ScaleCopy(const Vector2 &vector) const noexcept;


			/*
				Transforming
			*/

			//Transform obb based on the given matrix
			Obb& Transform(const Matrix3 &matrix) noexcept;

			//Transform obb based on the given matrix
			//Returns the result as a copy
			[[nodiscard]] Obb TransformCopy(const Matrix3 &matrix) const noexcept;


			/*
				Translating
			*/

			//Translate obb by the given vector
			Obb& Translate(const Vector2 &vector) noexcept;

			//Translate obb by the given vector
			//Returns the result as a copy
			[[nodiscard]] Obb TranslateCopy(const Vector2 &vector) const noexcept;


			/*
				Drawing
			*/

			//Draw the bounds of this obb with the given color
			void Draw(const Color &color) noexcept;
	};


	namespace obb
	{
		/*
			Predefined constant obbs
		*/

		inline const auto Zero = Obb{vector2::Zero, vector2::Zero};
		inline const auto Unit = Obb{-0.5_r, 0.5_r};
	} //obb
} //ion::graphics::utilities

#endif