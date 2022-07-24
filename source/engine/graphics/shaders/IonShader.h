/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShader.h
-------------------------------------------
*/

#ifndef ION_SHADER_H
#define ION_SHADER_H

#include <optional>
#include <string_view>

#include "resources/IonFileResource.h"
#include "utilities/IonStringUtility.h"

namespace ion::graphics::shaders
{
	class ShaderManager; //Forward declaration

	namespace shader
	{
		enum class ShaderType : bool
		{
			Vertex,
			Fragment	
		};

		namespace detail
		{
			inline auto shader_type(std::string_view name) noexcept -> std::optional<ShaderType>
			{
				if (ion::utilities::string::EndsWith(name, ".vert", ion::utilities::string::StringCase::Insensitive) ||
					ion::utilities::string::EndsWith(name, ".vs", ion::utilities::string::StringCase::Insensitive) ||
					ion::utilities::string::EndsWith(name, ".vsh", ion::utilities::string::StringCase::Insensitive) ||
					ion::utilities::string::EndsWith(name, ".vshader", ion::utilities::string::StringCase::Insensitive) ||
					ion::utilities::string::EndsWith(name, ".glslv", ion::utilities::string::StringCase::Insensitive))
					return ShaderType::Vertex;

				else if (ion::utilities::string::EndsWith(name, ".frag", ion::utilities::string::StringCase::Insensitive) ||
						 ion::utilities::string::EndsWith(name, ".fs", ion::utilities::string::StringCase::Insensitive) ||
						 ion::utilities::string::EndsWith(name, ".fsh", ion::utilities::string::StringCase::Insensitive) ||
						 ion::utilities::string::EndsWith(name, ".fshader", ion::utilities::string::StringCase::Insensitive) ||
						 ion::utilities::string::EndsWith(name, ".glslf", ion::utilities::string::StringCase::Insensitive))
					return ShaderType::Fragment;

				else
					return {};
			}
		} //detail
	} //shader


	class Shader final : public resources::FileResource<ShaderManager>
	{
		private:

			std::optional<int> handle_;

		public:

			using resources::FileResource<ShaderManager>::FileResource;


			/*
				Modifiers
			*/

			//Sets the handle for the shader to the given value
			inline void Handle(std::optional<int> handle) noexcept
			{
				handle_ = handle;
			}


			/*
				Observers
			*/

			//Returns the type of the shader
			//Returns nullopt if the shader is not loaded
			[[nodiscard]] inline auto Type() const noexcept
			{
				return shader::detail::shader_type(AssetName());
			}

			//Returns the handle to the shader
			//Returns nullopt if the shader is not loaded
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}	
	};
} //ion::graphics::shaders

#endif