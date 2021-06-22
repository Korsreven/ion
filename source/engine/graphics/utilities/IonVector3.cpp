/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonVector3.cpp
-------------------------------------------
*/

#include "IonVector3.h"

#include <algorithm>
#include <cmath>

#include "IonVector2.h"
#include "utilities/IonRandom.h"
#include "utilities/IonMath.h"

namespace ion::graphics::utilities
{

using namespace ion::utilities;

namespace vector3::detail
{
} //vector3::detail


//Vector3

Vector3::Vector3(real x, real y, real z) noexcept :
	x_{x}, y_{y}, z_{z}
{
	//Empty
}

Vector3::Vector3(real scalar) noexcept :
	x_{scalar}, y_{scalar}, z_{scalar}
{
	//Empty
}

Vector3::Vector3(const Vector2 &vector) noexcept :
	x_{vector.X()}, y_{vector.Y()}, z_{0.0_r}
{
	//Empty
}


/*
	Operators
*/

Vector3& Vector3::operator=(const Vector2 &vector) noexcept
{
	auto [x, y] = vector.XY();
	x_ = x;
	y_ = y;
	return *this;
}


/*
	Angle
*/

real Vector3::Angle() const noexcept
{
	auto angle = SignedAngle();
	return angle < 0.0_r ?
		angle + math::TwoPi :
		angle;
}

real Vector3::SignedAngle() const noexcept
{
	#ifdef ION_LEFT_HANDED
	//Left-hand rotation CW
	return std::atan2(-y_, x_);
	#else
	//Right-hand rotation CCW
	return std::atan2(y_, x_);
	#endif
}

real Vector3::AngleBetween(const Vector3 &vector) const noexcept
{
	return math::Abs(SignedAngleBetween(vector));
}

real Vector3::SignedAngleBetween(const Vector3 &vector) const noexcept
{
	#ifdef ION_LEFT_HANDED
	//Left-hand rotation CW
	return std::atan2(-CrossProduct(vector).DotProduct(vector3::UnitZ), DotProduct(vector));
	#else
	//Right-hand rotation CCW
	return std::atan2(CrossProduct(vector).DotProduct(vector3::UnitZ), DotProduct(vector));
	#endif
}

real Vector3::AngleTo(const Vector3 &vector) const noexcept
{
	auto angle = AngleBetween(vector);
	return CrossProduct(vector) < 0.0_r ?
		math::TwoPi - angle :
		angle;
}


/*
	Cross and dot product
*/

Vector3 Vector3::CrossProduct(const Vector3 &vector) const noexcept
{
	return {y_ * vector.z_ - z_ * vector.y_,
		    z_ * vector.x_ - x_ * vector.z_,
		    x_ * vector.y_ - y_ * vector.x_};
}

real Vector3::DotProduct(const Vector3 &vector) const noexcept
{
	return x_ * vector.x_ + y_ * vector.y_ + z_ * vector.z_;
}


/*
	Deviant
*/

Vector3 Vector3::Deviant(real angle) const noexcept
{
	auto sin_of_angle = math::Sin(angle);
	auto cos_of_angle = math::Cos(angle);

	#ifdef ION_LEFT_HANDED
	//Left-hand rotation CW
	return {cos_of_angle * x_ + sin_of_angle * y_,
			-sin_of_angle * x_ + cos_of_angle * y_,
			z_};
	#else
	//Right-hand rotation CCW
	return {cos_of_angle * x_ - sin_of_angle * y_,
			sin_of_angle * x_ + cos_of_angle * y_,
			z_};
	#endif
}

Vector3 Vector3::RandomDeviant(real angle) const noexcept
{
	return Deviant(angle * random::Number(-1.0_r, 1.0_r));
}


/*
	Distance
*/

real Vector3::Distance(const Vector3 &vector) const noexcept
{
	return (*this - vector).Length();
}

real Vector3::SquaredDistance(const Vector3 &vector) const noexcept
{
	return (*this - vector).SquaredLength();
}

bool Vector3::ZeroDistance(const Vector3 &vector) const noexcept
{
	return SquaredDistance(vector) < math::Epsilon;
}


/*
	Floor and ceiling
*/

Vector3& Vector3::Ceil(const Vector3 &vector) noexcept
{
	return *this = CeilCopy(vector);
}

Vector3 Vector3::CeilCopy(const Vector3 &vector) const noexcept
{
	return {x_ > vector.x_ ? x_ : vector.x_,
			y_ > vector.y_ ? y_ : vector.y_,
			z_ > vector.z_ ? z_ : vector.z_};
}

Vector3& Vector3::Floor(const Vector3 &vector) noexcept
{
	return *this = FloorCopy(vector);
}

Vector3 Vector3::FloorCopy(const Vector3 &vector) const noexcept
{
	return {x_ < vector.x_ ? x_ : vector.x_,
			y_ < vector.y_ ? y_ : vector.y_,
			z_ < vector.z_ ? z_ : vector.z_};
}


/*
	Interpolation
*/

Vector3 Vector3::Lerp(const Vector3 &vector, real amount) const noexcept
{
	if (amount <= 0.0_r)
		return *this;
	else if (amount >= 1.0_r)
		return vector;

	//Do linear interpolation
	return {math::Lerp(x_, vector.x_, amount),
			math::Lerp(y_, vector.y_, amount),
			math::Lerp(z_, vector.z_, amount)};
}
			
Vector3 Vector3::Slerp(Vector3 vector, real amount) const noexcept
{
	if (amount <= 0.0_r)
		return *this;
	else if (amount >= 1.0_r)
		return vector;

	//Do spherical linear interpolation
	auto dot = DotProduct(vector);

	if (dot < 0.0_r)
	{
		dot = -dot;
		vector = -vector;
	}

	//Fixed potential acos domain error and division by zero
	if (dot > 1.0_r - math::Epsilon)
		return Lerp(vector, amount);
	else
	{
		auto angle = std::acos(dot);
		return {math::Slerp(x_, vector.x_, angle, amount),
				math::Slerp(y_, vector.y_, angle, amount),
				math::Slerp(z_, vector.z_, angle, amount)};
	}
}


/*
	Length
*/

real Vector3::Length() const noexcept
{
	return std::sqrt(SquaredLength()); //Expensive
}

real Vector3::SquaredLength() const noexcept
{
	return x_ * x_ + y_ * y_ + z_ * z_; //Inexpensive
}

bool Vector3::ZeroLength() const noexcept
{
	return SquaredLength() < math::Epsilon;
}


/*
	Minimum and maximum
*/

real& Vector3::Max() noexcept
{
	return x_ > y_ ? (x_ > z_ ? x_ : z_) : (y_ > z_ ? y_ : z_);
}

real Vector3::Max() const noexcept
{
	return x_ > y_ ? (x_ > z_ ? x_ : z_) : (y_ > z_ ? y_ : z_);
}

real& Vector3::Min() noexcept
{
	return z_ < y_ ? (z_ < x_ ? z_ : x_) : (y_ < x_ ? y_ : x_);
}

real Vector3::Min() const noexcept
{
	return z_ < y_ ? (z_ < x_ ? z_ : x_) : (y_ < x_ ? y_ : x_);
}


/*
	Normalizing
*/

real Vector3::Normalize() noexcept
{
	auto length = Length();

	if (length > 0.0_r)
	{
		x_ /= length;
		y_ /= length;
		z_ /= length;
	}

	return length;
}

Vector3 Vector3::NormalizeCopy() const noexcept
{
	auto vector = *this;
	vector.Normalize();
	return vector;
}


/*
	Point arithmetic
	The vectors are points rather than directions
*/

Vector3 Vector3::Midpoint(const Vector3 &point) const noexcept
{
	return {(x_ + point.x_) * 0.5_r,
			(y_ + point.y_) * 0.5_r,
			(z_ + point.z_) * 0.5_r};
}

Vector3 Vector3::LeftMostPoint(const Vector3 &point) const noexcept
{
	return x_ < point.x_ ? *this : point;
}

Vector3 Vector3::RightMostPoint(const Vector3 &point) const noexcept
{
	return x_ > point.x_ ? *this : point;
}


Vector3 Vector3::TopMostPoint(const Vector3 &point) const noexcept
{
	return y_ > point.y_ ? *this : point;
}

Vector3 Vector3::BottomMostPoint(const Vector3 &point) const noexcept
{
	return y_ < point.y_ ? *this : point;
}


Vector3 Vector3::NearestPoint(const Vector3 &point) const noexcept
{
	return z_ > point.z_ ? *this : point;
}

Vector3 Vector3::FarthestPoint(const Vector3 &point) const noexcept
{
	return z_ < point.z_ ? *this : point;
}


/*
	Reflection
*/

Vector3 Vector3::Perpendicular() const noexcept
{
	auto vector = CrossProduct(vector3::UnitX);

	if (vector.ZeroLength())
		vector = CrossProduct(vector3::UnitY);

	return vector;
}

Vector3 Vector3::Reflect(const Vector3 &normal) const noexcept
{
	return *this - (normal * DotProduct(normal) * 2.0_r);
}


/*
	Rotation
*/

Vector3& Vector3::Rotate(real angle, const Vector3 &origin) noexcept
{
	return *this = RotateCopy(angle, origin);
}

Vector3 Vector3::RotateCopy(real angle, const Vector3 &origin) const noexcept
{
	return (*this - origin).Deviant(angle) + origin;
}


/*
	Scaling
*/

Vector3& Vector3::Scale(const Vector3 &scaling, const Vector3 &origin) noexcept
{
	return *this = ScaleCopy(scaling, origin);
}

Vector3 Vector3::ScaleCopy(const Vector3 &scaling, const Vector3 &origin) const noexcept
{
	return (*this - origin) * scaling + origin;
}

} //ion::graphics::utilities