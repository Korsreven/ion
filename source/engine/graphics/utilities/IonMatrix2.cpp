/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonMatrix2.cpp
-------------------------------------------
*/

#include "IonMatrix2.h"

#include <cassert>
#include <cmath>

#include "IonMatrix3.h"
#include "utilities/IonMath.h"

namespace ion::graphics::utilities
{

using namespace matrix2;
using namespace ion::utilities;

namespace matrix2::detail
{
} //matrix2::detail

Matrix2::Matrix2(real m00, real m01,
				 real m10, real m11) noexcept :
	m_{{m00, m01},
	   {m10, m11}}
{
	//Empty
}

Matrix2::Matrix2(const Matrix3 &matrix) noexcept :
	Matrix2{matrix.M00(), matrix.M01(),
			matrix.M10(), matrix.M11()}
{
	//Empty
}


/*
	Static matrix conversions
*/

Matrix2 Matrix2::Reflection(real angle) noexcept
{
	auto sin_of_angle = math::Sin(angle * 2.0_r);
	auto cos_of_angle = math::Cos(angle * 2.0_r);

	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	//Same for both left and right handed rotation
	return {cos_of_angle, sin_of_angle,
			sin_of_angle, -cos_of_angle};
	#else
	//Column-major layout (OpenGL)
	//Same for both left and right handed rotation
	return {cos_of_angle, sin_of_angle,
			sin_of_angle, -cos_of_angle};
	#endif
}

Matrix2 Matrix2::Rotation(real angle) noexcept
{
	auto sin_of_angle = math::Sin(angle);
	auto cos_of_angle = math::Cos(angle);
	
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
		#ifdef ION_LEFT_HANDED
		//Left-hand rotation CW
		return {cos_of_angle, -sin_of_angle,
				sin_of_angle, cos_of_angle};
		#else
		//Right-hand rotation CCW
		return {cos_of_angle, sin_of_angle,
				-sin_of_angle, cos_of_angle};
		#endif
	#else
	//Column-major layout (OpenGL)
		#ifdef ION_LEFT_HANDED
		//Left-hand rotation CW
		return {cos_of_angle, sin_of_angle,
				-sin_of_angle, cos_of_angle};
		#else
		//Right-hand rotation CCW
		return {cos_of_angle, -sin_of_angle,
				sin_of_angle, cos_of_angle};
		#endif
	#endif
}

Matrix2 Matrix2::Scaling(const Vector2 &vector) noexcept
{
	return {vector.X(), 0.0_r,
			0.0_r, vector.Y()};
}

Matrix2 Matrix2::Shearing(const Vector2 &vector) noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {1.0_r, vector.Y(),
			vector.X(), 1.0_r};
	#else
	//Column-major layout (OpenGL)
	return {1.0_r, vector.X(),
			vector.Y(), 1.0_r};
	#endif	
}

Matrix2 Matrix2::Transformation(real rotation, const Vector2 &scaling) noexcept
{
	auto rot = Rotation(rotation);

	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {rot.M00() * scaling.X(), rot.M01() * scaling.X(),
			rot.M10() * scaling.Y(), rot.M11() * scaling.Y()};
	#else
	//Column-major layout (OpenGL)
	return {rot.M00() * scaling.X(), rot.M01() * scaling.Y(),
			rot.M10() * scaling.X(), rot.M11() * scaling.Y()};
	#endif
}


/*
	Operators
*/

Matrix2& Matrix2::operator=(const Matrix3 &matrix) noexcept
{
	m_[0][0] = matrix.M00();		m_[0][1] = matrix.M01();
	m_[1][0] = matrix.M10();		m_[1][1] = matrix.M11();
	return *this;
}


/*
	Matrix conversions
*/

real Matrix2::ToReflection() const noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	//Same for both left and right handed rotation
	return std::atan2(m_[1][0], m_[0][0]) / 2.0_r;
	#else
	//Column-major layout (OpenGL)
	//Same for both left and right handed rotation
	return std::atan2(m_[0][1], m_[0][0]) / 2.0_r;
	#endif
}

real Matrix2::ToRotation() const noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
		#ifdef ION_LEFT_HANDED
		//Left-hand rotation CW
		return std::atan2(-m_[0][1], m_[0][0]);
		#else
		//Right-hand rotation CCW
		return std::atan2(m_[0][1], m_[0][0]);
		#endif
	#else
	//Column-major layout (OpenGL)
		#ifdef ION_LEFT_HANDED
		//Left-hand rotation CW
		return std::atan2(-m_[1][0], m_[0][0]);
		#else
		//Right-hand rotation CCW
		return std::atan2(m_[1][0], m_[0][0]);
		#endif
	#endif
}

Vector2 Matrix2::ToScaling() const noexcept
{
	//Same for both row and column-major
	return {m_[0][0], m_[1][1]};
}

Vector2 Matrix2::ToShearing() const noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {m_[1][0], m_[0][1]};
	#else
	//Column-major layout (OpenGL)
	return {m_[0][1], m_[1][0]};
	#endif
}


/*
	Adjoint/adjugate
*/

Matrix2& Matrix2::Adjoint() noexcept
{
	return *this = AdjointCopy();
}

Matrix2 Matrix2::AdjointCopy() const noexcept
{
	Matrix2 matrix{*this};
	return matrix.Cofactor().Transpose();
}


/*
	Concatenating
*/

Matrix2& Matrix2::Concatenate(const Matrix2 &matrix) noexcept
{
	return *this = ConcatenateCopy(matrix);
}

Matrix2 Matrix2::ConcatenateCopy(const Matrix2 &matrix) const noexcept
{
	return *this * matrix;
}


/*
	Cofactoring
*/

Matrix2& Matrix2::Cofactor() noexcept
{
	return *this = CofactorCopy();
}

Matrix2 Matrix2::CofactorCopy() const noexcept
{
	return { m_[1][1],
			-m_[0][1],
			-m_[1][0],
			 m_[0][0]};
}


/*
	Determinant and inversing
*/

real Matrix2::Determinant() const noexcept
{
	return m_[0][0] * m_[1][1] -
		   m_[0][1] * m_[1][0];
}


Matrix2& Matrix2::Inverse() noexcept
{
	return *this = InverseCopy();
}

Matrix2 Matrix2::InverseCopy() const noexcept
{
	Matrix2 matrix{*this};
	return matrix.Adjoint() * (1.0_r / Determinant());
}


/*
	Reflecting
*/

Matrix2& Matrix2::Reflect(real angle) noexcept
{
	return *this = ReflectCopy(angle);
}

Matrix2 Matrix2::ReflectCopy(real angle) const noexcept
{
	return Reflection(angle) * (*this);
}


/*
	Rotating
*/

Matrix2& Matrix2::Rotate(real angle) noexcept
{
	return *this = RotateCopy(angle);
}

Matrix2 Matrix2::RotateCopy(real angle) const noexcept
{
	return Rotation(angle) * (*this);
}


/*
	Scaling
*/

Matrix2& Matrix2::Scale(const Vector2 &vector) noexcept
{
	return *this = ScaleCopy(vector);
}

Matrix2 Matrix2::ScaleCopy(const Vector2 &vector) const noexcept
{
	return Scaling(vector) * (*this);
}


/*
	Shearing
*/

Matrix2& Matrix2::Shear(const Vector2 &vector) noexcept
{
	return *this = ShearCopy(vector);
}

Matrix2 Matrix2::ShearCopy(const Vector2 &vector) const noexcept
{
	return Shearing(vector) * (*this);
}


/*
	Transforming
*/

Vector2 Matrix2::TransformPoint(const Vector2 &point) const noexcept
{
	auto [x, y] = point.XY();

	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {m_[0][0] * x + m_[1][0] * y,
			m_[0][1] * x + m_[1][1] * y};
	#else
	//Column-major layout (OpenGL)
	return {m_[0][0] * x + m_[0][1] * y,
			m_[1][0] * x + m_[1][1] * y};
	#endif
}


/*
	Transposition
*/

Matrix2& Matrix2::Transpose() noexcept
{
	return *this = TransposeCopy();
}

Matrix2 Matrix2::TransposeCopy() const noexcept
{
	return {m_[0][0], m_[1][0],
			m_[0][1], m_[1][1]};
}

} //ion::graphics::utilities