/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderStruct.h
-------------------------------------------
*/

#ifndef ION_SHADER_STRUCT_H
#define ION_SHADER_STRUCT_H

#include <optional>
#include <string>
#include <string_view>

#include "IonShaderLayout.h"
#include "managed/IonObjectManager.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "variables/IonShaderUniform.h"

namespace ion::graphics::shaders
{
	class ShaderProgram; //Forward declaration

	namespace shader_struct::detail
	{
		using mapped_members = std::vector<NonOwningPtr<variables::UniformVariable>>;

		std::string get_fully_qualified_name(std::string_view struct_name, std::string_view variable_name, std::optional<int> off) noexcept;
	} //shader_struct::detail


	//A class representing a shader struct that can contain multiple uniform variables (members)
	class ShaderStruct final :
		public managed::ManagedObject<ShaderProgram>,
		public managed::ObjectManager<variables::UniformVariable, ShaderProgram>
	{
		private:

			const int size_ = 1;
			shader_struct::detail::mapped_members mapped_members_;

		protected:

			void Created(variables::UniformVariable &uniform_variable) noexcept override;
			void Removed(variables::UniformVariable &uniform_variable) noexcept override;

		public:
			
			//Constructs a new shader struct with the given name and size
			explicit ShaderStruct(std::string name, int size = 1) noexcept;

			//Deleted copy constructor
			ShaderStruct(const ShaderStruct&) = delete;

			//Default move constructor
			ShaderStruct(ShaderStruct&&) = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			ShaderStruct& operator=(const ShaderStruct&) = delete;

			//Default move assignment
			ShaderStruct& operator=(ShaderStruct&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all uniform variables in this shader struct
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto UniformVariables() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all uniform variables in this shader struct
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto UniformVariables() const noexcept
			{
				return Objects();
			}


			/*
				Observers
			*/

			//Returns the number of instances of this struct
			//For a single instance, the size returned will be 1
			//For multiple instances, the size returned will be the size of the array
			[[nodiscard]] inline auto Size() const noexcept
			{
				return size_;
			}


			/*
				Uniform variables
				Creating
			*/
			
			//Creates a uniform variable with the given name and size
			template <typename T>
			auto CreateUniform(std::string name, int size = 1)
			{
				auto ptr =
					Create(variables::Uniform<T>{
						shader_struct::detail::get_fully_qualified_name(
							*Name(), name, size_ > 1 ? std::make_optional(0) : std::nullopt),
						size
					});

				//For an array of struct instances
				for (auto i = 1; i < size_; ++i)
					Create(variables::Uniform<T>{
						shader_struct::detail::get_fully_qualified_name(*Name(), name, i),
						size
					});

				mapped_members_.push_back(ptr);
				return static_pointer_cast<variables::Uniform<T>>(ptr);
			}


			/*
				Uniform variables
				Retrieving
			*/

			//Gets a pointer to a mutable uniform variable with the given name
			//Returns nullptr if uniform variable could not be found
			[[nodiscard]] NonOwningPtr<variables::UniformVariable> GetUniform(std::string_view name, std::optional<int> off = {}) noexcept;

			//Gets a pointer to an immutable uniform variable with the given name
			//Returns nullptr if uniform variable could not be found
			[[nodiscard]] NonOwningPtr<const variables::UniformVariable> GetUniform(std::string_view name, std::optional<int> off = {}) const noexcept;


			/*
				Uniform variables
				Removing
			*/

			//Clears all removable uniform variables from this shader struct
			void ClearUniforms() noexcept;

			//Removes a removable uniform variable from this shader struct
			bool RemoveUniform(variables::UniformVariable &uniform_variable) noexcept;

			//Removes a removable uniform variable with the given name from this manager
			bool RemoveUniform(std::string_view name, std::optional<int> off = {}) noexcept;
	};
} //ion::graphics::shaders

#endif