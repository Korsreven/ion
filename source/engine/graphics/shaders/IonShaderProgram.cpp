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

using namespace shader_program;

namespace shader_program::detail
{

void remap_attribute(NonOwningPtr<variables::AttributeVariable> attribute_variable, ShaderLayout &shader_layout, mapped_attributes &attributes) noexcept
{
	//Has name
	if (auto &name = attribute_variable->Name(); name)
	{
		if (auto attribute_name = shader_layout.GetAttributeName(*name); attribute_name)
			attributes[static_cast<int>(*attribute_name)] = attribute_variable;
	}
	//Has location
	else if (auto location = attribute_variable->Location(); location)
	{
		if (auto attribute_name = shader_layout.GetAttributeName(*location); attribute_name)
			attributes[static_cast<int>(*attribute_name)] = attribute_variable;
	}
}

void remap_uniform(NonOwningPtr<variables::UniformVariable> uniform_variable, ShaderLayout &shader_layout, mapped_uniforms &uniforms) noexcept
{
	//Has name
	if (auto &name = uniform_variable->Name(); name)
	{
		if (auto uniform_name = shader_layout.GetUniformName(*name); uniform_name)
			uniforms[static_cast<int>(*uniform_name)] = uniform_variable;
	}
	//Has location
	else if (auto location = uniform_variable->Location(); location)
	{
		if (auto uniform_name = shader_layout.GetUniformName(*location); uniform_name)
			uniforms[static_cast<int>(*uniform_name)] = uniform_variable;
	}
}

} //shader_program::detail


//Protected

/*
	Events
*/

void ShaderProgram::Created(variables::AttributeVariable &attribute_variable) noexcept
{
	if (shader_layout_)
	{
		if (auto &name = attribute_variable.Name(); name)
			detail::remap_attribute(GetAttribute(*name), *shader_layout_, mapped_attributes_);
	}
}

void ShaderProgram::Created(variables::UniformVariable &uniform_variable) noexcept
{
	if (shader_layout_)
	{
		if (auto &name = uniform_variable.Name(); name)
			detail::remap_uniform(GetUniform(*name), *shader_layout_, mapped_uniforms_);
	}
}


void ShaderProgram::Removed(variables::AttributeVariable &attribute_variable) noexcept
{
	if (shader_layout_)
	{
		for (auto i = 0; auto &attribute : mapped_attributes_)
		{
			if (attribute.get() == &attribute_variable)
			{
				mapped_attributes_[i] = nullptr;
				break;
			}
			else
				++i;
		}
	}
}

void ShaderProgram::Removed(variables::UniformVariable &uniform_variable) noexcept
{
	if (shader_layout_)
	{
		for (auto i = 0; auto &uniform : mapped_uniforms_)
		{
			if (uniform.get() == &uniform_variable)
			{
				mapped_uniforms_[i] = nullptr;
				break;
			}
			else
				++i;
		}
	}
}


//Public

ShaderProgram::ShaderProgram(std::string name, NonOwningPtr<Shader> shader) :
	resources::Resource<ShaderProgramManager>{std::move(name)}
{
	//The given shader can either be a vertex or a fragment shader, try both
	VertexShader(shader);
	FragmentShader(shader);
}

ShaderProgram::ShaderProgram(std::string name, NonOwningPtr<Shader> shader,
	NonOwningPtr<ShaderLayout> shader_layout) :

	resources::Resource<ShaderProgramManager>{std::move(name)},
	shader_layout_{shader_layout}
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

ShaderProgram::ShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader,
	NonOwningPtr<ShaderLayout> shader_layout) :

	resources::Resource<ShaderProgramManager>{std::move(name)},
	shader_layout_{shader_layout}
{
	//The given shaders much match the correct shader type
	VertexShader(vertex_shader);
	FragmentShader(fragment_shader);
}

ShaderProgram::~ShaderProgram() noexcept
{
	shader_layout_ = nullptr; //Ignore removed events on destruction
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


void ShaderProgram::Layout(NonOwningPtr<ShaderLayout> shader_layout) noexcept
{
	if (shader_layout_ != shader_layout)
	{
		shader_layout_ = shader_layout;

		mapped_attributes_.fill(nullptr);
		mapped_uniforms_.fill(nullptr);

		if (shader_layout_)
		{
			//Remap all attributes
			for (auto &attribute : AttributeVariables())
				Created(attribute); //Reuse functionality
			
			//Remap all uniforms
			for (auto &uniform : UniformVariables())
				Created(uniform); //Reuse functionality
		}
	}
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


NonOwningPtr<variables::AttributeVariable> ShaderProgram::GetAttribute(shader_layout::AttributeName name) noexcept
{
	return mapped_attributes_[static_cast<int>(name)];
}

NonOwningPtr<const variables::AttributeVariable> ShaderProgram::GetAttribute(shader_layout::AttributeName name) const noexcept
{
	return mapped_attributes_[static_cast<int>(name)];
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


NonOwningPtr<variables::UniformVariable> ShaderProgram::GetUniform(shader_layout::UniformName name) noexcept
{
	return mapped_uniforms_[static_cast<int>(name)];
}

NonOwningPtr<const variables::UniformVariable> ShaderProgram::GetUniform(shader_layout::UniformName name) const noexcept
{
	return mapped_uniforms_[static_cast<int>(name)];
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