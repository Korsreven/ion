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

std::string_view get_unqualified_name(std::string_view variable_name) noexcept
{
	if (auto off = variable_name.find_last_of('.'); off != std::string_view::npos)
		variable_name.remove_prefix(off + 1);

	return variable_name;
}

} //shader_struct::detail

//Protected

/*
	Events
*/

void ShaderStruct::Created(variables::UniformVariable &uniform_variable) noexcept
{
	Owner()->Created(uniform_variable); //Use ShaderProgram functionality
	uniform_variable.ParentStruct(*this, std::size(mapped_members_));
}

void ShaderStruct::Removed(variables::UniformVariable &uniform_variable) noexcept
{
	Owner()->Removed(uniform_variable); //Use ShaderProgram functionality

	for (auto i = 0; auto &uniform : mapped_members_)
	{
		if (uniform.get() == &uniform_variable)
		{
			mapped_members_.erase(std::begin(mapped_members_) + i);

			//Refresh member offsets
			for (auto &member : UniformVariables())
			{
				if (member.MemberOffset() > uniform_variable.MemberOffset())
					member.ParentStruct(*this, *member.MemberOffset() - 1);
			}

			break;
		}
		else
			++i;
	}
}


//Public

ShaderStruct::ShaderStruct(std::string name, int size) noexcept :
	
	managed::ManagedObject<ShaderProgram>{std::move(name)},
	size_{size > 0 ? size : 1}
{
	//Empty
}


/*
	Uniform variables - Retrieving
*/

NonOwningPtr<variables::UniformVariable> ShaderStruct::GetUniform(std::string_view name, std::optional<int> off) noexcept
{
	return Get(detail::get_fully_qualified_name(*Name(), name, off));
}

NonOwningPtr<const variables::UniformVariable> ShaderStruct::GetUniform(std::string_view name, std::optional<int> off) const noexcept
{
	return Get(detail::get_fully_qualified_name(*Name(), name, off));
}


/*
	Uniform variables - Removing
*/

void ShaderStruct::ClearUniforms() noexcept
{
	mapped_members_.clear();
	mapped_members_.shrink_to_fit();
	Clear();
}

bool ShaderStruct::RemoveUniform(variables::UniformVariable &uniform_variable) noexcept
{
	if (size_ > 1)
	{
		auto removed = true;

		for (auto i = size_; i > 0; --i)
			removed &= Remove(uniform_variable[i - 1]);

		return removed;
	}
	else
		return Remove(uniform_variable);
}

bool ShaderStruct::RemoveUniform(std::string_view name, std::optional<int> off) noexcept
{
	if (size_ > 1)
	{
		if (auto uniform_variable = GetUniform(name, off); uniform_variable)
			return RemoveUniform(*uniform_variable);
		else
			return false;
	}
	else
		return Remove(detail::get_fully_qualified_name(*Name(), name, off));
}

} //ion::graphics::shaders