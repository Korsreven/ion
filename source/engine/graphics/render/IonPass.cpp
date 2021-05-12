/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonPass.cpp
-------------------------------------------
*/

#include "IonPass.h"

namespace ion::graphics::render
{

using namespace pass;

namespace pass::detail
{
} //pass::detail


Pass::Pass(int iterations, pass::BlendingMode blending, NonOwningPtr<shaders::ShaderProgram> shader_program) noexcept :

	iterations_{iterations},
	blending_{blending},
	shader_program_{shader_program}
{
	//Empty
}


/*
	Static pass conversions
*/

Pass Pass::Default(int iterations, NonOwningPtr<shaders::ShaderProgram> shader_program) noexcept
{
	return {iterations, BlendingMode::Default, shader_program};
}

Pass Pass::Additive(int iterations, NonOwningPtr<shaders::ShaderProgram> shader_program) noexcept
{
	return {iterations, BlendingMode::Additive, shader_program};
}

Pass Pass::Subtractive(int iterations, NonOwningPtr<shaders::ShaderProgram> shader_program) noexcept
{
	return {iterations, BlendingMode::Subtractive, shader_program};
}

Pass Pass::Multiplicative(int iterations, NonOwningPtr<shaders::ShaderProgram> shader_program) noexcept
{
	return {iterations, BlendingMode::Multiplicative, shader_program};
}

} //ion::graphics::render