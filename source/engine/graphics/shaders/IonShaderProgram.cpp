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

#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::shaders
{

using namespace shader_program;

namespace shader_program::detail
{

void remap_struct(NonOwningPtr<ShaderStruct> shader_struct, ShaderLayout &shader_layout, mapped_structs &shader_structs) noexcept
{
	//Map with name
	if (auto name = shader_layout.GetStructName(*shader_struct->Name()); name)
		shader_structs[static_cast<int>(*name)] = shader_struct;
}

void remap_attribute(NonOwningPtr<variables::AttributeVariable> attribute_variable, ShaderLayout &shader_layout, mapped_attributes &attributes) noexcept
{
	//Map with name
	if (auto name = shader_layout.GetAttributeName(*attribute_variable->Name()); name)
		attributes[static_cast<int>(*name)] = attribute_variable;

	//Has location
	else if (auto location = attribute_variable->Location(); location)
	{
		//Map with location
		if (name = shader_layout.GetAttributeName(*location); name)
			attributes[static_cast<int>(*name)] = attribute_variable;
	}
}

void remap_uniform(NonOwningPtr<variables::UniformVariable> uniform_variable, ShaderLayout &shader_layout, mapped_uniforms &uniforms) noexcept
{
	//Map with name
	if (auto name = shader_layout.GetUniformName(*uniform_variable->Name()); name)
		uniforms[static_cast<int>(*name)] = uniform_variable;

	//Has location
	else if (auto location = uniform_variable->Location(); location)
	{
		//Map with location
		if (name = shader_layout.GetUniformName(*location); name)
			uniforms[static_cast<int>(*name)] = uniform_variable;
	}
}


int get_next_texture_unit(int &next_texture_unit) noexcept
{
	static auto max_texture_units = gl::MaxTextureUnits();

	if (next_texture_unit < max_texture_units)
		return next_texture_unit++;
	else
		return -1; //Invalid unit
}

} //shader_program::detail


//Protected

/*
	Events
*/

void ShaderProgram::Created(ShaderStruct &shader_struct) noexcept
{
	if (shader_layout_)
		detail::remap_struct(
			shader_struct.Owner()->GetStruct(*shader_struct.Name()), *shader_layout_, mapped_structs_);
}

void ShaderProgram::Created(variables::AttributeVariable &attribute_variable) noexcept
{
	if (shader_layout_)
		detail::remap_attribute(
			attribute_variable.Owner()->GetAttribute(*attribute_variable.Name()), *shader_layout_, mapped_attributes_);
}

void ShaderProgram::Created(variables::UniformVariable &uniform_variable) noexcept
{
	if (shader_layout_)
		detail::remap_uniform(
			uniform_variable.Owner()->GetUniform(*uniform_variable.Name()), *shader_layout_, mapped_uniforms_);

	uniform_variable.Visit(	
		//1D sampler types
		[&](variables::glsl::uniform<variables::glsl::isampler1D> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::usampler1D> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::sampler1D> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::isampler1DArray> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::usampler1DArray> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::sampler1DArray> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},

		//2D sampler types
		[&](variables::glsl::uniform<variables::glsl::isampler2D> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::usampler2D> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::sampler2D> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::isampler2DArray> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::usampler2DArray> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[&](variables::glsl::uniform<variables::glsl::sampler2DArray> &sampler)
		{
			sampler = detail::get_next_texture_unit(next_texture_unit_);
		},
		[](auto&&) {}
	);
}


void ShaderProgram::Removed(ShaderStruct &shader_struct) noexcept
{
	if (shader_layout_)
	{
		for (auto i = 0; auto &s_struct : mapped_structs_)
		{
			if (s_struct.get() == &shader_struct)
			{
				mapped_structs_[i] = nullptr;
				break;
			}
			else
				++i;
		}
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

ShaderProgram::ShaderProgram(std::string name, NonOwningPtr<Shader> shader) noexcept :
	resources::Resource<ShaderProgramManager>{std::move(name)}
{
	//The given shader can either be a vertex or a fragment shader, try both
	VertexShader(shader);
	FragmentShader(shader);
}

ShaderProgram::ShaderProgram(std::string name, NonOwningPtr<Shader> shader,
	NonOwningPtr<ShaderLayout> shader_layout) noexcept :

	resources::Resource<ShaderProgramManager>{std::move(name)},
	shader_layout_{shader_layout}
{
	//The given shader can either be a vertex or a fragment shader, try both
	VertexShader(shader);
	FragmentShader(shader);
}

ShaderProgram::ShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader) noexcept :
	resources::Resource<ShaderProgramManager>{std::move(name)}
{
	//The given shaders much match the correct shader type
	VertexShader(vertex_shader);
	FragmentShader(fragment_shader);
}

ShaderProgram::ShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader,
	NonOwningPtr<ShaderLayout> shader_layout) noexcept :

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

		mapped_structs_.fill(nullptr);
		mapped_attributes_.fill(nullptr);
		mapped_uniforms_.fill(nullptr);

		if (shader_layout_)
		{
			//Remap all structs
			for (auto &shader_struct : Structs())
				Created(shader_struct); //Reuse functionality

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
	Shader structs
	Creating
*/

NonOwningPtr<ShaderStruct> ShaderProgram::CreateStruct(std::string name, int size)
{
	return ShaderStructBase::Create(std::move(name), size);
}


/*
	Shader structs
	Retrieving
*/

NonOwningPtr<ShaderStruct> ShaderProgram::GetStruct(std::string_view name) noexcept
{
	return ShaderStructBase::Get(name);
}

NonOwningPtr<const ShaderStruct> ShaderProgram::GetStruct(std::string_view name) const noexcept
{
	return ShaderStructBase::Get(name);
}


NonOwningPtr<ShaderStruct> ShaderProgram::GetStruct(shader_layout::StructName name) noexcept
{
	return mapped_structs_[static_cast<int>(name)];
}

NonOwningPtr<const ShaderStruct> ShaderProgram::GetStruct(shader_layout::StructName name) const noexcept
{
	return mapped_structs_[static_cast<int>(name)];
}


/*
	Shader structs
	Removing
*/

void ShaderProgram::ClearStructs() noexcept
{
	mapped_structs_.fill(nullptr);
	ShaderStructBase::Clear();
}

bool ShaderProgram::RemoveStruct(ShaderStruct &shader_struct) noexcept
{
	return ShaderStructBase::Remove(shader_struct);
}

bool ShaderProgram::RemoveStruct(std::string_view name) noexcept
{
	return ShaderStructBase::Remove(name);
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
	mapped_attributes_.fill(nullptr);
	AttributeVariablesBase::Clear();
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
	next_texture_unit_ = 0;
	mapped_uniforms_.fill(nullptr);
	UniformVariablesBase::Clear();

	//Clear from structs
	for (auto &shader_struct : Structs())
		shader_struct.ClearUniforms();
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


/*
	Struct / variables
	Removing
*/

void ShaderProgram::Clear() noexcept
{
	ClearVariables();
	ClearStructs();
}

} //ion::graphics::shaders