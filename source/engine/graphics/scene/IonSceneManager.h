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
#include "IonMovableAnimation.h"
#include "IonMovableParticleSystem.h"
#include "IonMovableText.h"
#include "events/IonListenable.h"
#include "events/listeners/IonCameraListener.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::particles
{
	class ParticleSystem; //Forward declaration
}

namespace ion::graphics::render
{
	class Viewport; //Forward declaration
}

namespace ion::graphics::scene
{
	namespace scene_manager::detail
	{
	} //scene_manager::detail


	class SceneManager :
		public managed::ObjectManager<Camera, SceneManager, events::listeners::CameraListener>,
		public managed::ObjectManager<Light, SceneManager>,
		public managed::ObjectManager<Model, SceneManager>,
		public managed::ObjectManager<MovableAnimation, SceneManager>,
		public managed::ObjectManager<MovableParticleSystem, SceneManager>,
		public managed::ObjectManager<MovableText, SceneManager>
	{
		private:

			using CameraBase = managed::ObjectManager<Camera, SceneManager, events::listeners::CameraListener>;
			using LightBase = managed::ObjectManager<Light, SceneManager>;
			using ModelBase = managed::ObjectManager<Model, SceneManager>;
			using AnimationBase = managed::ObjectManager<MovableAnimation, SceneManager>;
			using ParticleSystemBase = managed::ObjectManager<MovableParticleSystem, SceneManager>;
			using TextBase = managed::ObjectManager<MovableText, SceneManager>;

			using CameraEventsBase = events::Listenable<events::listeners::CameraListener>;


			NonOwningPtr<render::Viewport> viewport_;

		public:

			//Default constructor
			SceneManager() = default;

			//Deleted copy constructor
			SceneManager(const SceneManager&) = delete;

			//Default move constructor
			SceneManager(SceneManager&&) = default;

			//Default destructor
			~SceneManager() = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			SceneManager& operator=(const SceneManager&) = delete;

			//Move assignment
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
				return CameraBase::Objects();
			}

			//Returns an immutable range of all cameras in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Cameras() const noexcept
			{
				return CameraBase::Objects();
			}


			//Returns a mutable range of all lights in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Lights() noexcept
			{
				return LightBase::Objects();
			}

			//Returns an immutable range of all lights in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Lights() const noexcept
			{
				return LightBase::Objects();
			}


			//Returns a mutable range of all models in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Models() noexcept
			{
				return ModelBase::Objects();
			}

			//Returns an immutable range of all models in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Models() const noexcept
			{
				return ModelBase::Objects();
			}


			//Returns a mutable range of all animations in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Animations() noexcept
			{
				return AnimationBase::Objects();
			}

			//Returns an immutable range of all animations in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Animations() const noexcept
			{
				return AnimationBase::Objects();
			}


			//Returns a mutable range of all particle systems in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ParticleSystems() noexcept
			{
				return ParticleSystemBase::Objects();
			}

			//Returns an immutable range of all particle systems in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ParticleSystems() const noexcept
			{
				return ParticleSystemBase::Objects();
			}


			//Returns a mutable range of all texts in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Texts() noexcept
			{
				return TextBase::Objects();
			}

			//Returns an immutable range of all texts in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Texts() const noexcept
			{
				return TextBase::Objects();
			}


			/*
				Cameras
				Creating
			*/

			//Create a camera with the given name
			NonOwningPtr<Camera> CreateCamera(std::string name);

			//Create a camera with the given name and a custom frustum
			NonOwningPtr<Camera> CreateCamera(std::string name, const render::Frustum &frustum);


			//Create a camera as a copy of the given camera
			NonOwningPtr<Camera> CreateCamera(const Camera &camera);

			//Create a camera by moving the given camera
			NonOwningPtr<Camera> CreateCamera(Camera &&camera);


			/*
				Cameras
				Retrieving
			*/

			//Gets a pointer to a mutable camera with the given name
			//Returns nullptr if camera could not be found
			[[nodiscard]] NonOwningPtr<Camera> GetCamera(std::string_view name) noexcept;

			//Gets a pointer to an immutable camera with the given name
			//Returns nullptr if camera could not be found
			[[nodiscard]] NonOwningPtr<const Camera> GetCamera(std::string_view name) const noexcept;


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


			/*
				Lights
				Creating
			*/

			//Create a light
			NonOwningPtr<Light> CreateLight();

			//Create a light with the given values
			NonOwningPtr<Light> CreateLight(light::LightType type,
				const Vector3 &position, const Vector3 &direction,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle,
				bool cast_shadows = true);


			//Create a light as a copy of the given light
			NonOwningPtr<Light> CreateLight(const Light &light);

			//Create a light by moving the given light
			NonOwningPtr<Light> CreateLight(Light &&light);


			/*
				Lights
				Removing
			*/

			//Clear all removable lights from this manager
			void ClearLights() noexcept;

			//Remove a removable light from this manager
			bool RemoveLight(Light &light) noexcept;


			/*
				Models
				Creating
			*/

			//Create a model
			NonOwningPtr<Model> CreateModel();

			//Create a model with the given vertex buffer usage pattern and visibility
			NonOwningPtr<Model> CreateModel(bool visible);


			/*
				Models
				Removing
			*/

			//Clear all removable models from this manager
			void ClearModels() noexcept;

			//Remove a removable model from this manager
			bool RemoveModel(Model &model) noexcept;


			/*
				Animations
				Creating
			*/

			//Create a movable animation with the given animation, size and visibility
			NonOwningPtr<MovableAnimation> CreateAnimation(const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible = true);

			//Create a movable animation with the given animation, size, color and visibility
			NonOwningPtr<MovableAnimation> CreateAnimation(const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible = true);


			/*
				Animations
				Removing
			*/

			//Clear all removable animations from this manager
			void ClearAnimations() noexcept;

			//Remove a removable animation from this manager
			bool RemoveAnimation(MovableAnimation &animation) noexcept;


			/*
				Particle systems
				Creating
			*/

			//Create a movable particle system with the given particle system and visibility
			NonOwningPtr<MovableParticleSystem> CreateParticleSystem(NonOwningPtr<particles::ParticleSystem> particle_system, bool visible = true);


			/*
				Particle systems
				Removing
			*/

			//Clear all removable particle systems from this manager
			void ClearParticleSystems() noexcept;

			//Remove a removable particle system from this manager
			bool RemoveParticleSystem(MovableParticleSystem &particle_system) noexcept;


			/*
				Text
				Creating
			*/

			//Create a movable text with the given text and visibility
			NonOwningPtr<MovableText> CreateText(NonOwningPtr<fonts::Text> text, bool visible = true);


			/*
				Text
				Removing
			*/

			//Clear all removable texts from this manager
			void ClearTexts() noexcept;

			//Remove a removable text from this manager
			bool RemoveText(MovableText &text) noexcept;


			/*
				Viewport
			*/

			//Sets the viewport connected to this scene manager to the given viewport
			inline void ConnectedViewport(NonOwningPtr<graphics::render::Viewport> viewport) noexcept
			{
				viewport_ = viewport;
			}

			//Returns a pointer to the viewport connected to this scene manager
			//Returns nullptr if this scene manager does not have a viewport connected
			[[nodiscard]] inline auto ConnectedViewport() const noexcept
			{
				return viewport_;
			}
	};
} //ion::graphics::scene

#endif