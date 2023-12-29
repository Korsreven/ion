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

namespace ion::graphics::scene
{
	class Camera;
	class SceneManager;
} //ion::graphics::render

namespace ion::events::listeners
{
	///@brief A class representing a listener that listens to camera events
	struct CameraListener : ManagedObjectListener<graphics::scene::Camera, graphics::scene::SceneManager, CameraListener>
	{
		/**
			@name Events
			@{
		*/

		///@brief Called when a camera frustum has been changed, with a reference to the camera
		virtual void CameraFrustumChanged([[maybe_unused]] graphics::scene::Camera &camera) noexcept
		{
			//Optional to override
		}

		///@brief Called when a camera has been repositioned, with a reference to the camera
		virtual void CameraMoved([[maybe_unused]] graphics::scene::Camera &camera) noexcept
		{
			//Optional to override
		}

		///@brief Called when a camera has been rotated, with a reference to the camera
		virtual void CameraRotated([[maybe_unused]] graphics::scene::Camera &camera) noexcept
		{
			//Optional to override
		}

		///@brief Called when a camera has been scaled, with a reference to the camera
		virtual void CameraScaled([[maybe_unused]] graphics::scene::Camera &camera) noexcept
		{
			//Optional to override
		}

		///@}
	};
} //ion::events::listeners

#endif