/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonViewportListener.h
-------------------------------------------
*/

#ifndef ION_VIEWPORT_LISTENER_H
#define ION_VIEWPORT_LISTENER_H

#include "IonManagedObjectListener.h"

namespace ion::graphics::render
{
	class RenderTarget;
	class Viewport;
} //ion::graphics::render

namespace ion::events::listeners
{
	///@brief A class representing a listener that listens to viewport events
	struct ViewportListener : ManagedObjectListener<graphics::render::Viewport, graphics::render::RenderTarget, ViewportListener>
	{
		/**
			@name Events
			@{
		*/

		///@brief Called when a viewport has been resized, with a reference to the viewport
		virtual void ViewportResized([[maybe_unused]] graphics::render::Viewport &viewport) noexcept
		{
			//Optional to override
		}

		///@brief Called when a viewport has been moved, with a reference to the viewport
		virtual void ViewportMoved([[maybe_unused]] graphics::render::Viewport &viewport) noexcept
		{
			//Optional to override
		}

		///@}
	};
} //ion::events::listeners

#endif