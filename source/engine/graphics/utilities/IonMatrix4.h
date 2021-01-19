/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonMatrix4.h
-------------------------------------------
*/

#ifndef ION_MATRIX4_H
#define ION_MATRIX4_H

#include "IonVector3.h"
#include "types/IonTypes.h"

namespace ion::graphics::utilities
{
	class Matrix3; //Forward declaration
	using namespace types::type_literals;

	namespace matrix4::detail
	{
	} //matrix4::detail


	//A 4x4 column-major matrix class with right hand rotation
	//Follows the OpenGL matrix convention by default
	//Define ION_ROW_MAJOR to use this class with Direct3D
	class Matrix4 final
	{
		private:

			union
			{
				real m_[4][4];
				real numbers_[16]{1.0_r, 0.0_r, 0.0_r, 0.0_r,
								  0.0_r, 1.0_r, 0.0_r, 0.0_r,
								  0.0_r, 0.0_r, 1.0_r, 0.0_r,
								  0.0_r, 0.0_r, 0.0_r, 1.0_r};
			};

		public:

			Matrix4() = default;

			//Constructs a new matrix from the given numbers
			Matrix4(real m00, real m01, real m02, real m03,
					real m10, real m11, real m12, real m13,
					real m20, real m21, real m22, real m23,
					real m30, real m31, real m32, real m33) noexcept;
			
			#ifdef ION_ROW_MAJOR
			//Row-major layout (Direct3D)
			//Constructs a new matrix from only the three first columns
			//The fourth column is filled with {0, 0, 0, 1}
			Matrix4(real m00, real m01, real m02,
					real m10, real m11, real m12,
					real m20, real m21, real m22,
					real m30, real m31, real m32) noexcept;
			#else
			//Column-major layout (OpenGL)
			//Constructs a new matrix from only the three first rows
			//The fourth row is filled with {0, 0, 0, 1}
			Matrix4(real m00, real m01, real m02, real m03,
					real m10, real m11, real m12, real m13,
					real m20, real m21, real m22, real m23) noexcept;
			#endif

			//Constructs a new matrix from the given 3x3 matrix
			Matrix4(const Matrix3 &matrix) noexcept;
			

			/*
				Static matrix conversions
			*/

			//Returns a new reflection matrix from the given angle (radians)
			[[nodiscard]] static Matrix4 Reflection(real angle) noexcept;

			//Returns a new rotation matrix from the given angle (radians)
			[[nodiscard]] static Matrix4 Rotation(real angle) noexcept;

			//Returns a new rotation matrix from the given angle (radians) and origin vector
			[[nodiscard]] static Matrix4 Rotation(real angle, const Vector3 &origin) noexcept;

			//Returns a new scaling matrix from the given vector
			[[nodiscard]] static Matrix4 Scaling(const Vector3 &vector) noexcept;

			//Returns a new shearing matrix from the given vector
			[[nodiscard]] static Matrix4 Shearing(const Vector3 &vector) noexcept;

			//Returns a new translation matrix from the given vector
			[[nodiscard]] static Matrix4 Translation(const Vector3 &vector) noexcept;


			/*
				Operators
			*/

			//Sets this matrix equal to the given 3x3 matrix
			Matrix4& operator=(const Matrix3 &matrix) noexcept;


			//Checks if two matrices are equal (all numbers are equal)
			[[nodiscard]] inline auto operator==(const Matrix4 &rhs) const noexcept
			{
				return m_[0][0] == rhs.m_[0][0] && m_[0][1] == rhs.m_[0][1] && m_[0][2] == rhs.m_[0][2] && m_[0][3] == rhs.m_[0][3] &&
					   m_[1][0] == rhs.m_[1][0] && m_[1][1] == rhs.m_[1][1] && m_[1][2] == rhs.m_[1][2] && m_[1][3] == rhs.m_[1][3] &&
					   m_[2][0] == rhs.m_[2][0] && m_[2][1] == rhs.m_[2][1] && m_[2][2] == rhs.m_[2][2] && m_[2][3] == rhs.m_[2][3] &&
					   m_[3][0] == rhs.m_[3][0] && m_[3][1] == rhs.m_[3][1] && m_[3][2] == rhs.m_[3][2] && m_[3][3] == rhs.m_[3][3];
			}

			//Checks if two matrices are different (one or more numbers are different)
			[[nodiscard]] inline auto operator!=(const Matrix4 &rhs) const noexcept
			{
				return !(*this == rhs);
			}


			//Negate matrix (negating each number)
			[[nodiscard]] inline auto operator-() const noexcept
			{
				return Matrix4{-m_[0][0], -m_[0][1], -m_[0][2], -m_[0][3],
							   -m_[1][0], -m_[1][1], -m_[1][2], -m_[1][3],
							   -m_[2][0], -m_[2][1], -m_[2][2], -m_[2][3],
							   -m_[3][0], -m_[3][1], -m_[3][2], -m_[3][3]};
			}


			/*
				Adding
			*/

			//Add two matrices (adding each number)
			[[nodiscard]] inline auto operator+(const Matrix4 &rhs) const noexcept
			{
				return Matrix4{m_[0][0] + rhs.m_[0][0], m_[0][1] + rhs.m_[0][1], m_[0][2] + rhs.m_[0][2], m_[0][3] + rhs.m_[0][3],
							   m_[1][0] + rhs.m_[1][0], m_[1][1] + rhs.m_[1][1], m_[1][2] + rhs.m_[1][2], m_[1][3] + rhs.m_[1][3],
							   m_[2][0] + rhs.m_[2][0], m_[2][1] + rhs.m_[2][1], m_[2][2] + rhs.m_[2][2], m_[2][3] + rhs.m_[2][3],
							   m_[3][0] + rhs.m_[3][0], m_[3][1] + rhs.m_[3][1], m_[3][2] + rhs.m_[3][2], m_[3][3] + rhs.m_[3][3]};
			}

			//Add all numbers to the given scalar
			[[nodiscard]] inline auto operator+(real scalar) const noexcept
			{
				return Matrix4{m_[0][0] + scalar, m_[0][1] + scalar, m_[0][2] + scalar, m_[0][3] + scalar,
							   m_[1][0] + scalar, m_[1][1] + scalar, m_[1][2] + scalar, m_[1][3] + scalar,
							   m_[2][0] + scalar, m_[2][1] + scalar, m_[2][2] + scalar, m_[2][3] + scalar,
							   m_[3][0] + scalar, m_[3][1] + scalar, m_[3][2] + scalar, m_[3][3] + scalar};
			}

			//Add all numbers to the given scalar
			[[nodiscard]] friend inline auto operator+(real scalar, const Matrix4 &rhs) noexcept
			{
				return rhs + scalar; //Commutative
			}

			//Add two matrices (adding each number)
			inline auto& operator+=(const Matrix4 &rhs) noexcept
			{
				return *this = *this + rhs;
			}

			//Add all numbers to the given scalar
			inline auto& operator+=(real scalar) noexcept
			{
				return *this = *this + scalar;
			}


			/*
				Subtracting
			*/

			//Subtract two matrices (subtracting each number)
			[[nodiscard]] inline auto operator-(const Matrix4 &rhs) const noexcept
			{
				return Matrix4{m_[0][0] - rhs.m_[0][0], m_[0][1] - rhs.m_[0][1], m_[0][2] - rhs.m_[0][2], m_[0][3] - rhs.m_[0][3],
							   m_[1][0] - rhs.m_[1][0], m_[1][1] - rhs.m_[1][1], m_[1][2] - rhs.m_[1][2], m_[1][3] - rhs.m_[1][3],
							   m_[2][0] - rhs.m_[2][0], m_[2][1] - rhs.m_[2][1], m_[2][2] - rhs.m_[2][2], m_[2][3] - rhs.m_[2][3],
							   m_[3][0] - rhs.m_[3][0], m_[3][1] - rhs.m_[3][1], m_[3][2] - rhs.m_[3][2], m_[3][3] - rhs.m_[3][3]};
			}

			//Subtract all numbers with the given scalar
			[[nodiscard]] inline auto operator-(real scalar) const noexcept
			{
				return Matrix4{m_[0][0] - scalar, m_[0][1] - scalar, m_[0][2] - scalar, m_[0][3] - scalar,
							   m_[1][0] - scalar, m_[1][1] - scalar, m_[1][2] - scalar, m_[1][3] - scalar,
							   m_[2][0] - scalar, m_[2][1] - scalar, m_[2][2] - scalar, m_[2][3] - scalar,
							   m_[3][0] - scalar, m_[3][1] - scalar, m_[3][2] - scalar, m_[3][3] - scalar};
			}

			//Subtract all numbers with the given scalar
			[[nodiscard]] friend inline auto operator-(real scalar, const Matrix4 &rhs) noexcept
			{
				return Matrix4{scalar - rhs.m_[0][0], scalar - rhs.m_[0][1], scalar - rhs.m_[0][2], scalar - rhs.m_[0][3],
							   scalar - rhs.m_[1][0], scalar - rhs.m_[1][1], scalar - rhs.m_[1][2], scalar - rhs.m_[1][3],
							   scalar - rhs.m_[2][0], scalar - rhs.m_[2][1], scalar - rhs.m_[2][2], scalar - rhs.m_[2][3],
							   scalar - rhs.m_[3][0], scalar - rhs.m_[3][1], scalar - rhs.m_[3][2], scalar - rhs.m_[3][3]};
			}

			//Subtract two matrices (subtracting each number)
			inline auto& operator-=(const Matrix4 &rhs) noexcept
			{
				return *this = *this - rhs;
			}

			//Subtract all numbers with the given scalar
			inline auto& operator-=(real scalar) noexcept
			{
				return *this = *this - scalar;
			}


			/*
				Multiplying
			*/

			//Multiply two matrices (matrix multiplication)
			[[nodiscard]] inline auto operator*(const Matrix4 &rhs) const noexcept
			{
				#ifdef ION_ROW_MAJOR
				//Row-major layout (Direct3D)
				return Matrix4{m_[0][0] * rhs.m_[0][0] + m_[1][0] * rhs.m_[0][1] + m_[2][0] * rhs.m_[0][2] + m_[3][0] * rhs.m_[0][3],
							   m_[0][1] * rhs.m_[0][0] + m_[1][1] * rhs.m_[0][1] + m_[2][1] * rhs.m_[0][2] + m_[3][1] * rhs.m_[0][3],
							   m_[0][2] * rhs.m_[0][0] + m_[1][2] * rhs.m_[0][1] + m_[2][2] * rhs.m_[0][2] + m_[3][2] * rhs.m_[0][3],
							   m_[0][3] * rhs.m_[0][0] + m_[1][3] * rhs.m_[0][1] + m_[2][3] * rhs.m_[0][2] + m_[3][3] * rhs.m_[0][3],

							   m_[0][0] * rhs.m_[1][0] + m_[1][0] * rhs.m_[1][1] + m_[2][0] * rhs.m_[1][2] + m_[3][0] * rhs.m_[1][3],
							   m_[0][1] * rhs.m_[1][0] + m_[1][1] * rhs.m_[1][1] + m_[2][1] * rhs.m_[1][2] + m_[3][1] * rhs.m_[1][3],
							   m_[0][2] * rhs.m_[1][0] + m_[1][2] * rhs.m_[1][1] + m_[2][2] * rhs.m_[1][2] + m_[3][2] * rhs.m_[1][3],
							   m_[0][3] * rhs.m_[1][0] + m_[1][3] * rhs.m_[1][1] + m_[2][3] * rhs.m_[1][2] + m_[3][3] * rhs.m_[1][3],

							   m_[0][0] * rhs.m_[2][0] + m_[1][0] * rhs.m_[2][1] + m_[2][0] * rhs.m_[2][2] + m_[3][0] * rhs.m_[2][3],
							   m_[0][1] * rhs.m_[2][0] + m_[1][1] * rhs.m_[2][1] + m_[2][1] * rhs.m_[2][2] + m_[3][1] * rhs.m_[2][3],
							   m_[0][2] * rhs.m_[2][0] + m_[1][2] * rhs.m_[2][1] + m_[2][2] * rhs.m_[2][2] + m_[3][2] * rhs.m_[2][3],
							   m_[0][3] * rhs.m_[2][0] + m_[1][3] * rhs.m_[2][1] + m_[2][3] * rhs.m_[2][2] + m_[3][3] * rhs.m_[2][3],

							   m_[0][0] * rhs.m_[3][0] + m_[1][0] * rhs.m_[3][1] + m_[2][0] * rhs.m_[3][2] + m_[3][0] * rhs.m_[3][3],
							   m_[0][1] * rhs.m_[3][0] + m_[1][1] * rhs.m_[3][1] + m_[2][1] * rhs.m_[3][2] + m_[3][1] * rhs.m_[3][3],
							   m_[0][2] * rhs.m_[3][0] + m_[1][2] * rhs.m_[3][1] + m_[2][2] * rhs.m_[3][2] + m_[3][2] * rhs.m_[3][3],
							   m_[0][3] * rhs.m_[3][0] + m_[1][3] * rhs.m_[3][1] + m_[2][3] * rhs.m_[3][2] + m_[3][3] * rhs.m_[3][3]};
				#else
				//Column-major layout (OpenGL)
				return Matrix4{m_[0][0] * rhs.m_[0][0] + m_[0][1] * rhs.m_[1][0] + m_[0][2] * rhs.m_[2][0] + m_[0][3] * rhs.m_[3][0],
							   m_[0][0] * rhs.m_[0][1] + m_[0][1] * rhs.m_[1][1] + m_[0][2] * rhs.m_[2][1] + m_[0][3] * rhs.m_[3][1],
							   m_[0][0] * rhs.m_[0][2] + m_[0][1] * rhs.m_[1][2] + m_[0][2] * rhs.m_[2][2] + m_[0][3] * rhs.m_[3][2],
							   m_[0][0] * rhs.m_[0][3] + m_[0][1] * rhs.m_[1][3] + m_[0][2] * rhs.m_[2][3] + m_[0][3] * rhs.m_[3][3],
							   
							   m_[1][0] * rhs.m_[0][0] + m_[1][1] * rhs.m_[1][0] + m_[1][2] * rhs.m_[2][0] + m_[1][3] * rhs.m_[3][0],
							   m_[1][0] * rhs.m_[0][1] + m_[1][1] * rhs.m_[1][1] + m_[1][2] * rhs.m_[2][1] + m_[1][3] * rhs.m_[3][1],
							   m_[1][0] * rhs.m_[0][2] + m_[1][1] * rhs.m_[1][2] + m_[1][2] * rhs.m_[2][2] + m_[1][3] * rhs.m_[3][2],
							   m_[1][0] * rhs.m_[0][3] + m_[1][1] * rhs.m_[1][3] + m_[1][2] * rhs.m_[2][3] + m_[1][3] * rhs.m_[3][3],
							   
							   m_[2][0] * rhs.m_[0][0] + m_[2][1] * rhs.m_[1][0] + m_[2][2] * rhs.m_[2][0] + m_[2][3] * rhs.m_[3][0],
							   m_[2][0] * rhs.m_[0][1] + m_[2][1] * rhs.m_[1][1] + m_[2][2] * rhs.m_[2][1] + m_[2][3] * rhs.m_[3][1],
							   m_[2][0] * rhs.m_[0][2] + m_[2][1] * rhs.m_[1][2] + m_[2][2] * rhs.m_[2][2] + m_[2][3] * rhs.m_[3][2],
							   m_[2][0] * rhs.m_[0][3] + m_[2][1] * rhs.m_[1][3] + m_[2][2] * rhs.m_[2][3] + m_[2][3] * rhs.m_[3][3],
							   
							   m_[3][0] * rhs.m_[0][0] + m_[3][1] * rhs.m_[1][0] + m_[3][2] * rhs.m_[2][0] + m_[3][3] * rhs.m_[3][0],
							   m_[3][0] * rhs.m_[0][1] + m_[3][1] * rhs.m_[1][1] + m_[3][2] * rhs.m_[2][1] + m_[3][3] * rhs.m_[3][1],
							   m_[3][0] * rhs.m_[0][2] + m_[3][1] * rhs.m_[1][2] + m_[3][2] * rhs.m_[2][2] + m_[3][3] * rhs.m_[3][2],
							   m_[3][0] * rhs.m_[0][3] + m_[3][1] * rhs.m_[1][3] + m_[3][2] * rhs.m_[2][3] + m_[3][3] * rhs.m_[3][3]};
				#endif
			}

			//Multiply all numbers with the given scalar
			[[nodiscard]] inline auto operator*(real scalar) const noexcept
			{
				return Matrix4{m_[0][0] * scalar, m_[0][1] * scalar, m_[0][2] * scalar, m_[0][3] * scalar,
							   m_[1][0] * scalar, m_[1][1] * scalar, m_[1][2] * scalar, m_[1][3] * scalar,
							   m_[2][0] * scalar, m_[2][1] * scalar, m_[2][2] * scalar, m_[2][3] * scalar,
							   m_[3][0] * scalar, m_[3][1] * scalar, m_[3][2] * scalar, m_[3][3] * scalar};
			}

			//Multiply matrix with the given vector
			//This operation results in a vector
			[[nodiscard]] inline auto operator*(const Vector3 &vector) const noexcept
			{
				auto [x, y, z] = vector.XYZ();

				#ifdef ION_ROW_MAJOR
				//Row-major layout (Direct3D)
				auto inv_w = 1.0_r / (m_[0][3] * x + m_[1][3] * y + m_[2][3], + m_[3][3]);
				return Vector3{(m_[0][0] * x + m_[1][0] * y + m_[2][0] * z + m_[3][0]) * inv_w,
							   (m_[0][1] * x + m_[1][1] * y + m_[2][1] * z + m_[3][1]) * inv_w,
							   (m_[0][2] * x + m_[1][2] * y + m_[2][2] * z + m_[3][2]) * inv_w};
				#else
				//Column-major layout (OpenGL)
				auto inv_w = 1.0_r / (m_[3][0] * x + m_[3][1] * y + m_[3][2], + m_[3][3]);
				return Vector3{(m_[0][0] * x + m_[0][1] * y + m_[0][2] * z + m_[0][3]) * inv_w,
							   (m_[1][0] * x + m_[1][1] * y + m_[1][2] * z + m_[1][3]) * inv_w,
							   (m_[2][0] * x + m_[2][1] * y + m_[2][2] * z + m_[2][3]) * inv_w};
				#endif
			}

			//Multiply all numbers with the given scalar
			[[nodiscard]] friend inline auto operator*(real scalar, const Matrix4 &rhs) noexcept
			{
				return rhs * scalar; //Commutative
			}

			//Multiply two matrices (matrix multiplication)
			inline auto& operator*=(const Matrix4 &rhs) noexcept
			{
				return *this = *this * rhs;
			}

			//Multiply all numbers with the given scalar
			inline auto& operator*=(real scalar) noexcept
			{
				return *this = *this * scalar;
			}


			/*
				Subscripting
			*/

			//Returns a modifiable reference to the number at the given offset
			//Accesses matrix linearly in order {m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33}
			[[nodiscard]] inline auto& operator[](int off) noexcept
			{
				assert(off >= 0 && off < 16);
				return numbers_[off];
			}

			//Returns the number at the given offset
			//Accesses matrix linearly in order {m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33}
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 16);
				return numbers_[off];
			}


			/*
				Modifiers
			*/

			//Sets the m00 number to the given value
			inline void M00(real m00) noexcept
			{
				m_[0][0] = m00;
			}
			
			//Sets the m01 number to the given value
			inline void M01(real m01) noexcept
			{
				m_[0][1] = m01;
			}

			//Sets the m02 number to the given value
			inline void M02(real m02) noexcept
			{
				m_[0][2] = m02;
			}

			//Sets the m10 number to the given value
			inline void M10(real m10) noexcept
			{
				m_[1][0] = m10;
			}

			//Sets the m11 number to the given value
			inline void M11(real m11) noexcept
			{
				m_[1][1] = m11;
			}

			//Sets the m12 number to the given value
			inline void M12(real m12) noexcept
			{
				m_[1][2] = m12;
			}

			//Sets the m20 number to the given value
			inline void M20(real m20) noexcept
			{
				m_[2][0] = m20;
			}

			//Sets the m21 number to the given value
			inline void M21(real m21) noexcept
			{
				m_[2][1] = m21;
			}

			//Sets the m22 number to the given value
			inline void M22(real m22) noexcept
			{
				m_[2][2] = m22;
			}

			#ifdef ION_ROW_MAJOR
			//Sets the m30 number to the given value
			inline void M30(real m30) noexcept
			{
				m_[3][0] = m30;
			}

			//Sets the m31 number to the given value
			inline void M31(real m31) noexcept
			{
				m_[3][1] = m31;
			}

			//Sets the m32 number to the given value
			inline void M32(real m32) noexcept
			{
				m_[3][2] = m32;
			}
			#else
			//Sets the m03 number to the given value
			inline void M03(real m03) noexcept
			{
				m_[0][3] = m03;
			}

			//Sets the m13 number to the given value
			inline void M13(real m13) noexcept
			{
				m_[1][3] = m13;
			}

			//Sets the m23 number to the given value
			inline void M23(real m23) noexcept
			{
				m_[2][3] = m23;
			}
			#endif


			/*
				Observers
			*/

			//Returns the m00 number
			[[nodiscard]] inline auto M00() const noexcept
			{
				return m_[0][0];
			}

			//Returns the m01 number
			[[nodiscard]] inline auto M01() const noexcept
			{
				return m_[0][1];
			}

			//Returns the m02 number
			[[nodiscard]] inline auto M02() const noexcept
			{
				return m_[0][2];
			}

			//Returns the m10 number
			[[nodiscard]] inline auto M10() const noexcept
			{
				return m_[1][0];
			}

			//Returns the m11 number
			[[nodiscard]] inline auto M11() const noexcept
			{
				return m_[1][1];
			}

			//Returns the m12 number
			[[nodiscard]] inline auto M12() const noexcept
			{
				return m_[1][2];
			}

			//Returns the m20 number
			[[nodiscard]] inline auto M20() const noexcept
			{
				return m_[2][0];
			}

			//Returns the m21 number
			[[nodiscard]] inline auto M21() const noexcept
			{
				return m_[2][1];
			}

			//Returns the m22 number
			[[nodiscard]] inline auto M22() const noexcept
			{
				return m_[2][2];
			}

			#ifdef ION_ROW_MAJOR
			//Returns the m30 number
			[[nodiscard]] inline auto M30() const noexcept
			{
				return m_[3][0];
			}

			//Returns the m31 number
			[[nodiscard]] inline auto M31() const noexcept
			{
				return m_[3][1];
			}

			//Returns the m32 number
			[[nodiscard]] inline auto M32() const noexcept
			{
				return m_[3][2];
			}
			#else
			//Returns the m03 number
			[[nodiscard]] inline auto M03() const noexcept
			{
				return m_[0][3];
			}

			//Returns the m13 number
			[[nodiscard]] inline auto M13() const noexcept
			{
				return m_[1][3];
			}

			//Returns the m23 number
			[[nodiscard]] inline auto M23() const noexcept
			{
				return m_[2][3];
			}
			#endif


			//Returns direct access to the matrix
			[[nodiscard]] inline auto M() const noexcept
			{
				return m_;
			}


			/*
				Matrix conversions
			*/

			//Returns angle (radians) of the matrix reflection
			[[nodiscard]] real ToReflection() const noexcept;

			//Returns angle (radians) of the matrix rotation
			[[nodiscard]] real ToRotation() const noexcept;

			//Returns vector of the matrix scaling
			[[nodiscard]] Vector3 ToScaling() const noexcept;

			//Returns vector of the matrix shearing
			[[nodiscard]] Vector3 ToShearing() const noexcept;

			//Returns vector of the matrix translation
			[[nodiscard]] Vector3 ToTranslation() const noexcept;

			
			/*
				Adjoint/adjugate
			*/

			//Adjoint/adjugate of the matrix
			//It is the transpose of its cofactor matrix
			Matrix4& Adjoint() noexcept;

			//Adjoint/adjugate of the matrix
			//It is the transpose of its cofactor matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix4 AdjointCopy() const noexcept;


			/*
				Affine
			*/

			//Make matrix an affine matrix
			Matrix4& Affine() noexcept;

			//Make matrix an affine matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix4 AffineCopy() const noexcept;

			//Returns true if this matrix is affine
			[[nodiscard]] bool IsAffine() const noexcept;


			/*
				Concatenating
			*/

			//Concatenate this matrix with the given matrix
			//This is the same as matrix multiplication
			Matrix4& Concatenate(const Matrix4 &matrix) noexcept;

			//Concatenate this matrix with the given matrix
			//This is the same as matrix multiplication
			//Returns the result as a copy
			[[nodiscard]] Matrix4 ConcatenateCopy(const Matrix4 &matrix) const noexcept;


			//Concatenate this affine matrix with the given affine matrix
			//This uses a faster version of matrix multiplication
			Matrix4& ConcatenateAffine(const Matrix4 &matrix) noexcept;

			//Concatenate this affine matrix with the given affine matrix
			//This uses a faster version of matrix multiplication
			//Returns the result as a copy
			[[nodiscard]] Matrix4 ConcatenateAffineCopy(const Matrix4 &matrix) const noexcept;


			/*
				Cofactoring
			*/

			//Make matrix a cofactor matrix
			Matrix4& Cofactor() noexcept;

			//Make matrix a cofactor matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix4 CofactorCopy() const noexcept;


			/*
				Determinant and inversing
			*/

			//Returns the determinant of the matrix
			[[nodiscard]] real Determinant() const noexcept;
			

			//Inverse matrix
			//It is the inverse of its determinant multiplied with its adjoint matrix
			Matrix4& Inverse() noexcept;

			//Inverse matrix
			//It is the inverse of its determinant multiplied with its adjoint matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix4 InverseCopy() const noexcept;


			/*
				Reflecting
			*/

			//Reflect matrix by the given angle (radians)
			Matrix4& Reflect(real angle) noexcept;

			//Reflect matrix by the given angle (radians)
			//Returns the result as a copy
			[[nodiscard]] Matrix4 ReflectCopy(real angle) const noexcept;


			/*
				Rotating
			*/

			//Rotate matrix by the given angle (radians)
			Matrix4& Rotate(real angle) noexcept;

			//Rotate matrix by the given angle (radians)
			//Returns the result as a copy
			[[nodiscard]] Matrix4 RotateCopy(real angle) const noexcept;


			/*
				Scaling
			*/

			//Scale matrix by the given vector
			Matrix4& Scale(const Vector3 &vector) noexcept;

			//Scale matrix by the given vector
			//Returns the result as a copy
			[[nodiscard]] Matrix4 ScaleCopy(const Vector3 &vector) const noexcept;


			/*
				Shearing
			*/

			//Shear matrix by the given vector
			Matrix4& Shear(const Vector3 &vector) noexcept;

			//Shear matrix by the given vector
			//Returns the result as a copy
			[[nodiscard]] Matrix4 ShearCopy(const Vector3 &vector) const noexcept;


			/*
				Transforming
			*/

			//Transform the given point based on matrix
			[[nodiscard]] Vector3 TransformPoint(const Vector3 &point) const noexcept;


			/*
				Translating
			*/

			//Translate matrix by the given vector
			Matrix4& Translate(const Vector3 &vector) noexcept;

			//Translate matrix by the given vector
			//Returns the result as a copy
			[[nodiscard]] Matrix4 TranslateCopy(const Vector3 &vector) const noexcept;


			/*
				Transposition
			*/

			//Transpose the matrix
			Matrix4& Transpose() noexcept;

			//Transpose the matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix4 TransposeCopy() const noexcept;
	};


	namespace matrix4
	{
		/*
			Predefined constant matrices
		*/

		inline const auto Zero = Matrix4{0.0_r, 0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r, 0.0_r};

		inline const auto Identity = Matrix4{1.0_r, 0.0_r, 0.0_r, 0.0_r,
											 0.0_r, 1.0_r, 0.0_r, 0.0_r,
											 0.0_r, 0.0_r, 1.0_r, 0.0_r,
											 0.0_r, 0.0_r, 0.0_r, 1.0_r};
	}
} //ion::graphics::utilities

#endif