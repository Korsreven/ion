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

#include <string>
#include <string_view>

#include "IonCamera.h"
#include "IonLight.h"
#include "IonModel.h"
#include "events/IonListenable.h"
#include "events/listeners/IonCameraListener.h"
#include "managed/IonObjectManager.h"

namespace ion::graphics::scene
{
	namespace scene_manager::detail
	{
	} //scene_manager::detail


	class SceneManager :
		public managed::ObjectManager<Camera, SceneManager, events::listeners::CameraListener>,
		protected unmanaged::ObjectFactory<Light>,
		protected unmanaged::ObjectFactory<Model>
	{
		private:

			using CameraEventsBase = events::Listenable<events::listeners::CameraListener>;

			using CameraManagerBase = managed::ObjectManager<Camera, SceneManager, events::listeners::CameraListener>;
			using LightFactoryBase = unmanaged::ObjectFactory<Light>;
			using ModelFactoryBase = unmanaged::ObjectFactory<Model>;

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
				Ranges
			*/

			//Returns a mutable range of all cameras in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Cameras() noexcept
			{
				return CameraManagerBase::Objects();
			}

			//Returns an immutable range of all cameras in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Cameras() const noexcept
			{
				return CameraManagerBase::Objects();
			}


			//Returns a mutable range of all lights in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Lights() noexcept
			{
				return LightFactoryBase::Objects();
			}

			//Returns an immutable range of all lights in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Lights() const noexcept
			{
				return LightFactoryBase::Objects();
			}


			//Returns a mutable range of all models in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Models() noexcept
			{
				return ModelFactoryBase::Objects();
			}

			//Returns an immutable range of all models in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Models() const noexcept
			{
				return ModelFactoryBase::Objects();
			}


			/*
				Cameras
				Creating
			*/

			//Create a camera as a copy of the given camera
			Camera& CreateCamera(const Camera &camera);

			//Create a camera by moving the given camera
			Camera& CreateCamera(Camera &&camera);


			/*
				Cameras
				Retrieving
			*/

			//Gets a pointer to a mutable camera with the given name
			//Returns nullptr if camera could not be found
			[[nodiscard]] Camera* GetCamera(std::string_view name) noexcept;

			//Gets a pointer to an immutable camera with the given name
			//Returns nullptr if camera could not be found
			[[nodiscard]] const Camera* GetCamera(std::string_view name) const noexcept;


			/*
				Cameras
				Removing
			*/

			//Clear all removable cameras from this manager
			void ClearCameras() noexcept;

			//Remove a removable camera from this manager
			bool RemoveCamera(Camera &camera) noexcept;

			//Remove a removable camera with the given name from this manager
			bool RemoveCamera(std::string_view name) noexcept;
	};
} //ion::graphics::scene

#endif