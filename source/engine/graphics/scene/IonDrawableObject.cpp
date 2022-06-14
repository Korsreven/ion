/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonDrawableObject.cpp
-------------------------------------------
*/

#include "IonDrawableObject.h"

#include <algorithm>
#include <cassert>

#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "query/IonSceneQuery.h"

namespace ion::graphics::scene
{

using namespace drawable_object;

namespace drawable_object::detail
{
} //drawable_object::detail


//Protected

/*
	Events
*/

void DrawableObject::OpacityChanged() noexcept
{
	//Empty
}


//Public

DrawableObject::DrawableObject(std::optional<std::string> name, bool visible) :
	MovableObject{std::move(name), visible}
{
	query_type_flags_ |= query::scene_query::QueryType::Drawable;
}


/*
	Rendering
*/

void DrawableObject::Render() noexcept
{
	Prepare();

	//No passes added, add default pass
	if (std::empty(passes_))
		AddPass(render::Pass{});

	static NonOwningPtr<shaders::ShaderProgram> active_shader_program = nullptr;

	for (auto &pass : passes_)
	{
		pass.Blend();

		auto shader_program = pass.RenderProgram().get();
		auto active_program = active_shader_program.get();
		auto use_shader = shader_program && shader_program->Owner() && shader_program->Handle();

		//Switch shader program
		if (shader_program != active_program)
		{
			if (use_shader) //Custom pipeline
				shader_program->Owner()->ActivateShaderProgram(*shader_program);
			else if (active_program) //Fixed-function pipeline
				active_program->Owner()->DeactivateShaderProgram(*active_program);

			active_shader_program = pass.RenderProgram();
		}

		for (auto iterations = pass.Iterations(); iterations > 0; --iterations)
			Draw(shader_program);
	}

	//Optimization
	//When not deactivating the active shader program below...
	//a program will always be active until switched with another program

	//if (use_shader)
	//	shader_program->Owner()->DeactivateShaderProgram(*shader_program);

	MovableObject::Render(); //Render bounding volumes
}

const movable_object::ShaderPrograms& DrawableObject::RenderPrograms(bool derive) const
{
	if (derive)
	{
		shader_programs_.clear();

		for (auto &pass : passes_)
		{
			if (auto shader_program = pass.RenderProgram().get(); shader_program)
			{
				//There is probably <= 3 distinct shader programs per drawable object
				//So std::find with its linear complexity will be the fastest method to make sure each added element is unique
				if (std::find(std::begin(shader_programs_), std::end(shader_programs_), shader_program) == std::end(shader_programs_))
					shader_programs_.push_back(shader_program); //Only distinct
			}
		}
	}
	
	return shader_programs_;
}


/*
	Elapse time
*/

void DrawableObject::Elapse([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}


/*
	Passes
	Adding
*/

void DrawableObject::AddPass(render::Pass pass)
{
	passes_.push_back(std::move(pass));
}

void DrawableObject::AddPasses(drawable_object::Passes passes)
{
	if (std::empty(passes_))
		passes_ = std::move(passes);
	else
		std::move(std::begin(passes), std::end(passes), std::back_inserter(passes_));
}


/*
	Passes
	Retrieving
*/

render::Pass& DrawableObject::GetPass(int off) noexcept
{
	assert(off >= 0 && off < std::ssize(passes_));
	return passes_[off];
}

const render::Pass& DrawableObject::GetPass(int off) const noexcept
{
	assert(off >= 0 && off < std::ssize(passes_));
	return passes_[off];
}


/*
	Passes
	Removing
*/

void DrawableObject::ClearPasses() noexcept
{
	passes_.clear();
	passes_.shrink_to_fit();
}

bool DrawableObject::RemovePass(int off) noexcept
{
	if (off >= 0 && off < std::ssize(passes_))
	{
		passes_.erase(std::begin(passes_) + off);
		return true;
	}
	else
		return false;
}

} //ion::graphics::scene