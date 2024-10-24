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
#include "utilities/IonStringUtility.h"

namespace ion::graphics::shaders
{

using namespace shader_manager;

namespace shader_manager::detail
{

void prepend_defines(std::string &shader_source, const std::string &shader_defines)
{
	using namespace ion::utilities;

	if (auto parts = string::Split(shader_defines, ","); !std::empty(parts))
	{
		std::string defines;

		for (auto &define : parts)
			defines += string::Concat("#define ", string::Trim(define), '\n');

		size_t off = 0;

		//#version macro should always be on top
		if (auto off2 = shader_source.find("#version "); off2 != std::string::npos)
		{
			if (off2 = shader_source.find('\n', off2); off2 != std::string::npos)
				off = off2 + 1;
		}
		
		shader_source.insert(std::begin(shader_source) + off, std::begin(defines), std::end(defines));
	}
}


std::optional<int> load_shader(shader::ShaderType shader_type, const std::string &shader_defines,
	std::string shader_source, const std::filesystem::path &shader_source_path, std::optional<InfoLogLevel> log_level) noexcept
{
	auto shader_handle = 0;
	auto compile_status = 0;

	if (!std::empty(shader_defines))
		prepend_defines(shader_source, shader_defines);

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

			auto source_ptr = shader_source.c_str();
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

			auto source_ptr = shader_source.c_str();
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

bool ShaderManager::LoadResource(Shader &shader)
{
	auto type = shader.Type();
	auto &defines = shader.Defines();
	auto &source = shader.FileData();
	auto &source_path = shader.FilePath();

	if (type && source && source_path)
	{
		shader.Handle(detail::load_shader(*type, defines, *source, *source_path, log_level_));
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
	UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Shaders - Creating
*/

NonOwningPtr<Shader> ShaderManager::CreateShader(std::string name, std::string asset_name)
{
	return CreateResource(std::move(name), std::move(asset_name));
}


NonOwningPtr<Shader> ShaderManager::CreateShader(const Shader &shader)
{
	return CreateResource(shader);
}

NonOwningPtr<Shader> ShaderManager::CreateShader(Shader &&shader)
{
	return CreateResource(std::move(shader));
}


/*
	Shaders - Retrieving
*/

NonOwningPtr<Shader> ShaderManager::GetShader(std::string_view name) noexcept
{
	return GetResource(name);
}

NonOwningPtr<const Shader> ShaderManager::GetShader(std::string_view name) const noexcept
{
	return GetResource(name);
}


/*
	Shaders - Removing
*/

void ShaderManager::ClearShaders() noexcept
{
	ClearResources();
}

bool ShaderManager::RemoveShader(Shader &shader) noexcept
{
	return RemoveResource(shader);
}

bool ShaderManager::RemoveShader(std::string_view name) noexcept
{
	return RemoveResource(name);
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