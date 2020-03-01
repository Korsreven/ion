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
	for (auto &listener : Listeners())
		Notify(&events::listeners::RenderTargetListener::RenderTargetResized, listener, size);
}


//Public

void RenderTarget::SwapBuffers() noexcept
{
	DoSwapBuffers();
}

} //ion::graphics::render