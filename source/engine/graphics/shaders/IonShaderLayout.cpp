/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderLayout.cpp
-------------------------------------------
*/

#include "IonShaderLayout.h"

namespace ion::graphics::shaders
{

using namespace shader_layout;

namespace shader_layout
{

VariableDeclaration::VariableDeclaration(std::string name) :
	name_{name}
{
	//Empty
}

VariableDeclaration::VariableDeclaration(int location) noexcept :
	location_{location}
{
	//Empty
}

VariableDeclaration::VariableDeclaration(std::string name, int location) :
	name_{name},
	location_{location}
{
	//Empty
}

} //shader_layout

ShaderLayout::ShaderLayout(std::string name) :
	managed::ManagedObject<ShaderProgramManager>{std::move(name)}
{
	//Empty
}

ShaderLayout::ShaderLayout(std::string name,
	AttributeBindings attribute_bindings, UniformBindings uniform_bindings) :

	managed::ManagedObject<ShaderProgramManager>{std::move(name)},

	attribute_bindings_{detail::make_binding_map(std::move(attribute_bindings))},
	uniform_bindings_{detail::make_binding_map(std::move(uniform_bindings))}
{
	//Empty
}


/*
	Modifiers
*/

bool ShaderLayout::BindAttribute(AttributeName name, shader_layout::VariableDeclaration declaration)
{
	if (detail::is_declaration_unique(declaration, attribute_bindings_))
	{
		auto iter = attribute_bindings_.insert({std::move(name), std::move(declaration)});
		return iter.second;
	}
	else
		return false;
}

bool ShaderLayout::BindUniform(UniformName name, shader_layout::VariableDeclaration declaration)
{
	if (detail::is_declaration_unique(declaration, uniform_bindings_))
	{
		auto iter = uniform_bindings_.insert({std::move(name), std::move(declaration)});
		return iter.second;
	}
	else
		return false;
}


/*
	Observers
*/

std::optional<shader_layout::VariableDeclaration> ShaderLayout::BoundAttribute(AttributeName name) const noexcept
{
	if (auto iter = attribute_bindings_.find(name); iter != std::end(attribute_bindings_))
		return iter->second;
	else
		return {};
}

std::optional<shader_layout::VariableDeclaration> ShaderLayout::BoundUniform(UniformName name) const noexcept
{
	if (auto iter = uniform_bindings_.find(name); iter != std::end(uniform_bindings_))
		return iter->second;
	else
		return {};
}


std::optional<shader_layout::AttributeName> ShaderLayout::GetAttributeName(std::string_view name) const noexcept
{
	for (auto &[key, value] : attribute_bindings_)
	{
		if (value.Name() && *value.Name() == name)
			return key;
	}

	return {};
}

std::optional<shader_layout::UniformName> ShaderLayout::GetUniformName(std::string_view name) const noexcept
{
	for (auto &[key, value] : uniform_bindings_)
	{
		if (value.Name() && *value.Name() == name)
			return key;
	}

	return {};
}


std::optional<shader_layout::AttributeName> ShaderLayout::GetAttributeName(int location) const noexcept
{
	for (auto &[key, value] : attribute_bindings_)
	{
		if (value.Location() && *value.Location() == location)
			return key;
	}

	return {};
}

std::optional<shader_layout::UniformName> ShaderLayout::GetUniformName(int location) const noexcept
{
	for (auto &[key, value] : uniform_bindings_)
	{
		if (value.Location() && *value.Location() == location)
			return key;
	}

	return {};
}

} //ion::graphics::shaders