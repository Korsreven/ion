/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonMatrix4.cpp
-------------------------------------------
*/

#include "IonMatrix4.h"

#include <cassert>
#include <cmath>

#include "IonMatrix3.h"
#include "utilities/IonMath.h"

namespace ion::graphics::utilities
{

using namespace matrix4;
using namespace ion::utilities;

namespace matrix4::detail
{
} //matrix4::detail

Matrix4::Matrix4(real m00, real m01, real m02, real m03,
				 real m10, real m11, real m12, real m13,
				 real m20, real m21, real m22, real m23,
				 real m30, real m31, real m32, real m33) noexcept :
	m_{{m00, m01, m02, m03},
	   {m10, m11, m12, m13},
	   {m20, m21, m22, m23},
	   {m30, m31, m32, m33}}
{
	//Empty
}

#ifdef ION_ROW_MAJOR
//Row-major layout (Direct3D)
Matrix4::Matrix4(real m00, real m01, real m02,
				 real m10, real m11, real m12,
				 real m20, real m21, real m22,
				 real m30, real m31, real m32) noexcept :
	m_{{m00, m01, m02, 0.0_r},
	   {m10, m11, m12, 0.0_r},
	   {m20, m21, m22, 0.0_r},
	   {m30, m31, m32, 1.0_r}}
{
	//Empty
}
#else
//Column-major layout (OpenGL)
Matrix4::Matrix4(real m00, real m01, real m02, real m03,
				 real m10, real m11, real m12, real m13,
				 real m20, real m21, real m22, real m23) noexcept :
	m_{{m00, m01, m02, m03},
	   {m10, m11, m12, m13},
	   {m20, m21, m22, m23},
	   {0.0_r, 0.0_r, 0.0_r, 1.0_r}}
{
	//Empty
}
#endif

Matrix4::Matrix4(const Matrix3 &matrix) noexcept :
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	Matrix4{matrix.M00(), matrix.M01(), 0.0_r,
			matrix.M10(), matrix.M11(), 0.0_r,
			0.0_r, 0.0_r, 1.0_r,
			matrix.M20(), matrix.M21(), 0.0_r}
	#else
	//Column-major layout (OpenGL)
	Matrix4{matrix.M00(), matrix.M01(), 0.0_r, matrix.M02(),
			matrix.M10(), matrix.M11(), 0.0_r, matrix.M12(),
			0.0_r, 0.0_r, 1.0_r, 0.0_r}
	#endif
{
	//Empty
}


/*
	Static matrix conversions
*/

Matrix4 Matrix4::Reflection(real angle) noexcept
{
	auto sin_of_angle = math::Sin(angle * 2.0_r);
	auto cos_of_angle = math::Cos(angle * 2.0_r);

	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	//Same for both left and right handed rotation
	//cos, sin
	//sin, -cos
	return {cos_of_angle, sin_of_angle, 0.0_r,
			sin_of_angle, -cos_of_angle, 0.0_r,
			0.0_r, 0.0_r, 1.0_r,
			0.0_r, 0.0_r, 0.0_r};
	#else
	//Column-major layout (OpenGL)
	//Same for both left and right handed rotation
	//cos, sin
	//sin, -cos
	return {cos_of_angle, sin_of_angle, 0.0_r, 0.0_r,
			sin_of_angle, -cos_of_angle, 0.0_r, 0.0_r,
			0.0_r, 0.0_r, 1.0_r, 0.0_r};
	#endif
}

Matrix4 Matrix4::Rotation(real angle) noexcept
{
	auto sin_of_angle = math::Sin(angle);
	auto cos_of_angle = math::Cos(angle);
	
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	//Left-hand rotation CW
	// cos, sin
	//-sin, cos
	return {cos_of_angle, sin_of_angle, 0.0_r,
			-sin_of_angle, cos_of_angle, 0.0_r,
			0.0_r, 0.0_r, 1.0_r,
			0.0_r, 0.0_r, 0.0_r};
	#else
	//Column-major layout (OpenGL)
	//Right-hand rotation CCW
	//cos, -sin
	//sin, cos
	return {cos_of_angle, -sin_of_angle, 0.0_r, 0.0_r,
			sin_of_angle, cos_of_angle, 0.0_r, 0.0_r,
			0.0_r, 0.0_r, 1.0_r, 0.0_r};
	#endif
}

Matrix4 Matrix4::Rotation(real angle, const Vector3 &origin) noexcept
{
	return Translation(origin).Rotate(angle).Translate(-origin);
}

Matrix4 Matrix4::Scaling(const Vector3 &vector) noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {vector.X(), 0.0_r, 0.0_r,
			0.0_r, vector.Y(), 0.0_r,
			0.0_r, 0.0_r, vector.Z(),
			0.0_r, 0.0_r, 0.0_r};
	#else
	//Column-major layout (OpenGL)
	return {vector.X(), 0.0_r, 0.0_r, 0.0_r,
			0.0_r, vector.Y(), 0.0_r, 0.0_r,
			0.0_r, 0.0_r, vector.Z(), 0.0_r};
	#endif
}

Matrix4 Matrix4::Shearing(const Vector3 &vector) noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {1.0_r, vector.Y(), 0.0_r,
			vector.X(), 1.0_r, 0.0_r,
			0.0_r, 0.0_r, 1.0_r,
			0.0_r, 0.0_r, 0.0_r};
	#else
	//Column-major layout (OpenGL)
	return {1.0_r, vector.X(), 0.0_r, 0.0_r,
			vector.Y(), 1.0_r, 0.0_r, 0.0_r,
			0.0_r, 0.0_r, 1.0_r, 0.0_r};
	#endif
}

Matrix4 Matrix4::Translation(const Vector3 &vector) noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {1.0_r, 0.0_r, 0.0_r,
			0.0_r, 1.0_r, 0.0_r,
			0.0_r, 0.0_r, 1.0_r,
			vector.X(), vector.Y(), vector.Z()};
	#else
	//Column-major layout (OpenGL)
	return {1.0_r, 0.0_r, 0.0_r, vector.X(),
			0.0_r, 1.0_r, 0.0_r, vector.Y(),
			0.0_r, 0.0_r, 1.0_r, vector.Z()};
	#endif
}

Matrix4 Matrix4::Transformation(real rotation, const Vector3 &scaling, const Vector3 &translation) noexcept
{
	auto rot = Rotation(rotation);

	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {rot.M00() * scaling.X(), rot.M01() * scaling.X(), rot.M02() * scaling.X(),
			rot.M10() * scaling.Y(), rot.M11() * scaling.Y(), rot.M12() * scaling.Y(),
			rot.M20() * scaling.Z(), rot.M21() * scaling.Z(), rot.M22() * scaling.Z(),
			translation.X(), translation.Y(), translation.Z()};
	#else
	//Column-major layout (OpenGL)
	return {rot.M00() * scaling.X(), rot.M01() * scaling.Y(), rot.M02() * scaling.Z(), translation.X(),
			rot.M10() * scaling.X(), rot.M11() * scaling.Y(), rot.M12() * scaling.Z(), translation.Y(),
			rot.M20() * scaling.X(), rot.M21() * scaling.Y(), rot.M22() * scaling.Z(), translation.Z()};
	#endif
}


/*
	Operators
*/

Matrix4& Matrix4::operator=(const Matrix3 &matrix) noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	m_[0][0] = matrix.M00();		m_[0][1] = matrix.M01();		m_[0][2] = 0.0_r;				m_[0][3] = 0.0_r;
	m_[1][0] = matrix.M10();		m_[1][1] = matrix.M11();		m_[1][2] = 0.0_r;				m_[1][3] = 0.0_r;
	m_[2][0] = 0.0_r;				m_[2][1] = 0.0_r;				m_[2][2] = 1.0_r;				m_[2][3] = 0.0_r;
	m_[3][0] = matrix.M20();		m_[3][1] = matrix.M21();		m_[3][2] = 0.0_r;				m_[3][3] = 1.0_r;
	#else
	//Column-major layout (OpenGL)
	m_[0][0] = matrix.M00();		m_[0][1] = matrix.M01();		m_[0][2] = 0.0_r;				m_[0][3] = matrix.M02();
	m_[1][0] = matrix.M10();		m_[1][1] = matrix.M11();		m_[1][2] = 0.0_r;				m_[1][3] = matrix.M12();
	m_[2][0] = 0.0_r;				m_[2][1] = 0.0_r;				m_[2][2] = 1.0_r;				m_[2][3] = 0.0_r;
	m_[3][0] = 0.0_r;				m_[3][1] = 0.0_r;				m_[3][2] = 0.0_r;				m_[3][3] = 1.0_r;
	#endif

	return *this;
}


/*
	Matrix conversions
*/

real Matrix4::ToReflection() const noexcept
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

real Matrix4::ToRotation() const noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	//Left-hand rotation CW
	return std::atan2(m_[0][1], m_[0][0]);
	#else
	//Column-major layout (OpenGL)
	//Right-hand rotation CCW
	return std::atan2(m_[1][0], m_[0][0]);
	#endif
}

Vector3 Matrix4::ToScaling() const noexcept
{
	//Same for both row and column-major
	return {m_[0][0], m_[1][1], m_[2][2]};
}

Vector3 Matrix4::ToShearing() const noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {m_[1][0], m_[0][1], 0.0_r};
	#else
	//Column-major layout (OpenGL)
	return {m_[0][1], m_[1][0], 0.0_r};
	#endif
}

Vector3 Matrix4::ToTranslation() const noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {m_[3][0], m_[3][1], m_[3][2]};
	#else
	//Column-major layout (OpenGL)
	return {m_[0][3], m_[1][3], m_[2][3]};
	#endif
}


/*
	Adjoint/adjugate
*/

Matrix4& Matrix4::Adjoint() noexcept
{
	return *this = AdjointCopy();
}

Matrix4 Matrix4::AdjointCopy() const noexcept
{
	Matrix4 matrix{*this};
	return matrix.Cofactor().Transpose();
}


/*
	Affine
*/

Matrix4& Matrix4::Affine() noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	m_[0][3] = 0.0_r;
	m_[1][3] = 0.0_r;
	m_[2][3] = 0.0_r;
	m_[3][3] = 1.0_r;
	#else
	//Column-major layout (OpenGL)
	m_[3][0] = 0.0_r;
	m_[3][1] = 0.0_r;
	m_[3][2] = 0.0_r;
	m_[3][3] = 1.0_r;
	#endif
	return *this;
}

Matrix4 Matrix4::AffineCopy() const noexcept
{
	Matrix4 matrix{*this};
	return matrix.Affine();
}

bool Matrix4::IsAffine() const noexcept
{
	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return m_[0][3] == 0.0_r && m_[1][3] == 0.0_r && m_[2][3] == 0.0_r && m_[3][3] == 1.0_r;
	#else
	//Column-major layout (OpenGL)
	return m_[3][0] == 0.0_r && m_[3][1] == 0.0_r && m_[3][2] == 0.0_r && m_[3][3] == 1.0_r;
	#endif
}


/*
	Concatenating
*/

Matrix4& Matrix4::Concatenate(const Matrix4 &matrix) noexcept
{
	return *this = ConcatenateCopy(matrix);
}

Matrix4 Matrix4::ConcatenateCopy(const Matrix4 &matrix) const noexcept
{
	return *this * matrix;
}


Matrix4& Matrix4::ConcatenateAffine(const Matrix4 &matrix) noexcept
{
	return *this = ConcatenateAffineCopy(matrix);
}

Matrix4 Matrix4::ConcatenateAffineCopy(const Matrix4 &matrix) const noexcept
{
	//Matrices must be affine
	assert(IsAffine() && matrix.IsAffine());

	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	return {m_[0][0] * matrix.m_[0][0] + m_[1][0] * matrix.m_[0][1] + m_[2][0] * matrix.m_[0][2],
			m_[0][1] * matrix.m_[0][0] + m_[1][1] * matrix.m_[0][1] + m_[2][1] * matrix.m_[0][2],
			m_[0][2] * matrix.m_[0][0] + m_[1][2] * matrix.m_[0][1] + m_[2][2] * matrix.m_[0][2],
			0.0_r,

			m_[0][0] * matrix.m_[1][0] + m_[1][0] * matrix.m_[1][1] + m_[2][0] * matrix.m_[1][2],
			m_[0][1] * matrix.m_[1][0] + m_[1][1] * matrix.m_[1][1] + m_[2][1] * matrix.m_[1][2],
			m_[0][2] * matrix.m_[1][0] + m_[1][2] * matrix.m_[1][1] + m_[2][2] * matrix.m_[1][2],
			0.0_r,

			m_[0][0] * matrix.m_[2][0] + m_[1][0] * matrix.m_[2][1] + m_[2][0] * matrix.m_[2][2],
			m_[0][1] * matrix.m_[2][0] + m_[1][1] * matrix.m_[2][1] + m_[2][1] * matrix.m_[2][2],
			m_[0][2] * matrix.m_[2][0] + m_[1][2] * matrix.m_[2][1] + m_[2][2] * matrix.m_[2][2],
			0.0_r,

			m_[0][0] * matrix.m_[3][0] + m_[1][0] * matrix.m_[3][1] + m_[2][0] * matrix.m_[3][2] + m_[3][0],
			m_[0][1] * matrix.m_[3][0] + m_[1][1] * matrix.m_[3][1] + m_[2][1] * matrix.m_[3][2] + m_[3][1],
			m_[0][2] * matrix.m_[3][0] + m_[1][2] * matrix.m_[3][1] + m_[2][2] * matrix.m_[3][2] + m_[3][2],
			1.0_r};
	#else
	//Column-major layout (OpenGL)
	return {m_[0][0] * matrix.m_[0][0] + m_[0][1] * matrix.m_[1][0] + m_[0][2] * matrix.m_[2][0],
			m_[0][0] * matrix.m_[0][1] + m_[0][1] * matrix.m_[1][1] + m_[0][2] * matrix.m_[2][1],
			m_[0][0] * matrix.m_[0][2] + m_[0][1] * matrix.m_[1][2] + m_[0][2] * matrix.m_[2][2],
			m_[0][0] * matrix.m_[0][3] + m_[0][1] * matrix.m_[1][3] + m_[0][2] * matrix.m_[2][3] + m_[0][3],

			m_[1][0] * matrix.m_[0][0] + m_[1][1] * matrix.m_[1][0] + m_[1][2] * matrix.m_[2][0],
			m_[1][0] * matrix.m_[0][1] + m_[1][1] * matrix.m_[1][1] + m_[1][2] * matrix.m_[2][1],
			m_[1][0] * matrix.m_[0][2] + m_[1][1] * matrix.m_[1][2] + m_[1][2] * matrix.m_[2][2],
			m_[1][0] * matrix.m_[0][3] + m_[1][1] * matrix.m_[1][3] + m_[1][2] * matrix.m_[2][3] + m_[1][3],

			m_[2][0] * matrix.m_[0][0] + m_[2][1] * matrix.m_[1][0] + m_[2][2] * matrix.m_[2][0],
			m_[2][0] * matrix.m_[0][1] + m_[2][1] * matrix.m_[1][1] + m_[2][2] * matrix.m_[2][1],
			m_[2][0] * matrix.m_[0][2] + m_[2][1] * matrix.m_[1][2] + m_[2][2] * matrix.m_[2][2],
			m_[2][0] * matrix.m_[0][3] + m_[2][1] * matrix.m_[1][3] + m_[2][2] * matrix.m_[2][3] + m_[2][3],

			0.0_r, 0.0_r, 0.0_r, 1.0_r};
	#endif
}


/*
	Cofactoring
*/

Matrix4& Matrix4::Cofactor() noexcept
{
	return *this = CofactorCopy();
}

Matrix4 Matrix4::CofactorCopy() const noexcept
{
	return {  m_[1][1] * m_[2][2] * m_[3][3] + m_[1][2] * m_[2][3] * m_[3][1] + m_[1][3] * m_[2][1] * m_[3][2] - m_[1][3] * m_[2][2] * m_[3][1] - m_[1][2] * m_[2][1] * m_[3][3] - m_[1][1] * m_[2][3] * m_[3][2],
			-(m_[0][1] * m_[2][2] * m_[3][3] + m_[0][2] * m_[2][3] * m_[3][1] + m_[0][3] * m_[2][1] * m_[3][2] - m_[0][3] * m_[2][2] * m_[3][1] - m_[0][2] * m_[2][1] * m_[3][3] - m_[0][1] * m_[2][3] * m_[3][2]),
			  m_[0][1] * m_[1][2] * m_[3][3] + m_[0][2] * m_[1][3] * m_[3][1] + m_[0][3] * m_[1][1] * m_[3][2] - m_[0][3] * m_[1][2] * m_[3][1] - m_[0][2] * m_[1][1] * m_[3][3] - m_[0][1] * m_[1][3] * m_[3][2],
			-(m_[0][1] * m_[1][2] * m_[2][3] + m_[0][2] * m_[1][3] * m_[2][1] + m_[0][3] * m_[1][1] * m_[2][2] - m_[0][3] * m_[1][2] * m_[2][1] - m_[0][2] * m_[1][1] * m_[2][3] - m_[0][1] * m_[1][3] * m_[2][2]),

			-(m_[1][0] * m_[2][2] * m_[3][3] + m_[1][2] * m_[2][3] * m_[3][0] + m_[1][3] * m_[2][0] * m_[3][2] - m_[1][3] * m_[2][2] * m_[3][0] - m_[1][2] * m_[2][0] * m_[3][3] - m_[1][0] * m_[2][3] * m_[3][2]),
			  m_[0][0] * m_[2][2] * m_[3][3] + m_[0][2] * m_[2][3] * m_[3][0] + m_[0][3] * m_[2][0] * m_[3][2] - m_[0][3] * m_[2][2] * m_[3][0] - m_[0][2] * m_[2][0] * m_[3][3] - m_[0][0] * m_[2][3] * m_[3][2],
			-(m_[0][0] * m_[1][2] * m_[3][3] + m_[0][2] * m_[1][3] * m_[3][0] + m_[0][3] * m_[1][0] * m_[3][2] - m_[0][3] * m_[1][2] * m_[3][0] - m_[0][2] * m_[1][0] * m_[3][3] - m_[0][0] * m_[1][3] * m_[3][2]),
			  m_[0][0] * m_[1][2] * m_[2][3] + m_[0][2] * m_[1][3] * m_[2][0] + m_[0][3] * m_[1][0] * m_[2][2] - m_[0][3] * m_[1][2] * m_[2][0] - m_[0][2] * m_[1][0] * m_[2][3] - m_[0][0] * m_[1][3] * m_[2][2],

			  m_[1][0] * m_[2][1] * m_[3][3] + m_[1][1] * m_[2][3] * m_[3][0] + m_[1][3] * m_[2][0] * m_[3][1] - m_[1][3] * m_[2][1] * m_[3][0] - m_[1][1] * m_[2][0] * m_[3][3] - m_[1][0] * m_[2][3] * m_[3][1],
			-(m_[0][0] * m_[2][1] * m_[3][3] + m_[0][1] * m_[2][3] * m_[3][0] + m_[0][3] * m_[2][0] * m_[3][1] - m_[0][3] * m_[2][1] * m_[3][0] - m_[0][1] * m_[2][0] * m_[3][3] - m_[0][0] * m_[2][3] * m_[3][1]),
			  m_[0][0] * m_[1][1] * m_[3][3] + m_[0][1] * m_[1][3] * m_[3][0] + m_[0][3] * m_[1][0] * m_[3][1] - m_[0][3] * m_[1][1] * m_[3][0] - m_[0][1] * m_[1][0] * m_[3][3] - m_[0][0] * m_[1][3] * m_[3][1],
			-(m_[0][0] * m_[1][1] * m_[2][3] + m_[0][1] * m_[1][3] * m_[2][0] + m_[0][3] * m_[1][0] * m_[2][1] - m_[0][3] * m_[1][1] * m_[2][0] - m_[0][1] * m_[1][0] * m_[2][3] - m_[0][0] * m_[1][3] * m_[2][1]),
			
			-(m_[1][0] * m_[2][1] * m_[3][2] + m_[1][1] * m_[2][2] * m_[3][0] + m_[1][2] * m_[2][0] * m_[3][1] - m_[1][2] * m_[2][1] * m_[3][0] - m_[1][1] * m_[2][0] * m_[3][2] - m_[1][0] * m_[2][2] * m_[3][1]),
			  m_[0][0] * m_[2][1] * m_[3][2] + m_[0][1] * m_[2][2] * m_[3][0] + m_[0][2] * m_[2][0] * m_[3][1] - m_[0][2] * m_[2][1] * m_[3][0] - m_[0][1] * m_[2][0] * m_[3][2] - m_[0][0] * m_[2][2] * m_[3][1],
			-(m_[0][0] * m_[1][1] * m_[3][2] + m_[0][1] * m_[1][2] * m_[3][0] + m_[0][2] * m_[1][0] * m_[3][1] - m_[0][2] * m_[1][1] * m_[3][0] - m_[0][1] * m_[1][0] * m_[3][2] - m_[0][0] * m_[1][2] * m_[3][1]),
			  m_[0][0] * m_[1][1] * m_[2][2] + m_[0][1] * m_[1][2] * m_[2][0] + m_[0][2] * m_[1][0] * m_[2][1] - m_[0][2] * m_[1][1] * m_[2][0] - m_[0][1] * m_[1][0] * m_[2][2] - m_[0][0] * m_[1][2] * m_[2][1]};
}


/*
	Determinant and inversing
*/

real Matrix4::Determinant() const noexcept
{
	return m_[0][0] * (m_[1][1] * (m_[2][2] * m_[3][3] - m_[3][2] * m_[2][3]) -
					   m_[1][2] * (m_[2][1] * m_[3][3] - m_[3][1] * m_[2][3]) +
					   m_[1][3] * (m_[2][1] * m_[3][2] - m_[3][1] * m_[2][2])) -

		   m_[0][1] * (m_[1][0] * (m_[2][2] * m_[3][3] - m_[3][2] * m_[2][3]) -
					   m_[1][2] * (m_[2][0] * m_[3][3] - m_[3][0] * m_[2][3]) +
					   m_[1][3] * (m_[2][0] * m_[3][2] - m_[3][0] * m_[2][2])) +

		   m_[0][2] * (m_[1][0] * (m_[2][1] * m_[3][3] - m_[3][1] * m_[2][3]) -
					   m_[1][1] * (m_[2][0] * m_[3][3] - m_[3][0] * m_[2][3]) +
					   m_[1][3] * (m_[2][0] * m_[3][1] - m_[3][0] * m_[2][1])) -

		   m_[0][3] * (m_[1][0] * (m_[2][1] * m_[3][2] - m_[3][1] * m_[2][2]) -
					   m_[1][1] * (m_[2][0] * m_[3][2] - m_[3][0] * m_[2][2]) +
					   m_[1][2] * (m_[2][0] * m_[3][1] - m_[3][0] * m_[2][1]));
}


Matrix4& Matrix4::Inverse() noexcept
{
	return *this = InverseCopy();
}

Matrix4 Matrix4::InverseCopy() const noexcept
{
	Matrix4 matrix{*this};
	return matrix.Adjoint() * (1.0_r / Determinant());
}


/*
	Reflecting
*/

Matrix4& Matrix4::Reflect(real angle) noexcept
{
	return *this = ReflectCopy(angle);
}

Matrix4 Matrix4::ReflectCopy(real angle) const noexcept
{
	return Reflection(angle) * (*this);
}


/*
	Rotating
*/

Matrix4& Matrix4::Rotate(real angle) noexcept
{
	return *this = RotateCopy(angle);
}

Matrix4 Matrix4::RotateCopy(real angle) const noexcept
{
	return Rotation(angle) * (*this);
}


/*
	Scaling
*/

Matrix4& Matrix4::Scale(const Vector3 &vector) noexcept
{
	return *this = ScaleCopy(vector);
}

Matrix4 Matrix4::ScaleCopy(const Vector3 &vector) const noexcept
{
	return Scaling(vector) * (*this);
}


/*
	Shearing
*/

Matrix4& Matrix4::Shear(const Vector3 &vector) noexcept
{
	return *this = ShearCopy(vector);
}

Matrix4 Matrix4::ShearCopy(const Vector3 &vector) const noexcept
{
	return Shearing(vector) * (*this);
}


/*
	Transforming
*/

Vector3 Matrix4::TransformPoint(const Vector3 &point) const noexcept
{
	auto [x, y, z] = point.XYZ();

	#ifdef ION_ROW_MAJOR
	//Row-major layout (Direct3D)
	//Left-hand rotation CW
	return {(m_[0][0] * x + m_[1][0] * y + m_[2][0] * z) + m_[3][0],
			(m_[0][1] * x + m_[1][1] * y + m_[2][1] * z) + m_[3][1],
			(m_[0][2] * x + m_[1][2] * y + m_[2][2] * z) + m_[3][2]};
	#else
	//Column-major layout (OpenGL)
	//Right-hand rotation CCW
	return {(m_[0][0] * x + m_[0][1] * y + m_[0][2] * z) + m_[0][3],
			(m_[1][0] * x + m_[1][1] * y + m_[1][2] * z) + m_[1][3],
			(m_[2][0] * x + m_[2][1] * y + m_[2][2] * z) + m_[2][3]};
	#endif
}


/*
	Translating
*/

Matrix4& Matrix4::Translate(const Vector3 &vector) noexcept
{
	return *this = TranslateCopy(vector);
}

Matrix4 Matrix4::TranslateCopy(const Vector3 &vector) const noexcept
{
	return Translation(vector) * (*this);
}


/*
	Transposition
*/

Matrix4& Matrix4::Transpose() noexcept
{
	return *this = TransposeCopy();
}

Matrix4 Matrix4::TransposeCopy() const noexcept
{
	return {m_[0][0], m_[1][0], m_[2][0], m_[3][0],
			m_[0][1], m_[1][1], m_[2][1], m_[3][1],
			m_[0][2], m_[1][2], m_[2][2], m_[3][2],
			m_[0][3], m_[1][3], m_[2][3], m_[3][3]};
}

} //ion::graphics::utilities