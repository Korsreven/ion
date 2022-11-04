/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonRenderTargetListener.h
-------------------------------------------
*/

#ifndef ION_RENDER_TARGET_LISTENER_H
#define ION_RENDER_TARGET_LISTENER_H

#include "IonListener.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::events::listeners
{
	///@brief A class representing a listener that listens to render target events
	struct RenderTargetListener : Listener<RenderTargetListener>
	{
		/**
			@name Events
			@{
		*/

		///@brief Called when a render target has been resized, with the new size
		virtual void RenderTargetResized([[maybe_unused]] graphics::utilities::Vector2 size) noexcept
		{
			//Optional to override
		}

		///@}
	};
} //ion::events::listeners

#endif