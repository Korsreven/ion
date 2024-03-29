/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/variables/types
File:	IonShaderTypes.h
-------------------------------------------
*/

#ifndef ION_SHADER_TYPES_H
#define ION_SHADER_TYPES_H

#include <array>
#include <cassert>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonMatrix2.h"
#include "graphics/utilities/IonMatrix3.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonTypes.h"

///@brief Namespace containing all supported GLSL types, from basic types, to vector, matrix and sampler types
namespace ion::graphics::shaders::variables::glsl
{
	using utilities::Color;
	using utilities::Matrix2;
	using utilities::Matrix3;
	using utilities::Matrix4;
	using utilities::Vector2;
	using utilities::Vector3;

	/**
		@name Basic vector type for:

		Vec<2, T>, Vec<3, T>, Vec<4, T>
		@{
	*/

	template <int N, typename T = float32>
	struct Vec final
	{
		static_assert(N >= 2 && N <= 4); //Components
	};

	///@}

	/**
		@name Basic matrix type for:

		Mat<2, 2, T>, Mat<2, 3, T>, Mat<2, 4, T>
		Mat<3, 2, T>, Mat<3, 3, T>, Mat<3, 4, T>
		Mat<4, 2, T>, Mat<4, 3, T>, Mat<4, 4, T>
		@{
	*/

	template <int N, int M, typename T = float32>
	struct Mat final
	{
		static_assert(N >= 2 && N <= 4); //Columns
		static_assert(M >= 2 && M <= 4); //Rows
	};

	///@}

	/**
		@name Basic sampler types
		@{
	*/

	template <typename T = float32>
	struct Sampler1D final
	{
	};

	template <typename T = float32>
	struct Sampler1DArray final
	{
	};

	template <typename T = float32>
	struct Sampler2D final
	{
	};

	template <typename T = float32>
	struct Sampler2DArray final
	{
	};

	///@}

	namespace detail
	{
		/**
			@name Basic scalar types
			@{
		*/

		template <typename T>
		struct basic_scalar_type_impl
		{
			using type = void; //Not supported
		};

		template <>
		struct basic_scalar_type_impl<bool>
		{
			using type = int32; //bool values in GLSL should be of type int (0 or 1)
		};

		template <>
		struct basic_scalar_type_impl<int32>
		{
			using type = int32;
		};

		template <>
		struct basic_scalar_type_impl<uint32>
		{
			using type = uint32;
		};

		template <>
		struct basic_scalar_type_impl<float32>
		{
			using type = float32;
		};

		template <>
		struct basic_scalar_type_impl<float64>
		{
			using type = float64;
		};

		///@}

		/**
			@name Basic types
			@{
		*/

		template <typename T>
		struct basic_type_impl : basic_scalar_type_impl<T>
		{
		};


		template <typename T, int N>
		struct basic_type_impl<Vec<N, T>> : basic_scalar_type_impl<T>
		{
		};

		template <typename T, int N, int M>
		struct basic_type_impl<Mat<N, M, T>> : basic_scalar_type_impl<T>
		{
		};


		template <typename T>
		struct basic_type_impl<Sampler1D<T>> : basic_scalar_type_impl<int32> //Texture unit is always int
		{
		};

		template <typename T>
		struct basic_type_impl<Sampler1DArray<T>> : basic_scalar_type_impl<int32> //Texture unit is always int
		{
		};

		template <typename T>
		struct basic_type_impl<Sampler2D<T>> : basic_scalar_type_impl<int32> //Texture unit is always int
		{
		};

		template <typename T>
		struct basic_type_impl<Sampler2DArray<T>> : basic_scalar_type_impl<int32> //Texture unit is always int
		{
		};

		///@}

		/**
			@name Type components
			@{
		*/

		template <typename T, int N = 1, int M = 1>
		struct type_components_impl
		{
			static constexpr auto value = N * M;
		};

		template <typename T, int N>
		struct type_components_impl<Vec<N, T>> :
			type_components_impl<T, N>
		{
		};

		template <typename T, int N, int M>
		struct type_components_impl<Mat<N, M, T>> :
			type_components_impl<T, N, M>
		{
		};

		///@}

		/**
			@name Uniform value
			@{
		*/

		template <typename T, size_t Size>
		using sbo_container_type = std::array<T, Size>;

		template <typename T>
		using container_type = std::vector<T>;

		///@}
	} //detail


	/**
		@name Basic type
		@{
	*/

	template <typename T>
	struct basic_type : detail::basic_type_impl<T>
	{
	};

	template <typename T>
	using basic_type_t = typename basic_type<T>::type;

	///@}

	/**
		@name Is basic type
		@{
	*/

	template <typename T>
	struct is_basic_type
	{
		static constexpr auto value = !std::is_same_v<basic_type_t<T>, void>;
	};

	template <typename T>
	constexpr auto is_basic_type_v = is_basic_type<T>::value;

	///@}

	/**
		@name Type components
		@{
	*/

	template <typename T>
	struct type_components
	{
		static constexpr auto value = detail::type_components_impl<T>::value;
	};

	template <typename T>
	constexpr auto type_components_v = type_components<T>::value;

	///@}

	/*
		Value accessors
	*/

	template <typename T>
	class ValueAccessorBase
	{
		protected:

			basic_type_t<T> *values_ = nullptr;
			int off_ = 0;

		public:

			ValueAccessorBase() = default;

			///@brief Constructor
			explicit ValueAccessorBase(basic_type_t<T> *values, int off = 0) noexcept :
				values_{values},
				off_{off}
			{
				//Empty
			}


			/**
				@name Operators
				@{
			*/

			///@brief Sets all components to the given value
			inline auto& operator=(basic_type_t<T> value) noexcept
			{
				for (auto i = 0; i < type_components_v<T>; ++i)
					values_[type_components_v<T> * off_ + i] = value;

				return *this;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns a pointer to all values (mutable) from off
			[[nodiscard]] inline auto Values() noexcept
			{
				return &values_[type_components_v<T> * off_];
			}

			///@brief Returns a pointer to all values (immutable) from off
			[[nodiscard]] inline auto Values() const noexcept
			{
				return &values_[type_components_v<T> * off_];
			}

			///@}
	};


	/*
		Scalar accessor
	*/

	template <typename T>
	struct ValueAccessor : ValueAccessorBase<T>
	{
		using ValueAccessorBase<T>::ValueAccessorBase;


		/**
			@name Operators
			@{
		*/

		///@brief Sets all components to the given value
		inline auto& operator=(basic_type_t<T> value) noexcept
		{
			ValueAccessorBase<T>::operator=(value);
			return *this;
		}

		///@brief Returns a modifiable reference to the value stored
		inline operator basic_type_t<T>&() noexcept
		{
			return this->Values()[0];
		}

		///@brief Returns the value stored
		inline operator basic_type_t<T>() const noexcept
		{
			return this->Values()[0];
		}

		///@}
	};


	/*
		Scalar accessor (bool)
	*/

	template <>
	struct ValueAccessor<bool> : ValueAccessorBase<bool>
	{
		using ValueAccessorBase<bool>::ValueAccessorBase;


		/**
			@name Operators
			@{
		*/

		///@brief Sets all components to the given value
		inline auto& operator=(bool value) noexcept
		{
			ValueAccessorBase<bool>::operator=(value ? 1 : 0);
			return *this;
		}

		///@brief Returns a modifiable reference to the value stored
		inline operator basic_type_t<bool>&() noexcept
		{
			return this->Values()[0];
		}

		///@brief Returns the value stored
		inline operator bool() const noexcept
		{
			return this->Values()[0] != 0;
		}

		///@}
	};


	/*
		Vector accessor
	*/

	template <typename T>
	struct ValueAccessor<Vec<2, T>> : ValueAccessorBase<Vec<2, T>>
	{
		using ValueAccessorBase<Vec<2, T>>::ValueAccessorBase;


		/**
			@name Operators
			@{
		*/

		///@brief Sets all components to the given value
		inline auto& operator=(T value) noexcept
		{
			ValueAccessorBase<Vec<2, T>>::operator=(value);
			return *this;
		}

		///@brief Returns a modifiable reference to the component at the given offset
		[[nodiscard]] inline auto& operator[](int off) noexcept
		{
			assert(off >= 0 && off < 2);
			return this->Values()[off];
		}

		///@brief Returns the component at the given offset
		[[nodiscard]] inline auto operator[](int off) const noexcept
		{
			assert(off >= 0 && off < 2);
			return this->Values()[off];
		}

		///@}

		/**
			@name Modifiers
			@{
		*/

		///@brief Sets the x component to the given value
		inline void X(T x) noexcept
		{
			this->Values()[0] = x;
		}

		///@brief Sets the y component to the given value
		inline void Y(T y) noexcept
		{
			this->Values()[1] = y;
		}

		///@brief Sets the x and y components to the given values
		inline void XY(T x, T y) noexcept
		{
			X(x);
			Y(y);
		}

		///@}

		/**
			@name Observers
			@{
		*/

		///@brief Returns the x component
		[[nodiscard]] inline auto X() const noexcept
		{
			return this->Values()[0];
		}

		///@brief Returns the y component
		[[nodiscard]] inline auto Y() const noexcept
		{
			return this->Values()[1];
		}

		///@brief Returns both the x and y components
		[[nodiscard]] inline auto XY() const noexcept
		{
			return std::pair{X(), Y()};
		}

		///@}

		/**
			@name Operators
			@{
		*/

		///@brief Sets vec2 components to the given vector
		inline auto& operator=(const Vector2 &vector) noexcept
		{
			auto [x, y] = vector.XY();
			XY(static_cast<T>(x), static_cast<T>(y));
			return *this;
		}

		///@}
	};

	template <typename T>
	struct ValueAccessor<Vec<3, T>> : ValueAccessorBase<Vec<3, T>>
	{
		using ValueAccessorBase<Vec<3, T>>::ValueAccessorBase;


		/**
			@name Operators
			@{
		*/

		///@brief Sets all components to the given value
		inline auto& operator=(T value) noexcept
		{
			ValueAccessorBase<Vec<3, T>>::operator=(value);
			return *this;
		}

		///@brief Returns a modifiable reference to the component at the given offset
		[[nodiscard]] inline auto& operator[](int off) noexcept
		{
			assert(off >= 0 && off < 3);
			return this->Values()[off];
		}

		///@brief Returns the component at the given offset
		[[nodiscard]] inline auto operator[](int off) const noexcept
		{
			assert(off >= 0 && off < 3);
			return this->Values()[off];
		}

		///@}

		/**
			@name Modifiers
			@{
		*/

		///@brief Sets the x component to the given value
		inline void X(T x) noexcept
		{
			this->Values()[0] = x;
		}

		///@brief Sets the y component to the given value
		inline void Y(T y) noexcept
		{
			this->Values()[1] = y;
		}

		///@brief Sets the z component to the given value
		inline void Z(T z) noexcept
		{
			this->Values()[2] = z;
		}

		///@brief Sets the x, y and z components to the given values
		inline void XYZ(T x, T y, T z) noexcept
		{
			X(x);
			Y(y);
			Z(z);
		}

		///@}

		/**
			@name Observers
			@{
		*/

		///@brief Returns the x component
		[[nodiscard]] inline auto X() const noexcept
		{
			return this->Values()[0];
		}

		///@brief Returns the y component
		[[nodiscard]] inline auto Y() const noexcept
		{
			return this->Values()[1];
		}

		///@brief Returns the z component
		[[nodiscard]] inline auto Z() const noexcept
		{
			return this->Values()[2];
		}

		///@brief Returns the x, y and z components
		[[nodiscard]] inline auto XYZ() const noexcept
		{
			return std::tuple{X(), Y(), Z()};
		}

		///@}

		/**
			@name Operators
			@{
		*/

		///@brief Sets vec3 components to the given vector
		inline auto& operator=(const Vector3 &vector) noexcept
		{
			auto [x, y, z] = vector.XYZ();
			XYZ(static_cast<T>(x), static_cast<T>(y), static_cast<T>(z));
			return *this;
		}

		///@}
	};

	template <typename T>
	struct ValueAccessor<Vec<4, T>> : ValueAccessorBase<Vec<4, T>>
	{
		using ValueAccessorBase<Vec<4, T>>::ValueAccessorBase;


		/**
			@name Operators
			@{
		*/

		///@brief Sets all components to the given value
		inline auto& operator=(T value) noexcept
		{
			ValueAccessorBase<Vec<4, T>>::operator=(value);
			return *this;
		}

		///@brief Returns a modifiable reference to the component at the given offset
		[[nodiscard]] inline auto& operator[](int off) noexcept
		{
			assert(off >= 0 && off < 4);
			return this->Values()[off];
		}

		///@brief Returns the component at the given offset
		[[nodiscard]] inline auto operator[](int off) const noexcept
		{
			assert(off >= 0 && off < 4);
			return this->Values()[off];
		}

		///@}

		/**
			@name Modifiers
			@{
		*/

		///@brief Sets the x component to the given value
		inline void X(T x) noexcept
		{
			this->Values()[0] = x;
		}

		///@brief Sets the y component to the given value
		inline void Y(T y) noexcept
		{
			this->Values()[1] = y;
		}

		///@brief Sets the z component to the given value
		inline void Z(T z) noexcept
		{
			this->Values()[2] = z;
		}

		///@brief Sets the w component to the given value
		inline void W(T w) noexcept
		{
			this->Values()[3] = w;
		}

		///@brief Sets the x, y, z and w components to the given values
		inline void XYZW(T x, T y, T z, T w) noexcept
		{
			X(x);
			Y(y);
			Z(z);
			W(w);
		}

		///@}

		/**
			@name Observers
			@{
		*/

		///@brief Returns the x component
		[[nodiscard]] inline auto X() const noexcept
		{
			return this->Values()[0];
		}

		///@brief Returns the y component
		[[nodiscard]] inline auto Y() const noexcept
		{
			return this->Values()[1];
		}

		///@brief Returns the z component
		[[nodiscard]] inline auto Z() const noexcept
		{
			return this->Values()[2];
		}

		///@brief Returns the w component
		[[nodiscard]] inline auto W() const noexcept
		{
			return this->Values()[3];
		}


		///@brief Returns the x, y, z and w components
		[[nodiscard]] inline auto XYZW() const noexcept
		{
			return std::tuple{X(), Y(), Z(), W()};
		}

		///@}

		/**
			@name Operators
			@{
		*/

		///@brief Sets vec4 components to the given color
		inline auto& operator=(const Color &color) noexcept
		{
			auto [r, g, b, a] = color.RGBA();
			XYZW(static_cast<T>(r), static_cast<T>(g), static_cast<T>(b), static_cast<T>(a));
			return *this;
		}

		///@}
	};


	/*
		Matrix accessor
	*/

	template <typename T, int N, int M>
	struct ValueAccessor<Mat<N, M, T>> : ValueAccessorBase<Mat<N, M, T>>
	{
		using ValueAccessorBase<Mat<N, M, T>>::ValueAccessorBase;


		/**
			@name Operators
			@{
		*/

		///@brief Sets all components to the given value
		inline auto& operator=(T value) noexcept
		{
			ValueAccessorBase<Mat<N, M, T>>::operator=(value);
			return *this;
		}

		///@brief Sets matNxM elements to the given matrix
		template <typename Matrix,
			typename = std::enable_if_t<(N == 2 && M == 2 && std::is_same_v<Matrix, Matrix2>) ||
										(N == 3 && M == 3 && std::is_same_v<Matrix, Matrix3>) ||
										(N == 4 && M == 4 && std::is_same_v<Matrix, Matrix4>)>>
		inline auto& operator=(const Matrix &matrix) noexcept
		{
			for (auto i = 0; i < N; ++i)
			{
				for (auto j = 0; j < M; ++j)
					this->Values()[M * i + j] = static_cast<T>(matrix.M()[i][j]);
			}

			return *this;
		}

		///@brief Returns the column values as vecM at the given column offset
		[[nodiscard]] inline auto operator[](int column_off) noexcept
		{
			assert(column_off >= 0 && column_off < N);
			return ValueAccessor<Vec<M, T>>{this->Values() + M * column_off};
		}

		///@}
	};


	/*
		Shader value
	*/

	template <typename T>
	struct ShaderValue
	{
		using basic_type = basic_type_t<T>;


		/**
			@name Observers
			@{
		*/

		///@brief Returns the number of components in the values stored
		[[nodiscard]] constexpr auto Components() const noexcept
		{
			return type_components_v<T>;
		}

		///@}
	};


	/*
		Attribute value
	*/

	template <typename T>
	class AttributeValue : public ShaderValue<T>
	{
		private:

			void *pointer_ = nullptr;
			int stride_ = 0;
			bool normalized_ = false;

		public:

			///@brief Default constructor
			AttributeValue() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Returns the vertex attribute at the given offset
			[[nodiscard]] inline auto operator[](int vertex_off) noexcept
			{
				auto stride =
					stride_ == 0 ?
					//Vertex attributes are tightly packed
					static_cast<int>(sizeof(basic_type_t<T>)) :
					//User defined stride
					stride_;

				return ValueAccessor<T>{
					reinterpret_cast<basic_type_t<T>*>(
						reinterpret_cast<std::byte*>(pointer_) + vertex_off * stride
					), 0};
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the vertex pointer, stride between consecutive vertex attributes and whether or not data values should be normalized
			inline void Pointer(void *pointer, int stride = 0, bool normalized = false) noexcept
			{
				assert(stride >= 0);
				pointer_ = pointer;
				stride_ = stride;
				normalized_ = normalized;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns a pointer to the first vertex attribute
			[[nodiscard]] inline auto Pointer() const noexcept
			{
				return pointer_;
			}


			///@brief Returns the stride (byte offset) between consecutive vertex attributes
			[[nodiscard]] inline auto Stride() const noexcept
			{
				return stride_;
			}

		
			///@brief Returns true if vertex attribute data values should be normalized
			[[nodiscard]] inline auto Normalized() const noexcept
			{
				return normalized_;
			}

			///@}
	};


	/*
		Uniform value
	*/

	template <typename T>
	class UniformValue : public ShaderValue<T>, public ValueAccessor<T>
	{
		private:

			const int size_ = 1;

			detail::sbo_container_type<basic_type_t<T>, type_components_v<T>> stack_values_{}; //SBO (small buffer optimization)
			detail::container_type<basic_type_t<T>> heap_values_; //Heap allocated if size is greater than 1
			basic_type_t<T> *external_values_ = nullptr;


			inline auto Data() noexcept
			{
				return external_values_ ? external_values_ :
					size_ > 1 ? std::data(heap_values_) : std::data(stack_values_);
			}

		public:

			///@brief Constructor
			explicit UniformValue(int size = 1) :

				size_{size > 0 ? size : 1},
				heap_values_{size_ > 1 ?
					decltype(heap_values_)(type_components_v<T> * size_, basic_type_t<T>{}) :
					decltype(heap_values_){}}
			{
				ValueAccessor<T>::values_ = Data();
			}

			///@brief Copy constructor
			UniformValue(const UniformValue &rhs) :

				size_{rhs.size_},
				stack_values_{rhs.stack_values_},
				heap_values_{rhs.heap_values_},
				external_values_{rhs.external_values_}
			{
				ValueAccessor<T>::values_ = Data();
			}

			///@brief Move constructor
			UniformValue(UniformValue &&rhs) noexcept :

				size_{rhs.size_},
				stack_values_{std::move(rhs.stack_values_)},
				heap_values_{std::move(rhs.heap_values_)},
				external_values_{rhs.external_values_}
			{
				ValueAccessor<T>::values_ = Data();
			}


			/**
				@name Operators
				@{
			*/

			///@brief Copy assignment
			inline auto& operator=(const UniformValue &rhs) noexcept
			{
				if (size_ == rhs.size_)
				{
					stack_values_ = rhs.stack_values_;
					heap_values_ = rhs.heap_values_;
					external_values_ = rhs.external_values_;
				}

				return *this;
			}

			///@brief Sets all components to the given value
			inline auto& operator=(basic_type_t<T> value) noexcept
			{
				ValueAccessor<T>::operator=(value);
				return *this;
			}

			///@brief Sets all components to the given value
			template <typename U>
			inline auto& operator=(const U &value) noexcept
			{
				ValueAccessor<T>::operator=(value);
				return *this;
			}

			///@brief Returns the uniform value at the given (array) offset
			[[nodiscard]] inline auto At(int off) noexcept
			{
				assert(off < size_);
				return ValueAccessor<T>{Data(), off};
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the (array) size of the uniform
			[[nodiscard]] inline auto Size() const noexcept
			{
				return size_;
			}

			///@}
	};


	/**
		@name Type aliases
		@{
	*/

	using bvec2 = Vec<2, bool>;
	using ivec2 = Vec<2, int32>;
	using uvec2 = Vec<2, uint32>;
	using vec2 = Vec<2>;
	using dvec2 = Vec<2, float64>;

	using bvec3 = Vec<3, bool>;
	using ivec3 = Vec<3, int32>;
	using uvec3 = Vec<3, uint32>;
	using vec3 = Vec<3>;
	using dvec3 = Vec<3, float64>;

	using bvec4 = Vec<4, bool>;
	using ivec4 = Vec<4, int32>;
	using uvec4 = Vec<4, uint32>;
	using vec4 = Vec<4>;
	using dvec4 = Vec<4, float64>;
	
	using mat2x2 = Mat<2, 2>;
	using dmat2x2 = Mat<2, 2, float64>;

	using mat2x3 = Mat<2, 3>;
	using dmat2x3 = Mat<2, 3, float64>;

	using mat2x4 = Mat<2, 4>;
	using dmat2x4 = Mat<2, 4, float64>;

	using mat3x2 = Mat<3, 2>;
	using dmat3x2 = Mat<3, 2, float64>;

	using mat3x3 = Mat<3, 3>;
	using dmat3x3 = Mat<3, 3, float64>;

	using mat3x4 = Mat<3, 4>;
	using dmat3x4 = Mat<3, 4, float64>;

	using mat4x2 = Mat<4, 2>;
	using dmat4x2 = Mat<4, 2, float64>;

	using mat4x3 = Mat<4, 3>;
	using dmat4x3 = Mat<4, 3, float64>;

	using mat4x4 = Mat<4, 4>;
	using dmat4x4 = Mat<4, 4, float64>;

	using isampler1D = Sampler1D<int32>;
	using usampler1D = Sampler1D<uint32>;
	using sampler1D = Sampler1D<>;

	using isampler1DArray = Sampler1DArray<int32>;
	using usampler1DArray = Sampler1DArray<uint32>;
	using sampler1DArray = Sampler1DArray<>;

	using isampler2D = Sampler2D<int32>;
	using usampler2D = Sampler2D<uint32>;
	using sampler2D = Sampler2D<>;

	using isampler2DArray = Sampler2DArray<int32>;
	using usampler2DArray = Sampler2DArray<uint32>;
	using sampler2DArray = Sampler2DArray<>;

	///@}

	/**
		@name Shorthand matrix type aliases
		@{
	*/

	using mat2 = mat2x2;
	using dmat2 = dmat2x2;

	using mat3 = mat3x3;
	using dmat3 = dmat3x3;

	using mat4 = mat4x4;
	using dmat4 = dmat4x4;

	///@}

	/**
		@name Storage qualifier type aliases
		@{
	*/

	template <typename T>
	using uniform = UniformValue<T>;

	template <typename T>
	using attribute = AttributeValue<T>;

	///@}
} //ion::graphics::shaders::variables::glsl

#endif