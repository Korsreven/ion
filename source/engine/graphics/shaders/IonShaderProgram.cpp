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

ShaderProgram::ShaderProgram(std::string name, NonOwningPtr<Shader> shader) :
	resources::Resource<ShaderProgramManager>{std::move(name)}
{
	//The given shader can either be a vertex or a fragment shader, try both
	VertexShader(shader);
	FragmentShader(shader);
}

ShaderProgram::ShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader) :
	resources::Resource<ShaderProgramManager>{std::move(name)}
{
	//The given shaders much match the correct shader type
	VertexShader(vertex_shader);
	FragmentShader(fragment_shader);
}


/*
	Modifiers
*/

void ShaderProgram::VertexShader(NonOwningPtr<Shader> shader) noexcept
{
	if (shader)
	{
		if (shader->Type() == shader::ShaderType::Vertex)
			vertex_shader_ = shader;
	}
	else
		vertex_shader_ = nullptr;
}

void ShaderProgram::FragmentShader(NonOwningPtr<Shader> shader) noexcept
{
	if (shader)
	{
		if (shader->Type() == shader::ShaderType::Fragment)
			fragment_shader_ = shader;
	}
	else
		fragment_shader_ = nullptr;
}


/*
	Attribute variables
	Retrieving
*/

NonOwningPtr<variables::AttributeVariable> ShaderProgram::GetAttribute(std::string_view name) noexcept
{
	return AttributeVariablesBase::Get(name);
}

NonOwningPtr<const variables::AttributeVariable> ShaderProgram::GetAttribute(std::string_view name) const noexcept
{
	return AttributeVariablesBase::Get(name);
}


/*
	Attribute variables
	Removing
*/

void ShaderProgram::ClearAttributes() noexcept
{
	return AttributeVariablesBase::Clear();
}

bool ShaderProgram::RemoveAttribute(variables::AttributeVariable &attribute_variable) noexcept
{
	return AttributeVariablesBase::Remove(attribute_variable);
}

bool ShaderProgram::RemoveAttribute(std::string_view name) noexcept
{
	return AttributeVariablesBase::Remove(name);
}


/*
	Uniform variables
	Retrieving
*/

NonOwningPtr<variables::UniformVariable> ShaderProgram::GetUniform(std::string_view name) noexcept
{
	return UniformVariablesBase::Get(name);
}

NonOwningPtr<const variables::UniformVariable> ShaderProgram::GetUniform(std::string_view name) const noexcept
{
	return UniformVariablesBase::Get(name);
}


/*
	Uniform variables
	Removing
*/

void ShaderProgram::ClearUniforms() noexcept
{
	return UniformVariablesBase::Clear();
}

bool ShaderProgram::RemoveUniform(variables::UniformVariable &uniform_variable) noexcept
{
	return UniformVariablesBase::Remove(uniform_variable);
}

bool ShaderProgram::RemoveUniform(std::string_view name) noexcept
{
	return UniformVariablesBase::Remove(name);
}


/*
	Variables
	Removing
*/

void ShaderProgram::ClearVariables() noexcept
{
	ClearAttributes();
	ClearUniforms();
}

} //ion::graphics::shaders