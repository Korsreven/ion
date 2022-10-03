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

#include "graphics/render/IonRenderPrimitive.h"
#include "graphics/scene/IonSceneManager.h"
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

void DrawableObject::AddPrimitive(render::RenderPrimitive &primitive)
{
	if (auto renderer = ParentRenderer(); renderer)
	{
		if (std::empty(primitive.RenderPasses()))
			primitive.RenderPasses(passes_);

		primitive.Opacity(opacity_);

		renderer->AddPrimitive(primitive);
		render_primitives_.push_back(&primitive);
	}
}

void DrawableObject::RemovePrimitive(render::RenderPrimitive &primitive) noexcept
{
	if (auto iter = std::find(std::begin(render_primitives_), std::end(render_primitives_), &primitive);
		iter != std::end(render_primitives_))
		render_primitives_.erase(iter);
}

void DrawableObject::UpdatePassesOnAllPrimitives(const render::pass::Passes &passes) noexcept
{
	for (auto &primitive : render_primitives_)
		primitive->RenderPasses(passes);
}

void DrawableObject::UpdateOpacityOnAllPrimitives(real opacity) noexcept
{
	for (auto &primitive : render_primitives_)
		primitive->Opacity(opacity);
}


//Public

DrawableObject::DrawableObject(std::optional<std::string> name, bool visible) :
	MovableObject{std::move(name), visible}
{
	query_type_flags_ |= query::scene_query::QueryType::Drawable;
}


/*
	Modifiers
*/

void DrawableObject::Opacity(real opacity) noexcept
{
	if (opacity_ != opacity)
	{
		opacity_ = opacity;
		UpdateOpacityOnAllPrimitives(opacity);
	}
}


/*
	Observers
*/

movable_object::RenderPrimitiveRange DrawableObject::AllRenderPrimitives() noexcept
{
	return render_primitives_;
}

movable_object::ShaderProgramRange DrawableObject::AllShaderPrograms() noexcept
{
	return shader_programs_;
}


/*
	Notifying
*/

void DrawableObject::NotifyPassesChanged([[maybe_unused]] render::RenderPrimitive &primitive) noexcept
{
	update_passes_ = true;
}


/*
	Rendering
*/

void DrawableObject::Prepare()
{
	//No passes added, add pass with default shader program (if any)
	if (std::empty(passes_))
	{
		auto default_shader_program =
			[&]() noexcept -> NonOwningPtr<shaders::ShaderProgram>
			{
				if (auto owner = Owner(); owner)
					return owner->GetDefaultShaderProgram(query_type_flags_);
				else
					return nullptr;
			}();

		AddPass(render::Pass{default_shader_program});
	}

	if (update_passes_)
	{
		shader_programs_.clear();

		//Update all shader programs
		for (auto &primitive : render_primitives_)
		{
			for (auto &pass : primitive->RenderPasses())
			{
				if (auto shader_program = pass.RenderProgram(); shader_program)
				{
					//There are not many distinct shader programs per drawable object
					//So std::find with its linear complexity will be the fastest method to make sure each added element is unique
					if (std::find(std::begin(shader_programs_), std::end(shader_programs_), shader_program) == std::end(shader_programs_))
						shader_programs_.push_back(shader_program); //Only distinct
				}
			}
		}

		update_passes_ = false;
	}

	//Set render primitive visibility
	if (!std::empty(render_primitives_))
	{
		auto parent_node = ParentNode();
		auto node_visible = parent_node && parent_node->Visible();

		for (auto &primitive : render_primitives_)
		{
			auto world_visible = node_visible && visible_ && primitive->Visible();
			primitive->WorldVisible(world_visible);

			if (world_visible)
				primitive->ModelMatrix(parent_node->FullTransformation());
		}
	}

	MovableObject::Prepare();
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
	UpdatePassesOnAllPrimitives(passes_);
}

void DrawableObject::AddPasses(render::pass::Passes passes)
{
	if (std::empty(passes_))
		passes_ = std::move(passes);
	else
		std::move(std::begin(passes), std::end(passes), std::back_inserter(passes_));

	UpdatePassesOnAllPrimitives(passes_);
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
	UpdatePassesOnAllPrimitives(passes_);
}

bool DrawableObject::RemovePass(int off) noexcept
{
	if (off >= 0 && off < std::ssize(passes_))
	{
		passes_.erase(std::begin(passes_) + off);
		UpdatePassesOnAllPrimitives(passes_);
		return true;
	}
	else
		return false;
}

} //ion::graphics::scene