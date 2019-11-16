/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system/events/listeners
File:	IonSystemMessageListener.h
-------------------------------------------
*/

#ifndef ION_SYSTEM_MESSAGE_LISTENER_H
#define ION_SYSTEM_MESSAGE_LISTENER_H

#ifdef _WIN32
//Dependencies for windows 32/64 bit
#include <windows.h>
#endif

#include "events/listeners/IonListener.h"

namespace ion::system::events::listeners
{
	//User defined message constants

	#ifdef _WIN32
	constexpr UINT WM_GLSIZE = WM_USER + 0;
	#endif


	struct MessageListener : ion::events::listeners::Listener<MessageListener>
	{
		/*
			Events
		*/

		//Called when a system message is received, with the message that was sent
		
		#ifdef _WIN32
		//Function signature for windows 32/64 bit
		virtual bool MessageReceived(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept = 0;
		#endif
	};
} //ion::events::listeners

#endif