/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonCameraListener.h
-------------------------------------------
*/

#ifndef ION_CAMERA_LISTENER_H
#define ION_CAMERA_LISTENER_H

#include "IonListener.h"
//#include "graphics/utilities/IonFrustum.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::utilities { class Frustum; }

namespace ion::events::listeners
{
	struct CameraListener : Listener<CameraListener>
	{
		/*
			Events
		*/

		//Called when a camera frustum has been changed, with the new frustum
		virtual void CameraFrustumChanged(const graphics::utilities::Frustum &frustum) noexcept = 0;

		//Called when a camera has been repositioned, with the new position
		virtual void CameraRepositioned([[maybe_unused]] const graphics::utilities::Vector2 &position) noexcept
		{
			//Optional to override
		}
	};
} //ion::events::listeners

#endif