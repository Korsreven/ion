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

/*
	Modifiers
*/

void ShaderProgram::VertexShader(Shader &shader)
{
	vertex_shader_.Observe(shader);
}

void ShaderProgram::VertexShader(std::nullptr_t) noexcept
{
	vertex_shader_.Release();
}


void ShaderProgram::FragmentShader(Shader &shader)
{
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