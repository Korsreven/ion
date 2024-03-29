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


	///@brief A class representing a 4x4 column-major matrix with right hand rotation
	///@details Follows the OpenGL matrix convention by default.
	///Define ION_ROW_MAJOR and ION_LEFT_HANDED to use this class with Direct3D
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

			///@brief Default constructor
			Matrix4() = default;

			///@brief Constructs a new matrix from the given numbers
			Matrix4(real m00, real m01, real m02, real m03,
					real m10, real m11, real m12, real m13,
					real m20, real m21, real m22, real m23,
					real m30, real m31, real m32, real m33) noexcept;
			
			#ifdef ION_ROW_MAJOR
			///@brief Row-major layout (Direct3D)
			///@details Constructs a new matrix from only the three first columns.
			///The fourth column is filled with {0, 0, 0, 1}
			Matrix4(real m00, real m01, real m02,
					real m10, real m11, real m12,
					real m20, real m21, real m22,
					real m30, real m31, real m32) noexcept;
			#else
			///@brief Column-major layout (OpenGL)
			///@details Constructs a new matrix from only the three first rows.
			///The fourth row is filled with {0, 0, 0, 1}
			Matrix4(real m00, real m01, real m02, real m03,
					real m10, real m11, real m12, real m13,
					real m20, real m21, real m22, real m23) noexcept;
			#endif

			///@brief Constructs a new matrix from the given 3x3 matrix
			Matrix4(const Matrix3 &matrix) noexcept;
			
			///@}

			/**
				@name Static matrix conversions
				@{
			*/

			///@brief Returns a new reflection matrix from the given angle (radians)
			[[nodiscard]] static Matrix4 Reflection(real angle) noexcept;

			///@brief Returns a new rotation matrix from the given angle (radians)
			[[nodiscard]] static Matrix4 Rotation(real angle) noexcept;

			///@brief Returns a new rotation matrix from the given angle (radians) and origin vector
			[[nodiscard]] static Matrix4 Rotation(real angle, const Vector3 &origin) noexcept;

			///@brief Returns a new scaling matrix from the given vector
			[[nodiscard]] static Matrix4 Scaling(const Vector3 &vector) noexcept;

			///@brief Returns a new shearing matrix from the given vector
			[[nodiscard]] static Matrix4 Shearing(const Vector3 &vector) noexcept;

			///@brief Returns a new translation matrix from the given vector
			[[nodiscard]] static Matrix4 Translation(const Vector3 &vector) noexcept;

			///@brief Returns a new transformation matrix from the given rotation angle (radians), scaling and translation
			[[nodiscard]] static Matrix4 Transformation(real rotation, const Vector3 &scaling, const Vector3 &translation) noexcept;

			///@brief Returns a new transformation matrix from the given 3x3 matrix
			[[nodiscard]] static Matrix4 Transformation(const Matrix3 &matrix) noexcept;

			///@}

			/**
				@name Operators
				@{
			*/

			///@brief Sets this matrix equal to the given 3x3 matrix
			Matrix4& operator=(const Matrix3 &matrix) noexcept;


			///@brief Checks if two matrices are equal (all numbers are equal)
			[[nodiscard]] inline auto operator==(const Matrix4 &rhs) const noexcept
			{
				return m_[0][0] == rhs.m_[0][0] && m_[0][1] == rhs.m_[0][1] && m_[0][2] == rhs.m_[0][2] && m_[0][3] == rhs.m_[0][3] &&
					   m_[1][0] == rhs.m_[1][0] && m_[1][1] == rhs.m_[1][1] && m_[1][2] == rhs.m_[1][2] && m_[1][3] == rhs.m_[1][3] &&
					   m_[2][0] == rhs.m_[2][0] && m_[2][1] == rhs.m_[2][1] && m_[2][2] == rhs.m_[2][2] && m_[2][3] == rhs.m_[2][3] &&
					   m_[3][0] == rhs.m_[3][0] && m_[3][1] == rhs.m_[3][1] && m_[3][2] == rhs.m_[3][2] && m_[3][3] == rhs.m_[3][3];
			}

			///@brief Checks if two matrices are different (one or more numbers are different)
			[[nodiscard]] inline auto operator!=(const Matrix4 &rhs) const noexcept
			{
				return !(*this == rhs);
			}


			///@brief Negates matrix (negating each number)
			[[nodiscard]] inline auto operator-() const noexcept
			{
				return Matrix4{-m_[0][0], -m_[0][1], -m_[0][2], -m_[0][3],
							   -m_[1][0], -m_[1][1], -m_[1][2], -m_[1][3],
							   -m_[2][0], -m_[2][1], -m_[2][2], -m_[2][3],
							   -m_[3][0], -m_[3][1], -m_[3][2], -m_[3][3]};
			}

			///@}

			/**
				@name Adding
				@{
			*/

			///@brief Adds two matrices (adding each number)
			[[nodiscard]] inline auto operator+(const Matrix4 &rhs) const noexcept
			{
				return Matrix4{m_[0][0] + rhs.m_[0][0], m_[0][1] + rhs.m_[0][1], m_[0][2] + rhs.m_[0][2], m_[0][3] + rhs.m_[0][3],
							   m_[1][0] + rhs.m_[1][0], m_[1][1] + rhs.m_[1][1], m_[1][2] + rhs.m_[1][2], m_[1][3] + rhs.m_[1][3],
							   m_[2][0] + rhs.m_[2][0], m_[2][1] + rhs.m_[2][1], m_[2][2] + rhs.m_[2][2], m_[2][3] + rhs.m_[2][3],
							   m_[3][0] + rhs.m_[3][0], m_[3][1] + rhs.m_[3][1], m_[3][2] + rhs.m_[3][2], m_[3][3] + rhs.m_[3][3]};
			}

			///@brief Adds all numbers to the given scalar
			[[nodiscard]] inline auto operator+(real scalar) const noexcept
			{
				return Matrix4{m_[0][0] + scalar, m_[0][1] + scalar, m_[0][2] + scalar, m_[0][3] + scalar,
							   m_[1][0] + scalar, m_[1][1] + scalar, m_[1][2] + scalar, m_[1][3] + scalar,
							   m_[2][0] + scalar, m_[2][1] + scalar, m_[2][2] + scalar, m_[2][3] + scalar,
							   m_[3][0] + scalar, m_[3][1] + scalar, m_[3][2] + scalar, m_[3][3] + scalar};
			}

			///@brief Adds all numbers to the given scalar
			[[nodiscard]] friend inline auto operator+(real scalar, const Matrix4 &rhs) noexcept
			{
				return rhs + scalar; //Commutative
			}

			///@brief Adds two matrices (adding each number)
			inline auto& operator+=(const Matrix4 &rhs) noexcept
			{
				return *this = *this + rhs;
			}

			///@brief Adds all numbers to the given scalar
			inline auto& operator+=(real scalar) noexcept
			{
				return *this = *this + scalar;
			}

			///@}

			/**
				@name Subtracting
				@{
			*/

			///@brief Subtracts two matrices (subtracting each number)
			[[nodiscard]] inline auto operator-(const Matrix4 &rhs) const noexcept
			{
				return Matrix4{m_[0][0] - rhs.m_[0][0], m_[0][1] - rhs.m_[0][1], m_[0][2] - rhs.m_[0][2], m_[0][3] - rhs.m_[0][3],
							   m_[1][0] - rhs.m_[1][0], m_[1][1] - rhs.m_[1][1], m_[1][2] - rhs.m_[1][2], m_[1][3] - rhs.m_[1][3],
							   m_[2][0] - rhs.m_[2][0], m_[2][1] - rhs.m_[2][1], m_[2][2] - rhs.m_[2][2], m_[2][3] - rhs.m_[2][3],
							   m_[3][0] - rhs.m_[3][0], m_[3][1] - rhs.m_[3][1], m_[3][2] - rhs.m_[3][2], m_[3][3] - rhs.m_[3][3]};
			}

			///@brief Subtracts all numbers with the given scalar
			[[nodiscard]] inline auto operator-(real scalar) const noexcept
			{
				return Matrix4{m_[0][0] - scalar, m_[0][1] - scalar, m_[0][2] - scalar, m_[0][3] - scalar,
							   m_[1][0] - scalar, m_[1][1] - scalar, m_[1][2] - scalar, m_[1][3] - scalar,
							   m_[2][0] - scalar, m_[2][1] - scalar, m_[2][2] - scalar, m_[2][3] - scalar,
							   m_[3][0] - scalar, m_[3][1] - scalar, m_[3][2] - scalar, m_[3][3] - scalar};
			}

			///@brief Subtracts all numbers with the given scalar
			[[nodiscard]] friend inline auto operator-(real scalar, const Matrix4 &rhs) noexcept
			{
				return Matrix4{scalar - rhs.m_[0][0], scalar - rhs.m_[0][1], scalar - rhs.m_[0][2], scalar - rhs.m_[0][3],
							   scalar - rhs.m_[1][0], scalar - rhs.m_[1][1], scalar - rhs.m_[1][2], scalar - rhs.m_[1][3],
							   scalar - rhs.m_[2][0], scalar - rhs.m_[2][1], scalar - rhs.m_[2][2], scalar - rhs.m_[2][3],
							   scalar - rhs.m_[3][0], scalar - rhs.m_[3][1], scalar - rhs.m_[3][2], scalar - rhs.m_[3][3]};
			}

			///@brief Subtracts two matrices (subtracting each number)
			inline auto& operator-=(const Matrix4 &rhs) noexcept
			{
				return *this = *this - rhs;
			}

			///@brief Subtracts all numbers with the given scalar
			inline auto& operator-=(real scalar) noexcept
			{
				return *this = *this - scalar;
			}

			///@}

			/**
				@name Multiplying
				@{
			*/

			///@brief Muliplies two matrices (matrix multiplication)
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

			///@brief Muliplies all numbers with the given scalar
			[[nodiscard]] inline auto operator*(real scalar) const noexcept
			{
				return Matrix4{m_[0][0] * scalar, m_[0][1] * scalar, m_[0][2] * scalar, m_[0][3] * scalar,
							   m_[1][0] * scalar, m_[1][1] * scalar, m_[1][2] * scalar, m_[1][3] * scalar,
							   m_[2][0] * scalar, m_[2][1] * scalar, m_[2][2] * scalar, m_[2][3] * scalar,
							   m_[3][0] * scalar, m_[3][1] * scalar, m_[3][2] * scalar, m_[3][3] * scalar};
			}

			///@brief Muliplies matrix with the given vector
			///@details This operation results in a vector
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

			///@brief Muliplies all numbers with the given scalar
			[[nodiscard]] friend inline auto operator*(real scalar, const Matrix4 &rhs) noexcept
			{
				return rhs * scalar; //Commutative
			}

			///@brief Muliplies two matrices (matrix multiplication)
			inline auto& operator*=(const Matrix4 &rhs) noexcept
			{
				return *this = *this * rhs;
			}

			///@brief Muliplies all numbers with the given scalar
			inline auto& operator*=(real scalar) noexcept
			{
				return *this = *this * scalar;
			}

			///@}

			/**
				@name Subscripting
				@{
			*/

			///@brief Returns a modifiable reference to the number at the given offset
			///@details Accesses matrix linearly in order {m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33}
			[[nodiscard]] inline auto& operator[](int off) noexcept
			{
				assert(off >= 0 && off < 16);
				return numbers_[off];
			}

			///@brief Returns the number at the given offset
			///@details Accesses matrix linearly in order {m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33}
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 16);
				return numbers_[off];
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the m00 number to the given value
			inline void M00(real m00) noexcept
			{
				m_[0][0] = m00;
			}
			
			///@brief Sets the m01 number to the given value
			inline void M01(real m01) noexcept
			{
				m_[0][1] = m01;
			}

			///@brief Sets the m02 number to the given value
			inline void M02(real m02) noexcept
			{
				m_[0][2] = m02;
			}

			///@brief Sets the m10 number to the given value
			inline void M10(real m10) noexcept
			{
				m_[1][0] = m10;
			}

			///@brief Sets the m11 number to the given value
			inline void M11(real m11) noexcept
			{
				m_[1][1] = m11;
			}

			///@brief Sets the m12 number to the given value
			inline void M12(real m12) noexcept
			{
				m_[1][2] = m12;
			}

			///@brief Sets the m20 number to the given value
			inline void M20(real m20) noexcept
			{
				m_[2][0] = m20;
			}

			///@brief Sets the m21 number to the given value
			inline void M21(real m21) noexcept
			{
				m_[2][1] = m21;
			}

			///@brief Sets the m22 number to the given value
			inline void M22(real m22) noexcept
			{
				m_[2][2] = m22;
			}

			#ifdef ION_ROW_MAJOR
			///@brief Sets the m30 number to the given value
			inline void M30(real m30) noexcept
			{
				m_[3][0] = m30;
			}

			///@brief Sets the m31 number to the given value
			inline void M31(real m31) noexcept
			{
				m_[3][1] = m31;
			}

			///@brief Sets the m32 number to the given value
			inline void M32(real m32) noexcept
			{
				m_[3][2] = m32;
			}
			#else
			///@brief Sets the m03 number to the given value
			inline void M03(real m03) noexcept
			{
				m_[0][3] = m03;
			}

			///@brief Sets the m13 number to the given value
			inline void M13(real m13) noexcept
			{
				m_[1][3] = m13;
			}

			///@brief Sets the m23 number to the given value
			inline void M23(real m23) noexcept
			{
				m_[2][3] = m23;
			}
			#endif

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the m00 number
			[[nodiscard]] inline auto M00() const noexcept
			{
				return m_[0][0];
			}

			///@brief Returns the m01 number
			[[nodiscard]] inline auto M01() const noexcept
			{
				return m_[0][1];
			}

			///@brief Returns the m02 number
			[[nodiscard]] inline auto M02() const noexcept
			{
				return m_[0][2];
			}

			///@brief Returns the m10 number
			[[nodiscard]] inline auto M10() const noexcept
			{
				return m_[1][0];
			}

			///@brief Returns the m11 number
			[[nodiscard]] inline auto M11() const noexcept
			{
				return m_[1][1];
			}

			///@brief Returns the m12 number
			[[nodiscard]] inline auto M12() const noexcept
			{
				return m_[1][2];
			}

			///@brief Returns the m20 number
			[[nodiscard]] inline auto M20() const noexcept
			{
				return m_[2][0];
			}

			///@brief Returns the m21 number
			[[nodiscard]] inline auto M21() const noexcept
			{
				return m_[2][1];
			}

			///@brief Returns the m22 number
			[[nodiscard]] inline auto M22() const noexcept
			{
				return m_[2][2];
			}

			#ifdef ION_ROW_MAJOR
			///@brief Returns the m30 number
			[[nodiscard]] inline auto M30() const noexcept
			{
				return m_[3][0];
			}

			///@brief Returns the m31 number
			[[nodiscard]] inline auto M31() const noexcept
			{
				return m_[3][1];
			}

			///@brief Returns the m32 number
			[[nodiscard]] inline auto M32() const noexcept
			{
				return m_[3][2];
			}
			#else
			///@brief Returns the m03 number
			[[nodiscard]] inline auto M03() const noexcept
			{
				return m_[0][3];
			}

			///@brief Returns the m13 number
			[[nodiscard]] inline auto M13() const noexcept
			{
				return m_[1][3];
			}

			///@brief Returns the m23 number
			[[nodiscard]] inline auto M23() const noexcept
			{
				return m_[2][3];
			}
			#endif


			///@brief Returns direct access to the matrix
			[[nodiscard]] inline auto M() const noexcept
			{
				return m_;
			}

			///@}

			/**
				@name Matrix conversions
				@{
			*/

			///@brief Returns angle (radians) of the matrix reflection
			[[nodiscard]] real ToReflection() const noexcept;

			///@brief Returns angle (radians) of the matrix rotation
			[[nodiscard]] real ToRotation() const noexcept;

			///@brief Returns vector of the matrix scaling
			[[nodiscard]] Vector3 ToScaling() const noexcept;

			///@brief Returns vector of the matrix shearing
			[[nodiscard]] Vector3 ToShearing() const noexcept;

			///@brief Returns vector of the matrix translation
			[[nodiscard]] Vector3 ToTranslation() const noexcept;

			///@}

			/**
				@name Adjoint/adjugate
				@{
			*/

			///@brief Adjoint/adjugate of the matrix
			///@details It is the transpose of its cofactor matrix
			Matrix4& Adjoint() noexcept;

			///@brief Adjoint/adjugate of the matrix
			///@details It is the transpose of its cofactor matrix.
			///Returns the result as a copy
			[[nodiscard]] Matrix4 AdjointCopy() const noexcept;

			///@}

			/**
				@name Affine
				@{
			*/

			///@brief Makes matrix an affine matrix
			Matrix4& Affine() noexcept;

			///@brief Makes matrix an affine matrix
			///@details Returns the result as a copy
			[[nodiscard]] Matrix4 AffineCopy() const noexcept;

			///@brief Returns true if this matrix is affine
			[[nodiscard]] bool IsAffine() const noexcept;

			///@}

			/**
				@name Concatenating
				@{
			*/

			///@brief Concatenates this matrix with the given matrix
			///@details This is the same as matrix multiplication
			Matrix4& Concatenate(const Matrix4 &matrix) noexcept;

			///@brief Concatenates this matrix with the given matrix
			///@details This is the same as matrix multiplication.
			///Returns the result as a copy
			[[nodiscard]] Matrix4 ConcatenateCopy(const Matrix4 &matrix) const noexcept;


			///@brief Concatenates this affine matrix with the given affine matrix
			///@details This uses a faster version of matrix multiplication
			Matrix4& ConcatenateAffine(const Matrix4 &matrix) noexcept;

			///@brief Concatenates this affine matrix with the given affine matrix
			///@details This uses a faster version of matrix multiplication.
			///Returns the result as a copy
			[[nodiscard]] Matrix4 ConcatenateAffineCopy(const Matrix4 &matrix) const noexcept;

			///@}

			/**
				@name Cofactoring
				@{
			*/

			///@brief Makes matrix a cofactor matrix
			Matrix4& Cofactor() noexcept;

			///@brief Makes matrix a cofactor matrix
			///@details Returns the result as a copy
			[[nodiscard]] Matrix4 CofactorCopy() const noexcept;

			///@}

			/**
				@name Determinant and inversing
				@{
			*/

			///@brief Returns the determinant of the matrix
			[[nodiscard]] real Determinant() const noexcept;
			

			///@brief Inverse matrix
			///@details It is the inverse of its determinant multiplied with its adjoint matrix
			Matrix4& Inverse() noexcept;

			///@brief Inverse matrix
			///@details It is the inverse of its determinant multiplied with its adjoint matrix.
			///Returns the result as a copy
			[[nodiscard]] Matrix4 InverseCopy() const noexcept;

			///@}

			/**
				@name Reflecting
				@{
			*/

			///@brief Reflect matrix by the given angle (radians)
			Matrix4& Reflect(real angle) noexcept;

			///@brief Reflect matrix by the given angle (radians)
			///@details Returns the result as a copy
			[[nodiscard]] Matrix4 ReflectCopy(real angle) const noexcept;

			///@}

			/**
				@name Rotating
				@{
			*/

			///@brief Rotates matrix by the given angle (radians)
			Matrix4& Rotate(real angle) noexcept;

			///@brief Rotates matrix by the given angle (radians)
			///@details Returns the result as a copy
			[[nodiscard]] Matrix4 RotateCopy(real angle) const noexcept;

			///@}

			/**
				@name Scaling
				@{
			*/

			///@brief Scales matrix by the given vector
			Matrix4& Scale(const Vector3 &vector) noexcept;

			///@brief Scales matrix by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Matrix4 ScaleCopy(const Vector3 &vector) const noexcept;

			///@}

			/**
				@name Shearing
				@{
			*/

			///@brief Shear matrix by the given vector
			Matrix4& Shear(const Vector3 &vector) noexcept;

			///@brief Shear matrix by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Matrix4 ShearCopy(const Vector3 &vector) const noexcept;

			///@}

			/**
				@name Transforming
				@{
			*/

			///@brief Transforms the given point based on matrix
			[[nodiscard]] Vector3 TransformPoint(const Vector3 &point) const noexcept;

			///@}

			/**
				@name Translating
				@{
			*/

			///@brief Translates matrix by the given vector
			Matrix4& Translate(const Vector3 &vector) noexcept;

			///@brief Translates matrix by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Matrix4 TranslateCopy(const Vector3 &vector) const noexcept;

			///@}

			/**
				@name Transposition
				@{
			*/

			///@brief Transpose the matrix
			Matrix4& Transpose() noexcept;

			///@brief Transpose the matrix
			///@details Returns the result as a copy
			[[nodiscard]] Matrix4 TransposeCopy() const noexcept;

			///@}
	};


	namespace matrix4
	{
		/**
			@name Predefined constant matrices
			@{
		*/

		inline const auto Zero = Matrix4{0.0_r, 0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r, 0.0_r};

		inline const auto Identity = Matrix4{1.0_r, 0.0_r, 0.0_r, 0.0_r,
											 0.0_r, 1.0_r, 0.0_r, 0.0_r,
											 0.0_r, 0.0_r, 1.0_r, 0.0_r,
											 0.0_r, 0.0_r, 0.0_r, 1.0_r};

		///@}
	}
} //ion::graphics::utilities

#endif