/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderPass.cpp
-------------------------------------------
*/

#include "IonRenderPass.h"

#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::render
{

using namespace render_pass;

namespace render_pass::detail
{

int render_pass_blend_factor_to_gl_blend_factor(BlendFactor factor) noexcept
{
	switch (factor)
	{
		case BlendFactor::SourceColor:
		return GL_SRC_COLOR;

		case BlendFactor::OneMinus_SourceColor:
		return GL_ONE_MINUS_SRC_COLOR;

		case BlendFactor::DestinationColor:
		return GL_DST_COLOR;

		case BlendFactor::OneMinus_DestinationColor:
		return GL_ONE_MINUS_DST_COLOR;


		case BlendFactor::SourceAlpha:
		return GL_SRC_ALPHA;

		case BlendFactor::OneMinus_SourceAlpha:
		return GL_ONE_MINUS_SRC_ALPHA;

		case BlendFactor::DestinationAlpha:
		return GL_DST_ALPHA;

		case BlendFactor::OneMinus_DestinationAlpha:
		return GL_ONE_MINUS_DST_ALPHA;


		case BlendFactor::ConstantColor:
		return GL_CONSTANT_COLOR;

		case BlendFactor::OneMinus_ConstantColor:
		return GL_ONE_MINUS_CONSTANT_COLOR;

		case BlendFactor::ConstantAlpha:
		return GL_CONSTANT_ALPHA;

		case BlendFactor::OneMinus_ConstantAlpha:
		return GL_ONE_MINUS_CONSTANT_ALPHA;


		case BlendFactor::SourceOneColor:
		return GL_SRC1_COLOR;

		case BlendFactor::OneMinus_SourceOneColor:
		return GL_ONE_MINUS_SRC1_COLOR;

		case BlendFactor::SourceOneAlpha:
		return GL_SRC1_ALPHA;

		case BlendFactor::OneMinus_SourceOneAlpha:
		return GL_ONE_MINUS_SRC1_ALPHA;


		case BlendFactor::SourceAlphaSaturate:
		return GL_SRC_ALPHA_SATURATE;


		case BlendFactor::Zero:
		return GL_ZERO;

		case BlendFactor::One:
		default:
		return GL_ONE;
	}
}

int render_pass_blend_equation_mode_to_gl_blend_equation_mode(BlendEquationMode mode) noexcept
{
	switch (mode)
	{
		case BlendEquationMode::Subtract:
		return GL_FUNC_SUBTRACT;

		case BlendEquationMode::ReverseSubtract:
		return GL_FUNC_REVERSE_SUBTRACT;

		case BlendEquationMode::Min:
		return GL_MIN;

		case BlendEquationMode::Max:
		return GL_MAX;

		case BlendEquationMode::Add:
		default:
		return GL_FUNC_ADD;
	}
}


/*
	Graphics API
*/

void blend(BlendFactor source_factor, BlendFactor destination_factor, BlendEquationMode equation_mode) noexcept
{
	glBlendFunc(render_pass_blend_factor_to_gl_blend_factor(source_factor), render_pass_blend_factor_to_gl_blend_factor(destination_factor));
	glBlendEquation(render_pass_blend_equation_mode_to_gl_blend_equation_mode(equation_mode));
	glEnable(GL_BLEND);
}

void blend_separate(BlendFactor source_factor, BlendFactor destination_factor,
					BlendFactor source_factor_alpha, BlendFactor destination_factor_alpha,
					BlendEquationMode equation_mode, BlendEquationMode equation_mode_alpha) noexcept
{
	switch (gl::BlendFuncSeparate_Support())
	{
		case gl::Extension::Core:
		glBlendFuncSeparate(render_pass_blend_factor_to_gl_blend_factor(source_factor), render_pass_blend_factor_to_gl_blend_factor(destination_factor),
							render_pass_blend_factor_to_gl_blend_factor(source_factor_alpha), render_pass_blend_factor_to_gl_blend_factor(destination_factor_alpha));
		break;

		case gl::Extension::EXT:
		glBlendFuncSeparateEXT(render_pass_blend_factor_to_gl_blend_factor(source_factor), render_pass_blend_factor_to_gl_blend_factor(destination_factor),
							   render_pass_blend_factor_to_gl_blend_factor(source_factor_alpha), render_pass_blend_factor_to_gl_blend_factor(destination_factor_alpha));
		break;

		default:
		{
			//Blend non-separated instead
			blend(source_factor, destination_factor, equation_mode);
			return;
		}
	}

	switch (gl::BlendEquationSeparate_Support())
	{
		case gl::Extension::Core:
		glBlendEquationSeparate(render_pass_blend_equation_mode_to_gl_blend_equation_mode(equation_mode),
								render_pass_blend_equation_mode_to_gl_blend_equation_mode(equation_mode_alpha));
		break;

		case gl::Extension::EXT:
		glBlendEquationSeparateEXT(render_pass_blend_equation_mode_to_gl_blend_equation_mode(equation_mode),
								   render_pass_blend_equation_mode_to_gl_blend_equation_mode(equation_mode_alpha));
		break;
	}

	glEnable(GL_BLEND);
}

} //render_pass::detail


RenderPass::RenderPass(NonOwningPtr<shaders::ShaderProgram> shader_program, int iterations) noexcept :

	iterations_{iterations},
	shader_program_{shader_program}
{
	//Empty
}


RenderPass::RenderPass(BlendFactor blend_source_factor, BlendFactor blend_destination_factor,
	NonOwningPtr<shaders::ShaderProgram> shader_program, int iterations) noexcept :

	iterations_{iterations},

	blend_source_factor_{blend_source_factor},
	blend_destination_factor_{blend_destination_factor},

	shader_program_{shader_program}
{
	//Empty
}

RenderPass::RenderPass(BlendFactor blend_source_factor, BlendFactor blend_destination_factor,
	BlendEquationMode blend_equation_mode,
	NonOwningPtr<shaders::ShaderProgram> shader_program, int iterations) noexcept :

	iterations_{iterations},

	blend_source_factor_{blend_source_factor},
	blend_destination_factor_{blend_destination_factor},
	blend_equation_mode_{blend_equation_mode},

	shader_program_{shader_program}
{
	//Empty
}


RenderPass::RenderPass(BlendFactor blend_source_factor, BlendFactor blend_destination_factor,
	BlendFactor blend_source_factor_alpha_, BlendFactor blend_destination_factor_alpha,
	NonOwningPtr<shaders::ShaderProgram> shader_program, int iterations) noexcept :

	iterations_{iterations},

	blend_source_factor_{blend_source_factor},
	blend_destination_factor_{blend_destination_factor},
	blend_source_factor_alpha_{blend_source_factor_alpha_},
	blend_destination_factor_alpha_{blend_destination_factor_alpha},

	shader_program_{shader_program}
{
	//Empty
}

RenderPass::RenderPass(BlendFactor blend_source_factor, BlendFactor blend_destination_factor,
	BlendFactor blend_source_factor_alpha_, BlendFactor blend_destination_factor_alpha,
	BlendEquationMode blend_equation_mode, BlendEquationMode blend_equation_mode_alpha,
	NonOwningPtr<shaders::ShaderProgram> shader_program, int iterations) noexcept :

	iterations_{iterations},

	blend_source_factor_{blend_source_factor},
	blend_destination_factor_{blend_destination_factor},
	blend_source_factor_alpha_{blend_source_factor_alpha_},
	blend_destination_factor_alpha_{blend_destination_factor_alpha},

	blend_equation_mode_{blend_equation_mode},
	blend_equation_mode_alpha_{blend_equation_mode_alpha},

	shader_program_{shader_program}
{
	//Empty
}


/*
	Blending
*/

void RenderPass::Blend() const noexcept
{
	if (blend_source_factor_alpha_ && blend_destination_factor_alpha_)
		detail::blend_separate(blend_source_factor_, blend_destination_factor_,
							   *blend_source_factor_alpha_, *blend_destination_factor_alpha_,
							   blend_equation_mode_, blend_equation_mode_alpha_.value_or(blend_equation_mode_));
	else
		detail::blend(blend_source_factor_, blend_destination_factor_, blend_equation_mode_);
}

} //ion::graphics::render