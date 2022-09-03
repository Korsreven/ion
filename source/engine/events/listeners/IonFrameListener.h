/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonFrameListener.h
-------------------------------------------
*/

#ifndef ION_FRAME_LISTENER_H
#define ION_FRAME_LISTENER_H

#include "IonListener.h"
#include "types/IonTypes.h"

namespace ion::events::listeners
{
	//A class representing a listener that listens to frame events
	struct FrameListener : Listener<FrameListener>
	{
		/*
			Events
		*/

		//Called when a frame is about to begin rendering, with elapsed time in seconds since the last frame
		//Return true to continue rendering, or false to drop out of the rendering loop
		virtual bool FrameStarted(duration time) noexcept = 0;
		
		//Called just after a frame has been rendered, with elapsed time in seconds since the last frame
		//Return true to continue rendering, or false to drop out of the rendering loop
		virtual bool FrameEnded(duration time) noexcept = 0;
	};
} //ion::events::listeners

#endif