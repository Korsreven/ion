/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderProgram.h
-------------------------------------------
*/

#ifndef ION_SHADER_PROGRAM_H
#define ION_SHADER_PROGRAM_H

#include <optional>

#include "IonShaderManager.h"
#include "managed/IonObjectManager.h"
#include "managed/IonObservedObject.h"
#include "resources/IonResource.h"
#include "variables/IonShaderAttribute.h"
#include "variables/IonShaderUniform.h"

namespace ion::graphics::shaders
{
	namespace shader_program::detail
	{
	} //shader::detail


	class ShaderProgramManager; //Forward declaration

	class ShaderProgram final :
		public resources::Resource<ShaderProgramManager>,
		public managed::ObjectManager<variables::AttributeVariable, ShaderProgram>,
		public managed::ObjectManager<variables::UniformVariable, ShaderProgram>
	{
		private:

			using AttributeVariablesBase = managed::ObjectManager<variables::AttributeVariable, ShaderProgram>;
			using UniformVariablesBase = managed::ObjectManager<variables::UniformVariable, ShaderProgram>;


			std::optional<int> handle_;
			managed::ObservedObject<Shader> vertex_shader_;
			managed::ObservedObject<Shader> fragment_shader_;

		public:
			
			//Constructs a new shader program with the given name and a shader
			ShaderProgram(std::string name, Shader &shader);

			//Constructs a new shader program with the given name, a vertex and fragment shader
			ShaderProgram(std::string name, Shader &vertex_shader, Shader &fragment_shader);


			/*
				Managers
			*/

			//Return a mutable reference to the attribute variable manager of this shader program
			[[nodiscard]] inline auto& AttributeVariableManager() noexcept
			{
				return static_cast<AttributeVariablesBase&>(*this);
			}

			//Return a immutable reference to the attribute variable manager of this shader program
			[[nodiscard]] inline auto& AttributeVariableManager() const noexcept
			{
				return static_cast<const AttributeVariablesBase&>(*this);
			}


			//Return a mutable reference to the uniform variable manager of this shader program
			[[nodiscard]] inline auto& UniformVariableManager() noexcept
			{
				return static_cast<UniformVariablesBase&>(*this);
			}

			//Return a immutable reference to the uniform variable manager of this shader program
			[[nodiscard]] inline auto& UniformVariableManager() const noexcept
			{
				return static_cast<const UniformVariablesBase&>(*this);
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all attribute variables in this shader program
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttributeVariables() noexcept
			{
				return AttributeVariablesBase::Objects();
			}

			//Returns an immutable range of all attribute variables in this shader program
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto AttributeVariables() const noexcept
			{
				return AttributeVariablesBase::Objects();
			}


			//Returns a mutable range of all uniform variables in this shader program
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto UniformVariables() noexcept
			{
				return UniformVariablesBase::Objects();
			}

			//Returns an immutable range of all uniform variables in this shader program
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto UniformVariables() const noexcept
			{
				return UniformVariablesBase::Objects();
			}


			/*
				Modifiers
			*/

			//Sets the handle for the shader program to the given value
			inline void Handle(std::optional<int> handle) noexcept
			{
				handle_ = handle;
			}


			//Attach the given vertex shader to the shader program
			void VertexShader(Shader &shader);

			//Detach the vertex shader from the shader program
			void VertexShader(std::nullptr_t) noexcept;


			//Attach the given fragment shader to the shader program
			void FragmentShader(Shader &shader);

			//Detach the fragment shader from the shader program
			void FragmentShader(std::nullptr_t) noexcept;


			/*
				Observers
			*/

			//Returns the handle to the shader program
			//Returns nullopt if the shader program is not loaded
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}


			//Returns the attached vertex shader
			//Returns nullptr if no vertex shader is attached
			[[nodiscard]] inline auto VertexShader() const noexcept
			{
				return vertex_shader_.Object();
			}

			//Returns the attached fragment shader
			//Returns nullptr if no fragment shader is attached
			[[nodiscard]] inline auto FragmentShader() const noexcept
			{
				return fragment_shader_.Object();
			}


			/*
				Attribute variables
			*/

			//Create an attribute variable with the given name
			template <typename T>
			auto& CreateAttribute(std::string name)
			{
				auto &var = AttributeVariablesBase::Create(variables::Attribute<T>{std::move(name)});
				return static_cast<variables::Attribute<T>&>(var);
			}


			//Clear all removable attribute variables from this shader program
			void ClearAttributes() noexcept;

			//Remove a removable attribute variable from this shader program
			bool RemoveAttribute(variables::AttributeVariable &attribute_variable) noexcept;


			/*
				Uniform variables
			*/

			//Create a uniform variable with the given name and size
			template <typename T>
			auto& CreateUniform(std::string name, int size = 1)
			{
				auto &var = UniformVariablesBase::Create(variables::Uniform<T>{std::move(name), size});
				return static_cast<variables::Uniform<T>&>(var);
			}


			//Clear all removable uniform variables from this shader program
			void ClearUniforms() noexcept;

			//Remove a removable uniform variable from this shader program
			bool RemoveUniform(variables::UniformVariable &uniform_variable) noexcept;


			/*
				Variables
			*/

			//Clear all removable attributes and uniform variables from this shader program
			void ClearVariables() noexcept;
	};
} //ion::graphics::shaders

#endif