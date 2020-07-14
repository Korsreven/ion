/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderManager.cpp
-------------------------------------------
*/

#include "IonShaderManager.h"

#include "graphics/IonGraphicsAPI.h"
#include "utilities/IonFileUtility.h"

namespace ion::graphics::shaders
{

using namespace shader_manager;

namespace shader_manager::detail
{

std::optional<int> load_shader(shader::ShaderType shader_type, const std::string &shader_source,
	const std::filesystem::path &shader_source_path, std::optional<InfoLogLevel> log_level) noexcept
{
	auto shader_handle = 0;
	auto compile_status = 0;

	//Compile shader
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		{
			switch (shader_type)
			{
				case shader::ShaderType::Vertex:
				shader_handle = GL_VERTEX_SHADER;
				break;

				case shader::ShaderType::Fragment:
				shader_handle = GL_FRAGMENT_SHADER;
				break;

				default:
				return {};
			}

			auto source_ptr = std::data(shader_source);
			shader_handle = glCreateShader(shader_handle);
			glShaderSource(shader_handle, 1, &source_ptr, 0); //Set GLSL code

			glCompileShader(shader_handle); //Compile GLSL
			glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compile_status);
			break;
		}

		case gl::Extension::ARB:
		{
			switch (shader_type)
			{
				case shader::ShaderType::Vertex:
				shader_handle = GL_VERTEX_SHADER_ARB;
				break;

				case shader::ShaderType::Fragment:
				shader_handle = GL_FRAGMENT_SHADER_ARB;
				break;

				default:
				return {};
			}

			auto source_ptr = std::data(shader_source);
			shader_handle = glCreateShaderObjectARB(shader_handle);
			glShaderSourceARB(shader_handle, 1, &source_ptr, 0); //Set GLSL code

			glCompileShaderARB(shader_handle); //Compile GLSL
			glGetObjectParameterivARB(shader_handle, GL_OBJECT_COMPILE_STATUS_ARB, &compile_status);
			break;
		}

		default:
		return {};
	}

	//Save shader info log
	if (log_level &&
		(compile_status == 0 || *log_level == InfoLogLevel::Warning))
			//Compile error, or log anyways (could be warnings)
	{
		if (auto log = print_info_log(shader_handle); log)
			ion::utilities::file::Save(shader_source_path.string() + ".output.txt", *log);
	}

	//Shader did not compile properly, unload
	if (compile_status == 0)
	{
		unload_shader(shader_handle);
		return {};
	}
	else
		return shader_handle;
}

void unload_shader(int shader_handle) noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glDeleteShader(shader_handle);
		break;

		case gl::Extension::ARB:
		glDeleteObjectARB(shader_handle);
		break;
	}
}


std::optional<std::string> print_info_log(int shader_handle)
{
	auto length = 0;

	//Retrieve log length from shader object
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &length);
		break;

		case gl::Extension::ARB:
		glGetObjectParameterivARB(shader_handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
		break;
	}

	//Retrieve log from shader object
	if (length > 0)
	{
		auto written = 0;
		std::string log(length, '\0');

		switch (gl::Shader_Support())
		{
			case gl::Extension::Core:
			glGetShaderInfoLog(shader_handle, length, &written, &log[0]);
			break;

			case gl::Extension::ARB:
			glGetInfoLogARB(shader_handle, length, &written, &log[0]);
			break;
		}

		return std::move(log);
	}
	else
		return {};
}


} //shader_manager::detail

//Protected

/*
	Events
*/

bool ShaderManager::LoadResource(Shader &shader) noexcept
{
	auto type = shader.Type();
	auto &source = shader.FileData();
	auto &source_path = shader.FilePath();

	if (type && source && source_path)
	{
		shader.Handle(detail::load_shader(*type, *source, *source_path, log_level_));
		return shader.Handle().has_value();
	}
	else
		return false;
}

bool ShaderManager::UnloadResource(Shader &shader) noexcept
{
	if (auto handle = shader.Handle(); handle)
	{
		detail::unload_shader(*handle);
		shader.Handle({});
		return true;
	}
	else
		return false;
}


//Public

ShaderManager::~ShaderManager() noexcept
{
	this->UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Shaders
	Creating
*/

Shader& ShaderManager::CreateShader(std::string name)
{
	return this->CreateResource(std::move(name));
}


/*
	Shaders
	Removing
*/

void ShaderManager::ClearShaders() noexcept
{
	this->ClearResources();
}

bool ShaderManager::RemoveShader(Shader &shader) noexcept
{
	return this->RemoveResource(shader);
}


/*
	Outputting
*/

std::optional<std::string> ShaderManager::PrintInfoLog(const Shader &shader) const
{
	if (shader.Owner() != this)
		return {};

	if (auto handle = shader.Handle(); handle)
		return detail::print_info_log(*handle);
	else
		return {};
}

} //ion::graphics::shaders