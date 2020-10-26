/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderProgram.cpp
-------------------------------------------
*/

#include "IonShaderProgram.h"

namespace ion::graphics::shaders
{

ShaderProgram::ShaderProgram(std::string name, Shader &shader) :
	resources::Resource<ShaderProgramManager>{std::move(name)}
{
	//The given shader can either be a vertex or a fragment shader, try both
	VertexShader(shader);
	FragmentShader(shader);
}

ShaderProgram::ShaderProgram(std::string name, Shader &vertex_shader, Shader &fragment_shader) :
	resources::Resource<ShaderProgramManager>{std::move(name)}
{
	//The given shaders much match the correct shader type
	VertexShader(vertex_shader);
	FragmentShader(fragment_shader);
}


/*
	Modifiers
*/

void ShaderProgram::VertexShader(Shader &shader)
{
	if (shader.Type() == shader::ShaderType::Vertex)
		vertex_shader_.Observe(shader);
}

void ShaderProgram::VertexShader(std::nullptr_t) noexcept
{
	vertex_shader_.Release();
}


void ShaderProgram::FragmentShader(Shader &shader)
{
	if (shader.Type() == shader::ShaderType::Fragment)
		fragment_shader_.Observe(shader);
}

void ShaderProgram::FragmentShader(std::nullptr_t) noexcept
{
	fragment_shader_.Release();
}


/*
	Attribute variables
*/

void ShaderProgram::ClearAttributes() noexcept
{
	return AttributeVariablesBase::Clear();
}

bool ShaderProgram::RemoveAttribute(variables::AttributeVariable &attribute_variable) noexcept
{
	return AttributeVariablesBase::Remove(attribute_variable);
}


/*
	Uniform variables
*/

void ShaderProgram::ClearUniforms() noexcept
{
	return UniformVariablesBase::Clear();
}

bool ShaderProgram::RemoveUniform(variables::UniformVariable &uniform_variable) noexcept
{
	return UniformVariablesBase::Remove(uniform_variable);
}


/*
	Variables
*/

void ShaderProgram::ClearVariables() noexcept
{
	ClearAttributes();
	ClearUniforms();
}

} //ion::graphics::shaders