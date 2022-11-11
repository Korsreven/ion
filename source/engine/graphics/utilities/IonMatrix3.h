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
	class Matrix2; //Forward declaration
	class Matrix4; //Forward declaration
	using namespace types::type_literals;

	namespace matrix3::detail
	{
	} //matrix3::detail


	///@brief A class representing a 3x3 column-major matrix with right hand rotation
	///@details Follows the OpenGL matrix convention by default.
	///Define ION_ROW_MAJOR and ION_LEFT_HANDED to use this class with Direct3D
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

			///@brief Default constructor
			Matrix3() = default;

			///@brief Constructs a new matrix from the given numbers
			Matrix3(real m00, real m01, real m02,
					real m10, real m11, real m12,
					real m20, real m21, real m22) noexcept;
			
			#ifdef ION_ROW_MAJOR
			///@brief Row-major layout (Direct3D)
			///@details Constructs a new matrix from only the two first columns.
			///The third column is filled with {0, 0, 1}
			Matrix3(real m00, real m01,
					real m10, real m11,
					real m20, real m21) noexcept;
			#else
			///@brief Column-major layout (OpenGL)
			///@details Constructs a new matrix from only the two first rows.
			///The third row is filled with {0, 0, 1}
			Matrix3(real m00, real m01, real m02,
					real m10, real m11, real m12) noexcept;
			#endif

			///@brief Constructs a new matrix from the given 2x2 matrix
			Matrix3(const Matrix2 &matrix) noexcept;

			///@brief Constructs a new matrix from the given 4x4 matrix
			Matrix3(const Matrix4 &matrix) noexcept;
			

			/**
				@name Static matrix conversions
				@{
			*/

			///@brief Returns a new reflection matrix from the given angle (radians)
			[[nodiscard]] static Matrix3 Reflection(real angle) noexcept;

			///@brief Returns a new rotation matrix from the given angle (radians)
			[[nodiscard]] static Matrix3 Rotation(real angle) noexcept;

			///@brief Returns a new rotation matrix from the given angle (radians) and origin vector
			[[nodiscard]] static Matrix3 Rotation(real angle, const Vector2 &origin) noexcept;

			///@brief Returns a new scaling matrix from the given vector
			[[nodiscard]] static Matrix3 Scaling(const Vector2 &vector) noexcept;

			///@brief Returns a new shearing matrix from the given vector
			[[nodiscard]] static Matrix3 Shearing(const Vector2 &vector) noexcept;

			///@brief Returns a new translation matrix from the given vector
			[[nodiscard]] static Matrix3 Translation(const Vector2 &vector) noexcept;

			///@brief Returns a new transformation matrix from the given rotation angle (radians), scaling and translation
			[[nodiscard]] static Matrix3 Transformation(real rotation, const Vector2 &scaling, const Vector2 &translation) noexcept;

			///@brief Returns a new transformation matrix from the given 4x4 matrix
			[[nodiscard]] static Matrix3 Transformation(const Matrix4 &matrix) noexcept;

			///@}

			/**
				@name Operators
				@{
			*/

			///@brief Sets this matrix equal to the given 2x2 matrix
			Matrix3& operator=(const Matrix2 &matrix) noexcept;

			///@brief Sets this matrix equal to the given 4x4 matrix
			Matrix3& operator=(const Matrix4 &matrix) noexcept;


			///@brief Checks if two matrices are equal (all numbers are equal)
			[[nodiscard]] inline auto operator==(const Matrix3 &rhs) const noexcept
			{
				return m_[0][0] == rhs.m_[0][0] && m_[0][1] == rhs.m_[0][1] && m_[0][2] == rhs.m_[0][2] &&
					   m_[1][0] == rhs.m_[1][0] && m_[1][1] == rhs.m_[1][1] && m_[1][2] == rhs.m_[1][2] &&
					   m_[2][0] == rhs.m_[2][0] && m_[2][1] == rhs.m_[2][1] && m_[2][2] == rhs.m_[2][2];
			}

			///@brief Checks if two matrices are different (one or more numbers are different)
			[[nodiscard]] inline auto operator!=(const Matrix3 &rhs) const noexcept
			{
				return !(*this == rhs);
			}


			///@brief Negates matrix (negating each number)
			[[nodiscard]] inline auto operator-() const noexcept
			{
				return Matrix3{-m_[0][0], -m_[0][1], -m_[0][2],
							   -m_[1][0], -m_[1][1], -m_[1][2],
							   -m_[2][0], -m_[2][1], -m_[2][2]};
			}

			///@}

			/**
				@name Adding
				@{
			*/

			///@brief Adds two matrices (adding each number)
			[[nodiscard]] inline auto operator+(const Matrix3 &rhs) const noexcept
			{
				return Matrix3{m_[0][0] + rhs.m_[0][0], m_[0][1] + rhs.m_[0][1], m_[0][2] + rhs.m_[0][2],
							   m_[1][0] + rhs.m_[1][0], m_[1][1] + rhs.m_[1][1], m_[1][2] + rhs.m_[1][2],
							   m_[2][0] + rhs.m_[2][0], m_[2][1] + rhs.m_[2][1], m_[2][2] + rhs.m_[2][2]};
			}

			///@brief Adds all numbers to the given scalar
			[[nodiscard]] inline auto operator+(real scalar) const noexcept
			{
				return Matrix3{m_[0][0] + scalar, m_[0][1] + scalar, m_[0][2] + scalar,
							   m_[1][0] + scalar, m_[1][1] + scalar, m_[1][2] + scalar,
							   m_[2][0] + scalar, m_[2][1] + scalar, m_[2][2] + scalar};
			}

			///@brief Adds all numbers to the given scalar
			[[nodiscard]] friend inline auto operator+(real scalar, const Matrix3 &rhs) noexcept
			{
				return rhs + scalar; ///@brief Commutative
			}

			///@brief Adds two matrices (adding each number)
			inline auto& operator+=(const Matrix3 &rhs) noexcept
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
			[[nodiscard]] inline auto operator-(const Matrix3 &rhs) const noexcept
			{
				return Matrix3{m_[0][0] - rhs.m_[0][0], m_[0][1] - rhs.m_[0][1], m_[0][2] - rhs.m_[0][2],
							   m_[1][0] - rhs.m_[1][0], m_[1][1] - rhs.m_[1][1], m_[1][2] - rhs.m_[1][2],
							   m_[2][0] - rhs.m_[2][0], m_[2][1] - rhs.m_[2][1], m_[2][2] - rhs.m_[2][2]};
			}

			///@brief Subtracts all numbers with the given scalar
			[[nodiscard]] inline auto operator-(real scalar) const noexcept
			{
				return Matrix3{m_[0][0] - scalar, m_[0][1] - scalar, m_[0][2] - scalar,
							   m_[1][0] - scalar, m_[1][1] - scalar, m_[1][2] - scalar,
							   m_[2][0] - scalar, m_[2][1] - scalar, m_[2][2] - scalar};
			}

			///@brief Subtracts all numbers with the given scalar
			[[nodiscard]] friend inline auto operator-(real scalar, const Matrix3 &rhs) noexcept
			{
				return Matrix3{scalar - rhs.m_[0][0], scalar - rhs.m_[0][1], scalar - rhs.m_[0][2],
							   scalar - rhs.m_[1][0], scalar - rhs.m_[1][1], scalar - rhs.m_[1][2],
							   scalar - rhs.m_[2][0], scalar - rhs.m_[2][1], scalar - rhs.m_[2][2]};
			}

			///@brief Subtracts two matrices (subtracting each number)
			inline auto& operator-=(const Matrix3 &rhs) noexcept
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
			[[nodiscard]] inline auto operator*(const Matrix3 &rhs) const noexcept
			{
				#ifdef ION_ROW_MAJOR
				//Row-major layout (Direct3D)
				return Matrix3{m_[0][0] * rhs.m_[0][0] + m_[1][0] * rhs.m_[0][1] + m_[2][0] * rhs.m_[0][2],
							   m_[0][1] * rhs.m_[0][0] + m_[1][1] * rhs.m_[0][1] + m_[2][1] * rhs.m_[0][2],
							   m_[0][2] * rhs.m_[0][0] + m_[1][2] * rhs.m_[0][1] + m_[2][2] * rhs.m_[0][2],

							   m_[0][0] * rhs.m_[1][0] + m_[1][0] * rhs.m_[1][1] + m_[2][0] * rhs.m_[1][2],
							   m_[0][1] * rhs.m_[1][0] + m_[1][1] * rhs.m_[1][1] + m_[2][1] * rhs.m_[1][2],
							   m_[0][2] * rhs.m_[1][0] + m_[1][2] * rhs.m_[1][1] + m_[2][2] * rhs.m_[1][2],

							   m_[0][0] * rhs.m_[2][0] + m_[1][0] * rhs.m_[2][1] + m_[2][0] * rhs.m_[2][2],
							   m_[0][1] * rhs.m_[2][0] + m_[1][1] * rhs.m_[2][1] + m_[2][1] * rhs.m_[2][2],
							   m_[0][2] * rhs.m_[2][0] + m_[1][2] * rhs.m_[2][1] + m_[2][2] * rhs.m_[2][2]};
				#else
				//Column-major layout (OpenGL)
				return Matrix3{m_[0][0] * rhs.m_[0][0] + m_[0][1] * rhs.m_[1][0] + m_[0][2] * rhs.m_[2][0],
							   m_[0][0] * rhs.m_[0][1] + m_[0][1] * rhs.m_[1][1] + m_[0][2] * rhs.m_[2][1],
							   m_[0][0] * rhs.m_[0][2] + m_[0][1] * rhs.m_[1][2] + m_[0][2] * rhs.m_[2][2],
							   
							   m_[1][0] * rhs.m_[0][0] + m_[1][1] * rhs.m_[1][0] + m_[1][2] * rhs.m_[2][0],
							   m_[1][0] * rhs.m_[0][1] + m_[1][1] * rhs.m_[1][1] + m_[1][2] * rhs.m_[2][1],
							   m_[1][0] * rhs.m_[0][2] + m_[1][1] * rhs.m_[1][2] + m_[1][2] * rhs.m_[2][2],
							   
							   m_[2][0] * rhs.m_[0][0] + m_[2][1] * rhs.m_[1][0] + m_[2][2] * rhs.m_[2][0],
							   m_[2][0] * rhs.m_[0][1] + m_[2][1] * rhs.m_[1][1] + m_[2][2] * rhs.m_[2][1],
							   m_[2][0] * rhs.m_[0][2] + m_[2][1] * rhs.m_[1][2] + m_[2][2] * rhs.m_[2][2]};
				#endif
			}

			///@brief Muliplies all numbers with the given scalar
			[[nodiscard]] inline auto operator*(real scalar) const noexcept
			{
				return Matrix3{m_[0][0] * scalar, m_[0][1] * scalar, m_[0][2] * scalar,
							   m_[1][0] * scalar, m_[1][1] * scalar, m_[1][2] * scalar,
							   m_[2][0] * scalar, m_[2][1] * scalar, m_[2][2] * scalar};
			}

			///@brief Muliplies matrix with the given vector
			///@details This operation results in a vector
			[[nodiscard]] inline auto operator*(const Vector2 &vector) const noexcept
			{
				auto [x, y] = vector.XY();

				#ifdef ION_ROW_MAJOR
				//Row-major layout (Direct3D)
				auto inv_w = 1.0_r / (m_[0][2] * x + m_[1][2] * y + m_[2][2]);
				return Vector2{(m_[0][0] * x + m_[1][0] * y + m_[2][0]) * inv_w,
							   (m_[0][1] * x + m_[1][1] * y + m_[2][1]) * inv_w};
				#else
				//Column-major layout (OpenGL)
				auto inv_w = 1.0_r / (m_[2][0] * x + m_[2][1] * y + m_[2][2]);
				return Vector2{(m_[0][0] * x + m_[0][1] * y + m_[0][2]) * inv_w,
							   (m_[1][0] * x + m_[1][1] * y + m_[1][2]) * inv_w};
				#endif
			}

			///@brief Muliplies all numbers with the given scalar
			[[nodiscard]] friend inline auto operator*(real scalar, const Matrix3 &rhs) noexcept
			{
				return rhs * scalar; //Commutative
			}

			///@brief Muliplies two matrices (matrix multiplication)
			inline auto& operator*=(const Matrix3 &rhs) noexcept
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
			///@details Accesses matrix linearly in order {m00, m01, m02, m10, m11, m12, m20, m21, m22}
			[[nodiscard]] inline auto& operator[](int off) noexcept
			{
				assert(off >= 0 && off < 9);
				return numbers_[off];
			}

			///@brief Returns the number at the given offset
			///@details Accesses matrix linearly in order {m00, m01, m02, m10, m11, m12, m20, m21, m22}
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 9);
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

			#ifdef ION_ROW_MAJOR
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
			#else
			///@brief Sets the m02 number to the given value
			inline void M02(real m02) noexcept
			{
				m_[0][2] = m02;
			}

			///@brief Sets the m12 number to the given value
			inline void M12(real m12) noexcept
			{
				m_[1][2] = m12;
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

			#ifdef ION_ROW_MAJOR
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
			#else
			///@brief Returns the m02 number
			[[nodiscard]] inline auto M02() const noexcept
			{
				return m_[0][2];
			}

			///@brief Returns the m12 number
			[[nodiscard]] inline auto M12() const noexcept
			{
				return m_[1][2];
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
			[[nodiscard]] Vector2 ToScaling() const noexcept;

			///@brief Returns vector of the matrix shearing
			[[nodiscard]] Vector2 ToShearing() const noexcept;

			///@brief Returns vector of the matrix translation
			[[nodiscard]] Vector2 ToTranslation() const noexcept;

			///@}

			/**
				@name Adjoint/adjugate
				@{
			*/

			///@brief Adjoint/adjugate of the matrix
			///@details It is the transpose of its cofactor matrix
			Matrix3& Adjoint() noexcept;

			///@brief Adjoint/adjugate of the matrix
			///@details It is the transpose of its cofactor matrix.
			///Returns the result as a copy
			[[nodiscard]] Matrix3 AdjointCopy() const noexcept;

			///@}

			/**
				@name Affine
				@{
			*/

			///@brief Makes matrix an affine matrix
			Matrix3& Affine() noexcept;

			///@brief Makes matrix an affine matrix
			///@details Returns the result as a copy
			[[nodiscard]] Matrix3 AffineCopy() const noexcept;

			///@brief Returns true if this matrix is affine
			[[nodiscard]] bool IsAffine() const noexcept;

			///@}

			/**
				@name Concatenating
				@{
			*/

			///@brief Concatenates this matrix with the given matrix
			///@details This is the same as matrix multiplication
			Matrix3& Concatenate(const Matrix3 &matrix) noexcept;

			///@brief Concatenates this matrix with the given matrix
			///@details This is the same as matrix multiplication
			///Returns the result as a copy
			[[nodiscard]] Matrix3 ConcatenateCopy(const Matrix3 &matrix) const noexcept;


			///@brief Concatenates this affine matrix with the given affine matrix
			///@details This uses a faster version of matrix multiplication
			Matrix3& ConcatenateAffine(const Matrix3 &matrix) noexcept;

			///@brief Concatenates this affine matrix with the given affine matrix
			///@details This uses a faster version of matrix multiplication
			///Returns the result as a copy
			[[nodiscard]] Matrix3 ConcatenateAffineCopy(const Matrix3 &matrix) const noexcept;

			///@}

			/**
				@name Cofactoring
				@{
			*/

			///@brief Makes matrix a cofactor matrix
			Matrix3& Cofactor() noexcept;

			///@brief Makes matrix a cofactor matrix
			///@details Returns the result as a copy
			[[nodiscard]] Matrix3 CofactorCopy() const noexcept;

			///@}

			/**
				@name Determinant and inversing
				@{
			*/

			///@brief Returns the determinant of the matrix
			[[nodiscard]] real Determinant() const noexcept;
			

			///@brief Inverse matrix
			///@details It is the inverse of its determinant multiplied with its adjoint matrix
			Matrix3& Inverse() noexcept;

			///@brief Inverse matrix
			///@details It is the inverse of its determinant multiplied with its adjoint matrix
			///Returns the result as a copy
			[[nodiscard]] Matrix3 InverseCopy() const noexcept;

			///@}

			/**
				@name Reflecting
				@{
			*/

			///@brief Reflect matrix by the given angle (radians)
			Matrix3& Reflect(real angle) noexcept;

			///@brief Reflect matrix by the given angle (radians)
			///@details Returns the result as a copy
			[[nodiscard]] Matrix3 ReflectCopy(real angle) const noexcept;

			///@}

			/**
				@name Rotating
				@{
			*/

			///@brief Rotates matrix by the given angle (radians)
			Matrix3& Rotate(real angle) noexcept;

			///@brief Rotates matrix by the given angle (radians)
			///@details Returns the result as a copy
			[[nodiscard]] Matrix3 RotateCopy(real angle) const noexcept;

			///@}

			/**
				@name Scaling
				@{
			*/

			///@brief Scales matrix by the given vector
			Matrix3& Scale(const Vector2 &vector) noexcept;

			///@brief Scales matrix by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Matrix3 ScaleCopy(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Shearing
				@{
			*/

			///@brief Shear matrix by the given vector
			Matrix3& Shear(const Vector2 &vector) noexcept;

			///@brief Shear matrix by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Matrix3 ShearCopy(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Transforming
				@{
			*/

			///@brief Transforms the given point based on matrix
			[[nodiscard]] Vector2 TransformPoint(const Vector2 &point) const noexcept;

			///@}

			/**
				@name Translating
				@{
			*/

			///@brief Translates matrix by the given vector
			Matrix3& Translate(const Vector2 &vector) noexcept;

			///@brief Translates matrix by the given vector
			///@details Returns the result as a copy
			[[nodiscard]] Matrix3 TranslateCopy(const Vector2 &vector) const noexcept;

			///@}

			/**
				@name Transposition
				@{
			*/

			///@brief Transpose the matrix
			Matrix3& Transpose() noexcept;

			///@brief Transpose the matrix
			///@details Returns the result as a copy
			[[nodiscard]] Matrix3 TransposeCopy() const noexcept;

			///@}
	};


	namespace matrix3
	{
		/**
			@name Predefined constant matrices
			@{
		*/

		inline const auto Zero = Matrix3{0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r,
										 0.0_r, 0.0_r, 0.0_r};

		inline const auto Identity = Matrix3{1.0_r, 0.0_r, 0.0_r,
											 0.0_r, 1.0_r, 0.0_r,
											 0.0_r, 0.0_r, 1.0_r};

		///@}
	}
} //ion::graphics::utilities

#endif