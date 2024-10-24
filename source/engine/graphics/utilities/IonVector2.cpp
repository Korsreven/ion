/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonVector2.cpp
-------------------------------------------
*/

#include "IonVector2.h"

#include <cmath>

#include "IonVector3.h"
#include "utilities/IonRandom.h"
#include "utilities/IonMath.h"

namespace ion::graphics::utilities
{

using namespace ion::utilities;

namespace vector2::detail
{
} //vector2::detail


//Vector2

Vector2::Vector2(real x, real y) noexcept :
	x_{x}, y_{y}
{
	//Empty
}

Vector2::Vector2(real scalar) noexcept :
	x_{scalar}, y_{scalar}
{
	//Empty
}

Vector2::Vector2(const Vector3 &vector) noexcept :
	x_{vector.X()}, y_{vector.Y()}
{
	//Empty
}


/*
	Operators
*/

Vector2& Vector2::operator=(const Vector3 &vector) noexcept
{
	auto [x, y, z] = vector.XYZ();
	x_ = x;
	y_ = y;
	return *this;
}


/*
	Angle
*/

real Vector2::Angle() const noexcept
{
	auto angle = SignedAngle();
	return angle < 0.0_r ?
		angle + math::TwoPi :
		angle;
}

real Vector2::SignedAngle() const noexcept
{
	#ifdef ION_LEFT_HANDED
	//Left-hand rotation CW
	return std::atan2(-y_, x_);
	#else
	//Right-hand rotation CCW
	return std::atan2(y_, x_);
	#endif
}

real Vector2::AngleBetween(const Vector2 &vector) const noexcept
{
	return math::Abs(SignedAngleBetween(vector));
}

real Vector2::SignedAngleBetween(const Vector2 &vector) const noexcept
{
	#ifdef ION_LEFT_HANDED
	//Left-hand rotation CW
	return std::atan2(-CrossProduct(vector), DotProduct(vector));
	#else
	//Right-hand rotation CCW
	return std::atan2(CrossProduct(vector), DotProduct(vector));
	#endif
}

real Vector2::AngleTo(const Vector2 &vector) const noexcept
{
	auto angle = AngleBetween(vector);
	return CrossProduct(vector) < 0.0_r ?
		math::TwoPi - angle :
		angle;
}


/*
	Cross and dot product
*/

real Vector2::CrossProduct(const Vector2 &vector) const noexcept
{
	return x_ * vector.y_ - y_ * vector.x_;
}

real Vector2::DotProduct(const Vector2 &vector) const noexcept
{
	return x_ * vector.x_ + y_ * vector.y_;
}


/*
	Deviant
*/

Vector2 Vector2::Deviant(real angle) const noexcept
{
	auto sin_of_angle = math::Sin(angle);
	auto cos_of_angle = math::Cos(angle);

	#ifdef ION_LEFT_HANDED
	//Left-hand rotation CW
	return {cos_of_angle * x_ + sin_of_angle * y_,
			-sin_of_angle * x_ + cos_of_angle * y_};
	#else
	//Right-hand rotation CCW
	return {cos_of_angle * x_ - sin_of_angle * y_,
			sin_of_angle * x_ + cos_of_angle * y_};
	#endif
}

Vector2 Vector2::RandomDeviant(real angle) const noexcept
{
	return Deviant(angle * random::Number(-1.0_r, 1.0_r));
}


/*
	Distance
*/

real Vector2::Distance(const Vector2 &vector) const noexcept
{
	return (*this - vector).Length();
}

real Vector2::SquaredDistance(const Vector2 &vector) const noexcept
{
	return (*this - vector).SquaredLength();
}

bool Vector2::ZeroDistance(const Vector2 &vector) const noexcept
{
	return SquaredDistance(vector) < math::Epsilon;
}


/*
	Floor and ceiling
*/

Vector2& Vector2::Ceil(const Vector2 &vector) noexcept
{
	return *this = CeilCopy(vector);
}

Vector2 Vector2::CeilCopy(const Vector2 &vector) const noexcept
{
	return {x_ > vector.x_ ? x_ : vector.x_,
			y_ > vector.y_ ? y_ : vector.y_};
}

Vector2& Vector2::Floor(const Vector2 &vector) noexcept
{
	return *this = FloorCopy(vector);
}

Vector2 Vector2::FloorCopy(const Vector2 &vector) const noexcept
{
	return {x_ < vector.x_ ? x_ : vector.x_,
			y_ < vector.y_ ? y_ : vector.y_};
}


/*
	Interpolation
*/

Vector2 Vector2::Lerp(const Vector2 &vector, real amount) const noexcept
{
	if (amount <= 0.0_r)
		return *this;
	else if (amount >= 1.0_r)
		return vector;

	return LerpUnclamped(vector, amount);
}

Vector2 Vector2::LerpUnclamped(const Vector2 &vector, real amount) const noexcept
{
	//Do linear interpolation
	return {math::Lerp(x_, vector.x_, amount),
			math::Lerp(y_, vector.y_, amount)};
}

Vector2 Vector2::Slerp(Vector2 vector, real amount) const noexcept
{
	if (amount <= 0.0_r)
		return *this;
	else if (amount >= 1.0_r)
		return vector;

	return SlerpUnclamped(vector, amount);
}

Vector2 Vector2::SlerpUnclamped(Vector2 vector, real amount) const noexcept
{
	//Do spherical linear interpolation
	auto dot = DotProduct(vector);

	if (dot < 0.0_r)
	{
		dot = -dot;
		vector = -vector;
	}

	//Fixed potential acos domain error and division by zero
	if (dot > 1.0_r - math::Epsilon)
		return LerpUnclamped(vector, amount);
	else
	{
		auto angle = std::acos(dot);
		return {math::Slerp(x_, vector.x_, angle, amount),
				math::Slerp(y_, vector.y_, angle, amount)};
	}
}


/*
	Length
*/

real Vector2::Length() const noexcept
{
	return std::sqrt(SquaredLength()); //Expensive
}

real Vector2::SquaredLength() const noexcept
{
	return x_ * x_ + y_ * y_; //Inexpensive
}

bool Vector2::ZeroLength() const noexcept
{
	return SquaredLength() < math::Epsilon;
}


/*
	Minimum and maximum
*/

real& Vector2::Max() noexcept
{
	return x_ > y_ ? x_ : y_;
}

real Vector2::Max() const noexcept
{
	return x_ > y_ ? x_ : y_;
}

real& Vector2::Min() noexcept
{
	return y_ < x_ ? y_ : x_;
}

real Vector2::Min() const noexcept
{
	return y_ < x_ ? y_ : x_;
}


/*
	Normalizing
*/

real Vector2::Normalize() noexcept
{
	auto length = Length();

	if (length > 0.0_r)
	{
		x_ /= length;
		y_ /= length;
	}

	return length;
}

Vector2 Vector2::NormalizeCopy() const noexcept
{
	auto vector = *this;
	vector.Normalize();
	return vector;
}


/*
	Point arithmetic
	The vectors are points rather than directions
*/

Vector2 Vector2::Midpoint(const Vector2 &point) const noexcept
{
	return {(x_ + point.x_) * 0.5_r,
			(y_ + point.y_) * 0.5_r};
}

Vector2 Vector2::LeftMostPoint(const Vector2 &point) const noexcept
{
	return x_ < point.x_ ? *this : point;
}

Vector2 Vector2::RightMostPoint(const Vector2 &point) const noexcept
{
	return x_ > point.x_ ? *this : point;
}


Vector2 Vector2::TopMostPoint(const Vector2 &point) const noexcept
{
	return y_ > point.y_ ? *this : point;
}

Vector2 Vector2::BottomMostPoint(const Vector2 &point) const noexcept
{
	return y_ < point.y_ ? *this : point;
}


/*
	Reflection
*/

Vector2 Vector2::Perpendicular() const noexcept
{
	return {-y_, x_};
}

Vector2 Vector2::Reflect(const Vector2 &normal) const noexcept
{
	return *this - (normal * DotProduct(normal) * 2.0_r);
}


/*
	Rotation
*/

Vector2& Vector2::Rotate(real angle, const Vector2 &origin) noexcept
{
	return *this = RotateCopy(angle, origin);
}

Vector2 Vector2::RotateCopy(real angle, const Vector2 &origin) const noexcept
{
	return (*this - origin).Deviant(angle) + origin;
}


/*
	Scaling
*/

Vector2& Vector2::Scale(const Vector2 &scaling, const Vector2 &origin) noexcept
{
	return *this = ScaleCopy(scaling, origin);
}

Vector2 Vector2::ScaleCopy(const Vector2 &scaling, const Vector2 &origin) const noexcept
{
	return (*this - origin) * scaling + origin;
}


/*
	Sign
*/

Vector2& Vector2::Sign() noexcept
{
	return *this = SignCopy();
}

Vector2 Vector2::SignCopy() const noexcept
{
	return {static_cast<real>(math::Sign(x_)), static_cast<real>(math::Sign(y_))};
}

} //ion::graphics::utilities