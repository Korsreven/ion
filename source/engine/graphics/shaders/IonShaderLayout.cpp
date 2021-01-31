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


namespace detail
{

adaptors::FlatMap<StructName, std::string> make_struct_binding_map(StructBindings struct_bindings) noexcept
{
	//Sort on name (asc)
	std::stable_sort(std::begin(struct_bindings), std::end(struct_bindings),
		[](auto &x, auto &y) noexcept
		{
			return x.second < y.second;
		});

	//Remove duplicate names
	struct_bindings.erase(
		std::unique(std::begin(struct_bindings), std::end(struct_bindings),
			[](auto &x, auto &y) noexcept
			{
				return x.second == y.second;
			}), std::end(struct_bindings));

	return adaptors::FlatMap<StructName, std::string>{std::move(struct_bindings)};
}

bool is_struct_unique(std::string_view name, const struct_binding_map &struct_bindings) noexcept
{
	//No duplicate names allowed
	for (auto &binding : struct_bindings)
	{
		if (binding.second == name)
			return false;
	}

	return true;
}

} //detail

} //shader_layout

ShaderLayout::ShaderLayout(std::string name) :
	managed::ManagedObject<ShaderProgramManager>{std::move(name)}
{
	//Empty
}

ShaderLayout::ShaderLayout(std::string name, shader_layout::StructBindings struct_bindings,
	AttributeBindings attribute_bindings, UniformBindings uniform_bindings) :

	managed::ManagedObject<ShaderProgramManager>{std::move(name)},

	struct_bindings_{detail::make_struct_binding_map(std::move(struct_bindings))},
	attribute_bindings_{detail::make_variable_binding_map(std::move(attribute_bindings))},
	uniform_bindings_{detail::make_variable_binding_map(std::move(uniform_bindings))}
{
	//Empty
}


/*
	Modifiers
*/

bool ShaderLayout::BindStruct(shader_layout::StructName name, std::string str_name)
{
	if (detail::is_struct_unique(str_name, struct_bindings_))
	{
		auto iter = struct_bindings_.insert({name, std::move(str_name)});
		return iter.second;
	}
	else
		return false;
}

bool ShaderLayout::BindAttribute(AttributeName name, shader_layout::VariableDeclaration declaration)
{
	if (detail::is_declaration_unique(declaration, attribute_bindings_))
	{
		auto iter = attribute_bindings_.insert({name, std::move(declaration)});
		return iter.second;
	}
	else
		return false;
}

bool ShaderLayout::BindUniform(UniformName name, shader_layout::VariableDeclaration declaration)
{
	if (detail::is_declaration_unique(declaration, uniform_bindings_))
	{
		auto iter = uniform_bindings_.insert({name, std::move(declaration)});
		return iter.second;
	}
	else
		return false;
}


/*
	Observers
*/

std::optional<std::string> ShaderLayout::BoundStruct(shader_layout::StructName name) const noexcept
{
	if (auto iter = struct_bindings_.find(name); iter != std::end(struct_bindings_))
		return iter->second;
	else
		return {};
}

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


std::optional<shader_layout::StructName> ShaderLayout::GetStructName(std::string_view name) const noexcept
{
	for (auto &[key, value] : struct_bindings_)
	{
		if (value == name)
			return key;
	}

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