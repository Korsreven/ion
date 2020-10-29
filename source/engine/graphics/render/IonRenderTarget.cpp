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

Viewport& RenderTarget::CreateViewport(const Viewport &viewport)
{
	return Create(viewport);
}

Viewport& RenderTarget::CreateViewport(Viewport &&viewport)
{
	return Create(std::move(viewport));
}


/*
	Viewports
	Retrieving
*/

Viewport* RenderTarget::GetViewport(std::string_view name) noexcept
{
	return Get(name);
}

const Viewport* RenderTarget::GetViewport(std::string_view name) const noexcept
{
	return Get(name);
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