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

#include "IonListener.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::events::listeners
{
	struct ViewportListener : Listener<ViewportListener>
	{
		/*
			Events
		*/

		//Called when a viewport has been resized, with the new size
		virtual void ViewportResized(graphics::utilities::Vector2 size) noexcept = 0;

		//Called when a viewport has been moved, with the new position
		virtual void ViewportMoved([[maybe_unused]] graphics::utilities::Vector2 position) noexcept
		{
			//Optional to override
		}
	};
} //ion::events::listeners

#endif