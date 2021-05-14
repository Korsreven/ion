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

#include <cassert>

namespace ion::graphics::scene
{

using namespace drawable_object;

namespace drawable_object::detail
{
} //drawable_object::detail


DrawableObject::DrawableObject(bool visible) :
	MovableObject{visible}
{
	//Empty
}

DrawableObject::DrawableObject(std::string name, bool visible) :
	MovableObject{std::move(name), visible}
{
	//Empty
}

DrawableObject::DrawableObject(const DrawableObject &rhs) noexcept :
	MovableObject{rhs}
{
	//Empty
}


/*
	Rendering
*/

void DrawableObject::Render(duration time) noexcept
{
	Elapse(time);
	Prepare();

	for (auto &pass : passes_)
	{
		pass.Blend();

		auto shader_program = pass.ShaderProg().get();
		for (auto iterations = pass.Iterations(); iterations > 0; --iterations)
			Draw(shader_program);
	}
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