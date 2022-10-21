/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderTarget.cpp
-------------------------------------------
*/

#include "IonRenderTarget.h"

namespace ion::graphics::render
{

namespace render_target::detail
{

} //render_target::detail


//Private

/*
	Notifying
*/

void RenderTarget::NotifyRenderTargetResized(const Vector2 &size) noexcept
{
	RenderTargetEventsBase::NotifyAll(Events().Listeners(), &events::listeners::RenderTargetListener::RenderTargetResized, size);
}


//Public

/*
	Buffers
*/

void RenderTarget::SwapBuffers() noexcept
{
	DoSwapBuffers();
}


/*
	Viewports
	Creating
*/

NonOwningPtr<Viewport> RenderTarget::CreateViewport(std::string name, RenderTarget &render_target)
{
	return Create(std::move(name), render_target);
}

NonOwningPtr<Viewport> RenderTarget::CreateViewport(std::string name, RenderTarget &render_target, const Aabb &bounds)
{
	return Create(std::move(name), render_target, bounds);
}

NonOwningPtr<Viewport> RenderTarget::CreateViewport(std::string name, RenderTarget &render_target, const Aabb &bounds,
	viewport::HorizontalAnchorType left_anchor, viewport::HorizontalAnchorType right_anchor,
	viewport::VerticalAnchorType top_anchor, viewport::VerticalAnchorType bottom_anchor)
{
	return Create(std::move(name), render_target, bounds,
				  left_anchor, right_anchor, top_anchor, bottom_anchor);
}


NonOwningPtr<Viewport> RenderTarget::CreateViewport(const Viewport &viewport)
{
	return Create(viewport);
}

NonOwningPtr<Viewport> RenderTarget::CreateViewport(Viewport &&viewport)
{
	return Create(std::move(viewport));
}


/*
	Viewports
	Retrieving
*/

NonOwningPtr<Viewport> RenderTarget::GetViewport(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const Viewport> RenderTarget::GetViewport(std::string_view name) const noexcept
{
	return Get(name);
}


NonOwningPtr<Viewport> RenderTarget::GetViewport(const Vector2 &position) noexcept
{
	for (auto &viewport : Viewports())
	{
		if (viewport.Bounds().Contains(position))
			return Get(*viewport.Name());
	}

	return nullptr;
}

NonOwningPtr<const Viewport> RenderTarget::GetViewport(const Vector2 &position) const noexcept
{
	return const_cast<RenderTarget&>(*this).GetViewport(position);
}


/*
	Viewports
	Removing
*/

void RenderTarget::ClearViewports() noexcept
{
	Clear();
}

bool RenderTarget::RemoveViewport(Viewport &viewport) noexcept
{
	return Remove(viewport);
}

bool RenderTarget::RemoveViewport(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::graphics::render