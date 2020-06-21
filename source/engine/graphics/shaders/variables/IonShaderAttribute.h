/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/variables
File:	IonShaderAttribute.h
-------------------------------------------
*/

#ifndef ION_SHADER_ATTRIBUTE_H
#define ION_SHADER_ATTRIBUTE_H

#include <optional>
#include <type_traits>
#include <variant>

#include "IonShaderTypes.h"
#include "IonShaderVariable.h"
#include "types/IonTypes.h"
#include "types/IonTypeTraits.h"

namespace ion::graphics::shaders::variables
{
	namespace attribute_variable
	{
		using VariableType =
			std::variant<
				//Scalar types
				glsl::attribute<bool>,
				glsl::attribute<int32>,
				glsl::attribute<uint32>,
				glsl::attribute<float32>,
				glsl::attribute<float64>,
				
				//2-component vector types
				glsl::attribute<glsl::bvec2>,
				glsl::attribute<glsl::ivec2>,
				glsl::attribute<glsl::uvec2>,
				glsl::attribute<glsl::vec2>,
				glsl::attribute<glsl::dvec2>,
				
				//3-component vector types
				glsl::attribute<glsl::bvec3>,
				glsl::attribute<glsl::ivec3>,
				glsl::attribute<glsl::uvec3>,
				glsl::attribute<glsl::vec3>,
				glsl::attribute<glsl::dvec3>,
				
				//4-component vector types
				glsl::attribute<glsl::bvec4>,
				glsl::attribute<glsl::ivec4>,
				glsl::attribute<glsl::uvec4>,
				glsl::attribute<glsl::vec4>,
				glsl::attribute<glsl::dvec4>,
				
				//2x2 matrix types
				glsl::attribute<glsl::bmat2x2>,
				glsl::attribute<glsl::imat2x2>,
				glsl::attribute<glsl::umat2x2>,
				glsl::attribute<glsl::mat2x2>,
				glsl::attribute<glsl::dmat2x2>,

				//2x3 matrix types
				glsl::attribute<glsl::bmat2x3>,
				glsl::attribute<glsl::imat2x3>,
				glsl::attribute<glsl::umat2x3>,
				glsl::attribute<glsl::mat2x3>,
				glsl::attribute<glsl::dmat2x3>,

				//2x4 matrix types
				glsl::attribute<glsl::bmat2x4>,
				glsl::attribute<glsl::imat2x4>,
				glsl::attribute<glsl::umat2x4>,
				glsl::attribute<glsl::mat2x4>,
				glsl::attribute<glsl::dmat2x4>,

				//3x2 matrix types
				glsl::attribute<glsl::bmat3x2>,
				glsl::attribute<glsl::imat3x2>,
				glsl::attribute<glsl::umat3x2>,
				glsl::attribute<glsl::mat3x2>,
				glsl::attribute<glsl::dmat3x2>,

				//3x3 matrix types
				glsl::attribute<glsl::bmat3x3>,
				glsl::attribute<glsl::imat3x3>,
				glsl::attribute<glsl::umat3x3>,
				glsl::attribute<glsl::mat3x3>,
				glsl::attribute<glsl::dmat3x3>,

				//3x4 matrix types
				glsl::attribute<glsl::bmat3x4>,
				glsl::attribute<glsl::imat3x4>,
				glsl::attribute<glsl::umat3x4>,
				glsl::attribute<glsl::mat3x4>,
				glsl::attribute<glsl::dmat3x4>,
				
				//4x2 matrix types
				glsl::attribute<glsl::bmat4x2>,
				glsl::attribute<glsl::imat4x2>,
				glsl::attribute<glsl::umat4x2>,
				glsl::attribute<glsl::mat4x2>,
				glsl::attribute<glsl::dmat4x2>,

				//4x3 matrix types
				glsl::attribute<glsl::bmat4x3>,
				glsl::attribute<glsl::imat4x3>,
				glsl::attribute<glsl::umat4x3>,
				glsl::attribute<glsl::mat4x3>,
				glsl::attribute<glsl::dmat4x3>,

				//4x4 matrix types
				glsl::attribute<glsl::bmat4x4>,
				glsl::attribute<glsl::imat4x4>,
				glsl::attribute<glsl::umat4x4>,
				glsl::attribute<glsl::mat4x4>,
				glsl::attribute<glsl::dmat4x4>>;

		namespace detail
		{
			template <typename T>
			auto inline is_value_different(const glsl::attribute<T> &lhs, const glsl::attribute<T> &rhs) noexcept
			{
				return lhs.VertexData() != rhs.VertexData() ||
					   lhs.Stride() != rhs.Stride() ||
					   lhs.Normalized() != rhs.Normalized();
			}
		} //detail
	} //attribute_variable


	class AttributeVariable : public ShaderVariable
	{
		protected:

			attribute_variable::VariableType value_;
			std::optional<attribute_variable::VariableType> current_value_;

		public:

			//Constructor
			AttributeVariable(std::string name, attribute_variable::VariableType value) :
				ShaderVariable{std::move(name)},
				value_{std::move(value)}
			{
				//Empty
			}

			//Default virtual destructor
			virtual ~AttributeVariable() = default;


			/*
				Visit
			*/

			//Mutable value visit
			//Call the correct overload for the given overload set, based on the value of the attribute variable
			template <typename T, typename ...Ts>
			inline auto Visit(T &&callable, Ts &&...callables) noexcept
			{
				return std::visit(types::overloaded{std::forward<T>(callable), std::forward<Ts>(callables)...}, value_);
			}

			//Immutable value visit
			//Call the correct overload for the given overload set, based on the value of the attribute variable
			template <typename T, typename ...Ts>
			inline auto Visit(T &&callable, Ts &&...callables) const noexcept
			{
				return std::visit(types::overloaded{std::forward<T>(callable), std::forward<Ts>(callables)...}, value_);
			}


			/*
				Values
			*/

			//Returns true if the attribute variable value has changed
			[[nodiscard]] inline auto HasNewValue() noexcept
			{
				auto changed = 
					current_value_ ?
						Visit(
							[&](auto &&value) noexcept
							{
								return attribute_variable::detail::is_value_different(
									value,
									std::get<std::remove_cvref_t<decltype(value)>>(*current_value_));
							}) :
						true;

				if (changed)
					current_value_ = value_;
				
				return changed;
			}

			//Force the attribute value to be refreshed next time it is processed
			inline void Refresh()
			{
				current_value_.reset();
			}
	};


	//Class provides a more naturally way of initialize an attribute variable
	template <typename T>
	struct Attribute final : AttributeVariable
	{
		static_assert(glsl::is_basic_type_v<T>);

		//Constructor
		explicit Attribute(std::string name) :
			AttributeVariable(std::move(name), glsl::attribute<T>{})
		{
			//Empty
		}


		/*
			Observers
		*/

		//Get a mutable reference to the contained glsl attribute value
		[[nodiscard]] inline auto& Get() noexcept
		{
			return std::get<glsl::attribute<T>>(value_);
		}

		//Get an immutable reference to the contained glsl attribute value
		[[nodiscard]] inline auto& Get() const noexcept
		{
			return std::get<glsl::attribute<T>>(value_);
		}
	};
} //ion::graphics::shaders::variables

#endif