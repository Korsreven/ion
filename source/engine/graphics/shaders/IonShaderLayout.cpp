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

#include <algorithm>

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


namespace detail
{

bool is_declaration_unique(VariableDeclaration declaration, const variable_binding_map &variable_bindings)
{
	//Declaration has location
	if (declaration.Location())
	{
		//No duplicate locations allowed
		for (auto &binding : variable_bindings)
		{
			if (binding.second.Location() == declaration.Location())
				return false;
		}
	}

	//Declaration has name
	if (declaration.Name())
	{
		//No duplicate names allowed
		for (auto &binding : variable_bindings)
		{
			if (binding.second.Name() == declaration.Name())
				return false;
		}
	}

	return true;
}

variable_binding_map make_bindings(VariableBindings variable_bindings)
{
	//Sort on location (asc)
	std::stable_sort(std::begin(variable_bindings), std::end(variable_bindings),
		[](auto &x, auto &y) noexcept
		{
			return x.second.Location() < y.second.Location();
		});

	//Remove duplicate locations
	variable_bindings.erase(
		std::unique(std::begin(variable_bindings), std::end(variable_bindings),
			[](auto &x, auto &y) noexcept
			{
				return x.second.Location() && y.second.Location() &&
					   x.second.Location() == y.second.Location();
			}), std::end(variable_bindings));


	//Sort on name (asc)
	std::stable_sort(std::begin(variable_bindings), std::end(variable_bindings),
		[](auto &x, auto &y) noexcept
		{
			return x.second.Name() < y.second.Name();
		});

	//Remove duplicate names
	variable_bindings.erase(
		std::unique(std::begin(variable_bindings), std::end(variable_bindings),
			[](auto &x, auto &y) noexcept
			{
				return x.second.Name() && y.second.Name() &&
					   x.second.Name() == y.second.Name();
			}), std::end(variable_bindings));

	return adaptors::FlatMap<std::string, shader_layout::VariableDeclaration>{std::move(variable_bindings)};
}

} //detail
} //shader_layout

ShaderLayout::ShaderLayout(std::string name) :
	managed::ManagedObject<ShaderProgramManager>{std::move(name)}
{
	//Empty
}

ShaderLayout::ShaderLayout(std::string name,
	VariableBindings attribute_bindings, VariableBindings uniform_bindings) :

	managed::ManagedObject<ShaderProgramManager>{std::move(name)},

	attribute_bindings_{detail::make_bindings(std::move(attribute_bindings))},
	uniform_bindings_{detail::make_bindings(std::move(uniform_bindings))}
{
	//Empty
}


/*
	Modifiers
*/

bool ShaderLayout::BindAttribute(std::string name, shader_layout::VariableDeclaration declaration)
{
	if (detail::is_declaration_unique(declaration, attribute_bindings_))
	{
		auto iter = attribute_bindings_.insert({std::move(name), std::move(declaration)});
		return iter.second;
	}
	else
		return false;
}

bool ShaderLayout::BindUniform(std::string name, shader_layout::VariableDeclaration declaration)
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

const VariableDeclaration* ShaderLayout::BindedAttribute(std::string_view name) const noexcept
{
	if (auto iter = attribute_bindings_.find(name); iter != std::end(attribute_bindings_))
		return &iter->second;
	else
		return nullptr;
}

const VariableDeclaration* ShaderLayout::BindedUniform(std::string_view name) const noexcept
{
	if (auto iter = uniform_bindings_.find(name); iter != std::end(uniform_bindings_))
		return &iter->second;
	else
		return nullptr;
}

} //ion::graphics::shaders