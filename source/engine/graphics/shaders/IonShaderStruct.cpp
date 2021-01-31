/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderStruct.cpp
-------------------------------------------
*/

#include "IonShaderStruct.h"

#include "IonShaderProgram.h"
#include "utilities/IonStringUtility.h"

namespace ion::graphics::shaders
{

using namespace shader_struct;

namespace shader_struct::detail
{

std::string get_fully_qualified_name(std::string_view struct_name, std::string_view variable_name, std::optional<int> off) noexcept
{
	using namespace ion::utilities;

	if (off)
		return string::Concat(struct_name, '[', *off, "].", variable_name);
	else
		return string::Concat(struct_name, '.', variable_name);
}

} //shader_struct::detail

//Protected

/*
	Events
*/

void ShaderStruct::Created(variables::UniformVariable &uniform_variable) noexcept
{
	if (size_ == 1)
		Owner()->Created(uniform_variable); //Use ShaderProgram functionality
}

void ShaderStruct::Removed(variables::UniformVariable &uniform_variable) noexcept
{
	if (size_ == 1)
		Owner()->Removed(uniform_variable); //Use ShaderProgram functionality
}


//Public

ShaderStruct::ShaderStruct(std::string name, int size) :
	
	managed::ManagedObject<ShaderProgram>{std::move(name)},
	size_{size > 0 ? size : 1}
{
	//Empty
}


/*
	Uniform variables
	Retrieving
*/

NonOwningPtr<variables::UniformVariable> ShaderStruct::GetUniform(std::string_view name, std::optional<int> off) noexcept
{
	return Get(detail::get_fully_qualified_name(*Name(), name, off));
}

NonOwningPtr<const variables::UniformVariable> ShaderStruct::GetUniform(std::string_view name, std::optional<int> off) const noexcept
{
	return Get(detail::get_fully_qualified_name(*Name(), name, off));
}


NonOwningPtr<variables::UniformVariable> ShaderStruct::GetUniform(shader_layout::UniformName name) noexcept
{
	return size_ == 1 ? Owner()->GetUniform(name) : nullptr;
}

NonOwningPtr<const variables::UniformVariable> ShaderStruct::GetUniform(shader_layout::UniformName name) const noexcept
{
	return size_ == 1 ? Owner()->GetUniform(name) : nullptr;
}


/*
	Uniform variables
	Removing
*/

void ShaderStruct::ClearUniforms() noexcept
{
	Clear();
}

bool ShaderStruct::RemoveUniform(variables::UniformVariable &uniform_variable) noexcept
{
	return Remove(uniform_variable);
}

bool ShaderStruct::RemoveUniform(std::string_view name, std::optional<int> off) noexcept
{
	return Remove(detail::get_fully_qualified_name(*Name(), name, off));
}

} //ion::graphics::shaders