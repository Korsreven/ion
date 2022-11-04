/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonWindowListener.h
-------------------------------------------
*/

#ifndef ION_WINDOW_LISTENER_H
#define ION_WINDOW_LISTENER_H

#include "IonListener.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::events::listeners
{
	enum class WindowAction
	{
		Open,
		Close,

		Activate,
		Deactivate,

		Maximize,
		Minimize,
		Restore,

		Move,
		Resize
	};


	///@brief A class representing a listener that listens to window events
	struct WindowListener : Listener<WindowListener>
	{
		/**
			@name Events
			@{
		*/

		///@brief Called when a window action has been received, with the action that was sent
		virtual void WindowActionReceived(WindowAction action) noexcept = 0;

		///@brief Called when a window has been resized, with the new size
		virtual void WindowResized([[maybe_unused]] graphics::utilities::Vector2 size) noexcept
		{
			//Optional to override
		}

		///@brief Called when a window has been moved, with the new position
		virtual void WindowMoved([[maybe_unused]] graphics::utilities::Vector2 position) noexcept
		{
			//Optional to override
		}

		///@}
	};
} //ion::events::listeners

#endif