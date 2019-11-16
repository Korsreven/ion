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
		Resize,

		EnterFullScreen,
		ExitFullScreen
	};

	struct WindowListener : Listener<WindowListener>
	{
		/*
			Events
		*/

		//Called when a window action is received, with the action that was sent
		virtual void WindowActionReceived(WindowAction action) noexcept = 0;
	};
} //ion::events::listeners

#endif