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

#include <algorithm>
#include <utility>

namespace ion::graphics::scene
{

using namespace scene_manager;

namespace scene_manager::detail
{
} //scene_manager::detail


//Public

SceneManager::SceneManager(std::optional<std::string> name) noexcept :
	managed::ManagedObject<graph::SceneGraph>{std::move(name)}
{
	//Empty
}


/*
	Default shader program - Adding
*/

void SceneManager::AddDefaultShaderProgram(uint32 type_mask, NonOwningPtr<shaders::ShaderProgram> shader_program)
{
	if (std::find_if(std::begin(default_shader_programs_), std::end(default_shader_programs_),
		[&](auto &def_shader_program) noexcept
		{
			return def_shader_program.type_mask == type_mask;
		}) == std::end(default_shader_programs_))

		default_shader_programs_.emplace_back(type_mask, shader_program);
}


/*
	Default shader program - Retrieving
*/

NonOwningPtr<shaders::ShaderProgram> SceneManager::GetDefaultShaderProgram(uint32 type_flags) const noexcept
{
	if (auto iter = std::find_if(std::begin(default_shader_programs_), std::end(default_shader_programs_),
		[&](auto &def_shader_program) noexcept
		{
			return def_shader_program.type_mask & type_flags;
		}); iter != std::end(default_shader_programs_))

		return iter->shader_program;

	else
		return nullptr;
}


/*
	Default shader program - Removing
*/

void SceneManager::ClearDefaultShaderPrograms() noexcept
{
	default_shader_programs_.clear();
	default_shader_programs_.shrink_to_fit();
}

void SceneManager::RemoveDefaultShaderProgram(uint32 type_flags) noexcept
{
	if (auto iter = std::find_if(std::begin(default_shader_programs_), std::end(default_shader_programs_),
		[&](auto &def_shader_program) noexcept
		{
			return def_shader_program.type_mask & type_flags;
		}); iter != std::end(default_shader_programs_))

		default_shader_programs_.erase(iter);
}

void SceneManager::RemoveAllDefaultShaderPrograms(uint32 type_flags) noexcept
{
	std::erase_if(default_shader_programs_,
		[&](auto &def_shader_program) noexcept
		{
			return def_shader_program.type_mask & type_flags;
		});
}


/*
	Cameras - Creating
*/

NonOwningPtr<Camera> SceneManager::CreateCamera(std::optional<std::string> name, bool visible)
{
	return CameraBase::Create(std::move(name), visible);
}

NonOwningPtr<Camera> SceneManager::CreateCamera(std::optional<std::string> name, const render::Frustum &frustum, bool visible)
{
	return CameraBase::Create(std::move(name), frustum, visible);
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
	Cameras - Retrieving
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
	Cameras - Removing
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
	Lights - Creating
*/

NonOwningPtr<Light> SceneManager::CreateLight(std::optional<std::string> name, bool visible)
{
	return LightBase::Create(std::move(name), visible);
}

NonOwningPtr<Light> SceneManager::CreateLight(std::optional<std::string> name, light::LightType type,
	const Vector3 &position, const Vector3 &direction, real radius,
	const Color &diffuse,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible)
{
	return LightBase::Create(std::move(name), type,
		position, direction, radius,
		diffuse,
		attenuation_constant, attenuation_linear, attenuation_quadratic,
		cutoff_angle, outer_cutoff_angle, visible);
}

NonOwningPtr<Light> SceneManager::CreateLight(std::optional<std::string> name, light::LightType type,
	const Vector3 &position, const Vector3 &direction, real radius,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible)
{
	return LightBase::Create(std::move(name), type,
		position, direction, radius,
		ambient, diffuse, specular,
		attenuation_constant, attenuation_linear, attenuation_quadratic,
		cutoff_angle, outer_cutoff_angle, visible);
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
	Lights - Retrieving
*/

NonOwningPtr<Light> SceneManager::GetLight(std::string_view name) noexcept
{
	return LightBase::Get(name);
}

NonOwningPtr<const Light> SceneManager::GetLight(std::string_view name) const noexcept
{
	return LightBase::Get(name);
}


/*
	Lights - Removing
*/

void SceneManager::ClearLights() noexcept
{
	LightBase::Clear();
}

bool SceneManager::RemoveLight(Light &light) noexcept
{
	return LightBase::Remove(light);
}

bool SceneManager::RemoveLight(std::string_view name) noexcept
{
	return LightBase::Remove(name);
}


/*
	Models - Creating
*/

NonOwningPtr<Model> SceneManager::CreateModel(std::optional<std::string> name, bool visible)
{
	return ModelBase::Create(std::move(name), visible);
}


/*
	Models - Retrieving
*/

NonOwningPtr<Model> SceneManager::GetModel(std::string_view name) noexcept
{
	return ModelBase::Get(name);
}

NonOwningPtr<const Model> SceneManager::GetModel(std::string_view name) const noexcept
{
	return ModelBase::Get(name);
}


/*
	Models - Removing
*/

void SceneManager::ClearModels() noexcept
{
	ModelBase::Clear();
}

bool SceneManager::RemoveModel(Model &model) noexcept
{
	return ModelBase::Remove(model);
}

bool SceneManager::RemoveModel(std::string_view name) noexcept
{
	return ModelBase::Remove(name);
}


/*
	Particle systems - Creating
*/

NonOwningPtr<DrawableParticleSystem> SceneManager::CreateParticleSystem(std::optional<std::string> name,
	NonOwningPtr<particles::ParticleSystem> particle_system, bool visible)
{
	return ParticleSystemBase::Create(std::move(name), particle_system, visible);
}


/*
	Particle systems - Retrieving
*/

NonOwningPtr<DrawableParticleSystem> SceneManager::GetParticleSystem(std::string_view name) noexcept
{
	return ParticleSystemBase::Get(name);
}

NonOwningPtr<const DrawableParticleSystem> SceneManager::GetParticleSystem(std::string_view name) const noexcept
{
	return ParticleSystemBase::Get(name);
}


/*
	Particle systems - Removing
*/

void SceneManager::ClearParticleSystems() noexcept
{
	ParticleSystemBase::Clear();
}

bool SceneManager::RemoveParticleSystem(DrawableParticleSystem &particle_system) noexcept
{
	return ParticleSystemBase::Remove(particle_system);
}

bool SceneManager::RemoveParticleSystem(std::string_view name) noexcept
{
	return ParticleSystemBase::Remove(name);
}


/*
	Texts - Creating
*/

NonOwningPtr<DrawableText> SceneManager::CreateText(std::optional<std::string> name,
	NonOwningPtr<fonts::Text> text, bool visible)
{
	return TextBase::Create(std::move(name), text, visible);
}

NonOwningPtr<DrawableText> SceneManager::CreateText(std::optional<std::string> name, const Vector3 &position,
	NonOwningPtr<fonts::Text> text, bool visible)
{
	return TextBase::Create(std::move(name), position, text, visible);
}

NonOwningPtr<DrawableText> SceneManager::CreateText(std::optional<std::string> name, const Vector3 &position, real rotation,
	NonOwningPtr<fonts::Text> text, bool visible)
{
	return TextBase::Create(std::move(name), position, rotation, text, visible);
}


/*
	Texts - Retrieving
*/

NonOwningPtr<DrawableText> SceneManager::GetText(std::string_view name) noexcept
{
	return TextBase::Get(name);
}

NonOwningPtr<const DrawableText> SceneManager::GetText(std::string_view name) const noexcept
{
	return TextBase::Get(name);
}


/*
	Texts - Removing
*/

void SceneManager::ClearTexts() noexcept
{
	TextBase::Clear();
}

bool SceneManager::RemoveText(DrawableText &text) noexcept
{
	return TextBase::Remove(text);
}

bool SceneManager::RemoveText(std::string_view name) noexcept
{
	return TextBase::Remove(name);
}


/*
	Sounds - Creating
*/

NonOwningPtr<MovableSound> SceneManager::CreateSound(std::optional<std::string> name,
	NonOwningPtr<sounds::Sound> sound, bool paused)
{
	return SoundBase::Create(std::move(name), sound, paused);
}

NonOwningPtr<MovableSound> SceneManager::CreateSound(std::optional<std::string> name, const Vector3 &position,
	NonOwningPtr<sounds::Sound> sound, bool paused)
{
	return SoundBase::Create(std::move(name), position, sound, paused);
}


NonOwningPtr<MovableSound> SceneManager::CreateSound(std::optional<std::string> name,
	NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused)
{
	return SoundBase::Create(std::move(name), sound, sound_channel_group, paused);
}

NonOwningPtr<MovableSound> SceneManager::CreateSound(std::optional<std::string> name, const Vector3 &position,
	NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused)
{
	return SoundBase::Create(std::move(name), position, sound, sound_channel_group, paused);
}


/*
	Sounds - Retrieving
*/

NonOwningPtr<MovableSound> SceneManager::GetSound(std::string_view name) noexcept
{
	return SoundBase::Get(name);
}

NonOwningPtr<const MovableSound> SceneManager::GetSound(std::string_view name) const noexcept
{
	return SoundBase::Get(name);
}


/*
	Sounds - Removing
*/

void SceneManager::ClearSounds() noexcept
{
	SoundBase::Clear();
}

bool SceneManager::RemoveSound(MovableSound &sound) noexcept
{
	return SoundBase::Remove(sound);
}

bool SceneManager::RemoveSound(std::string_view name) noexcept
{
	return SoundBase::Remove(name);
}


/*
	Sound listeners - Creating
*/

NonOwningPtr<MovableSoundListener> SceneManager::CreateSoundListener(std::optional<std::string> name,
	NonOwningPtr<sounds::SoundListener> sound_listener)
{
	return SoundListenerBase::Create(std::move(name), sound_listener);
}

NonOwningPtr<MovableSoundListener> SceneManager::CreateSoundListener(std::optional<std::string> name, const Vector3 &position,
	NonOwningPtr<sounds::SoundListener> sound_listener)
{
	return SoundListenerBase::Create(std::move(name), position, sound_listener);
}


/*
	Sound listeners - Retrieving
*/

NonOwningPtr<MovableSoundListener> SceneManager::GetSoundListener(std::string_view name) noexcept
{
	return SoundListenerBase::Get(name);
}

NonOwningPtr<const MovableSoundListener> SceneManager::GetSoundListener(std::string_view name) const noexcept
{
	return SoundListenerBase::Get(name);
}


/*
	Sound listeners - Removing
*/

void SceneManager::ClearSoundListeners() noexcept
{
	SoundListenerBase::Clear();
}

bool SceneManager::RemoveSoundListener(MovableSoundListener &sound_listener) noexcept
{
	return SoundListenerBase::Remove(sound_listener);
}

bool SceneManager::RemoveSoundListener(std::string_view name) noexcept
{
	return SoundListenerBase::Remove(name);
}


/*
	Objects - Removing
*/

void SceneManager::Clear() noexcept
{
	CameraBase::Clear();
	LightBase::Clear();
	ModelBase::Clear();
	ParticleSystemBase::Clear();
	TextBase::Clear();
	SoundBase::Clear();
	SoundListenerBase::Clear();
}

void SceneManager::ClearOrphaned() noexcept
{
	auto is_orphan = [](MovableObject &object) { return !object.ParentNode(); };

	CameraBase::RemoveIf(is_orphan);
	LightBase::RemoveIf(is_orphan);
	ModelBase::RemoveIf(is_orphan);
	ParticleSystemBase::RemoveIf(is_orphan);
	TextBase::RemoveIf(is_orphan);
	SoundBase::RemoveIf(is_orphan);
	SoundListenerBase::RemoveIf(is_orphan);
}

} //ion::graphics::scene