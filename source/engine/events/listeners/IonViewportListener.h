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
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::render
{
	class RenderTarget;
	class Viewport;
} //ion::graphics::render

namespace ion::events::listeners
{
	//A class representing a listener that listens to viewport events
	struct ViewportListener : ManagedObjectListener<graphics::render::Viewport, graphics::render::RenderTarget, ViewportListener>
	{
		/*
			Events
		*/

		//Called when a viewport has been resized, with the new size
		virtual void ViewportResized([[maybe_unused]] graphics::utilities::Vector2 size) noexcept
		{
			//Optional to override
		}

		//Called when a viewport has been moved, with the new position
		virtual void ViewportMoved([[maybe_unused]] graphics::utilities::Vector2 position) noexcept
		{
			//Optional to override
		}
	};
} //ion::events::listeners

#endif