/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonVector3.h
-------------------------------------------
*/

#ifndef ION_VECTOR3_H
#define ION_VECTOR3_H

#include <cassert>
#include <tuple>

#include "types/IonTypes.h"

#pragma warning(disable : 4201) //nameless struct/union

namespace ion::graphics::utilities
{
	class Vector2; //Forward declaration
	using namespace types::type_literals;

	namespace vector3::detail
	{
	} //vector3::detail


	//A class representing a three-component vector (x,y,z)
	class Vector3 final
	{
		private:

				union
				{
					struct
					{
						real x_;
						real y_;
						real z_;
					};

					real components_[3]{};
				};

		public:

			Vector3() = default;

			//Constructs a new vector from the given x, y and z components
			Vector3(real x, real y, real z) noexcept;

			//Constructs a new vector from the given scalar value
			Vector3(real scalar) noexcept;

			//Constructs a new vector from the given two component vector
			Vector3(const Vector2 &vector) noexcept;


			/*
				Operators
			*/

			//Sets this vector equal to the given scalar value
			inline auto& operator=(real scalar) noexcept
			{
				x_ = y_ = z_ = scalar;
				return *this;
			}

			//Sets this vector equal to the given two component vector
			Vector3& operator=(const Vector2 &vector) noexcept;


			//Checks if two vectors are equal (all components are equal)
			[[nodiscard]] inline auto operator==(const Vector3 &rhs) const noexcept
			{
				return x_ == rhs.x_ &&
					   y_ == rhs.y_ &&
					   z_ == rhs.z_;
			}

			//Checks if two vectors are different (one or more components are different)
			[[nodiscard]] inline auto operator!=(const Vector3 &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			//Checks if one vector is less than another one (component wise)
			//Needed for sorting two vectors (strict weak ordering)
			[[nodiscard]] inline auto operator<(const Vector3 &rhs) const noexcept
			{
				return std::tuple{x_, y_, z_} < std::tuple{rhs.x_, rhs.y_, rhs.z_};
			}


			//Negates vector (negating each component)
			[[nodiscard]] inline auto operator-() const noexcept
			{
				return Vector3{-x_, -y_, -z_};
			}


			/*
				Adding
			*/

			//Adds two vectors (adding each component)
			[[nodiscard]] inline auto operator+(const Vector3 &rhs) const noexcept
			{
				return Vector3{x_ + rhs.x_,
							   y_ + rhs.y_,
							   z_ + rhs.z_};
			}

			//Adds all components to the given scalar
			[[nodiscard]] inline auto operator+(real scalar) const noexcept
			{
				return Vector3{x_ + scalar,
							   y_ + scalar,
							   z_ + scalar};
			}

			//Adds all components to the given scalar
			[[nodiscard]] friend inline auto operator+(real scalar, const Vector3 &rhs) noexcept
			{
				return rhs + scalar; //Commutative
			}

			//Adds two vectors (adding each component)
			inline auto& operator+=(const Vector3 &rhs) noexcept
			{
				return *this = *this + rhs;
			}

			//Adds all components to the given scalar
			inline auto& operator+=(real scalar) noexcept
			{
				return *this = *this + scalar;
			}


			/*
				Subtracting
			*/

			//Subtracts two vectors (subtracting each component)
			[[nodiscard]] inline auto operator-(const Vector3 &rhs) const noexcept
			{
				return Vector3{x_ - rhs.x_,
							   y_ - rhs.y_,
							   z_ - rhs.z_};
			}

			//Subtracts all components with the given scalar
			[[nodiscard]] inline auto operator-(real scalar) const noexcept
			{
				return Vector3{x_ - scalar,
							   y_ - scalar,
							   z_ - scalar};
			}

			//Subtracts all components with the given scalar
			[[nodiscard]] friend inline auto operator-(real scalar, const Vector3 &rhs) noexcept
			{
				return Vector3{scalar - rhs.x_,
							   scalar - rhs.y_,
							   scalar - rhs.z_};
			}

			//Subtracts two vectors (subtracting each component)
			inline auto& operator-=(const Vector3 &rhs) noexcept
			{
				return *this = *this - rhs;
			}

			//Subtracts all components with the given scalar
			inline auto& operator-=(real scalar) noexcept
			{
				return *this = *this - scalar;
			}


			/*
				Multiplying
			*/

			//Muliplies two vectors (multiplying each component)
			[[nodiscard]] inline auto operator*(const Vector3 &rhs) const noexcept
			{
				return Vector3{x_ * rhs.x_,
							   y_ * rhs.y_,
							   z_ * rhs.z_};
			}

			//Muliplies all components with the given scalar
			[[nodiscard]] inline auto operator*(real scalar) const noexcept
			{
				return Vector3{x_ * scalar,
							   y_ * scalar,
							   z_ * scalar};
			}

			//Muliplies all components with the given scalar
			[[nodiscard]] friend inline auto operator*(real scalar, const Vector3 &rhs) noexcept
			{
				return rhs * scalar; //Commutative
			}

			//Muliplies two vectors (multiplying each component)
			inline auto& operator*=(const Vector3 &rhs) noexcept
			{
				return *this = *this * rhs;
			}

			//Muliplies all components with the given scalar
			inline auto& operator*=(real scalar) noexcept
			{
				return *this = *this * scalar;
			}


			/*
				Dividing
			*/

			//Divides two vectors (dividing each component)
			[[nodiscard]] inline auto operator/(const Vector3 &rhs) const noexcept
			{
				assert(rhs.x_ != 0.0_r && rhs.y_ != 0.0_r && rhs.z_ != 0.0_r);
				return Vector3{x_ / rhs.x_,
							   y_ / rhs.y_,
							   z_ / rhs.z_};
			}

			//Divides all components with the given scalar
			[[nodiscard]] inline auto operator/(real scalar) const noexcept
			{
				assert(scalar != 0.0_r);
				return Vector3{x_ / scalar,
							   y_ / scalar,
							   z_ / scalar};
			}

			//Divides all components with the given scalar
			[[nodiscard]] friend inline auto operator/(real scalar, const Vector3 &rhs) noexcept
			{
				assert(rhs.x_ != 0.0_r && rhs.y_ != 0.0_r && rhs.z_ != 0.0_r);
				return Vector3{scalar / rhs.x_,
							   scalar / rhs.y_,
							   scalar / rhs.z_};
			}

			//Divides two vectors (dividing each component)
			inline auto& operator/=(const Vector3 &rhs) noexcept
			{
				return *this = *this / rhs;
			}

			//Divides all components with the given scalar
			inline auto& operator/=(real scalar) noexcept
			{
				return *this = *this / scalar;
			}


			/*
				Subscripting
			*/

			//Returns a modifiable reference to the component at the given offset
			[[nodiscard]] inline auto& operator[](int off) noexcept
			{
				assert(off >= 0 && off < 3);
				return components_[off];
			}

			//Returns the component at the given offset
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 3);
				return components_[off];
			}


			/*
				Modifiers
			*/

			//Sets the x component to the given value
			inline void X(real x) noexcept
			{
				x_ = x;
			}

			//Sets the y component to the given value
			inline void Y(real y) noexcept
			{
				y_ = y;
			}

			//Sets the z component to the given value
			inline void Z(real z) noexcept
			{
				z_ = z;
			}


			/*
				Observers
			*/

			//Returns the x component
			[[nodiscard]] inline auto X() const noexcept
			{
				return x_;
			}

			//Returns the y component
			[[nodiscard]] inline auto Y() const noexcept
			{
				return y_;
			}

			//Returns the z component
			[[nodiscard]] inline auto Z() const noexcept
			{
				return z_;
			}

			//Returns the x, y and z components
			[[nodiscard]] inline auto XYZ() const noexcept
			{
				return std::tuple{x_, y_, z_};
			}

			//Returns direct access to all vector components
			[[nodiscard]] inline auto Components() const noexcept
			{
				return components_;
			}


			/*
				Angle
			*/

			//Returns the directional angle of the vector
			//Angle is returned as radians in range [0.0, 2pi]
			[[nodiscard]] real Angle() const noexcept;

			//Returns the directional angle of the vector
			//Angle is returned as radians in range [-pi, pi]
			[[nodiscard]] real SignedAngle() const noexcept;

			//Returns the angle between this and the given vector
			//Angle is returned as radians in range [0, pi]
			[[nodiscard]] real AngleBetween(const Vector3 &vector) const noexcept;

			//Returns the angle between this and the given vector
			//Angle is returned as radians in range [-pi, pi]
			[[nodiscard]] real SignedAngleBetween(const Vector3 &vector) const noexcept;

			//Returns the oriented angle between this and the given vector
			//Angle is returned as radians in range [0.0, 2pi]
			[[nodiscard]] real AngleTo(const Vector3 &vector) const noexcept;


			/*
				Cross and dot product
			*/

			//Returns the cross product between this and the given vector
			[[nodiscard]] Vector3 CrossProduct(const Vector3 &vector) const noexcept;

			//Returns the dot product between this and the given vector
			[[nodiscard]] real DotProduct(const Vector3 &vector) const noexcept;


			/*
				Deviant
			*/

			//Returns a vector that deviates from this vector by a given angle (radians)
			[[nodiscard]] Vector3 Deviant(real angle) const noexcept;

			//Returns a vector that deviates from this vector by a given angle (radians) in a random direction
			[[nodiscard]] Vector3 RandomDeviant(real angle) const noexcept;


			/*
				Distance
			*/

			//Returns the distance to the given vector (expensive square root)
			[[nodiscard]] real Distance(const Vector3 &vector) const noexcept;

			//Returns the squared distance to the given vector (for distance comparison)
			[[nodiscard]] real SquaredDistance(const Vector3 &vector) const noexcept;

			//Returns true if this vector is approximately zero in distance to the given vector
			[[nodiscard]] bool ZeroDistance(const Vector3 &vector) const noexcept;


			/*
				Floor and ceiling
			*/

			//Sets x, y and z components to the maximum of this and the given vector
			Vector3& Ceil(const Vector3 &vector) noexcept;

			//Sets x, y and z components to the maximum of this and the given vector
			//Returns the result as a copy
			[[nodiscard]] Vector3 CeilCopy(const Vector3 &vector) const noexcept;

			//Sets x, y and z components to the minimum of this and the given vector
			Vector3& Floor(const Vector3 &vector) noexcept;

			//Sets x, y and z components to the minimum of this and the given vector
			//Returns the result as a copy
			[[nodiscard]] Vector3 FloorCopy(const Vector3 &vector) const noexcept;


			/*
				Interpolation
			*/

			//Returns a vector based on linear interpolation between this and the given vector
			[[nodiscard]] Vector3 Lerp(const Vector3 &vector, real amount) const noexcept;
			
			//Returns a vector based on spherical linear interpolation between this and the given vector
			[[nodiscard]] Vector3 Slerp(Vector3 vector, real amount) const noexcept;


			/*
				Length
			*/

			//Returns the length/magnitude of the vector (expensive square root)
			[[nodiscard]] real Length() const noexcept;
	
			//Returns the squared length/magnitude of the vector (for length comparison)
			[[nodiscard]] real SquaredLength() const noexcept;

			//Returns true if this vector is approximately zero in length/magnitude
			[[nodiscard]] bool ZeroLength() const noexcept;


			/*
				Minimum and maximum
			*/

			//Returns a modifiable reference to the largest component
			//If equal, z component is returned
			[[nodiscard]] real& Max() noexcept;

			//Returns the largest component
			[[nodiscard]] real Max() const noexcept;

			//Returns a modifiable reference to the smallest component
			//If equal, x component is returned
			[[nodiscard]] real& Min() noexcept;

			//Returns the smallest component
			[[nodiscard]] real Min() const noexcept;


			/*
				Normalizing
			*/

			//Normalize and return the length of the vector
			//The vector is converted to a unit vector, meaning that the length is 1.0
			real Normalize() noexcept;

			//Normalize the vector
			//The vector is converted to a unit vector, meaning that the length is 1.0
			//Returns the result as a copy
			[[nodiscard]] Vector3 NormalizeCopy() const noexcept;


			/*
				Point arithmetic
				The vectors are points rather than directions
			*/

			//Returns the middle point from this and the given point
			[[nodiscard]] Vector3 Midpoint(const Vector3 &point) const noexcept;


			//Returns the left most point from this and the given point
			[[nodiscard]] Vector3 LeftMostPoint(const Vector3 &point) const noexcept;

			//Returns the right most point from this and the given point
			[[nodiscard]] Vector3 RightMostPoint(const Vector3 &point) const noexcept;


			//Returns the top most point from this and the given point
			[[nodiscard]] Vector3 TopMostPoint(const Vector3 &point) const noexcept;

			//Returns the bottom most point from this and the given point
			[[nodiscard]] Vector3 BottomMostPoint(const Vector3 &point) const noexcept;


			//Returns the nearest point from this and the given point
			[[nodiscard]] Vector3 NearestPoint(const Vector3 &point) const noexcept;

			//Returns the farthest point from this and the given point
			[[nodiscard]] Vector3 FarthestPoint(const Vector3 &point) const noexcept;
			

			/*
				Reflection
			*/

			//Returns an orthogonal vector with congruent adjacent angles
			[[nodiscard]] Vector3 Perpendicular() const noexcept;
			
			//Returns a reflection vector to the plane with the given normal
			[[nodiscard]] Vector3 Reflect(const Vector3 &normal) const noexcept;


			/*
				Rotation
			*/

			//Rotates point by the given angle (radians) and origin vector
			Vector3& Rotate(real angle, const Vector3 &origin) noexcept;

			//Rotates point by the given angle (radians) and origin vector
			//Returns the result as a copy
			[[nodiscard]] Vector3 RotateCopy(real angle, const Vector3 &origin) const noexcept;


			/*
				Scaling
			*/

			//Scales point by the given scaling vector and origin vector
			Vector3& Scale(const Vector3 &scaling, const Vector3 &origin) noexcept;

			//Scales point by the given scaling vector and origin vector
			//Returns the result as a copy
			[[nodiscard]] Vector3 ScaleCopy(const Vector3 &scaling, const Vector3 &origin) const noexcept;


			/*
				Sign
			*/

			//Sets each component to one, negative one or zero, depending on the signs of the components
			Vector3& Sign() noexcept;

			//Sets each component to one, negative one or zero, depending on the signs of the components
			//Returns the result as a copy
			[[nodiscard]] Vector3 SignCopy() const noexcept;
	};


	namespace vector3
	{
		/*
			Predefined constant vectors
		*/

		inline const auto Zero = Vector3{0.0_r, 0.0_r, 0.0_r};
		inline const auto UnitX = Vector3{1.0_r, 0.0_r, 0.0_r};
		inline const auto UnitY = Vector3{0.0_r, 1.0_r, 0.0_r};
		inline const auto UnitZ = Vector3{0.0_r, 0.0_r, 1.0_r};
		inline const auto NegativeUnitX = Vector3{-1.0_r, 0.0_r, 0.0_r};
		inline const auto NegativeUnitY = Vector3{0.0_r, -1.0_r, 0.0_r};
		inline const auto NegativeUnitZ = Vector3{0.0_r, 0.0_r, -1.0_r};
		inline const auto UnitScale = Vector3{1.0_r, 1.0_r, 1.0_r};


		inline namespace literals
		{
			/*
				User defined literals (UDLs)
				For scalar value to Vector3
			*/

			inline auto operator""_vec3(long double scalar) noexcept
			{
				return Vector3{static_cast<real>(scalar)};
			}
		} //literals
	} //vector3
} //ion::graphics::utilities

#endif