/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderProgramManager.cpp
-------------------------------------------
*/

#include "IonShaderProgramManager.h"

#include "graphics/IonGraphicsAPI.h"
#include "utilities/IonFileUtility.h"

namespace ion::graphics::shaders
{

using namespace shader_program_manager;

namespace shader_program_manager::detail
{

std::optional<int> load_shader_program(std::optional<int> vertex_shader_handle, std::optional<int> fragment_shader_handle,
	const std::filesystem::path &shader_source_path, std::optional<InfoLogLevel> log_level) noexcept
{
	auto shader_program_handle = 0;
	auto link_status = 0;

	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		{
			//Create shader program
			shader_program_handle = glCreateProgram();

			//Attach shaders
			if (shader_program_handle != 0)
			{
				if (vertex_shader_handle)
					glAttachShader(shader_program_handle, *vertex_shader_handle);
				if (fragment_shader_handle)
					glAttachShader(shader_program_handle, *fragment_shader_handle);
			}

			//Link program
			glLinkProgram(shader_program_handle);
			glGetProgramiv(shader_program_handle, GL_LINK_STATUS, &link_status);
			break;
		}

		case gl::Extension::ARB:
		{
			//Create shader program
			shader_program_handle = glCreateProgramObjectARB();

			//Attach shaders
			if (shader_program_handle != 0)
			{
				if (vertex_shader_handle)
					glAttachObjectARB(shader_program_handle, *vertex_shader_handle);
				if (fragment_shader_handle)
					glAttachObjectARB(shader_program_handle, *fragment_shader_handle);
			}

			//Link program
			glLinkProgramARB(shader_program_handle);
			glGetObjectParameterivARB(shader_program_handle, GL_OBJECT_LINK_STATUS_ARB, &link_status);
			break;
		}

		default:
		return {};
	}


	//Save shader program info log
	if (log_level &&
		(link_status == 0 || *log_level == InfoLogLevel::Warning))
			//Link error, or log anyways (could be warnings)
	{
		if (auto log = print_info_log(shader_program_handle); log)
			ion::utilities::file::Save(shader_source_path.stem().string() + ".output.txt", *log);
				//Remove shader extension (.vert or .frag) by using stem()
	}

	//Shader program did not link properly, unload
	if (link_status == 0)
	{
		unload_shader_program(shader_program_handle);
		return {};
	}
	else
		return shader_program_handle;
}

void unload_shader_program(int shader_program_handle) noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glDeleteProgram(shader_program_handle);
		break;

		case gl::Extension::ARB:
		glDeleteObjectARB(shader_program_handle);
		break;
	}

	//Note: Shaders are automatically detached when program is deleted
}


std::optional<std::string> print_info_log(int shader_program_handle)
{
	auto length = 0;

	//Retrieve log length from shader program object
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glGetProgramiv(shader_program_handle, GL_INFO_LOG_LENGTH, &length);
		break;

		case gl::Extension::ARB:
		glGetObjectParameterivARB(shader_program_handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
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
			glGetProgramInfoLog(shader_program_handle, length, &written, &log[0]);
			break;

			case gl::Extension::ARB:
			glGetInfoLogARB(shader_program_handle, length, &written, &log[0]);
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

bool ShaderProgramManager::LoadResource(ShaderProgram &shader_program) noexcept
{
	auto vertex_shader = shader_program.VertexShader();
	auto fragment_shader = shader_program.FragmentShader();	

	if (vertex_shader || fragment_shader)
	{
		auto loaded = true;

		//Make sure vertex or/and fragment shaders are loaded, otherwise load them eagerly
		if (vertex_shader)
			loaded &= vertex_shader->Owner() ? vertex_shader->Owner()->Load(*vertex_shader) : vertex_shader->IsLoaded();
		if (fragment_shader)
			loaded &= fragment_shader->Owner() ? fragment_shader->Owner()->Load(*fragment_shader) : fragment_shader->IsLoaded();

		auto &source_path = vertex_shader ? vertex_shader->FilePath() : fragment_shader->FilePath();

		if (loaded && source_path)
		{
			shader_program.Handle(detail::load_shader_program(vertex_shader->Handle(), fragment_shader->Handle(), *source_path, log_level_));
			return shader_program.Handle().has_value();
		}
	}
	
	return false;
}

bool ShaderProgramManager::UnloadResource(ShaderProgram &shader_program) noexcept
{
	if (auto handle = shader_program.Handle(); handle)
	{
		detail::unload_shader_program(*handle);
		shader_program.Handle({});
		return true;
	}
	else
		return false;
}


//Public

ShaderProgramManager::~ShaderProgramManager() noexcept
{
	this->UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Shader programs
	Creating
*/

ShaderProgram& ShaderProgramManager::CreateShaderProgram(Shader &shader)
{
	return this->CreateResource(std::ref(shader));
}

ShaderProgram& ShaderProgramManager::CreateShaderProgram(Shader &vertex_shader, Shader &fragment_shader)
{
	return this->CreateResource(std::ref(vertex_shader), std::ref(fragment_shader));
}


/*
	Shader programs
	Removing
*/

void ShaderProgramManager::ClearShaderPrograms() noexcept
{
	this->ClearResources();
}

bool ShaderProgramManager::RemoveShaderProgram(ShaderProgram &shader_program) noexcept
{
	return this->RemoveResource(shader_program);
}


/*
	Outputting
*/

std::optional<std::string> ShaderProgramManager::PrintInfoLog(const ShaderProgram &shader_program) const
{
	if (auto handle = shader_program.Handle(); handle)
		return detail::print_info_log(*handle);
	else
		return {};
}

} //ion::graphics::shaders