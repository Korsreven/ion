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
#include "IonShaderStruct.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonResource.h"
#include "variables/IonShaderAttribute.h"
#include "variables/IonShaderTypes.h"
#include "variables/IonShaderUniform.h"

namespace ion::graphics::shaders
{
	//Forward declarations
	class ShaderProgram;
	class ShaderProgramManager;

	namespace shader_program::detail
	{
		using mapped_structs = std::array<NonOwningPtr<ShaderStruct>, shader_layout::detail::struct_name_count>;
		using mapped_attributes = std::array<NonOwningPtr<variables::AttributeVariable>, shader_layout::detail::attribute_name_count>;
		using mapped_uniforms = std::array<NonOwningPtr<variables::UniformVariable>, shader_layout::detail::uniform_name_count>;


		void remap_struct(NonOwningPtr<ShaderStruct> shader_struct, ShaderLayout &shader_layout, mapped_structs &shader_structs) noexcept;
		void remap_attribute(NonOwningPtr<variables::AttributeVariable> attribute_variable, ShaderLayout &shader_layout, mapped_attributes &attributes) noexcept;
		void remap_uniform(NonOwningPtr<variables::UniformVariable> uniform_variable, ShaderLayout &shader_layout, mapped_uniforms &uniforms) noexcept;

		int get_next_texture_unit(int &next_texture_unit) noexcept;


		template <typename T>
		struct create_attribute_helper
		{
			T &owner_;
			std::string name_;

			template <typename U>
			inline void operator()(const variables::glsl::attribute<U>&) const noexcept
			{
				owner_.CreateAttribute<U>(std::move(name_));
			}
		};

		template <typename T>
		struct create_uniform_helper
		{
			T &owner_;
			std::string name_;

			template <typename U>
			inline void operator()(const variables::glsl::uniform<U> &value) const noexcept
			{
				owner_.CreateUniform<U>(std::move(name_), value.Size());
			}
		};
	} //shader_program::detail


	///@brief A class representing a shader program that contains a vertex and fragment shader
	///@details A shader program also contains attribute and uniform variables, as well as a shader layout
	class ShaderProgram final :
		public resources::Resource<ShaderProgramManager>,
		public managed::ObjectManager<ShaderStruct, ShaderProgram>,
		public managed::ObjectManager<variables::AttributeVariable, ShaderProgram>,
		public managed::ObjectManager<variables::UniformVariable, ShaderProgram>		
	{
		friend class ShaderStruct;

		private:

			using ShaderStructBase = managed::ObjectManager<ShaderStruct, ShaderProgram>;
			using AttributeVariablesBase = managed::ObjectManager<variables::AttributeVariable, ShaderProgram>;
			using UniformVariablesBase = managed::ObjectManager<variables::UniformVariable, ShaderProgram>;


			std::optional<int> handle_;
			NonOwningPtr<Shader> vertex_shader_;
			NonOwningPtr<Shader> fragment_shader_;
			NonOwningPtr<ShaderLayout> shader_layout_;

			int next_texture_unit_ = 0;
			shader_program::detail::mapped_structs mapped_structs_;
			shader_program::detail::mapped_attributes mapped_attributes_;
			shader_program::detail::mapped_uniforms mapped_uniforms_;

		protected:

			/**
				@name Events
				@{
			*/

			void Created(ShaderStruct &shader_struct) noexcept override;
			void Created(variables::AttributeVariable &attribute_variable) noexcept override;
			void Created(variables::UniformVariable &uniform_variable) noexcept override;

			void Removed(ShaderStruct &shader_struct) noexcept override;
			void Removed(variables::AttributeVariable &attribute_variable) noexcept override;
			void Removed(variables::UniformVariable &uniform_variable) noexcept override;

			///@}

		public:
			
			///@brief Constructs a new shader program with the given name and shader
			ShaderProgram(std::string name, NonOwningPtr<Shader> shader) noexcept;

			///@brief Constructs a new shader program with the given name, shader and a user defined shader layout
			ShaderProgram(std::string name, NonOwningPtr<Shader> shader,
				NonOwningPtr<ShaderLayout> shader_layout) noexcept;

			///@brief Constructs a new shader program with the given name, vertex and fragment shader
			ShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader) noexcept;

			///@brief Constructs a new shader program with the given name, vertex and fragment shader and a user defined shader layout
			ShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader,
				NonOwningPtr<ShaderLayout> shader_layout) noexcept;

			///@brief Deleted copy constructor
			ShaderProgram(const ShaderProgram&) = delete;

			///@brief Default move constructor
			ShaderProgram(ShaderProgram&&) = default;

			///@brief Destructor
			~ShaderProgram() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			ShaderProgram& operator=(const ShaderProgram&) = delete;

			///@brief Default move assignment
			ShaderProgram& operator=(ShaderProgram&&) = default;

			///@}

			/**
				@name Managers
				@{
			*/

			///@brief Returns a mutable reference to the struct manager of this shader program
			[[nodiscard]] inline auto& StructManager() noexcept
			{
				return static_cast<ShaderStructBase&>(*this);
			}

			///@brief Returns an immutable reference to the struct manager of this shader program
			[[nodiscard]] inline auto& StructManager() const noexcept
			{
				return static_cast<const ShaderStructBase&>(*this);
			}


			///@brief Returns a mutable reference to the attribute variable manager of this shader program
			[[nodiscard]] inline auto& AttributeVariableManager() noexcept
			{
				return static_cast<AttributeVariablesBase&>(*this);
			}

			///@brief Returns an immutable reference to the attribute variable manager of this shader program
			[[nodiscard]] inline auto& AttributeVariableManager() const noexcept
			{
				return static_cast<const AttributeVariablesBase&>(*this);
			}


			///@brief Returns a mutable reference to the uniform variable manager of this shader program
			[[nodiscard]] inline auto& UniformVariableManager() noexcept
			{
				return static_cast<UniformVariablesBase&>(*this);
			}

			///@brief Returns an immutable reference to the uniform variable manager of this shader program
			[[nodiscard]] inline auto& UniformVariableManager() const noexcept
			{
				return static_cast<const UniformVariablesBase&>(*this);
			}

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all structs in this shader program
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Structs() noexcept
			{
				return ShaderStructBase::Objects();
			}

			///@brief Returns an immutable range of all structs in this shader program
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Structs() const noexcept
			{
				return ShaderStructBase::Objects();
			}


			///@brief Returns a mutable range of all attribute variables in this shader program
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttributeVariables() noexcept
			{
				return AttributeVariablesBase::Objects();
			}

			///@brief Returns an immutable range of all attribute variables in this shader program
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttributeVariables() const noexcept
			{
				return AttributeVariablesBase::Objects();
			}


			///@brief Returns a mutable range of all uniform variables in this shader program
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto UniformVariables() noexcept
			{
				return UniformVariablesBase::Objects();
			}

			///@brief Returns an immutable range of all uniform variables in this shader program
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto UniformVariables() const noexcept
			{
				return UniformVariablesBase::Objects();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the handle for the shader program to the given value
			inline void Handle(std::optional<int> handle) noexcept
			{
				handle_ = handle;
			}


			///@brief Attaches the given vertex shader to the shader program
			void VertexShader(NonOwningPtr<Shader> shader) noexcept;

			///@brief Attaches the given fragment shader to the shader program
			void FragmentShader(NonOwningPtr<Shader> shader) noexcept;

			///@brief Uses the given shader layout for mapping variables in this shader program
			void Layout(NonOwningPtr<ShaderLayout> shader_layout) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the handle to the shader program
			///@details Returns nullopt if the shader program is not loaded
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}


			///@brief Returns the attached vertex shader
			///@details Returns nullptr if no vertex shader is attached
			[[nodiscard]] inline auto VertexShader() const noexcept
			{
				return vertex_shader_;
			}

			///@brief Returns the attached fragment shader
			///@details Returns nullptr if no fragment shader is attached
			[[nodiscard]] inline auto FragmentShader() const noexcept
			{
				return fragment_shader_;
			}

			///@brief Returns the shader layout used by this shader program
			///@details Returns nullptr if no shader layout is used
			[[nodiscard]] inline auto Layout() const noexcept
			{
				return shader_layout_;
			}

			///@}

			/**
				@name Shader structs - Creating
				@{
			*/

			///@brief Creates a struct with the given name and size
			NonOwningPtr<ShaderStruct> CreateStruct(std::string name, int size = 1);


			///@brief Copies a struct from the given shader program with the given name
			void CopyStruct(const ShaderProgram &shader_program, std::string name);

			///@brief Copies all structs from the given shader program
			void CopyStructs(const ShaderProgram &shader_program);

			///@}

			/**
				@name Shader structs - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable struct with the given name
			///@details Returns nullptr if struct could not be found
			[[nodiscard]] NonOwningPtr<ShaderStruct> GetStruct(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable struct with the given name
			///@details Returns nullptr if struct could not be found
			[[nodiscard]] NonOwningPtr<const ShaderStruct> GetStruct(std::string_view name) const noexcept;


			///@brief Gets a pointer to a mutable struct that is mapped to the given standardized name
			///@details Returns nullptr if that standardized name has no mapped struct
			[[nodiscard]] NonOwningPtr<ShaderStruct> GetStruct(shader_layout::StructName name) noexcept;

			///@brief Gets a pointer to an immutable struct that is mapped to the given standardized name
			///@details Returns nullptr if that standardized name has no mapped struct
			[[nodiscard]] NonOwningPtr<const ShaderStruct> GetStruct(shader_layout::StructName name) const noexcept;

			///@}

			/**
				@name Shader structs - Removing
				@{
			*/

			///@brief Clears all removable structs from this shader program
			void ClearStructs() noexcept;

			///@brief Removes a removable struct from this shader program
			bool RemoveStruct(ShaderStruct &shader_struct) noexcept;

			///@brief Removes a removable structs with the given name from this manager
			bool RemoveStruct(std::string_view name) noexcept;

			///@}

			/**
				@name Attribute variables - Creating
				@{
			*/

			///@brief Creates an attribute variable with the given name
			template <typename T>
			auto CreateAttribute(std::string name)
			{
				auto ptr = AttributeVariablesBase::Create(variables::Attribute<T>{std::move(name)});
				return static_pointer_cast<variables::Attribute<T>>(ptr);
			}


			///@brief Creates an attribute variable as a copy of the given attribute
			template <typename T>
			auto CreateAttribute(const variables::Attribute<T> &attribute)
			{
				auto ptr = AttributeVariablesBase::Create(attribute);
				return static_pointer_cast<variables::Attribute<T>>(ptr);
			}

			///@brief Creates an attribute variable by moving the given attribute
			template <typename T>
			auto CreateAttribute(variables::Attribute<T> &&attribute)
			{
				auto ptr = AttributeVariablesBase::Create(std::move(attribute));
				return static_pointer_cast<variables::Attribute<T>>(ptr);
			}


			///@brief Copies all attribute variables from the given shader program
			void CopyAttributes(const ShaderProgram &shader_program);

			///@}

			/**
				@name Attribute variables - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable attribute variable with the given name
			///@details Returns nullptr if attribute variable could not be found
			[[nodiscard]] NonOwningPtr<variables::AttributeVariable> GetAttribute(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable attribute variable with the given name
			///@details Returns nullptr if attribute variable could not be found
			[[nodiscard]] NonOwningPtr<const variables::AttributeVariable> GetAttribute(std::string_view name) const noexcept;


			///@brief Gets a pointer to a mutable attribute variable that is mapped to the given standardized name
			///@details Returns nullptr if that standardized name has no mapped attribute variable
			[[nodiscard]] NonOwningPtr<variables::AttributeVariable> GetAttribute(shader_layout::AttributeName name) noexcept;

			///@brief Gets a pointer to an immutable attribute variable that is mapped to the given standardized name
			///@details Returns nullptr if that standardized name has no mapped attribute variable
			[[nodiscard]] NonOwningPtr<const variables::AttributeVariable> GetAttribute(shader_layout::AttributeName name) const noexcept;

			///@}

			/**
				@name Attribute variables - Removing
				@{
			*/

			///@brief Clears all removable attribute variables from this shader program
			void ClearAttributes() noexcept;

			///@brief Removes a removable attribute variable from this shader program
			bool RemoveAttribute(variables::AttributeVariable &attribute_variable) noexcept;

			///@brief Removes a removable attribute variable with the given name from this manager
			bool RemoveAttribute(std::string_view name) noexcept;

			///@}

			/**
				@name Uniform variables - Creating
				@{
			*/

			///@brief Creates a uniform variable with the given name and size
			template <typename T>
			auto CreateUniform(std::string name, int size = 1)
			{
				auto ptr = UniformVariablesBase::Create(variables::Uniform<T>{std::move(name), size});
				return static_pointer_cast<variables::Uniform<T>>(ptr);
			}


			///@brief Creates a uniform variable as a copy of the given uniform
			template <typename T>
			auto CreateUniform(const variables::Uniform<T> &uniform)
			{
				auto ptr = UniformVariablesBase::Create(uniform);
				return static_pointer_cast<variables::Uniform<T>>(ptr);
			}

			///@brief Creates a uniform variable by moving the given uniform
			template <typename T>
			auto CreateUniform(variables::Uniform<T> &&uniform)
			{
				auto ptr = UniformVariablesBase::Create(std::move(uniform));
				return static_pointer_cast<variables::Uniform<T>>(ptr);
			}


			///@brief Copies all uniform variables from the given shader program
			void CopyUniforms(const ShaderProgram &shader_program);

			///@}

			/**
				@name Uniform variables - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable uniform variable with the given name
			///@details Returns nullptr if uniform variable could not be found
			[[nodiscard]] NonOwningPtr<variables::UniformVariable> GetUniform(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable uniform variable with the given name
			///@details Returns nullptr if uniform variable could not be found
			[[nodiscard]] NonOwningPtr<const variables::UniformVariable> GetUniform(std::string_view name) const noexcept;


			///@brief Gets a pointer to a mutable uniform variable that is mapped to the given standardized name
			///@details Returns nullptr if that standardized name has no mapped uniform variable
			[[nodiscard]] NonOwningPtr<variables::UniformVariable> GetUniform(shader_layout::UniformName name) noexcept;

			///@brief Gets a pointer to an immutable uniform variable that is mapped to the given standardized name
			///@details Returns nullptr if that standardized name has no mapped uniform variable
			[[nodiscard]] NonOwningPtr<const variables::UniformVariable> GetUniform(shader_layout::UniformName name) const noexcept;

			///@}

			/**
				@name Uniform variables - Removing
				@{
			*/

			///@brief Clears all removable uniform variables from this shader program
			void ClearUniforms() noexcept;

			///@brief Removes a removable uniform variable from this shader program
			bool RemoveUniform(variables::UniformVariable &uniform_variable) noexcept;

			///@brief Removes a removable uniform variable with the given name from this manager
			bool RemoveUniform(std::string_view name) noexcept;

			///@}

			/**
				@name Variables - Removing
				@{
			*/

			///@brief Clears all removable attributes and uniform variables from this shader program
			void ClearVariables() noexcept;

			///@}

			/**
				@name Struct/variables - Removing
				@{
			*/

			///@brief Clears all removable structs, attributes and uniform variables from this shader program
			void Clear() noexcept;

			///@}
	};
} //ion::graphics::shaders

#endif