/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/variables
File:	IonShaderAttribute.cpp
-------------------------------------------
*/

#include "IonShaderAttribute.h"

namespace ion::graphics::shaders::variables
{

using namespace attribute_variable;


AttributeVariable::AttributeVariable(std::string name, attribute_variable::VariableType value) noexcept :
	ShaderVariable{std::move(name)},
	value_{std::move(value)}
{
	//Empty
}


/*
	Values
*/

bool AttributeVariable::HasNewValue() noexcept
{
	auto changed = 
		current_value_ ?
			Visit(
				[&](auto &&value) noexcept
				{
					return attribute_variable::detail::is_value_different(
						value,
						std::get<std::remove_cvref_t<decltype(value)>>(*current_value_));
				}) :
			true;

	if (changed)
		current_value_ = value_;
				
	return changed;
}

void AttributeVariable::Refresh() noexcept
{
	current_value_.reset();
}

} //ion::graphics::shaders::variables