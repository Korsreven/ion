/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonMatrix3.h
-------------------------------------------
*/

#ifndef ION_MATRIX3_H
#define ION_MATRIX3_H

#include "IonVector2.h"
#include "types/IonTypes.h"

namespace ion::graphics::utilities
{
	using namespace types::type_literals;

	namespace matrix3::detail
	{
	} //matrix3::detail


	//A 3x3 column-major matrix class
	//Follows the OpenGL matrix convention
	class Matrix3 final
	{
		private:

			union
			{
				real m_[3][3];
				real numbers_[9]{1.0_r, 0.0_r, 0.0_r,
								 0.0_r, 1.0_r, 0.0_r,
								 0.0_r, 0.0_r, 1.0_r};
			};

		public:

			Matrix3() = default;

			//Constructs a new matrix from the given numbers in column-major order
			Matrix3(real m00, real m01, real m02,
					real m10, real m11, real m12,
					real m20, real m21, real m22) noexcept;
			
			//Constructs a new matrix from only the two first columns in column-major order
			//The third column is filled with {0, 0, 1}
			Matrix3(real m00, real m01,
					real m10, real m11,
					real m20, real m21) noexcept;
			

			/*
				Static matrix conversions
			*/

			//Returns a new reflection matrix from the given angle (radians)
			[[nodiscard]] static Matrix3 Reflection(real angle) noexcept;

			//Returns a new rotation matrix from the given angle (radians)
			[[nodiscard]] static Matrix3 Rotation(real angle) noexcept;

			//Returns a new rotation matrix from the given angle (radians) and origin vector
			[[nodiscard]] static Matrix3 Rotation(real angle, const Vector2 &origin) noexcept;

			//Returns a new scaling matrix from the given vector
			[[nodiscard]] static Matrix3 Scaling(const Vector2 &vector) noexcept;

			//Returns a new shearing matrix from the given vector
			[[nodiscard]] static Matrix3 Shearing(const Vector2 &vector) noexcept;

			//Returns a new translation matrix from the given vector
			[[nodiscard]] static Matrix3 Translation(const Vector2 &vector) noexcept;


			/*
				Operators
			*/

			//Checks if two matrices are equal (all numbers are equal)
			[[nodiscard]] inline auto operator==(const Matrix3 &rhs) const noexcept
			{
				return m_[0][0] == rhs.m_[0][0] && m_[0][1] == rhs.m_[0][1] && m_[0][2] == rhs.m_[0][2] &&
					   m_[1][0] == rhs.m_[1][0] && m_[1][1] == rhs.m_[1][1] && m_[1][2] == rhs.m_[1][2] &&
					   m_[2][0] == rhs.m_[2][0] && m_[2][1] == rhs.m_[2][1] && m_[2][2] == rhs.m_[2][2];
			}

			//Checks if two matrices are different (one or more numbers are different)
			[[nodiscard]] inline auto operator!=(const Matrix3 &rhs) const noexcept
			{
				return !(*this == rhs);
			}


			//Negate matrix (negating each number)
			[[nodiscard]] inline auto operator-() const noexcept
			{
				return Matrix3{-m_[0][0], -m_[0][1], -m_[0][2],
							   -m_[1][0], -m_[1][1], -m_[1][2],
							   -m_[2][0], -m_[2][1], -m_[2][2]};
			}


			/*
				Adding
			*/

			//Add two matrices (adding each number)
			[[nodiscard]] inline auto operator+(const Matrix3 &rhs) const noexcept
			{
				return Matrix3{m_[0][0] + rhs.m_[0][0], m_[0][1] + rhs.m_[0][1], m_[0][2] + rhs.m_[0][2],
							   m_[1][0] + rhs.m_[1][0], m_[1][1] + rhs.m_[1][1], m_[1][2] + rhs.m_[1][2],
							   m_[2][0] + rhs.m_[2][0], m_[2][1] + rhs.m_[2][1], m_[2][2] + rhs.m_[2][2]};
			}

			//Add all numbers to the given scalar
			[[nodiscard]] inline auto operator+(real scalar) const noexcept
			{
				return Matrix3{m_[0][0] + scalar, m_[0][1] + scalar, m_[0][2] + scalar,
							   m_[1][0] + scalar, m_[1][1] + scalar, m_[1][2] + scalar,
							   m_[2][0] + scalar, m_[2][1] + scalar, m_[2][2] + scalar};
			}

			//Add all numbers to the given scalar
			[[nodiscard]] friend inline auto operator+(real scalar, const Matrix3 &rhs) noexcept
			{
				return rhs + scalar; //Commutative
			}

			//Add two matrices (adding each number)
			inline auto& operator+=(const Matrix3 &rhs) noexcept
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
			[[nodiscard]] inline auto operator-(const Matrix3 &rhs) const noexcept
			{
				return Matrix3{m_[0][0] - rhs.m_[0][0], m_[0][1] - rhs.m_[0][1], m_[0][2] - rhs.m_[0][2],
							   m_[1][0] - rhs.m_[1][0], m_[1][1] - rhs.m_[1][1], m_[1][2] - rhs.m_[1][2],
							   m_[2][0] - rhs.m_[2][0], m_[2][1] - rhs.m_[2][1], m_[2][2] - rhs.m_[2][2]};
			}

			//Subtract all numbers with the given scalar
			[[nodiscard]] inline auto operator-(real scalar) const noexcept
			{
				return Matrix3{m_[0][0] - scalar, m_[0][1] - scalar, m_[0][2] - scalar,
							   m_[1][0] - scalar, m_[1][1] - scalar, m_[1][2] - scalar,
							   m_[2][0] - scalar, m_[2][1] - scalar, m_[2][2] - scalar};
			}

			//Subtract all numbers with the given scalar
			[[nodiscard]] friend inline auto operator-(real scalar, const Matrix3 &rhs) noexcept
			{
				return Matrix3{scalar - rhs.m_[0][0], scalar - rhs.m_[0][1], scalar - rhs.m_[0][2],
							   scalar - rhs.m_[1][0], scalar - rhs.m_[1][1], scalar - rhs.m_[1][2],
							   scalar - rhs.m_[2][0], scalar - rhs.m_[2][1], scalar - rhs.m_[2][2]};
			}

			//Subtract two matrices (subtracting each number)
			inline auto& operator-=(const Matrix3 &rhs) noexcept
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
			[[nodiscard]] inline auto operator*(const Matrix3 &rhs) const noexcept
			{
				return Matrix3{m_[0][0] * rhs.m_[0][0] + m_[0][1] * rhs.m_[1][0] + m_[0][2] * rhs.m_[2][0],
							   m_[0][0] * rhs.m_[0][1] + m_[0][1] * rhs.m_[1][1] + m_[0][2] * rhs.m_[2][1],
							   m_[0][0] * rhs.m_[0][2] + m_[0][1] * rhs.m_[1][2] + m_[0][2] * rhs.m_[2][2],
							   
							   m_[1][0] * rhs.m_[0][0] + m_[1][1] * rhs.m_[1][0] + m_[1][2] * rhs.m_[2][0],
							   m_[1][0] * rhs.m_[0][1] + m_[1][1] * rhs.m_[1][1] + m_[1][2] * rhs.m_[2][1],
							   m_[1][0] * rhs.m_[0][2] + m_[1][1] * rhs.m_[1][2] + m_[1][2] * rhs.m_[2][2],
							   
							   m_[2][0] * rhs.m_[0][0] + m_[2][1] * rhs.m_[1][0] + m_[2][2] * rhs.m_[2][0],
							   m_[2][0] * rhs.m_[0][1] + m_[2][1] * rhs.m_[1][1] + m_[2][2] * rhs.m_[2][1],
							   m_[2][0] * rhs.m_[0][2] + m_[2][1] * rhs.m_[1][2] + m_[2][2] * rhs.m_[2][2]};
			}

			//Multiply all numbers with the given scalar
			[[nodiscard]] inline auto operator*(real scalar) const noexcept
			{
				return Matrix3{m_[0][0] * scalar, m_[0][1] * scalar, m_[0][2] * scalar,
							   m_[1][0] * scalar, m_[1][1] * scalar, m_[1][2] * scalar,
							   m_[2][0] * scalar, m_[2][1] * scalar, m_[2][2] * scalar};
			}

			//Multiply matrix with the given vector
			//This operation results in a vector
			[[nodiscard]] inline auto operator*(const Vector2 &vector) const noexcept
			{
				auto [x, y] = vector.XY();
				auto inv_w = 1.0_r / (m_[2][0] * x + m_[2][1] * y + m_[2][2]);
				return Vector2{(m_[0][0] * x + m_[0][1] * y + m_[0][2]) * inv_w,
							   (m_[1][0] * x + m_[1][1] * y + m_[1][2]) * inv_w};
			}

			//Multiply all numbers with the given scalar
			[[nodiscard]] friend inline auto operator*(real scalar, const Matrix3 &rhs) noexcept
			{
				return rhs * scalar; //Commutative
			}

			//Multiply two matrices (matrix multiplication)
			inline auto& operator*=(const Matrix3 &rhs) noexcept
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
			//Accesses matrix linearly in order {m00, m01, m02, m10, m11, m12, m20, m21, m22}
			[[nodiscard]] inline auto& operator[](int off) noexcept
			{
				assert(off >= 0 && off < 9);
				return numbers_[off];
			}

			//Returns the number at the given offset
			//Accesses matrix linearly in order {m00, m01, m02, m10, m11, m12, m20, m21, m22}
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 9);
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


			//Returns direct access to the matrix
			[[nodiscard]] inline auto M() noexcept
			{
				return m_;
			}

			//Returns direct access to the matrix
			[[nodiscard]] inline const auto M() const noexcept
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
			[[nodiscard]] Vector2 ToScaling() const noexcept;

			//Returns vector of the matrix shearing
			[[nodiscard]] Vector2 ToShearing() const noexcept;

			//Returns vector of the matrix translation
			[[nodiscard]] Vector2 ToTranslation() const noexcept;

			
			/*
				Adjoint/adjugate
			*/

			//Adjoint/adjugate of the matrix
			//It is the transpose of its cofactor matrix
			Matrix3& Adjoint() noexcept;

			//Adjoint/adjugate of the matrix
			//It is the transpose of its cofactor matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix3 AdjointCopy() const noexcept;


			/*
				Affine
			*/

			//Make matrix an affine matrix
			Matrix3& Affine() noexcept;

			//Make matrix an affine matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix3 AffineCopy() const noexcept;

			//Returns true if this matrix is affine
			[[nodiscard]] bool IsAffine() const noexcept;


			/*
				Concatenating
			*/

			//Concatenate this matrix with the given matrix
			//This is the same as matrix multiplication
			Matrix3& Concatenate(const Matrix3 &matrix) noexcept;

			//Concatenate this matrix with the given matrix
			//This is the same as matrix multiplication
			//Returns the result as a copy
			[[nodiscard]] Matrix3 ConcatenateCopy(const Matrix3 &matrix) const noexcept;


			//Concatenate this affine matrix with the given affine matrix
			//This uses a faster version of matrix multiplication
			Matrix3& ConcatenateAffine(const Matrix3 &matrix) noexcept;

			//Concatenate this affine matrix with the given affine matrix
			//This uses a faster version of matrix multiplication
			//Returns the result as a copy
			[[nodiscard]] Matrix3 ConcatenateAffineCopy(const Matrix3 &matrix) const noexcept;


			/*
				Cofactoring
			*/

			//Make matrix a cofactor matrix
			Matrix3& Cofactor() noexcept;

			//Make matrix a cofactor matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix3 CofactorCopy() const noexcept;


			/*
				Determinant and inversing
			*/

			//Returns the determinant of the matrix
			[[nodiscard]] real Determinant() const noexcept;
			

			//Inverse matrix
			//It is the inverse of its determinant multiplied with its adjoint matrix
			Matrix3& Inverse() noexcept;

			//Inverse matrix
			//It is the inverse of its determinant multiplied with its adjoint matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix3 InverseCopy() const noexcept;


			/*
				Reflecting
			*/

			//Reflect matrix by the given angle (radians)
			Matrix3& Reflect(real angle) noexcept;

			//Reflect matrix by the given angle (radians)
			//Returns the result as a copy
			[[nodiscard]] Matrix3 ReflectCopy(real angle) const noexcept;


			/*
				Rotating
			*/

			//Rotate matrix by the given angle (radians)
			Matrix3& Rotate(real angle) noexcept;

			//Rotate matrix by the given angle (radians)
			//Returns the result as a copy
			[[nodiscard]] Matrix3 RotateCopy(real angle) const noexcept;


			/*
				Scaling
			*/

			//Scale matrix by the given vector
			Matrix3& Scale(const Vector2 &vector) noexcept;

			//Scale matrix by the given vector
			//Returns the result as a copy
			[[nodiscard]] Matrix3 ScaleCopy(const Vector2 &vector) const noexcept;


			/*
				Shearing
			*/

			//Shear matrix by the given vector
			Matrix3& Shear(const Vector2 &vector) noexcept;

			//Shear matrix by the given vector
			//Returns the result as a copy
			[[nodiscard]] Matrix3 ShearCopy(const Vector2 &vector) const noexcept;


			/*
				Transforming
			*/

			//Transform the given point based on matrix
			[[nodiscard]] Vector2 TransformPoint(const Vector2 &point) const noexcept;


			/*
				Translating
			*/

			//Translate matrix by the given vector
			Matrix3& Translate(const Vector2 &vector) noexcept;

			//Translate matrix by the given vector
			//Returns the result as a copy
			[[nodiscard]] Matrix3 TranslateCopy(const Vector2 &vector) const noexcept;


			/*
				Transposition
			*/

			//Transpose the matrix
			Matrix3& Transpose() noexcept;

			//Transpose the matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix3 TransposeCopy() const noexcept;
	};


	namespace matrix3
	{
		/*
			Predefined constant matrices
		*/

		inline const auto Zero = Matrix3{0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r};

		inline const auto Identity = Matrix3{1.0_r, 0.0_r, 0.0_r,
											 0.0_r, 1.0_r, 0.0_r,
											 0.0_r, 0.0_r, 1.0_r};
	}
} //ion::graphics::utilities

#endif