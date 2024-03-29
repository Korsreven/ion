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

#include "IonShaderManager.h"
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


int get_active_shader_program() noexcept
{
	auto program_handle = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program_handle);
	return program_handle;
}

void use_shader_program(int program_handle) noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUseProgram(program_handle);
		break;

		case gl::Extension::ARB:
		glUseProgramObjectARB(program_handle);
		break;
	}
}


/*
	Attribute
*/

std::optional<int> get_attribute_location(int shader_program_handle, const std::string &attribute_name) noexcept
{
	auto location = -1;

	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		location = glGetAttribLocation(shader_program_handle, std::data(attribute_name));
		break;

		case gl::Extension::ARB:
		location = glGetAttribLocationARB(shader_program_handle, std::data(attribute_name));
		break;
	}

	//Zero is a valid location
	if (location >= 0)
		return location;
	else
		return {};
}


void load_attribute_location(int shader_program_handle, AttributeVariable &attribute_variable) noexcept
{
	if (!attribute_variable.Location())
		attribute_variable.Location(get_attribute_location(shader_program_handle, *attribute_variable.Name()));
}

void send_attribute_value(int shader_program_handle, AttributeVariable &attribute_variable) noexcept
{
	[[maybe_unused]] auto need_update = attribute_variable.HasNewValue();
		//Vertex attribute pointer needs to be enabled each time (ignore flag)

	load_attribute_location(shader_program_handle, attribute_variable);

	if (auto location = attribute_variable.Location(); location)
		attribute_variable.Visit(set_attribute_value{*location});
}


set_attribute_value::set_attribute_value(int location) noexcept :
	location_{location}
{
	//Empty
}

void set_attribute_value::set_vertex_pointer(int index, int size, bool normalized, int stride, const void *pointer, int32) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glVertexAttribPointer(index, size, GL_INT, normalized, stride, pointer);
		break;

		case gl::Extension::ARB:
		glVertexAttribPointerARB(index, size, GL_INT, normalized, stride, pointer);
		break;
	}
}

void set_attribute_value::set_vertex_pointer(int index, int size, bool normalized, int stride, const void *pointer, uint32) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glVertexAttribPointer(index, size, GL_UNSIGNED_INT, normalized, stride, pointer);
		break;

		case gl::Extension::ARB:
		glVertexAttribPointerARB(index, size, GL_UNSIGNED_INT, normalized, stride, pointer);
		break;
	}
}

void set_attribute_value::set_vertex_pointer(int index, int size, bool normalized, int stride, const void *pointer, float32) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glVertexAttribPointer(index, size, GL_FLOAT, normalized, stride, pointer);
		break;

		case gl::Extension::ARB:
		glVertexAttribPointerARB(index, size, GL_FLOAT, normalized, stride, pointer);
		break;
	}
}

void set_attribute_value::set_vertex_pointer(int index, int size, bool normalized, int stride, const void *pointer, float64) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glVertexAttribPointer(index, size, GL_DOUBLE, normalized, stride, pointer);
		break;

		case gl::Extension::ARB:	
		glVertexAttribPointerARB(index, size, GL_DOUBLE, normalized, stride, pointer);
		break;
	}
}


void set_attribute_value::set_vertex_ipointer(int index, int size, bool normalized, int stride, const void *pointer, int32) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
	{		
		glVertexAttribIPointer(index, size, GL_INT, stride, pointer);
	}
	else //Use glVertexAttribPointer (will be converted to float)
		set_vertex_pointer(index, size, normalized, stride, pointer, int32{});
}

void set_attribute_value::set_vertex_ipointer(int index, int size, bool normalized, int stride, const void *pointer, uint32) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
	{	
		glVertexAttribIPointer(index, size, GL_UNSIGNED_INT, stride, pointer);
	}
	else //Use glVertexAttribPointer (will be converted to float)
		set_vertex_pointer(index, size, normalized, stride, pointer, uint32{});
}


void set_attribute_value::set_vertex_lpointer(int index, int size, bool normalized, int stride, const void *pointer) const noexcept
{
	if (gl::HasGL(gl::Version::v4_1))
	{		
		glVertexAttribLPointer(index, size, GL_DOUBLE, stride, pointer);
	}
	else //Use glVertexAttribPointer (will be converted to float)
		set_vertex_pointer(index, size, normalized, stride, pointer, float64{});
}

/*
	Uniform
*/

std::optional<int> get_uniform_location(int shader_program_handle, const std::string &uniform_name) noexcept
{
	auto location = -1;

	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		location = glGetUniformLocation(shader_program_handle, std::data(uniform_name));
		break;

		case gl::Extension::ARB:
		location = glGetUniformLocationARB(shader_program_handle, std::data(uniform_name));
		break;
	}

	//Zero is a valid location
	if (location >= 0)
		return location;
	else
		return {};
}


void load_uniform_location(int shader_program_handle, UniformVariable &uniform_variable) noexcept
{
	if (!uniform_variable.Location())
		uniform_variable.Location(get_uniform_location(shader_program_handle, *uniform_variable.Name()));
}

void send_uniform_value(int shader_program_handle, UniformVariable &uniform_variable) noexcept
{
	//Uniform has new value (need update)
	if (uniform_variable.HasNewValue())
	{
		load_uniform_location(shader_program_handle, uniform_variable);

		if (auto location = uniform_variable.Location(); location)
			uniform_variable.Visit(set_uniform_value{*location});
	}
}


set_uniform_value::set_uniform_value(int location) noexcept :
	location_{location}
{
	//Empty
}


//Scalars

void set_uniform_value::operator()(const glsl::uniform<bool> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (bool is passed as int)
		break;

		case gl::Extension::ARB:
		glUniform1ivARB(location_, value.Size(), value.Values()); //Use glUniform1ivARB (bool is passed as int)
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<int32> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform1iv(location_, value.Size(), value.Values());
		break;

		case gl::Extension::ARB:
		glUniform1ivARB(location_, value.Size(), value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<uint32> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
		glUniform1uiv(location_, value.Size(), value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<float32> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform1fv(location_, value.Size(), value.Values());
		break;

		case gl::Extension::ARB:
		glUniform1fvARB(location_, value.Size(), value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<float64> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniform1dv(location_, value.Size(), value.Values());
}


//Vectors
//2-components

void set_uniform_value::operator()(const glsl::uniform<glsl::bvec2> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform2iv(location_, value.Size(), value.Values()); //Use glUniform2iv (bool is passed as int)
		break;

		case gl::Extension::ARB:
		glUniform2ivARB(location_, value.Size(), value.Values()); //Use glUniform2ivARB (bool is passed as int)
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::ivec2> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform2iv(location_, value.Size(), value.Values());
		break;

		case gl::Extension::ARB:
		glUniform2ivARB(location_, value.Size(), value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::uvec2> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
		glUniform2uiv(location_, value.Size(), value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::vec2> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform2fv(location_, value.Size(), value.Values());
		break;

		case gl::Extension::ARB:
		glUniform2fvARB(location_, value.Size(), value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dvec2> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniform2dv(location_, value.Size(), value.Values());
}

//3-components

void set_uniform_value::operator()(const glsl::uniform<glsl::bvec3> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform3iv(location_, value.Size(), value.Values()); //Use glUniform3iv (bool is passed as int)
		break;

		case gl::Extension::ARB:
		glUniform3ivARB(location_, value.Size(), value.Values()); //Use glUniform3ivARB (bool is passed as int)
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::ivec3> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform3iv(location_, value.Size(), value.Values());
		break;

		case gl::Extension::ARB:
		glUniform3ivARB(location_, value.Size(), value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::uvec3> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
		glUniform3uiv(location_, value.Size(), value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::vec3> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform3fv(location_, value.Size(), value.Values());
		break;

		case gl::Extension::ARB:
		glUniform3fvARB(location_, value.Size(), value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dvec3> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniform3dv(location_, value.Size(), value.Values());
}

//4-components

void set_uniform_value::operator()(const glsl::uniform<glsl::bvec4> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform4iv(location_, value.Size(), value.Values()); //Use glUniform4iv (bool is passed as int)
		break;

		case gl::Extension::ARB:
		glUniform4ivARB(location_, value.Size(), value.Values()); //Use glUniform4ivARB (bool is passed as int)
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::ivec4> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform4iv(location_, value.Size(), value.Values());
		break;

		case gl::Extension::ARB:
		glUniform4ivARB(location_, value.Size(), value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::uvec4> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
		glUniform4uiv(location_, value.Size(), value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::vec4> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform4fv(location_, value.Size(), value.Values());
		break;

		case gl::Extension::ARB:
		glUniform4fvARB(location_, value.Size(), value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dvec4> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniform4dv(location_, value.Size(), value.Values());
}


//Matrices

void set_uniform_value::operator()(const glsl::uniform<glsl::mat2> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniformMatrix2fv(location_, value.Size(), false, value.Values());
		break;

		case gl::Extension::ARB:
		glUniformMatrix2fvARB(location_, value.Size(), false, value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dmat2> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniformMatrix2dv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::mat2x3> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v2_1))
		glUniformMatrix2x3fv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dmat2x3> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniformMatrix2x3dv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::mat2x4> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v2_1))
		glUniformMatrix2x4fv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dmat2x4> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniformMatrix2x4dv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::mat3x2> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v2_1))
		glUniformMatrix3x2fv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dmat3x2> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniformMatrix3x2dv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::mat3> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniformMatrix3fv(location_, value.Size(), false, value.Values());
		break;

		case gl::Extension::ARB:
		glUniformMatrix3fvARB(location_, value.Size(), false, value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dmat3> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniformMatrix3dv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::mat3x4> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v2_1))
		glUniformMatrix3x4fv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dmat3x4> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniformMatrix3x4dv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::mat4x2> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v2_1))
		glUniformMatrix4x2fv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dmat4x2> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniformMatrix4x2dv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::mat4x3> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v2_1))
		glUniformMatrix4x3fv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dmat4x3> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniformMatrix4x3dv(location_, value.Size(), false, value.Values());
}

void set_uniform_value::operator()(const glsl::uniform<glsl::mat4> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniformMatrix4fv(location_, value.Size(), false, value.Values());
		break;

		case gl::Extension::ARB:
		glUniformMatrix4fvARB(location_, value.Size(), false, value.Values());
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::dmat4> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v4_0))
		glUniformMatrix4dv(location_, value.Size(), false, value.Values());
}


//Samplers

void set_uniform_value::operator()(const glsl::uniform<glsl::isampler1D> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}

void set_uniform_value::operator()(const glsl::uniform<glsl::usampler1D> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}

void set_uniform_value::operator()(const glsl::uniform<glsl::sampler1D> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
		break;

		case gl::Extension::ARB:
		glUniform1ivARB(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::isampler1DArray> &value) const noexcept
{
	if (gl::ArrayTexture_Support() != gl::Extension::None)
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}

void set_uniform_value::operator()(const glsl::uniform<glsl::usampler1DArray> &value) const noexcept
{
	if (gl::ArrayTexture_Support() != gl::Extension::None)
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}

void set_uniform_value::operator()(const glsl::uniform<glsl::sampler1DArray> &value) const noexcept
{
	if (gl::ArrayTexture_Support() != gl::Extension::None)
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}


void set_uniform_value::operator()(const glsl::uniform<glsl::isampler2D> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}

void set_uniform_value::operator()(const glsl::uniform<glsl::usampler2D> &value) const noexcept
{
	if (gl::HasGL(gl::Version::v3_0))
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}

void set_uniform_value::operator()(const glsl::uniform<glsl::sampler2D> &value) const noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
		break;

		case gl::Extension::ARB:
		glUniform1ivARB(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
		break;
	}
}

void set_uniform_value::operator()(const glsl::uniform<glsl::isampler2DArray> &value) const noexcept
{
	if (gl::ArrayTexture_Support() != gl::Extension::None)
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}

void set_uniform_value::operator()(const glsl::uniform<glsl::usampler2DArray> &value) const noexcept
{
	if (gl::ArrayTexture_Support() != gl::Extension::None)
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}

void set_uniform_value::operator()(const glsl::uniform<glsl::sampler2DArray> &value) const noexcept
{
	if (gl::ArrayTexture_Support() != gl::Extension::None)
		glUniform1iv(location_, value.Size(), value.Values()); //Use glUniform1iv (texture unit is passed as int)
}

} //shader_manager::detail


//Private

void ShaderProgramManager::ActivateShaderProgram(int handle) noexcept
{
	if (handle != active_shader_program_handle_)
		detail::use_shader_program(active_shader_program_handle_ = handle);
}

void ShaderProgramManager::DeactivateShaderProgram(int handle) noexcept
{
	if (handle == active_shader_program_handle_)
		detail::use_shader_program(active_shader_program_handle_ = 0);
}

bool ShaderProgramManager::IsShaderProgramActive(int handle) noexcept
{
	return handle == active_shader_program_handle_;
}


//Protected

/*
	Events
*/

bool ShaderProgramManager::LoadResource(ShaderProgram &shader_program)
{
	auto vertex_shader = shader_program.VertexShader();
	auto fragment_shader = shader_program.FragmentShader();	

	if (vertex_shader || fragment_shader)
	{
		auto loaded = true;

		//Make sure vertex or/and fragment shaders are loaded, otherwise load them eagerly
		if (vertex_shader)
			loaded &= vertex_shader->IsLoaded() || (vertex_shader->Owner() && vertex_shader->Owner()->Load(*vertex_shader));
		if (fragment_shader)
			loaded &= fragment_shader->IsLoaded() || (fragment_shader->Owner() && fragment_shader->Owner()->Load(*fragment_shader));

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

		if (IsShaderProgramActive(*handle))
			active_shader_program_handle_ = 0;

		//Set all attribute variable locations to nullopt
		for (auto &attribute_variable : shader_program.AttributeVariables())
			attribute_variable.Location({});

		//Set all uniform variable locations to nullopt
		for (auto &uniform_variable : shader_program.UniformVariables())
			uniform_variable.Location({});

		//Set all struct member locations to nullopt
		for (auto &shader_struct : shader_program.Structs())
		{
			//Set all uniform variable locations inside struct to nullopt
			for (auto &uniform_variable : shader_struct.UniformVariables())
				uniform_variable.Location({});
		}

		return true;
	}
	else
		return false;
}


void ShaderProgramManager::Created(ShaderProgram &shader_program) noexcept
{
	//No user defined shader layout
	if (!shader_program.Layout())
		shader_program.Layout(GetShaderLayout("")); //Use default shader layout
}

bool ShaderProgramManager::Removable(ShaderLayout &shader_layout) noexcept
{
	return shader_layout.Name() != ""; //Don't remove default shader layout
}


//Public

ShaderProgramManager::ShaderProgramManager()
{
	CreateShaderLayout(shader_layout::DefaultShaderLayout);
		//All shader program managers should contain a default shader layout
		//When a shader program is created, without a user defined layout, the default shader layout is passed automatically
}

ShaderProgramManager::~ShaderProgramManager() noexcept
{
	UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Shader programs - Creating
*/

NonOwningPtr<ShaderProgram> ShaderProgramManager::CreateShaderProgram(std::string name, NonOwningPtr<Shader> shader)
{
	return CreateResource(std::move(name), shader);
}

NonOwningPtr<ShaderProgram> ShaderProgramManager::CreateShaderProgram(std::string name, NonOwningPtr<Shader> shader,
	NonOwningPtr<ShaderLayout> shader_layout)
{
	return CreateResource(std::move(name), shader, shader_layout);
}

NonOwningPtr<ShaderProgram>ShaderProgramManager::CreateShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader)
{
	return CreateResource(std::move(name), vertex_shader, fragment_shader);
}

NonOwningPtr<ShaderProgram>ShaderProgramManager::CreateShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader,
	NonOwningPtr<ShaderLayout> shader_layout)
{
	return CreateResource(std::move(name), vertex_shader, fragment_shader, shader_layout);
}


NonOwningPtr<ShaderProgram> ShaderProgramManager::CreateShaderProgram(ShaderProgram &&shader_program)
{
	return CreateResource(std::move(shader_program));
}


/*
	Shader programs - Retrieving
*/

NonOwningPtr<ShaderProgram> ShaderProgramManager::GetShaderProgram(std::string_view name) noexcept
{
	return GetResource(name);
}

NonOwningPtr<const ShaderProgram> ShaderProgramManager::GetShaderProgram(std::string_view name) const noexcept
{
	return GetResource(name);
}


/*
	Shader programs - Removing
*/

void ShaderProgramManager::ClearShaderPrograms() noexcept
{
	ClearResources();
}

bool ShaderProgramManager::RemoveShaderProgram(ShaderProgram &shader_program) noexcept
{
	return RemoveResource(shader_program);
}

bool ShaderProgramManager::RemoveShaderProgram(std::string_view name) noexcept
{
	return RemoveResource(name);
}


/*
	Shader programs - Activate/deactivate
*/

void ShaderProgramManager::ActivateShaderProgram(const ShaderProgram &shader_program) noexcept
{
	if (shader_program.Owner() != this)
		return;

	if (auto handle = shader_program.Handle(); handle)
		ActivateShaderProgram(*handle);
}

void ShaderProgramManager::DeactivateShaderProgram(const ShaderProgram &shader_program) noexcept
{
	if (shader_program.Owner() != this)
		return;

	if (auto handle = shader_program.Handle(); handle)
		DeactivateShaderProgram(*handle);
}

bool ShaderProgramManager::IsShaderProgramActive(const ShaderProgram &shader_program) const noexcept
{
	if (auto handle = shader_program.Handle(); handle)
		return IsShaderProgramActive(*handle);
	else
		return false;
}


/*
	Shader variables - Load/send
*/

void ShaderProgramManager::LoadShaderVariableLocations(ShaderProgram &shader_program) noexcept
{
	if (shader_program.Owner() != this)
		return;

	if (auto handle = shader_program.Handle(); handle)
	{
		//Load all attribute variable locations attached to shader program
		for (auto &attribute_variable : shader_program.AttributeVariables())
			detail::load_attribute_location(*handle, attribute_variable);

		//Load all uniform variable locations attached to shader program
		for (auto &uniform_variable : shader_program.UniformVariables())
			detail::load_uniform_location(*handle, uniform_variable);

		//Load all struct member locations attached to shader program
		for (auto &shader_struct : shader_program.Structs())
		{
			//Load all member uniform variable locations inside struct
			for (auto &uniform_variable : shader_struct.UniformVariables())
				detail::load_uniform_location(*handle, uniform_variable);
		}
	}
}

void ShaderProgramManager::LoadAttributeLocations(ShaderProgram &shader_program) noexcept
{
	if (shader_program.Owner() != this)
		return;

	if (auto handle = shader_program.Handle(); handle)
	{
		//Load all attribute variable locations attached to shader program
		for (auto &attribute_variable : shader_program.AttributeVariables())
			detail::load_attribute_location(*handle, attribute_variable);
	}
}

void ShaderProgramManager::LoadUniformLocations(ShaderProgram &shader_program) noexcept
{
	if (shader_program.Owner() != this)
		return;

	if (auto handle = shader_program.Handle(); handle)
	{
		//Load all uniform variable locations attached to shader program
		for (auto &uniform_variable : shader_program.UniformVariables())
			detail::load_uniform_location(*handle, uniform_variable);

		//Load all struct member locations attached to shader program
		for (auto &shader_struct : shader_program.Structs())
		{
			//Load all member uniform variable locations inside struct
			for (auto &uniform_variable : shader_struct.UniformVariables())
				detail::load_uniform_location(*handle, uniform_variable);
		}
	}
}


void ShaderProgramManager::SendShaderVariableValues(ShaderProgram &shader_program) noexcept
{
	if (shader_program.Owner() != this)
		return;

	if (auto handle = shader_program.Handle(); handle)
	{
		auto in_use = IsShaderProgramActive(*handle);

		if (!in_use)
			ActivateShaderProgram(*handle);

		//Update all attribute variables attached to shader program
		for (auto &attribute_variable : shader_program.AttributeVariables())
			detail::send_attribute_value(*handle, attribute_variable);

		//Update all uniform variables attached to shader program
		for (auto &uniform_variable : shader_program.UniformVariables())
			detail::send_uniform_value(*handle, uniform_variable);

		//Update all struct members attached to shader program
		for (auto &shader_struct : shader_program.Structs())
		{
			//Update all member uniform variables inside struct
			for (auto &uniform_variable : shader_struct.UniformVariables())
				detail::send_uniform_value(*handle, uniform_variable);
		}

		if (!in_use)
			DeactivateShaderProgram(*handle);
	}
}

void ShaderProgramManager::SendAttributeValues(ShaderProgram &shader_program) noexcept
{
	if (shader_program.Owner() != this)
		return;

	if (auto handle = shader_program.Handle(); handle)
	{
		auto in_use = IsShaderProgramActive(*handle);

		if (!in_use)
			ActivateShaderProgram(*handle);

		//Update all attribute variables attached to shader program
		for (auto &attribute_variable : shader_program.AttributeVariables())
			detail::send_attribute_value(*handle, attribute_variable);

		if (!in_use)
			DeactivateShaderProgram(*handle);
	}
}

void ShaderProgramManager::SendUniformValues(ShaderProgram &shader_program) noexcept
{
	if (shader_program.Owner() != this)
		return;

	if (auto handle = shader_program.Handle(); handle)
	{
		auto in_use = IsShaderProgramActive(*handle);

		if (!in_use)
			ActivateShaderProgram(*handle);

		//Update all uniform variables attached to shader program
		for (auto &uniform_variable : shader_program.UniformVariables())
			detail::send_uniform_value(*handle, uniform_variable);

		//Update all struct members attached to shader program
		for (auto &shader_struct : shader_program.Structs())
		{
			//Update all member uniform variables inside struct
			for (auto &uniform_variable : shader_struct.UniformVariables())
				detail::send_uniform_value(*handle, uniform_variable);
		}

		if (!in_use)
			DeactivateShaderProgram(*handle);
	}
}

void ShaderProgramManager::SendUniformValues(ShaderStruct &shader_struct) noexcept
{
	auto shader_program = shader_struct.Owner();

	if (!shader_program || shader_program->Owner() != this)
		return;

	if (auto handle = shader_program->Handle(); handle)
	{
		auto in_use = IsShaderProgramActive(*handle);

		if (!in_use)
			ActivateShaderProgram(*handle);

		//Update all struct members attached to shader program
		for (auto &s_struct : shader_program->Structs())
		{
			//Update all member uniform variables inside struct
			for (auto &uniform_variable : s_struct.UniformVariables())
				detail::send_uniform_value(*handle, uniform_variable);
		}

		if (!in_use)
			DeactivateShaderProgram(*handle);
	}
}


/*
	Shader layouts - Creating
*/

NonOwningPtr<ShaderLayout> ShaderProgramManager::CreateShaderLayout(std::string name)
{
	return ShaderLayoutBase::Create(std::move(name));
}

NonOwningPtr<ShaderLayout> ShaderProgramManager::CreateShaderLayout(std::string name, shader_layout::StructBindings struct_bindings,
	shader_layout::AttributeBindings attribute_bindings, shader_layout::UniformBindings uniform_bindings)
{
	return ShaderLayoutBase::Create(std::move(name), std::move(struct_bindings),
		std::move(attribute_bindings), std::move(uniform_bindings));
}


NonOwningPtr<ShaderLayout> ShaderProgramManager::CreateShaderLayout(const ShaderLayout &shader_layout)
{
	return ShaderLayoutBase::Create(shader_layout);
}

NonOwningPtr<ShaderLayout> ShaderProgramManager::CreateShaderLayout(ShaderLayout &&shader_layout)
{
	return ShaderLayoutBase::Create(std::move(shader_layout));
}


/*
	Shader layouts - Retrieving
*/

NonOwningPtr<ShaderLayout> ShaderProgramManager::GetShaderLayout(std::string_view name) noexcept
{
	return ShaderLayoutBase::Get(name);
}

NonOwningPtr<const ShaderLayout> ShaderProgramManager::GetShaderLayout(std::string_view name) const noexcept
{
	return ShaderLayoutBase::Get(name);
}


/*
	Shader layouts - Removing
*/

void ShaderProgramManager::ClearShaderLayouts() noexcept
{
	return ShaderLayoutBase::Clear();
}

bool ShaderProgramManager::RemoveShaderLayout(ShaderLayout &shader_layout) noexcept
{
	return ShaderLayoutBase::Remove(shader_layout);
}

bool ShaderProgramManager::RemoveShaderLayout(std::string_view name) noexcept
{
	return ShaderLayoutBase::Remove(name);
}


/*
	Outputting
*/

std::optional<std::string> ShaderProgramManager::PrintInfoLog(const ShaderProgram &shader_program) const
{
	if (shader_program.Owner() != this)
		return {};

	if (auto handle = shader_program.Handle(); handle)
		return detail::print_info_log(*handle);
	else
		return {};
}

} //ion::graphics::shaders