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

#include "IonManagedObjectListener.h"
#include "graphics/render/IonFrustum.h"
#include "graphics/utilities/IonVector3.h"

namespace ion::graphics::scene
{
	class Camera;
	class SceneManager;
} //ion::graphics::render

namespace ion::events::listeners
{
	struct CameraListener : ManagedObjectListener<graphics::scene::Camera, graphics::scene::SceneManager, CameraListener>
	{
		/*
			Events
		*/

		//Called when a camera frustum has been changed, with the new frustum
		virtual void CameraFrustumChanged([[maybe_unused]] graphics::render::Frustum frustum) noexcept
		{
			//Optional to override
		}

		//Called when a camera has been repositioned, with the new position
		virtual void CameraMoved([[maybe_unused]] graphics::utilities::Vector3 position) noexcept
		{
			//Optional to override
		}
	};
} //ion::events::listeners

#endif