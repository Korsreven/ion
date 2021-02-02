/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/variables
File:	IonShaderUniform.cpp
-------------------------------------------
*/

#include "IonShaderUniform.h"

#include <cassert>
#include "graphics/shaders/IonShaderStruct.h"

namespace ion::graphics::shaders::variables
{

using namespace uniform_variable;


UniformVariable::UniformVariable(std::string name, uniform_variable::VariableType value) :
	ShaderVariable{std::move(name)},
	value_{std::move(value)}
{
	//Empty
}


/*
	Operators
*/

UniformVariable& UniformVariable::operator[](int off) noexcept
{
	return const_cast<UniformVariable&>(const_cast<const UniformVariable&>(*this)[off]);
}

const UniformVariable& UniformVariable::operator[](int off) const noexcept
{
	assert(parent_struct_ && off < parent_struct_->Size());
	return *(std::begin(parent_struct_->UniformVariables()) + (parent_struct_->Size() * *member_offset_ + off));
}


/*
	Values
*/

bool UniformVariable::HasNewValue() noexcept
{
	auto changed = 
		current_value_ ?
			Visit(
				[&](auto &&value) noexcept
				{
					return uniform_variable::detail::is_value_different(
						value,
						std::get<std::remove_cvref_t<decltype(value)>>(*current_value_));
				}) :
			true;

	if (changed)
		current_value_ = value_;
				
	return changed;
}

void UniformVariable::Refresh() noexcept
{
	current_value_.reset();
}

} //ion::graphics::shaders::variables