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

#include "graphics/render/IonRenderer.h"
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
			primitive.RenderPasses(render_passes_);

		primitive.Opacity(opacity_);
		render_primitives_.push_back(&primitive);

		if (AttachedToSceneGraph())
			renderer->AddPrimitive(primitive);
	}
}

void DrawableObject::RemovePrimitive(render::RenderPrimitive &primitive) noexcept
{
	if (auto iter = std::find(std::begin(render_primitives_), std::end(render_primitives_), &primitive);
		iter != std::end(render_primitives_))
		render_primitives_.erase(iter);
}


void DrawableObject::UpdateRenderPassesOnAllPrimitives(const render::render_pass::Passes &passes) noexcept
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

DrawableObject::DrawableObject(std::optional<std::string> name, bool visible) noexcept :
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

void DrawableObject::NotifyRenderPassesChanged([[maybe_unused]] render::RenderPrimitive &primitive) noexcept
{
	update_render_passes_ = true;
}


/*
	Rendering
*/

void DrawableObject::Prepare()
{
	//No render passes added, add render pass with default shader program (if any)
	if (std::empty(render_passes_))
	{
		auto default_shader_program =
			[&]() noexcept -> NonOwningPtr<shaders::ShaderProgram>
			{
				if (auto owner = Owner(); owner)
					return owner->GetDefaultShaderProgram(query_type_flags_);
				else
					return nullptr;
			}();

		AddRenderPass(render::RenderPass{default_shader_program});
	}

	if (update_render_passes_)
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

		update_render_passes_ = false;
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
	Render passes - Adding
*/

void DrawableObject::AddRenderPass(render::RenderPass pass)
{
	render_passes_.push_back(std::move(pass));
	UpdateRenderPassesOnAllPrimitives(render_passes_);
}

void DrawableObject::AddRenderPasses(render::render_pass::Passes passes)
{
	if (std::empty(render_passes_))
		render_passes_ = std::move(passes);
	else
		std::move(std::begin(passes), std::end(passes), std::back_inserter(render_passes_));

	UpdateRenderPassesOnAllPrimitives(render_passes_);
}


/*
	Render passes - Retrieving
*/

render::RenderPass& DrawableObject::GetRenderPass(int off) noexcept
{
	assert(off >= 0 && off < std::ssize(render_passes_));
	return render_passes_[off];
}

const render::RenderPass& DrawableObject::GetRenderPass(int off) const noexcept
{
	assert(off >= 0 && off < std::ssize(render_passes_));
	return render_passes_[off];
}


/*
	Render passes - Removing
*/

void DrawableObject::ClearRenderPasses() noexcept
{
	render_passes_.clear();
	render_passes_.shrink_to_fit();
	UpdateRenderPassesOnAllPrimitives(render_passes_);
}

bool DrawableObject::RemoveRenderPass(int off) noexcept
{
	if (off >= 0 && off < std::ssize(render_passes_))
	{
		render_passes_.erase(std::begin(render_passes_) + off);
		UpdateRenderPassesOnAllPrimitives(render_passes_);
		return true;
	}
	else
		return false;
}

} //ion::graphics::scene