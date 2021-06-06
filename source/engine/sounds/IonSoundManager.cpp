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

#include "Fmod/fmod.hpp"

namespace ion::sounds
{

using namespace sound_manager;

namespace sound_manager::detail
{

FMOD::System* init_sound_system() noexcept
{
	if (FMOD::System *system = nullptr;
		FMOD::System_Create(&system) == FMOD_OK &&
		system->init(max_sound_channels, FMOD_INIT_NORMAL, nullptr) == FMOD_OK)
		
		return system;

	else
	{
		release_sound_system(system);
		return nullptr;
	}
}

void release_sound_system(FMOD::System *system) noexcept
{
	if (system)
	{
		system->release();
		system = nullptr;
	}
}


FMOD::Sound* load_sound(
	FMOD::System &sound_system,
	const std::string &file_data, const std::optional<std::string> &stream_data, sound::SoundType type,
	sound::SoundMixingMode mixing_mode, sound::SoundProcessingMode processing_mode,
	sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
	const std::optional<sound::SoundLoopingMode> &looping_mode) noexcept
{
	FMOD_CREATESOUNDEXINFO ex_info{};
	ex_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO); //Required
	ex_info.length = std::size(file_data);

	FMOD_MODE mode =
		FMOD_OPENMEMORY |
		FMOD_LOWMEM | //Save 256 bytes per sound, by disabling some unneeded functionality
		[&]() noexcept
		{
			switch (type)
			{
				case sound::SoundType::Stream:
				return FMOD_CREATESTREAM;

				case sound::SoundType::CompressedSample:
				return FMOD_CREATECOMPRESSEDSAMPLE;

				case sound::SoundType::Sample:
				default:
				return FMOD_CREATESAMPLE;
			}
		}() |
		[&]() noexcept
		{
			switch (processing_mode)
			{
				case sound::SoundProcessingMode::ThreeDimensional:
				return FMOD_3D |
					[&]() noexcept
					{
						switch (orientation_mode)
						{
							case sound::SoundOrientationMode::Head:
							return FMOD_3D_HEADRELATIVE;

							case sound::SoundOrientationMode::World:
							default:
							return FMOD_3D_WORLDRELATIVE;
						}
					}() |
					[&]() noexcept
					{
						switch (rolloff_mode)
						{
							case sound::SoundRolloffMode::Linear:
							return FMOD_3D_LINEARROLLOFF;

							case sound::SoundRolloffMode::LinearSquare:
							return FMOD_3D_LINEARSQUAREROLLOFF;

							case sound::SoundRolloffMode::InverseTapered:
							return FMOD_3D_INVERSETAPEREDROLLOFF;

							case sound::SoundRolloffMode::Inverse:
							default:
							return FMOD_3D_INVERSEROLLOFF;
						}
					}();

				case sound::SoundProcessingMode::TwoDimensional:
				default:
				return FMOD_2D;
			}
		}() |
		[&]() noexcept
		{
			if (looping_mode)
			{
				switch (*looping_mode)
				{
					case sound::SoundLoopingMode::Bidirectional:
					return FMOD_LOOP_BIDI;

					case sound::SoundLoopingMode::Forward:
					return FMOD_LOOP_NORMAL;
				}
			}
			
			return FMOD_LOOP_OFF;
		}();


	if (FMOD::Sound *sound_handle = nullptr;
		sound_system.createSound(stream_data ? std::data(*stream_data) : std::data(file_data), mode, &ex_info, &sound_handle) == FMOD_OK)

		return sound_handle;

	else
	{
		unload_sound(sound_handle);
		return nullptr;
	}
}

void unload_sound(FMOD::Sound *sound_handle) noexcept
{
	if (sound_handle)
		sound_handle->release();
}

} //sound_manager::detail


/*
	Events
*/

bool SoundManager::PrepareResource(Sound &sound) noexcept
{
	if (FileResourceManager::PrepareResource(sound))
	{
		if (sound.Type() == sound::SoundType::Stream)
		{
			if (sound.FileData())
				sound.StreamData(*sound.FileData());

			return sound.StreamData().has_value();
		}
		else
			return true;
	}
	else
		return false;
}

bool SoundManager::LoadResource(Sound &sound) noexcept
{
	auto &file_data = sound.FileData();
	auto &stream_data = sound.StreamData();

	auto type = sound.Type();
	auto mixing_mode = sound.MixingMode();
	auto processing_mode = sound.ProcessingMode();
	auto orientation_mode = sound.OrientationMode();
	auto rolloff_mode = sound.RolloffMode();
	auto &looping_mode = sound.LoopingMode();

	if (sound_system_ && file_data)
	{
		sound.Handle(
			detail::load_sound(
				*sound_system_,
				*file_data, stream_data, type, mixing_mode, processing_mode, orientation_mode, rolloff_mode, looping_mode
			));
		return sound.Handle();
	}
	else
		return false;
}

bool SoundManager::UnloadResource(Sound &sound) noexcept
{
	if (auto handle = sound.Handle(); handle)
	{
		detail::unload_sound(handle);
		sound.Handle(nullptr);
		return true;
	}
	else
		return false;
}


void SoundManager::ResourceLoaded(Sound &sound) noexcept
{
	FileResourceManager::ResourceLoaded(sound);
}

void SoundManager::ResourceUnloaded(Sound &sound) noexcept
{
	FileResourceManager::ResourceUnloaded(sound);
	sound.ResetStreamData();
		//Stream data not required after sound has failed (save memory)
}

void SoundManager::ResourceFailed(Sound &sound) noexcept
{
	FileResourceManager::ResourceFailed(sound);
	sound.ResetStreamData();
		//Stream data not required after sound has failed (save memory)
}


//Public

SoundManager::SoundManager() noexcept :
	sound_system_{detail::init_sound_system()}
{
	//Empty
}

SoundManager::~SoundManager() noexcept
{
	UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction

	detail::release_sound_system(sound_system_);
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