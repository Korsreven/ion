/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderProgramManager.h
-------------------------------------------
*/

#ifndef ION_SHADER_PROGRAM_MANAGER_H
#define ION_SHADER_PROGRAM_MANAGER_H

#include <filesystem>
#include <optional>
#include <string>

#include "IonShaderProgram.h"
#include "resources/IonResourceManager.h"

namespace ion::graphics::shaders
{
	namespace shader_program_manager
	{
		enum class InfoLogLevel : bool
		{
			Warning,
			Error
		};

		namespace detail
		{
			std::optional<int> load_shader_program(std::optional<int> vertex_shader_handle, std::optional<int> fragment_shader_handle,
				const std::filesystem::path &shader_source_path, std::optional<InfoLogLevel> log_level) noexcept;
			void unload_shader_program(int shader_program_handle) noexcept;

			std::optional<std::string> print_info_log(int shader_program_handle);
		} //detail
	} //shader_manager


	class ShaderProgramManager final :
		public resources::ResourceManager<ShaderProgram, ShaderProgramManager>
	{
		private:

			std::optional<shader_program_manager::InfoLogLevel> log_level_;

		protected:

			/*
				Events
			*/

			bool LoadResource(ShaderProgram &shader) noexcept override;
			bool UnloadResource(ShaderProgram &shader) noexcept override;

		public:

			//Default constructor
			ShaderProgramManager() = default;

			//Deleted copy constructor
			ShaderProgramManager(const ShaderProgramManager&) = delete;

			//Default move constructor
			ShaderProgramManager(ShaderProgramManager&&) = default;

			//Destructor
			~ShaderProgramManager() noexcept;


			/*
				Operators
			*/

			//Deleted copy assignment
			ShaderProgramManager& operator=(const ShaderProgramManager&) = delete;

			//Move assignment
			ShaderProgramManager& operator=(ShaderProgramManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all shader programs in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ShaderPrograms() noexcept
			{
				return this->Resources();
			}

			//Returns an immutable range of all shader programs in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto ShaderPrograms() const noexcept
			{
				return this->Resources();
			}


			/*
				Modifiers
			*/

			//Sets info log level for the shader program manager
			//Pass nullopt or {} to turn off any shader info log
			inline void LogLevel(std::optional<shader_program_manager::InfoLogLevel> log_level) noexcept
			{
				log_level_ = log_level;
			}


			/*
				Observers
			*/

			//Returns info log level for the shader program manager
			//Returns nullopt if shader info log is turned off
			[[nodiscard]] inline auto LogLevel() const noexcept
			{
				return log_level_;
			}


			/*
				Shader programs
				Creating
			*/

			//Create a shader program with the given shader
			ShaderProgram& CreateShaderProgram(Shader &shader);

			//Create a shader program with the given vertex and fragment shader
			ShaderProgram& CreateShaderProgram(Shader &vertex_shader, Shader &fragment_shader);


			/*
				Shader programs
				Removing
			*/

			//Clear all removable shader programs from this manager
			void ClearShaderPrograms() noexcept;

			//Remove a removable shader program from this manager
			bool RemoveShaderProgram(ShaderProgram &shader_program) noexcept;


			/*
				Outputting
			*/

			//Prints the info log (if any) from the given shader program
			//Returns nullopt if shader program is not valid or loaded
			[[nodiscard]] std::optional<std::string> PrintInfoLog(const ShaderProgram &shader_program) const;
	};
} //ion::graphics::shaders

#endif