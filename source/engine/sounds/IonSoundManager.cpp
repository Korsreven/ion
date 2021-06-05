/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSoundManager.cpp
-------------------------------------------
*/

#include "IonSoundManager.h"

namespace ion::sounds
{

using namespace sound_manager;

namespace sound_manager::detail
{

std::optional<int> load_sound(
	const std::string &file_data, sound::SoundType type,
	sound::SoundMixingMode mixing_mode, sound::SoundProcessingMode processing_mode,
	sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
	const std::optional<sound::SoundLoopingMode> &looping_mode) noexcept
{
	//Todo
	return {};
}

void unload_sound(int sound_handle) noexcept
{
	//Todo
}

} //sound_manager::detail


/*
	Events
*/

bool SoundManager::LoadResource(Sound &sound) noexcept
{
	auto &file_data = sound.FileData();
	auto type = sound.Type();
	auto mixing_mode = sound.MixingMode();
	auto processing_mode = sound.ProcessingMode();
	auto orientation_mode = sound.OrientationMode();
	auto rolloff_mode = sound.RolloffMode();
	auto &looping_mode = sound.LoopingMode();

	if (file_data)
	{
		auto sound_handle = detail::load_sound(*file_data, type, mixing_mode, processing_mode, orientation_mode, rolloff_mode, looping_mode);
		return sound_handle.has_value(); //TEMP
	}
	else
		return false;
}

bool SoundManager::UnloadResource(Sound &sound) noexcept
{
	int *sound_handle = nullptr; //TEMP

	if (auto handle = sound_handle; handle)
	{
		detail::unload_sound(*handle);
		//sound.Handle({});
		return true;
	}
	else
		return false;
}


void SoundManager::ResourceLoaded(Sound &sound) noexcept
{
	FileResourceManager::ResourceLoaded(sound);
}

void SoundManager::ResourceFailed(Sound &sound) noexcept
{
	FileResourceManager::ResourceFailed(sound);
	sound.ResetStreamData();
		//Stream data not required after sound has failed (save memory)
}


//Public

SoundManager::SoundManager() noexcept
{
	//Empty
}

SoundManager::~SoundManager() noexcept
{
	UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}


/*
	Sounds
	Creating
*/

NonOwningPtr<Sound> SoundManager::CreateSound(std::string name, std::string asset_name)
{
	return CreateResource(std::move(name), std::move(asset_name));
}

NonOwningPtr<Sound> SoundManager::CreateSound(std::string name, std::string asset_name, sound::SoundType type,
	sound::SoundMixingMode mixing_mode, sound::SoundProcessingMode processing_mode,
	sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
	std::optional<sound::SoundLoopingMode> looping_mode)
{
	return CreateResource(std::move(name), std::move(asset_name), type,
						  mixing_mode, processing_mode, orientation_mode, rolloff_mode,
						  looping_mode);
}

NonOwningPtr<Sound> SoundManager::CreateSound(std::string name, std::string asset_name, sound::SoundType type,
	sound::SoundMixingMode mixing_mode, sound::SoundProcessingMode processing_mode,
	std::optional<sound::SoundLoopingMode> looping_mode)
{
	return CreateResource(std::move(name), std::move(asset_name), type,
						  mixing_mode, processing_mode, looping_mode);
}

NonOwningPtr<Sound> SoundManager::CreateSound(std::string name, std::string asset_name, sound::SoundType type,
	std::optional<sound::SoundLoopingMode> looping_mode)
{
	return CreateResource(std::move(name), std::move(asset_name), type,
						  looping_mode);
}


NonOwningPtr<Sound> SoundManager::CreateSound(const Sound &sound)
{
	return CreateResource(sound);
}

NonOwningPtr<Sound> SoundManager::CreateSound(Sound &&sound)
{
	return CreateResource(std::move(sound));
}


/*
	Sounds
	Retrieving
*/

NonOwningPtr<Sound> SoundManager::GetSound(std::string_view name) noexcept
{
	return GetResource(name);
}

NonOwningPtr<const Sound> SoundManager::GetSound(std::string_view name) const noexcept
{
	return GetResource(name);
}


/*
	Sounds
	Removing
*/

void SoundManager::ClearSounds() noexcept
{
	ClearResources();
}

bool SoundManager::RemoveSound(Sound &sound) noexcept
{
	return RemoveResource(sound);
}

bool SoundManager::RemoveSound(std::string_view name) noexcept
{
	return RemoveResource(name);
}

} //ion::sounds