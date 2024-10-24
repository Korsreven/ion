/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonVector2.h
-------------------------------------------
*/

#ifndef ION_VECTOR2_H
#define ION_VECTOR2_H

#include <cassert>
#include <utility>

#include "types/IonTypes.h"

#pragma warning(disable : 4201) //nameless struct/union

namespace ion::graphics::utilities
{
	class Vector3; //Forward declaration
	using namespace types::type_literals;

	namespace vector2::detail
	{
	} //vector2::detail


	///@brief A class representing a two-component vector (x,y)
	class Vector2 final
	{
		private:

				union
				{
					struct
					{
						real x_;
						real y_;
					};

					real components_[2]{};
				};

		public:

			///@brief Default constructor
			Vector2() = default;

			///@brief Constructs a new vector from the given x and y components
			Vector2(real x, real y) noexcept;

			///@brief Constructs a new vector from the given scalar value
			Vector2(real scalar) noexcept;

			///@brief Constructs a new vector from the given three component vector
			Vector2(const Vector3 &vector) noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Sets this vector equal to the given scalar value
			inline auto& operator=(real scalar) noexcept
			{
				x_ = y_ = scalar;
				return *this;
			}

			///@brief Sets this vector equal to the given three component vector
			Vector2& operator=(const Vector3 &vector) noexcept;


			///@brief Checks if two vectors are equal (all components are equal)
			[[nodiscard]] inline auto operator==(const Vector2 &rhs) const noexcept
			{
				return x_ == rhs.x_ &&
					   y_ == rhs.y_;
			}

			///@brief Checks if two vectors are different (one or more components are different)
			[[nodiscard]] inline auto operator!=(const Vector2 &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			///@brief Checks if one vector is less than another one (component wise)
			///@details Needed for sorting two vectors (strict weak ordering)
			[[nodiscard]] inline auto operator<(const Vector2 &rhs) const noexcept
			{
				return std::pair{x_, y_} < std::pair{rhs.x_, rhs.y_};
			}


			///@brief Negates vector (negating each component)
			[[nodiscard]] inline auto operator-() const noexcept
			{
				return Vector2{-x_, -y_};
			}

			///@}

			/**
				@name Adding
				@{
			*/

			///@brief Adds two vectors (adding each component)
			[[nodiscard]] inline auto operator+(const Vector2 &rhs) const noexcept
			{
				return Vector2{x_ + rhs.x_,
							   y_ + rhs.y_};
			}

			///@brief Adds both components to the given scalar
			[[nodiscard]] inline auto operator+(real scalar) const noexcept
			{
				return Vector2{x_ + scalar,
							   y_ + scalar};
			}

			///@brief Adds both components to the given scalar
			[[nodiscard]] friend inline auto operator+(real scalar, const Vector2 &rhs) noexcept
			{
				return rhs + scalar; //Commutative
			}

			///@brief Adds two vectors (adding each component)
			inline auto& operator+=(const Vector2 &rhs) noexcept
			{
				return *this = *this + rhs;
			}

			///@brief Adds both components to the given scalar
			inline auto& operator+=(real scalar) noexcept
			{
				return *this = *this + scalar;
			}

			///@}

			/**
				@name Subtracting
				@{
			*/

			///@brief Subtracts two vectors (subtracting each component)
			[[nodiscard]] inline auto operator-(const Vector2 &rhs) const noexcept
			{
				return Vector2{x_ - rhs.x_,
							   y_ - rhs.y_};
			}

			///@brief Subtracts both components with the given scalar
			[[nodiscard]] inline auto operator-(real scalar) const noexcept
			{
				return Vector2{x_ - scalar,
							   y_ - scalar};
			}

			///@brief Subtracts both components with the given scalar
			[[nodiscard]] friend inline auto operator-(real scalar, const Vector2 &rhs) noexcept
			{
				return Vector2{scalar - rhs.x_,
							   scalar - rhs.y_};
			}

			///@brief Subtracts two vectors (subtracting each component)
			inline auto& operator-=(const Vector2 &rhs) noexcept
			{
				return *this = *this - rhs;
			}

			///@brief Subtracts both components with the given scalar
			inline auto& operator-=(real scalar) noexcept
			{
				return *this = *this - scalar;
			}

			///@}

			/**
				@name Multiplying
				@{
			*/

			///@brief Muliplies two vectors (multiplying each component)
			[[nodiscard]] inline auto operator*(const Vector2 &rhs) const noexcept
			{
				return Vector2{x_ * rhs.x_,
							   y_ * rhs.y_};
			}

			///@brief Muliplies both components with the given scalar
			[[nodiscard]] inline auto operator*(real scalar) const noexcept
			{
				return Vector2{x_ * scalar,
							   y_ * scalar};
			}

			///@brief Muliplies both components with the given scalar
			[[nodiscard]] friend inline auto operator*(real scalar, const Vector2 &rhs) noexcept
			{
				return rhs * scalar; //Commutative
			}

			///@brief Muliplies two vectors (multiplying each component)
			inline auto& operator*=(const Vector2 &rhs) noexcept
			{
				return *this = *this * rhs;
			}

			///@brief Muliplies both components with the given scalar
			inline auto& operator*=(real scalar) noexcept
			{
				return *this = *this * scalar;
			}

			///@}

			/**
				@name Dividing
				@{
			*/

			///@brief Divides two vectors (dividing each component)
			[[nodiscard]] inline auto operator/(const Vector2 &rhs) const noexcept
			{
				assert(rhs.x_ != 0.0_r && rhs.y_ != 0.0_r);
				return Vector2{x_ / rhs.x_,
							   y_ / rhs.y_};
			}

			///@brief Divides both components with the given scalar
			[[nodiscard]] inline auto operator/(real scalar) const noexcept
			{
				assert(scalar != 0.0_r);
				return Vector2{x_ / scalar,
							   y_ / scalar};
			}

			///@brief Divides both components with the given scalar
			[[nodiscard]] friend inline auto operator/(real scalar, const Vector2 &rhs) noexcept
			{
				assert(rhs.x_ != 0.0_r && rhs.y_ != 0.0_r);
				return Vector2{scalar / rhs.x_,
							   scalar / rhs.y_};
			}

			///@brief Divides two vectors (dividing each component)
			inline auto& operator/=(const Vector2 &rhs) noexcept
			{
				return *this = *this / rhs;
			}

			///@brief Divides both components with the given scalar
			inline auto& operator/=(real scalar) noexcept
			{
				return *this = *this / scalar;
			}

			///@}

			/**
				@name Subscripting
				@{
			*/

			///@brief Returns a modifiable reference to the component at the given offset
			[[nodiscard]] inline auto& operator[](int off) noexcept
			{
				assert(off >= 0 && off < 2);
				return components_[off];
			}

			///@brief Returns the component at the given offset
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 2);
				return components_[off];
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the x component to the given value
			inline void X(real x) noexcept
			{
				x_ = x;
			}

			///@brief Sets the y component to the given value
			inline void Y(real y) noexcept
			{
				y_ = y;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the x component
			[[nodiscard]] inline auto X() const noexcept
			{
				return x_;
			}

			///@brief Returns the y component
			[[nodiscard]] inline auto Y() const noexcept
			{
				return y_;
			}

			///@brief Returns both the x and y components
			[[nodiscard]] inline auto XY() const noexcept
			{
				return std::pair{x_, y_};
			}

			///@brief Returns direct access to both vector components
			[[nodiscard]] inline auto Components() const noexcept
			{
				return components_;
			}

			///@}

			/**
				@name Angle
				@{
			*/

			///@brief Returns the directional angle of the vector
			///@details Angle is returned as radians in range [0.0, 2pi]
			[[nodiscard]] real Angle() const noexcept;

			///@brief Returns the directional angle of the vector
			///@details Angle is returned as radians in range [-pi, pi]
			[[nodiscard]] real SignedAngle() const noexcept;

			///@brief Returns the angle between this and the given vector
			///@details Angle is returned as radians in range [0, pi]
			[[nodiscard]] real AngleBetween(const Vector2 &vector) const noexcept;

			///@brief Returns the angle between this and the given vector
			///@details Angle is returned as radians in range [-pi, pi]
			[[nodiscard]] real SignedAngleBetween(const Vector2 &vector) const noexcept;

			///@brief Returns the oriented angle between this and the given vector
			///@details Angle is returned as radians in range [0.0, 2pi]
			[[nodiscard]] real AngleTo(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Cross and dot product
				@{
			*/

			///@brief Returns the cross product between this and the given vector
			[[nodiscard]] real CrossProduct(const Vector2 &vector) const noexcept;

			///@brief Returns the dot product between this and the given vector
			[[nodiscard]] real DotProduct(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Deviant
				@{
			*/

			///@brief Returns a vector that deviates from this vector by a given angle (radians)
			[[nodiscard]] Vector2 Deviant(real angle) const noexcept;

			///@brief Returns a vector that deviates from this vector by a given angle (radians) in a random direction
			[[nodiscard]] Vector2 RandomDeviant(real angle) const noexcept;

			///@}

			/**
				@name Distance
				@{
			*/

			///@brief Returns the distance to the given vector (expensive square root)
			[[nodiscard]] real Distance(const Vector2 &vector) const noexcept;

			///@brief Returns the squared distance to the given vector (for distance comparison)
			[[nodiscard]] real SquaredDistance(const Vector2 &vector) const noexcept;

			///@brief Returns true if this vector is approximately zero in distance to the given vector
			[[nodiscard]] bool ZeroDistance(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Floor and ceiling
				@{
			*/

			///@brief Sets x and y components to the maximum of this and the given vector
			Vector2& Ceil(const Vector2 &vector) noexcept;

			///@brief Sets x and y components to the maximum of this and the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Vector2 CeilCopy(const Vector2 &vector) const noexcept;

			///@brief Sets x and y components to the minimum of this and the given vector
			Vector2& Floor(const Vector2 &vector) noexcept;

			///@brief Sets x and y components to the minimum of this and the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Vector2 FloorCopy(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Interpolation
				@{
			*/

			///@brief Returns a vector based on linear interpolation between this and the given vector
			[[nodiscard]] Vector2 Lerp(const Vector2 &vector, real amount) const noexcept;

			///@brief Returns a vector based on linear interpolation between this and the given vector
			[[nodiscard]] Vector2 LerpUnclamped(const Vector2 &vector, real amount) const noexcept;
			
			///@brief Returns a vector based on spherical linear interpolation between this and the given vector
			[[nodiscard]] Vector2 Slerp(Vector2 vector, real amount) const noexcept;

			///@brief Returns a vector based on spherical linear interpolation between this and the given vector
			[[nodiscard]] Vector2 SlerpUnclamped(Vector2 vector, real amount) const noexcept;

			///@}

			/**
				@name Length
				@{
			*/

			///@brief Returns the length/magnitude of the vector (expensive square root)
			[[nodiscard]] real Length() const noexcept;
	
			///@brief Returns the squared length/magnitude of the vector (for length comparison)
			[[nodiscard]] real SquaredLength() const noexcept;

			///@brief Returns true if this vector is approximately zero in length/magnitude
			[[nodiscard]] bool ZeroLength() const noexcept;

			///@}

			/**
				@name Minimum and maximum
				@{
			*/

			///@brief Returns a modifiable reference to the largest component
			///@details If equal, y component is returned
			[[nodiscard]] real& Max() noexcept;

			///@brief Returns the largest component
			[[nodiscard]] real Max() const noexcept;

			///@brief Returns a modifiable reference to the smallest component
			///@details If equal, x component is returned
			[[nodiscard]] real& Min() noexcept;

			///@brief Returns the smallest component
			[[nodiscard]] real Min() const noexcept;

			///@}

			/**
				@name Normalizing
				@{
			*/

			///@brief Normalize and return the length of the vector
			///@details The vector is converted to a unit vector, meaning that the length is 1.0
			real Normalize() noexcept;

			///@brief Normalize the vector
			///@details The vector is converted to a unit vector, meaning that the length is 1.0.
			///Returns the result as a copy
			[[nodiscard]] Vector2 NormalizeCopy() const noexcept;

			///@}

			/**
				@name Point arithmetic
				The vectors are points rather than directions
				@{
			*/

			///@brief Returns the middle point from this and the given point
			[[nodiscard]] Vector2 Midpoint(const Vector2 &point) const noexcept;


			///@brief Returns the left most point from this and the given point
			[[nodiscard]] Vector2 LeftMostPoint(const Vector2 &point) const noexcept;

			///@brief Returns the right most point from this and the given point
			[[nodiscard]] Vector2 RightMostPoint(const Vector2 &point) const noexcept;


			///@brief Returns the top most point from this and the given point
			[[nodiscard]] Vector2 TopMostPoint(const Vector2 &point) const noexcept;

			///@brief Returns the bottom most point from this and the given point
			[[nodiscard]] Vector2 BottomMostPoint(const Vector2 &point) const noexcept;
			
			///@}

			/**
				@name Reflection
				@{
			*/

			///@brief Returns an orthogonal vector with congruent adjacent angles
			[[nodiscard]] Vector2 Perpendicular() const noexcept;
			
			///@brief Returns a reflection vector to the plane with the given normal
			[[nodiscard]] Vector2 Reflect(const Vector2 &normal) const noexcept;

			///@}

			/**
				@name Rotation
				@{
			*/

			///@brief Rotates point by the given angle (radians) and origin vector
			Vector2& Rotate(real angle, const Vector2 &origin) noexcept;

			///@brief Rotates point by the given angle (radians) and origin vector
			///@details Returns the result as a copy
			[[nodiscard]] Vector2 RotateCopy(real angle, const Vector2 &origin) const noexcept;

			///@}

			/**
				@name Scaling
				@{
			*/

			///@brief Scales point by the given scaling vector and origin vector
			Vector2& Scale(const Vector2 &scaling, const Vector2 &origin) noexcept;

			///@brief Scales point by the given scaling vector and origin vector
			///@details Returns the result as a copy
			[[nodiscard]] Vector2 ScaleCopy(const Vector2 &scaling, const Vector2 &origin) const noexcept;

			///@}

			/**
				@name Sign
				@{
			*/

			///@brief Sets each component to one, negative one or zero, depending on the signs of the components
			Vector2& Sign() noexcept;

			///@brief Sets each component to one, negative one or zero, depending on the signs of the components
			///@details Returns the result as a copy
			[[nodiscard]] Vector2 SignCopy() const noexcept;
	};


	namespace vector2
	{
		/**
			@name Predefined constant vectors
			@{
		*/

		inline const auto Zero = Vector2{0.0_r, 0.0_r};
		inline const auto UnitX = Vector2{1.0_r, 0.0_r};
		inline const auto UnitY = Vector2{0.0_r, 1.0_r};
		inline const auto NegativeUnitX = Vector2{-1.0_r, 0.0_r};
		inline const auto NegativeUnitY = Vector2{0.0_r, -1.0_r};
		inline const auto UnitScale = Vector2{1.0_r, 1.0_r};

		///@}

		inline namespace literals
		{
			/**
				@name User defined literals (UDLs)
				For scalar value to vector2
				@{
			*/

			inline auto operator""_vec2(long double scalar) noexcept
			{
				return Vector2{static_cast<real>(scalar)};
			}

			///@}
		} //literals
	} //vector2
} //ion::graphics::utilities

#endif