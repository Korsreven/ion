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


	///@brief A class that manages and stores shaders
	class ShaderManager final :
		public resources::FileResourceManager<Shader, ShaderManager, assets::repositories::ShaderRepository>
	{
		private:

			std::optional<shader_manager::InfoLogLevel> log_level_;

		protected:

			/**
				@name Events
				@{
			*/

			bool LoadResource(Shader &shader) override;
			bool UnloadResource(Shader &shader) noexcept override;

			///@}

		public:

			///@brief Default constructor
			ShaderManager() = default;

			///@brief Deleted copy constructor
			ShaderManager(const ShaderManager&) = delete;

			///@brief Default move constructor
			ShaderManager(ShaderManager&&) = default;

			///@brief Destructor
			~ShaderManager() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			ShaderManager& operator=(const ShaderManager&) = delete;

			///@brief Default move assignment
			ShaderManager& operator=(ShaderManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all shaders in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Shaders() noexcept
			{
				return Resources();
			}

			///@brief Returns an immutable range of all shaders in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Shaders() const noexcept
			{
				return Resources();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets info log level for the shader manager
			///@details Pass nullopt or {} to turn off any shader info log
			inline void LogLevel(std::optional<shader_manager::InfoLogLevel> log_level) noexcept
			{
				log_level_ = log_level;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns info log level for the shader manager
			///@details Returns nullopt if shader info log is turned off
			[[nodiscard]] inline auto LogLevel() const noexcept
			{
				return log_level_;
			}

			///@}

			/**
				@name Shaders - Creating
				@{
			*/

			///@brief Creates a shader with the given name and asset name
			NonOwningPtr<Shader> CreateShader(std::string name, std::string asset_name);


			///@brief Creates a shader as a copy of the given shader
			NonOwningPtr<Shader> CreateShader(const Shader &shader);

			///@brief Creates a shader by moving the given shader
			NonOwningPtr<Shader> CreateShader(Shader &&shader);

			///@}

			/**
				@name Shaders - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable shader with the given name
			///@details Returns nullptr if shader could not be found
			[[nodiscard]] NonOwningPtr<Shader> GetShader(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable shader with the given name
			///@details Returns nullptr if shader could not be found
			[[nodiscard]] NonOwningPtr<const Shader> GetShader(std::string_view name) const noexcept;

			///@}

			/**
				@name Shaders - Removing
				@{
			*/

			///@brief Clears all removable shaders from this manager
			void ClearShaders() noexcept;

			///@brief Removes a removable shader from this manager
			bool RemoveShader(Shader &shader) noexcept;

			///@brief Removes a removable shader with the given name from this manager
			bool RemoveShader(std::string_view name) noexcept;

			///@}

			/**
				@name Outputting
				@{
			*/

			///@brief Prints the info log (if any) from the given shader
			///@details Returns nullopt if shader is not valid or loaded
			[[nodiscard]] std::optional<std::string> PrintInfoLog(const Shader &shader) const;

			///@}
	};
} //ion::graphics::shaders

#endif