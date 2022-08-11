/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/variables
File:	IonShaderUniform.h
-------------------------------------------
*/

#ifndef ION_SHADER_UNIFORM_H
#define ION_SHADER_UNIFORM_H

#include <optional>
#include <type_traits>
#include <variant>

#include "IonShaderTypes.h"
#include "IonShaderVariable.h"
#include "types/IonTypes.h"
#include "types/IonTypeTraits.h"

namespace ion::graphics::shaders
{
	class ShaderStruct; //Forward declaration
}

namespace ion::graphics::shaders::variables
{
	template <typename T>
	struct Uniform; //Forward declaration

	namespace uniform_variable
	{
		using VariableType =
			std::variant<
				//Scalar types
				glsl::uniform<bool>,
				glsl::uniform<int32>,
				glsl::uniform<uint32>,
				glsl::uniform<float32>,
				glsl::uniform<float64>,
				
				//2-component vector types
				glsl::uniform<glsl::bvec2>,
				glsl::uniform<glsl::ivec2>,
				glsl::uniform<glsl::uvec2>,
				glsl::uniform<glsl::vec2>,
				glsl::uniform<glsl::dvec2>,
				
				//3-component vector types
				glsl::uniform<glsl::bvec3>,
				glsl::uniform<glsl::ivec3>,
				glsl::uniform<glsl::uvec3>,
				glsl::uniform<glsl::vec3>,
				glsl::uniform<glsl::dvec3>,
				
				//4-component vector types
				glsl::uniform<glsl::bvec4>,
				glsl::uniform<glsl::ivec4>,
				glsl::uniform<glsl::uvec4>,
				glsl::uniform<glsl::vec4>,
				glsl::uniform<glsl::dvec4>,
				
				//2x2 matrix types
				glsl::uniform<glsl::mat2x2>,
				glsl::uniform<glsl::dmat2x2>,

				//2x3 matrix types
				glsl::uniform<glsl::mat2x3>,
				glsl::uniform<glsl::dmat2x3>,

				//2x4 matrix types
				glsl::uniform<glsl::mat2x4>,
				glsl::uniform<glsl::dmat2x4>,

				//3x2 matrix types
				glsl::uniform<glsl::mat3x2>,
				glsl::uniform<glsl::dmat3x2>,

				//3x3 matrix types
				glsl::uniform<glsl::mat3x3>,
				glsl::uniform<glsl::dmat3x3>,

				//3x4 matrix types
				glsl::uniform<glsl::mat3x4>,
				glsl::uniform<glsl::dmat3x4>,
				
				//4x2 matrix types
				glsl::uniform<glsl::mat4x2>,
				glsl::uniform<glsl::dmat4x2>,

				//4x3 matrix types
				glsl::uniform<glsl::mat4x3>,
				glsl::uniform<glsl::dmat4x3>,

				//4x4 matrix types
				glsl::uniform<glsl::mat4x4>,
				glsl::uniform<glsl::dmat4x4>,

				//1D sampler types
				glsl::uniform<glsl::isampler1D>,
				glsl::uniform<glsl::usampler1D>,
				glsl::uniform<glsl::sampler1D>,
				
				glsl::uniform<glsl::isampler1DArray>,
				glsl::uniform<glsl::usampler1DArray>,
				glsl::uniform<glsl::sampler1DArray>,
				
				//2D sampler types
				glsl::uniform<glsl::isampler2D>,
				glsl::uniform<glsl::usampler2D>,
				glsl::uniform<glsl::sampler2D>,
				
				glsl::uniform<glsl::isampler2DArray>,
				glsl::uniform<glsl::usampler2DArray>,
				glsl::uniform<glsl::sampler2DArray>>;

		namespace detail
		{
			template <typename T>
			inline auto is_value_different(const glsl::uniform<T> &lhs, const glsl::uniform<T> &rhs) noexcept
			{
				auto count = lhs.Components() * lhs.Size();

				for (auto i = 0; i < count; ++i)
				{
					if (lhs.Values()[i] != rhs.Values()[i])
						return true;
				}

				return false;
			}
		} //detail
	} //uniform_variable


	class UniformVariable : public ShaderVariable
	{
		private:

			ShaderStruct *parent_struct_ = nullptr;
			std::optional<int> member_offset_;

		protected:

			uniform_variable::VariableType value_;
			std::optional<uniform_variable::VariableType> current_value_;

		public:

			//Constructor
			UniformVariable(std::string name, uniform_variable::VariableType value);

			//Default virtual destructor
			virtual ~UniformVariable() = default;


			/*
				Operators
			*/

			//Returns a mutable member uniform variable at the given offset
			[[nodiscard]] UniformVariable& operator[](int off) noexcept;

			//Returns an immutable member uniform variable at the given offset
			[[nodiscard]] const UniformVariable& operator[](int off) const noexcept;


			/*
				Modifiers
			*/

			//Sets the parent struct of this member uniform variable
			inline void ParentStruct(ShaderStruct &shader_struct, int member_off) noexcept
			{
				parent_struct_ = &shader_struct;
				member_offset_ = member_off;
			}

			//Releases the parent struct of this member uniform variable
			inline void ParentStruct(std::nullptr_t) noexcept
			{
				parent_struct_ = nullptr;
				member_offset_ = {};
			}


			/*
				Observers
			*/

			//Returns the parent struct of this member uniform variable
			//Returns nullptr if this uniform variable is not a struct member
			[[nodiscard]] inline auto ParentStruct() const noexcept
			{
				return parent_struct_;
			}

			//Returns the member offset of this member uniform variable
			//Returns nullopt if this uniform variable is not a struct member
			[[nodiscard]] inline auto& MemberOffset() const noexcept
			{
				return member_offset_;
			}


			//Get a mutable reference to the contained glsl uniform value
			template <typename T, typename = std::enable_if_t<std::is_base_of_v<UniformVariable, Uniform<T>>>>
			[[nodiscard]] inline auto& Get() noexcept
			{
				return static_cast<Uniform<T>&>(*this).Get();
			}

			//Get an immutable reference to the contained glsl uniform value
			template <typename T, typename = std::enable_if_t<std::is_base_of_v<UniformVariable, Uniform<T>>>>
			[[nodiscard]] inline auto& Get() const noexcept
			{
				return static_cast<const Uniform<T>&>(*this).Get();
			}


			/*
				Visit
			*/

			//Mutable value visit
			//Call the correct overload for the given overload set, based on the value of the uniform variable
			template <typename T, typename ...Ts>
			inline auto Visit(T &&callable, Ts &&...callables) noexcept
			{
				return std::visit(types::overloaded{std::forward<T>(callable), std::forward<Ts>(callables)...}, value_);
			}

			//Immutable value visit
			//Call the correct overload for the given overload set, based on the value of the uniform variable
			template <typename T, typename ...Ts>
			inline auto Visit(T &&callable, Ts &&...callables) const noexcept
			{
				return std::visit(types::overloaded{std::forward<T>(callable), std::forward<Ts>(callables)...}, value_);
			}


			/*
				Values
			*/

			//Returns true if the uniform variable value has changed
			[[nodiscard]] bool HasNewValue() noexcept;

			//Force the uniform value to be refreshed next time it is processed
			void Refresh() noexcept;
	};


	//Class provides a more naturally way of initialize a uniform variable
	template <typename T>
	struct Uniform final : UniformVariable
	{
		static_assert(glsl::is_basic_type_v<T>);

		//Constructor
		explicit Uniform(std::string name, int size = 1) :
			UniformVariable(std::move(name), glsl::uniform<T>{size})
		{
			//Empty
		}


		/*
			Observers
		*/

		//Get a mutable reference to the contained glsl uniform value
		[[nodiscard]] inline auto& Get() noexcept
		{
			return std::get<glsl::uniform<T>>(value_);
		}

		//Get an immutable reference to the contained glsl uniform value
		[[nodiscard]] inline auto& Get() const noexcept
		{
			return std::get<glsl::uniform<T>>(value_);
		}
	};
	
} //ion::graphics::shaders::variables

#endif