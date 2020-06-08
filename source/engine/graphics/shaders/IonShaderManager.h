/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderManager.h
-------------------------------------------
*/

#ifndef ION_SHADER_MANAGER_H
#define ION_SHADER_MANAGER_H

#include <filesystem>
#include <optional>
#include <string>

#include "IonShader.h"
#include "resources/files/IonFileResourceManager.h"
#include "resources/files/repositories/IonShaderRepository.h"

namespace ion::graphics::shaders
{
	namespace shader_manager
	{
		enum class InfoLogLevel : bool
		{
			Warning,
			Error
		};

		namespace detail
		{
			std::optional<int> load_shader(shader::ShaderType shader_type, const std::string &shader_source,
				const std::filesystem::path &shader_source_path, std::optional<InfoLogLevel> log_level) noexcept;
			void unload_shader(int shader_handle) noexcept;

			std::optional<std::string> print_info_log(int shader_handle);
		} //detail
	} //shader_manager


	class ShaderManager final :
		public resources::files::FileResourceManager<Shader, ShaderManager, resources::files::repositories::ShaderRepository>
	{
		private:

			std::optional<shader_manager::InfoLogLevel> log_level_;

		protected:

			/*
				Events
			*/

			bool LoadResource(Shader &shader) noexcept override;
			bool UnloadResource(Shader &shader) noexcept override;

		public:

			//Default constructor
			ShaderManager() = default;

			//Deleted copy constructor
			ShaderManager(const ShaderManager&) = delete;

			//Default move constructor
			ShaderManager(ShaderManager&&) = default;

			//Destructor
			~ShaderManager() noexcept;


			/*
				Operators
			*/

			//Deleted copy assignment
			ShaderManager& operator=(const ShaderManager&) = delete;

			//Move assignment
			ShaderManager& operator=(ShaderManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all shaders in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Shaders() noexcept
			{
				return this->Resources();
			}

			//Returns an immutable range of all shaders in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Shaders() const noexcept
			{
				return this->Resources();
			}


			/*
				Modifiers
			*/

			//Sets info log level for the shader manager
			//Pass nullopt or {} to turn off any shader info log
			inline void LogLevel(std::optional<shader_manager::InfoLogLevel> log_level) noexcept
			{
				log_level_ = log_level;
			}


			/*
				Observers
			*/

			//Returns info log level for the shader manager
			//Returns nullopt if shader info log is turned off
			[[nodiscard]] inline auto LogLevel() const noexcept
			{
				return log_level_;
			}


			/*
				Shaders
				Creating
			*/

			//Create a shader with the given name
			Shader& CreateShader(std::string name);


			/*
				Shaders
				Removing
			*/

			//Clear all removable shaders from this manager
			void ClearShaders() noexcept;

			//Remove a removable shader from this manager
			bool RemoveShader(Shader &shader) noexcept;


			/*
				Outputting
			*/

			//Prints the info log (if any) from the given shader
			//Returns nullopt if shader is not valid or loaded
			[[nodiscard]] std::optional<std::string> PrintInfoLog(const Shader &shader) const;
	};
} //ion::graphics::shaders

#endif