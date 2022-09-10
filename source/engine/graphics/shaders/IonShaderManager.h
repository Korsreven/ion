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
#include <string_view>

#include "IonShader.h"
#include "assets/repositories/IonShaderRepository.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonFileResourceManager.h"

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


	//A class that manages and stores shaders
	class ShaderManager final :
		public resources::FileResourceManager<Shader, ShaderManager, assets::repositories::ShaderRepository>
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
				return Resources();
			}

			//Returns an immutable range of all shaders in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Shaders() const noexcept
			{
				return Resources();
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

			//Create a shader with the given name and asset name
			NonOwningPtr<Shader> CreateShader(std::string name, std::string asset_name);


			//Create a shader as a copy of the given shader
			NonOwningPtr<Shader> CreateShader(const Shader &shader);

			//Create a shader by moving the given shader
			NonOwningPtr<Shader> CreateShader(Shader &&shader);


			/*
				Shaders
				Retrieving
			*/

			//Gets a pointer to a mutable shader with the given name
			//Returns nullptr if shader could not be found
			[[nodiscard]] NonOwningPtr<Shader> GetShader(std::string_view name) noexcept;

			//Gets a pointer to an immutable shader with the given name
			//Returns nullptr if shader could not be found
			[[nodiscard]] NonOwningPtr<const Shader> GetShader(std::string_view name) const noexcept;


			/*
				Shaders
				Removing
			*/

			//Clear all removable shaders from this manager
			void ClearShaders() noexcept;

			//Remove a removable shader from this manager
			bool RemoveShader(Shader &shader) noexcept;

			//Remove a removable shader with the given name from this manager
			bool RemoveShader(std::string_view name) noexcept;


			/*
				Outputting
			*/

			//Prints the info log (if any) from the given shader
			//Returns nullopt if shader is not valid or loaded
			[[nodiscard]] std::optional<std::string> PrintInfoLog(const Shader &shader) const;
	};
} //ion::graphics::shaders

#endif