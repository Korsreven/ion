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

#include <array>
#include <optional>
#include <string>
#include <string_view>

#include "IonShader.h"
#include "IonShaderLayout.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonResource.h"
#include "variables/IonShaderAttribute.h"
#include "variables/IonShaderUniform.h"

namespace ion::graphics::shaders
{
	namespace shader_program::detail
	{
		using mapped_attributes = std::array<NonOwningPtr<variables::AttributeVariable>, shader_layout::detail::attribute_name_count>;
		using mapped_uniforms = std::array<NonOwningPtr<variables::UniformVariable>, shader_layout::detail::uniform_name_count>;


		void remap_attribute(NonOwningPtr<variables::AttributeVariable> attribute_variable, ShaderLayout &shader_layout, mapped_attributes &attributes) noexcept;
		void remap_uniform(NonOwningPtr<variables::UniformVariable> uniform_variable, ShaderLayout &shader_layout, mapped_uniforms &uniforms) noexcept;

		int get_next_texture_unit(int &next_texture_unit) noexcept;
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
			NonOwningPtr<Shader> vertex_shader_;
			NonOwningPtr<Shader> fragment_shader_;
			NonOwningPtr<ShaderLayout> shader_layout_;

			int next_texture_unit_ = 0;
			shader_program::detail::mapped_attributes mapped_attributes_;
			shader_program::detail::mapped_uniforms mapped_uniforms_;

		protected:

			/*
				Events
			*/

			void Created(variables::AttributeVariable &attribute_variable) noexcept override;
			void Created(variables::UniformVariable &uniform_variable) noexcept override;

			void Removed(variables::AttributeVariable &attribute_variable) noexcept override;
			void Removed(variables::UniformVariable &uniform_variable) noexcept override;

		public:
			
			//Constructs a new shader program with the given name and shader
			ShaderProgram(std::string name, NonOwningPtr<Shader> shader);

			//Constructs a new shader program with the given name, shader and a user defined shader layout
			ShaderProgram(std::string name, NonOwningPtr<Shader> shader,
				NonOwningPtr<ShaderLayout> shader_layout);

			//Constructs a new shader program with the given name, vertex and fragment shader
			ShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader);

			//Constructs a new shader program with the given name, vertex and fragment shader and a user defined shader layout
			ShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader,
				NonOwningPtr<ShaderLayout> shader_layout);

			//Deleted copy constructor
			ShaderProgram(const ShaderProgram&) = delete;

			//Default move constructor
			ShaderProgram(ShaderProgram&&) = default;

			//Destructor
			~ShaderProgram() noexcept;


			/*
				Operators
			*/

			//Deleted copy assignment
			ShaderProgram& operator=(const ShaderProgram&) = delete;

			//Move assignment
			ShaderProgram& operator=(ShaderProgram&&) = default;


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
			[[nodiscard]] inline auto AttributeVariables() const noexcept
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
			[[nodiscard]] inline auto UniformVariables() const noexcept
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
			void VertexShader(NonOwningPtr<Shader> shader) noexcept;

			//Attach the given fragment shader to the shader program
			void FragmentShader(NonOwningPtr<Shader> shader) noexcept;

			//Use the given shader layout for mapping variables in this shader program
			void Layout(NonOwningPtr<ShaderLayout> shader_layout) noexcept;


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
				return vertex_shader_;
			}

			//Returns the attached fragment shader
			//Returns nullptr if no fragment shader is attached
			[[nodiscard]] inline auto FragmentShader() const noexcept
			{
				return fragment_shader_;
			}

			//Returns the shader layout used by this shader program
			//Returns nullptr if no shader layout is used
			[[nodiscard]] inline auto Layout() const noexcept
			{
				return shader_layout_;
			}


			/*
				Attribute variables
				Creating
			*/

			//Create an attribute variable with the given name
			template <typename T>
			auto CreateAttribute(std::string name)
			{
				auto ptr = AttributeVariablesBase::Create(variables::Attribute<T>{std::move(name)});
				return static_pointer_cast<variables::Attribute<T>>(ptr);
			}


			//Create an attribute variable as a copy of the given attribute
			template <typename T>
			auto CreateAttribute(const variables::Attribute<T> &attribute)
			{
				auto ptr = AttributeVariablesBase::Create(attribute);
				return static_pointer_cast<variables::Attribute<T>>(ptr);
			}

			//Create an attribute variable by moving the given attribute
			template <typename T>
			auto CreateAttribute(variables::Attribute<T> &&attribute)
			{
				auto ptr = AttributeVariablesBase::Create(std::move(attribute));
				return static_pointer_cast<variables::Attribute<T>>(ptr);
			}


			/*
				Attribute variables
				Retrieving
			*/

			//Gets a pointer to a mutable attribute variable with the given name
			//Returns nullptr if attribute variable could not be found
			[[nodiscard]] NonOwningPtr<variables::AttributeVariable> GetAttribute(std::string_view name) noexcept;

			//Gets a pointer to an immutable attribute variable with the given name
			//Returns nullptr if attribute variable could not be found
			[[nodiscard]] NonOwningPtr<const variables::AttributeVariable> GetAttribute(std::string_view name) const noexcept;


			//Gets a pointer to a mutable attribute variable that is mapped to the given standardized name
			//Returns nullptr if that standardized name has no mapped attribute variable
			[[nodiscard]] NonOwningPtr<variables::AttributeVariable> GetAttribute(shader_layout::AttributeName name) noexcept;

			//Gets a pointer to an immutable attribute variable that is mapped to the given standardized name
			//Returns nullptr if that standardized name has no mapped attribute variable
			[[nodiscard]] NonOwningPtr<const variables::AttributeVariable> GetAttribute(shader_layout::AttributeName name) const noexcept;


			/*
				Attribute variables
				Removing
			*/

			//Clear all removable attribute variables from this shader program
			void ClearAttributes() noexcept;

			//Remove a removable attribute variable from this shader program
			bool RemoveAttribute(variables::AttributeVariable &attribute_variable) noexcept;

			//Remove a removable attribute variable with the given name from this manager
			bool RemoveAttribute(std::string_view name) noexcept;


			/*
				Uniform variables
				Creating
			*/

			//Create a uniform variable with the given name and size
			template <typename T>
			auto CreateUniform(std::string name, int size = 1)
			{
				auto ptr = UniformVariablesBase::Create(variables::Uniform<T>{std::move(name), size});
				return static_pointer_cast<variables::Uniform<T>>(ptr);
			}


			//Create an uniform variable as a copy of the given uniform
			template <typename T>
			auto CreateUniform(const variables::Uniform<T> &uniform)
			{
				auto ptr = AttributeVariablesBase::Create(uniform);
				return static_pointer_cast<variables::Uniform<T>>(ptr);
			}

			//Create an uniform variable by moving the given uniform
			template <typename T>
			auto CreateUniform(variables::Uniform<T> &&uniform)
			{
				auto ptr = AttributeVariablesBase::Create(std::move(uniform));
				return static_pointer_cast<variables::Uniform<T>>(ptr);
			}


			/*
				Uniform variables
				Retrieving
			*/

			//Gets a pointer to a mutable uniform variable with the given name
			//Returns nullptr if uniform variable could not be found
			[[nodiscard]] NonOwningPtr<variables::UniformVariable> GetUniform(std::string_view name) noexcept;

			//Gets a pointer to an immutable uniform variable with the given name
			//Returns nullptr if uniform variable could not be found
			[[nodiscard]] NonOwningPtr<const variables::UniformVariable> GetUniform(std::string_view name) const noexcept;


			//Gets a pointer to a mutable uniform variable that is mapped to the given standardized name
			//Returns nullptr if that standardized name has no mapped uniform variable
			[[nodiscard]] NonOwningPtr<variables::UniformVariable> GetUniform(shader_layout::UniformName name) noexcept;

			//Gets a pointer to an immutable uniform variable that is mapped to the given standardized name
			//Returns nullptr if that standardized name has no mapped uniform variable
			[[nodiscard]] NonOwningPtr<const variables::UniformVariable> GetUniform(shader_layout::UniformName name) const noexcept;


			/*
				Uniform variables
				Removing
			*/

			//Clear all removable uniform variables from this shader program
			void ClearUniforms() noexcept;

			//Remove a removable uniform variable from this shader program
			bool RemoveUniform(variables::UniformVariable &uniform_variable) noexcept;

			//Remove a removable uniform variable with the given name from this manager
			bool RemoveUniform(std::string_view name) noexcept;


			/*
				Variables
				Removing
			*/

			//Clear all removable attributes and uniform variables from this shader program
			void ClearVariables() noexcept;
	};
} //ion::graphics::shaders

#endif