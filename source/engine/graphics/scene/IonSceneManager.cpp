/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonSceneManager.cpp
-------------------------------------------
*/

#include "IonSceneManager.h"

namespace ion::graphics::scene
{

using namespace scene_manager;

namespace scene_manager::detail
{
} //scene_manager::detail


//Public

/*
	Cameras
	Creating
*/

NonOwningPtr<Camera> SceneManager::CreateCamera(std::string name)
{
	return CameraBase::Create(std::move(name));
}

NonOwningPtr<Camera> SceneManager::CreateCamera(std::string name, const render::Frustum &frustum)
{
	return CameraBase::Create(std::move(name), frustum);
}


NonOwningPtr<Camera> SceneManager::CreateCamera(const Camera &camera)
{
	return CameraBase::Create(camera);
}

NonOwningPtr<Camera> SceneManager::CreateCamera(Camera &&camera)
{
	return CameraBase::Create(std::move(camera));
}


/*
	Cameras
	Retrieving
*/

NonOwningPtr<Camera> SceneManager::GetCamera(std::string_view name) noexcept
{
	return CameraBase::Get(name);
}

NonOwningPtr<const Camera> SceneManager::GetCamera(std::string_view name) const noexcept
{
	return CameraBase::Get(name);
}


/*
	Cameras
	Removing
*/

void SceneManager::ClearCameras() noexcept
{
	CameraBase::Clear();
}

bool SceneManager::RemoveCamera(Camera &camera) noexcept
{
	return CameraBase::Remove(camera);
}

bool SceneManager::RemoveCamera(std::string_view name) noexcept
{
	return CameraBase::Remove(name);
}


/*
	Lights
	Creating
*/

NonOwningPtr<Light> SceneManager::CreateLight()
{
	return LightBase::Create();
}

NonOwningPtr<Light> SceneManager::CreateLight(light::LightType type,
	const Vector3 &position, const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle,
	bool cast_shadows)
{
	return LightBase::Create(type,
		position, direction,
		ambient, diffuse, specular,
		attenuation_constant, attenuation_linear, attenuation_quadratic,
		cutoff_angle, outer_cutoff_angle,
		cast_shadows);
}


NonOwningPtr<Light> SceneManager::CreateLight(const Light &light)
{
	return LightBase::Create(light);
}

NonOwningPtr<Light> SceneManager::CreateLight(Light &&light)
{
	return LightBase::Create(std::move(light));
}


/*
	Lights
	Removing
*/

void SceneManager::ClearLights() noexcept
{
	LightBase::Clear();
}

bool SceneManager::RemoveLight(Light &light) noexcept
{
	return LightBase::Remove(light);
}


/*
	Models
	Creating
*/

NonOwningPtr<Model> SceneManager::CreateModel()
{
	return ModelBase::Create();
}

NonOwningPtr<Model> SceneManager::CreateModel(bool visible)
{
	return ModelBase::Create(visible);
}


/*
	Models
	Removing
*/

void SceneManager::ClearModels() noexcept
{
	ModelBase::Clear();
}

bool SceneManager::RemoveModel(Model &model) noexcept
{
	return ModelBase::Remove(model);
}


/*
	Animations
	Creating
*/

NonOwningPtr<DrawableAnimation> SceneManager::CreateAnimation(const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible)
{
	return AnimationBase::Create(size, animation, visible);
}

NonOwningPtr<DrawableAnimation> SceneManager::CreateAnimation(const Vector3 &position, const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible)
{
	return AnimationBase::Create(position, size, animation, visible);
}

NonOwningPtr<DrawableAnimation> SceneManager::CreateAnimation(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<textures::Animation> animation, bool visible)
{
	return AnimationBase::Create(position, rotation, size, animation, visible);
}


NonOwningPtr<DrawableAnimation> SceneManager::CreateAnimation(const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible)
{
	return AnimationBase::Create(size, animation, color, visible);
}

NonOwningPtr<DrawableAnimation> SceneManager::CreateAnimation(const Vector3 &position, const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible)
{
	return AnimationBase::Create(position, size, animation, color, visible);
}

NonOwningPtr<DrawableAnimation> SceneManager::CreateAnimation(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<textures::Animation> animation, const Color &color, bool visible)
{
	return AnimationBase::Create(position, rotation, size, animation, color, visible);
}


/*
	Animations
	Removing
*/

void SceneManager::ClearAnimations() noexcept
{
	AnimationBase::Clear();
}

bool SceneManager::RemoveAnimation(DrawableAnimation &animation) noexcept
{
	return AnimationBase::Remove(animation);
}


/*
	Particle systems
	Creating
*/

NonOwningPtr<DrawableParticleSystem> SceneManager::CreateParticleSystem(NonOwningPtr<particles::ParticleSystem> particle_system, bool visible)
{
	return ParticleSystemBase::Create(particle_system, visible);
}


/*
	Particle systems
	Removing
*/

void SceneManager::ClearParticleSystems() noexcept
{
	ParticleSystemBase::Clear();
}

bool SceneManager::RemoveParticleSystem(DrawableParticleSystem &particle_system) noexcept
{
	return ParticleSystemBase::Remove(particle_system);
}


/*
	Text
	Creating
*/

NonOwningPtr<DrawableText> SceneManager::CreateText(NonOwningPtr<fonts::Text> text, bool visible)
{
	return TextBase::Create(text, visible);
}

NonOwningPtr<DrawableText> SceneManager::CreateText(const Vector3 &position, NonOwningPtr<fonts::Text> text, bool visible)
{
	return TextBase::Create(position, text, visible);
}

NonOwningPtr<DrawableText> SceneManager::CreateText(const Vector3 &position, real rotation, NonOwningPtr<fonts::Text> text, bool visible)
{
	return TextBase::Create(position, rotation, text, visible);
}


/*
	Text
	Removing
*/

void SceneManager::ClearTexts() noexcept
{
	TextBase::Clear();
}

bool SceneManager::RemoveText(DrawableText &text) noexcept
{
	return TextBase::Remove(text);
}


/*
	Sound
	Creating
*/

NonOwningPtr<MovableSound> SceneManager::CreateSound(NonOwningPtr<sounds::Sound> sound, bool paused)
{
	return SoundBase::Create(sound, paused);
}

NonOwningPtr<MovableSound> SceneManager::CreateSound(const Vector3 &position, NonOwningPtr<sounds::Sound> sound, bool paused)
{
	return SoundBase::Create(position, sound, paused);
}


NonOwningPtr<MovableSound> SceneManager::CreateSound(NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused)
{
	return SoundBase::Create(sound, sound_channel_group, paused);
}

NonOwningPtr<MovableSound> SceneManager::CreateSound(const Vector3 &position, NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused)
{
	return SoundBase::Create(position, sound, sound_channel_group, paused);
}


/*
	Sound
	Removing
*/

void SceneManager::ClearSounds() noexcept
{
	SoundBase::Clear();
}

bool SceneManager::RemoveSound(MovableSound &sound) noexcept
{
	return SoundBase::Remove(sound);
}


/*
	Sound listener
	Creating
*/

NonOwningPtr<MovableSoundListener> SceneManager::CreateSoundListener(NonOwningPtr<sounds::SoundListener> sound_listener)
{
	return SoundListenerBase::Create(sound_listener);
}

NonOwningPtr<MovableSoundListener> SceneManager::CreateSoundListener(const Vector3 &position, NonOwningPtr<sounds::SoundListener> sound_listener)
{
	return SoundListenerBase::Create(position, sound_listener);
}


/*
	Sound listener
	Removing
*/

void SceneManager::ClearSoundListeners() noexcept
{
	SoundListenerBase::Clear();
}

bool SceneManager::RemoveSoundListener(MovableSoundListener &sound_listener) noexcept
{
	return SoundListenerBase::Remove(sound_listener);
}

} //ion::graphics::scene