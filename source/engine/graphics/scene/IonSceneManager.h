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

#include <optional>
#include <string>
#include <string_view>

#include "IonCamera.h"
#include "IonDrawableAnimation.h"
#include "IonDrawableParticleSystem.h"
#include "IonDrawableText.h"
#include "IonLight.h"
#include "IonModel.h"
#include "IonMovableSound.h"
#include "IonMovableSoundListener.h"
#include "events/IonListenable.h"
#include "events/listeners/IonCameraListener.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion
{
	class Engine; //Forward declaration
}

namespace ion::graphics::scene
{
	namespace scene_manager::detail
	{
	} //scene_manager::detail


	class SceneManager final :
		public managed::ManagedObject<Engine>,
		public managed::ObjectManager<Camera, SceneManager, events::listeners::CameraListener>,
		public managed::ObjectManager<Light, SceneManager>,
		public managed::ObjectManager<Model, SceneManager>,
		public managed::ObjectManager<DrawableAnimation, SceneManager>,
		public managed::ObjectManager<DrawableParticleSystem, SceneManager>,
		public managed::ObjectManager<DrawableText, SceneManager>,
		public managed::ObjectManager<MovableSound, SceneManager>,
		public managed::ObjectManager<MovableSoundListener, SceneManager>
	{
		private:

			using CameraBase = managed::ObjectManager<Camera, SceneManager, events::listeners::CameraListener>;
			using LightBase = managed::ObjectManager<Light, SceneManager>;
			using ModelBase = managed::ObjectManager<Model, SceneManager>;
			using AnimationBase = managed::ObjectManager<DrawableAnimation, SceneManager>;
			using ParticleSystemBase = managed::ObjectManager<DrawableParticleSystem, SceneManager>;
			using TextBase = managed::ObjectManager<DrawableText, SceneManager>;
			using SoundBase = managed::ObjectManager<MovableSound, SceneManager>;
			using SoundListenerBase = managed::ObjectManager<MovableSoundListener, SceneManager>;

			using CameraEventsBase = events::Listenable<events::listeners::CameraListener>;

		public:

			using managed::ManagedObject<Engine>::ManagedObject;


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


			//Returns a mutable range of all sounds in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Sounds() noexcept
			{
				return SoundBase::Objects();
			}

			//Returns an immutable range of all sounds in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Sounds() const noexcept
			{
				return SoundBase::Objects();
			}


			//Returns a mutable range of all sound listeners in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SoundListeners() noexcept
			{
				return SoundListenerBase::Objects();
			}

			//Returns an immutable range of all sound listeners in this scene manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SoundListeners() const noexcept
			{
				return SoundListenerBase::Objects();
			}


			/*
				Cameras
				Creating
			*/

			//Create a camera with the given name
			NonOwningPtr<Camera> CreateCamera(std::optional<std::string> name);

			//Create a camera with the given name and a custom frustum
			NonOwningPtr<Camera> CreateCamera(std::optional<std::string> name, const render::Frustum &frustum);


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

			//Create a light with the given name and visibility
			NonOwningPtr<Light> CreateLight(std::optional<std::string> name, bool visible = true);

			//Create a light with the given name and values
			NonOwningPtr<Light> CreateLight(std::optional<std::string> name, light::LightType type,
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
				Retrieving
			*/

			//Gets a pointer to a mutable light with the given name
			//Returns nullptr if light could not be found
			[[nodiscard]] NonOwningPtr<Light> GetLight(std::string_view name) noexcept;

			//Gets a pointer to an immutable light with the given name
			//Returns nullptr if light could not be found
			[[nodiscard]] NonOwningPtr<const Light> GetLight(std::string_view name) const noexcept;


			/*
				Lights
				Removing
			*/

			//Clear all removable lights from this manager
			void ClearLights() noexcept;

			//Remove a removable light from this manager
			bool RemoveLight(Light &light) noexcept;

			//Remove a removable light with the given name from this manager
			bool RemoveLight(std::string_view name) noexcept;


			/*
				Models
				Creating
			*/

			//Create a model with the given name and visibility
			NonOwningPtr<Model> CreateModel(std::optional<std::string> name, bool visible = true);


			/*
				Models
				Retrieving
			*/
			
			//Gets a pointer to a mutable model with the given name
			//Returns nullptr if model could not be found
			[[nodiscard]] NonOwningPtr<Model> GetModel(std::string_view name) noexcept;

			//Gets a pointer to an immutable model with the given name
			//Returns nullptr if model could not be found
			[[nodiscard]] NonOwningPtr<const Model> GetModel(std::string_view name) const noexcept;


			/*
				Models
				Removing
			*/

			//Clear all removable models from this manager
			void ClearModels() noexcept;

			//Remove a removable model from this manager
			bool RemoveModel(Model &model) noexcept;

			//Remove a removable model with the given name from this manager
			bool RemoveModel(std::string_view name) noexcept;


			/*
				Animations
				Creating
			*/

			//Create a drawable animation with the given name, size, animation and visibility
			NonOwningPtr<DrawableAnimation> CreateAnimation(std::optional<std::string> name, const Vector2 &size,
				NonOwningPtr<textures::Animation> animation, bool visible = true);

			//Create a drawable animation with the given name, position, size, animation and visibility
			NonOwningPtr<DrawableAnimation> CreateAnimation(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
				NonOwningPtr<textures::Animation> animation, bool visible = true);

			//Create a drawable animation with the given name, position, rotation, size, animation and visibility
			NonOwningPtr<DrawableAnimation> CreateAnimation(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
				NonOwningPtr<textures::Animation> animation, bool visible = true);


			//Create a drawable animation with the given name, size, animation, color and visibility
			NonOwningPtr<DrawableAnimation> CreateAnimation(std::optional<std::string> name, const Vector2 &size,
				NonOwningPtr<textures::Animation> animation, const Color &color, bool visible = true);

			//Create a drawable animation with the given name, position, size, animation, color and visibility
			NonOwningPtr<DrawableAnimation> CreateAnimation(std::optional<std::string> name, const Vector3 &position, const Vector2 &size,
				NonOwningPtr<textures::Animation> animation, const Color &color, bool visible = true);

			//Create a drawable animation with the given name, position, rotation, size, animation, color and visibility
			NonOwningPtr<DrawableAnimation> CreateAnimation(std::optional<std::string> name, const Vector3 &position, real rotation, const Vector2 &size,
				NonOwningPtr<textures::Animation> animation, const Color &color, bool visible = true);


			/*
				Animations
				Retrieving
			*/
			
			//Gets a pointer to a mutable animation with the given name
			//Returns nullptr if animation could not be found
			[[nodiscard]] NonOwningPtr<DrawableAnimation> GetAnimation(std::string_view name) noexcept;

			//Gets a pointer to an immutable animation with the given name
			//Returns nullptr if animation could not be found
			[[nodiscard]] NonOwningPtr<const DrawableAnimation> GetAnimation(std::string_view name) const noexcept;


			/*
				Animations
				Removing
			*/

			//Clear all removable animations from this manager
			void ClearAnimations() noexcept;

			//Remove a removable animation from this manager
			bool RemoveAnimation(DrawableAnimation &animation) noexcept;

			//Remove a removable animation with the given name from this manager
			bool RemoveAnimation(std::string_view name) noexcept;


			/*
				Particle systems
				Creating
			*/

			//Create a drawable particle system with the given name, particle system and visibility
			NonOwningPtr<DrawableParticleSystem> CreateParticleSystem(std::optional<std::string> name,
				NonOwningPtr<particles::ParticleSystem> particle_system, bool visible = true);


			/*
				Particle systems
				Retrieving
			*/
			
			//Gets a pointer to a mutable particle system with the given name
			//Returns nullptr if particle system could not be found
			[[nodiscard]] NonOwningPtr<DrawableParticleSystem> GetParticleSystem(std::string_view name) noexcept;

			//Gets a pointer to an immutable particle system with the given name
			//Returns nullptr if particle system could not be found
			[[nodiscard]] NonOwningPtr<const DrawableParticleSystem> GetParticleSystem(std::string_view name) const noexcept;


			/*
				Particle systems
				Removing
			*/

			//Clear all removable particle systems from this manager
			void ClearParticleSystems() noexcept;

			//Remove a removable particle system from this manager
			bool RemoveParticleSystem(DrawableParticleSystem &particle_system) noexcept;

			//Remove a removable particle system with the given name from this manager
			bool RemoveParticleSystem(std::string_view name) noexcept;


			/*
				Texts
				Creating
			*/

			//Create a drawable text with the given name, text and visibility
			NonOwningPtr<DrawableText> CreateText(std::optional<std::string> name,
				NonOwningPtr<fonts::Text> text, bool visible = true);

			//Create a drawable text with the given name, position, text and visibility
			NonOwningPtr<DrawableText> CreateText(std::optional<std::string> name, const Vector3 &position,
				NonOwningPtr<fonts::Text> text, bool visible = true);

			//Create a drawable text with the given name, position, rotation, text and visibility
			NonOwningPtr<DrawableText> CreateText(std::optional<std::string> name, const Vector3 &position, real rotation,
				NonOwningPtr<fonts::Text> text, bool visible = true);


			/*
				Texts
				Retrieving
			*/
			
			//Gets a pointer to a mutable text with the given name
			//Returns nullptr if text could not be found
			[[nodiscard]] NonOwningPtr<DrawableText> GetText(std::string_view name) noexcept;

			//Gets a pointer to an immutable text with the given name
			//Returns nullptr if text could not be found
			[[nodiscard]] NonOwningPtr<const DrawableText> GetText(std::string_view name) const noexcept;


			/*
				Texts
				Removing
			*/

			//Clear all removable texts from this manager
			void ClearTexts() noexcept;

			//Remove a removable text from this manager
			bool RemoveText(DrawableText &text) noexcept;

			//Remove a removable text with the given name from this manager
			bool RemoveText(std::string_view name) noexcept;


			/*
				Sounds
				Creating
			*/

			//Create a movable sound with the given name, sound and pause state
			NonOwningPtr<MovableSound> CreateSound(std::optional<std::string> name,
				NonOwningPtr<sounds::Sound> sound, bool paused = false);

			//Create a movable sound with the given name, podition, sound and pause state
			NonOwningPtr<MovableSound> CreateSound(std::optional<std::string> name, const Vector3 &position,
				NonOwningPtr<sounds::Sound> sound, bool paused = false);


			//Create a movable sound with the given name, sound, sound channel group and pause state
			NonOwningPtr<MovableSound> CreateSound(std::optional<std::string> name,
				NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused = false);

			//Create a movable sound with the given name, position, sound, sound channel group and pause state
			NonOwningPtr<MovableSound> CreateSound(std::optional<std::string> name, const Vector3 &position,
				NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused = false);


			/*
				Sounds
				Retrieving
			*/
			
			//Gets a pointer to a mutable sound with the given name
			//Returns nullptr if sound could not be found
			[[nodiscard]] NonOwningPtr<MovableSound> GetSound(std::string_view name) noexcept;

			//Gets a pointer to an immutable sound with the given name
			//Returns nullptr if sound could not be found
			[[nodiscard]] NonOwningPtr<const MovableSound> GetSound(std::string_view name) const noexcept;


			/*
				Sounds
				Removing
			*/

			//Clear all removable sounds from this manager
			void ClearSounds() noexcept;

			//Remove a removable sound from this manager
			bool RemoveSound(MovableSound &sound) noexcept;

			//Remove a removable sound with the given name from this manager
			bool RemoveSound(std::string_view name) noexcept;


			/*
				Sound listeners
				Creating
			*/

			//Create a movable sound listener with the given name and sound listener
			NonOwningPtr<MovableSoundListener> CreateSoundListener(std::optional<std::string> name,
				NonOwningPtr<sounds::SoundListener> sound_listener);

			//Create a movable sound listener with the given name, position and sound listener
			NonOwningPtr<MovableSoundListener> CreateSoundListener(std::optional<std::string> name, const Vector3 &position,
				NonOwningPtr<sounds::SoundListener> sound_listener);


			/*
				Sound listeners
				Retrieving
			*/
			
			//Gets a pointer to a mutable sound listener with the given name
			//Returns nullptr if sound listener could not be found
			[[nodiscard]] NonOwningPtr<MovableSoundListener> GetSoundListener(std::string_view name) noexcept;

			//Gets a pointer to an immutable sound listener with the given name
			//Returns nullptr if sound listener could not be found
			[[nodiscard]] NonOwningPtr<const MovableSoundListener> GetSoundListener(std::string_view name) const noexcept;


			/*
				Sound listeners
				Removing
			*/

			//Clear all removable sound listeners from this manager
			void ClearSoundListeners() noexcept;

			//Remove a removable sound listener from this manager
			bool RemoveSoundListener(MovableSoundListener &sound_listener) noexcept;

			//Remove a removable sound listener with the given name from this manager
			bool RemoveSoundListener(std::string_view name) noexcept;
	};
} //ion::graphics::scene

#endif