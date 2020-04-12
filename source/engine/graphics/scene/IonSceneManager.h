/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonSceneManager.h
-------------------------------------------
*/

#ifndef ION_SCENE_MANAGER_H
#define ION_SCENE_MANAGER_H

#include "IonCamera.h"
#include "events/IonListenable.h"
#include "events/listeners/IonCameraListener.h"
#include "managed/IonObjectManager.h"

namespace ion::graphics::scene
{
	namespace scene_manager::detail
	{
	} //scene_manager::detail


	class SceneManager :
		public managed::ObjectManager<Camera, SceneManager, events::listeners::CameraListener>
	{
		private:

			using CameraEventsBase = events::Listenable<events::listeners::CameraListener>;

		public:

			//Default constructor
			SceneManager() = default;

			//Default copy constructor
			SceneManager(const SceneManager&) = default;

			//Default move constructor
			SceneManager(SceneManager&&) = default;

			//Default virtual destructor
			virtual ~SceneManager() = default;


			/*
				Operators
			*/

			//Default copy assignment
			SceneManager& operator=(const SceneManager&) = default;

			//Default move assignment
			SceneManager& operator=(SceneManager&&) = default;


			/*
				Events
			*/

			//Return a mutable reference to the camera events of this scene manager
			[[nodiscard]] inline auto& CameraEvents() noexcept
			{
				return static_cast<CameraEventsBase&>(*this);
			}

			//Return a immutable reference to the camera events of this scene manager
			[[nodiscard]] inline auto& CameraEvents() const noexcept
			{
				return static_cast<const CameraEventsBase&>(*this);
			}


			/*
				Camera
				Ranges
			*/

			//Returns a mutable range of all cameras in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Cameras() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all cameras in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Cameras() const noexcept
			{
				return Objects();
			}


			/*
				Camera
				Creating
			*/

			//Add the given camera to this scene manager by moving it
			//Returns a reference to the newly added camera
			[[nodiscard]] Camera& CreateCamera(Camera &&camera);
	};
} //ion::graphics::scene

#endif