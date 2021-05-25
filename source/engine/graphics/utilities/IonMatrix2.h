/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonMatrix2.h
-------------------------------------------
*/

#ifndef ION_MATRIX2_H
#define ION_MATRIX2_H

#include "IonVector2.h"
#include "types/IonTypes.h"

namespace ion::graphics::utilities
{
	class Matrix3; //Forward declaration
	using namespace types::type_literals;

	namespace matrix2::detail
	{
	} //matrix2::detail


	//A 2x2 column-major matrix class with right hand rotation
	//Follows the OpenGL matrix convention by default
	//Define ION_ROW_MAJOR to use this class with Direct3D
	class Matrix2 final
	{
		private:

			union
			{
				real m_[2][2];
				real numbers_[4]{1.0_r, 0.0_r,
								 0.0_r, 1.0_r};
			};

		public:

			Matrix2() = default;

			//Constructs a new matrix from the given numbers
			Matrix2(real m00, real m01,
					real m10, real m11) noexcept;

			//Constructs a new matrix from the given 3x3 matrix
			Matrix2(const Matrix3 &matrix) noexcept;
			

			/*
				Static matrix conversions
			*/

			//Returns a new reflection matrix from the given angle (radians)
			[[nodiscard]] static Matrix2 Reflection(real angle) noexcept;

			//Returns a new rotation matrix from the given angle (radians)
			[[nodiscard]] static Matrix2 Rotation(real angle) noexcept;

			//Returns a new scaling matrix from the given vector
			[[nodiscard]] static Matrix2 Scaling(const Vector2 &vector) noexcept;

			//Returns a new shearing matrix from the given vector
			[[nodiscard]] static Matrix2 Shearing(const Vector2 &vector) noexcept;

			//Returns a new transformation matrix from the given rotation angle (radians) and scaling
			[[nodiscard]] static Matrix2 Transformation(real rotation, const Vector2 &scaling) noexcept;


			/*
				Operators
			*/

			//Sets this matrix equal to the given 3x3 matrix
			Matrix2& operator=(const Matrix3 &matrix) noexcept;


			//Checks if two matrices are equal (all numbers are equal)
			[[nodiscard]] inline auto operator==(const Matrix2 &rhs) const noexcept
			{
				return m_[0][0] == rhs.m_[0][0] && m_[0][1] == rhs.m_[0][1] &&
					   m_[1][0] == rhs.m_[1][0] && m_[1][1] == rhs.m_[1][1];
			}

			//Checks if two matrices are different (one or more numbers are different)
			[[nodiscard]] inline auto operator!=(const Matrix2 &rhs) const noexcept
			{
				return !(*this == rhs);
			}


			//Negate matrix (negating each number)
			[[nodiscard]] inline auto operator-() const noexcept
			{
				return Matrix2{-m_[0][0], -m_[0][1],
							   -m_[1][0], -m_[1][1]};
			}


			/*
				Adding
			*/

			//Add two matrices (adding each number)
			[[nodiscard]] inline auto operator+(const Matrix2 &rhs) const noexcept
			{
				return Matrix2{m_[0][0] + rhs.m_[0][0], m_[0][1] + rhs.m_[0][1],
							   m_[1][0] + rhs.m_[1][0], m_[1][1] + rhs.m_[1][1]};
			}

			//Add all numbers to the given scalar
			[[nodiscard]] inline auto operator+(real scalar) const noexcept
			{
				return Matrix2{m_[0][0] + scalar, m_[0][1] + scalar,
							   m_[1][0] + scalar, m_[1][1] + scalar};
			}

			//Add all numbers to the given scalar
			[[nodiscard]] friend inline auto operator+(real scalar, const Matrix2 &rhs) noexcept
			{
				return rhs + scalar; //Commutative
			}

			//Add two matrices (adding each number)
			inline auto& operator+=(const Matrix2 &rhs) noexcept
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
			[[nodiscard]] inline auto operator-(const Matrix2 &rhs) const noexcept
			{
				return Matrix2{m_[0][0] - rhs.m_[0][0], m_[0][1] - rhs.m_[0][1],
							   m_[1][0] - rhs.m_[1][0], m_[1][1] - rhs.m_[1][1]};
			}

			//Subtract all numbers with the given scalar
			[[nodiscard]] inline auto operator-(real scalar) const noexcept
			{
				return Matrix2{m_[0][0] - scalar, m_[0][1] - scalar,
							   m_[1][0] - scalar, m_[1][1] - scalar};
			}

			//Subtract all numbers with the given scalar
			[[nodiscard]] friend inline auto operator-(real scalar, const Matrix2 &rhs) noexcept
			{
				return Matrix2{scalar - rhs.m_[0][0], scalar - rhs.m_[0][1],
							   scalar - rhs.m_[1][0], scalar - rhs.m_[1][1]};
			}

			//Subtract two matrices (subtracting each number)
			inline auto& operator-=(const Matrix2 &rhs) noexcept
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
			[[nodiscard]] inline auto operator*(const Matrix2 &rhs) const noexcept
			{
				#ifdef ION_ROW_MAJOR
				//Row-major layout (Direct3D)
				return Matrix2{m_[0][0] * rhs.m_[0][0] + m_[1][0] * rhs.m_[0][1],
							   m_[0][1] * rhs.m_[0][0] + m_[1][1] * rhs.m_[0][1],

							   m_[0][0] * rhs.m_[1][0] + m_[1][0] * rhs.m_[1][1],
							   m_[0][1] * rhs.m_[1][0] + m_[1][1] * rhs.m_[1][1]};
				#else
				//Column-major layout (OpenGL)
				return Matrix2{m_[0][0] * rhs.m_[0][0] + m_[0][1] * rhs.m_[1][0],
							   m_[0][0] * rhs.m_[0][1] + m_[0][1] * rhs.m_[1][1],
							   
							   m_[1][0] * rhs.m_[0][0] + m_[1][1] * rhs.m_[1][0],
							   m_[1][0] * rhs.m_[0][1] + m_[1][1] * rhs.m_[1][1]};
				#endif
			}

			//Multiply all numbers with the given scalar
			[[nodiscard]] inline auto operator*(real scalar) const noexcept
			{
				return Matrix2{m_[0][0] * scalar, m_[0][1] * scalar,
							   m_[1][0] * scalar, m_[1][1] * scalar};
			}

			//Multiply matrix with the given vector
			//This operation results in a vector
			[[nodiscard]] inline auto operator*(const Vector2 &vector) const noexcept
			{
				auto [x, y] = vector.XY();

				#ifdef ION_ROW_MAJOR
				//Row-major layout (Direct3D)
				return Vector2{m_[0][0] * x + m_[1][0] * y,
							   m_[0][1] * x + m_[1][1] * y};
				#else
				//Column-major layout (OpenGL)
				return Vector2{m_[0][0] * x + m_[0][1] * y,
							   m_[1][0] * x + m_[1][1] * y};
				#endif
			}

			//Multiply all numbers with the given scalar
			[[nodiscard]] friend inline auto operator*(real scalar, const Matrix2 &rhs) noexcept
			{
				return rhs * scalar; //Commutative
			}

			//Multiply two matrices (matrix multiplication)
			inline auto& operator*=(const Matrix2 &rhs) noexcept
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
			//Accesses matrix linearly in order {m00, m01, m02, m10}
			[[nodiscard]] inline auto& operator[](int off) noexcept
			{
				assert(off >= 0 && off < 4);
				return numbers_[off];
			}

			//Returns the number at the given offset
			//Accesses matrix linearly in order {m00, m01, m02, m10}
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 4);
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
			[[nodiscard]] Vector2 ToScaling() const noexcept;

			//Returns vector of the matrix shearing
			[[nodiscard]] Vector2 ToShearing() const noexcept;

			
			/*
				Adjoint/adjugate
			*/

			//Adjoint/adjugate of the matrix
			//It is the transpose of its cofactor matrix
			Matrix2& Adjoint() noexcept;

			//Adjoint/adjugate of the matrix
			//It is the transpose of its cofactor matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix2 AdjointCopy() const noexcept;


			/*
				Concatenating
			*/

			//Concatenate this matrix with the given matrix
			//This is the same as matrix multiplication
			Matrix2& Concatenate(const Matrix2 &matrix) noexcept;

			//Concatenate this matrix with the given matrix
			//This is the same as matrix multiplication
			//Returns the result as a copy
			[[nodiscard]] Matrix2 ConcatenateCopy(const Matrix2 &matrix) const noexcept;


			/*
				Cofactoring
			*/

			//Make matrix a cofactor matrix
			Matrix2& Cofactor() noexcept;

			//Make matrix a cofactor matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix2 CofactorCopy() const noexcept;


			/*
				Determinant and inversing
			*/

			//Returns the determinant of the matrix
			[[nodiscard]] real Determinant() const noexcept;
			

			//Inverse matrix
			//It is the inverse of its determinant multiplied with its adjoint matrix
			Matrix2& Inverse() noexcept;

			//Inverse matrix
			//It is the inverse of its determinant multiplied with its adjoint matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix2 InverseCopy() const noexcept;


			/*
				Reflecting
			*/

			//Reflect matrix by the given angle (radians)
			Matrix2& Reflect(real angle) noexcept;

			//Reflect matrix by the given angle (radians)
			//Returns the result as a copy
			[[nodiscard]] Matrix2 ReflectCopy(real angle) const noexcept;


			/*
				Rotating
			*/

			//Rotate matrix by the given angle (radians)
			Matrix2& Rotate(real angle) noexcept;

			//Rotate matrix by the given angle (radians)
			//Returns the result as a copy
			[[nodiscard]] Matrix2 RotateCopy(real angle) const noexcept;


			/*
				Scaling
			*/

			//Scale matrix by the given vector
			Matrix2& Scale(const Vector2 &vector) noexcept;

			//Scale matrix by the given vector
			//Returns the result as a copy
			[[nodiscard]] Matrix2 ScaleCopy(const Vector2 &vector) const noexcept;


			/*
				Shearing
			*/

			//Shear matrix by the given vector
			Matrix2& Shear(const Vector2 &vector) noexcept;

			//Shear matrix by the given vector
			//Returns the result as a copy
			[[nodiscard]] Matrix2 ShearCopy(const Vector2 &vector) const noexcept;


			/*
				Transforming
			*/

			//Transform the given point based on matrix
			[[nodiscard]] Vector2 TransformPoint(const Vector2 &point) const noexcept;


			/*
				Transposition
			*/

			//Transpose the matrix
			Matrix2& Transpose() noexcept;

			//Transpose the matrix
			//Returns the result as a copy
			[[nodiscard]] Matrix2 TransposeCopy() const noexcept;
	};


	namespace matrix2
	{
		/*
			Predefined constant matrices
		*/

		inline const auto Zero = Matrix2{0.0_r, 0.0_r,
										 0.0_r, 0.0_r};

		inline const auto Identity = Matrix2{1.0_r, 0.0_r,
											 0.0_r, 1.0_r};
	}
} //ion::graphics::utilities

#endif