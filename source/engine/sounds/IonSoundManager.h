/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSoundManager.h
-------------------------------------------
*/

#ifndef ION_SOUND_MANAGER_H
#define ION_SOUND_MANAGER_H

#include <optional>
#include <string>

#include "IonSound.h"
#include "assets/repositories/IonAudioRepository.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonFileResourceManager.h"

//Forward declarations
namespace FMOD
{
	class ChannelGroup;
	class Sound;
	class System;
};

namespace ion::sounds
{
	namespace sound_manager
	{
		namespace detail
		{
			constexpr auto max_sound_channels = 256;


			FMOD::System* init_sound_system() noexcept;
			void release_sound_system(FMOD::System *system) noexcept;

			FMOD::Sound* load_sound(
				FMOD::System &sound_system,
				const std::string &file_data, const std::optional<std::string> &stream_data, sound::SoundType type,
				sound::SoundMixingMode mixing_mode, sound::SoundProcessingMode processing_mode,
				sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
				const std::optional<sound::SoundLoopingMode> &looping_mode) noexcept;
			void unload_sound(FMOD::Sound *sound_handle) noexcept;

			FMOD::ChannelGroup* get_master_channel_group(FMOD::System &sound_system) noexcept;
		} //detail
	} //sound_manager


	class SoundManager final :
		public resources::FileResourceManager<Sound, SoundManager, assets::repositories::AudioRepository>
	{
		private:

			FMOD::System *sound_system_ = nullptr;

		protected:

			/*
				Events
			*/

			bool PrepareResource(Sound &sound) noexcept override;
			bool LoadResource(Sound &sound) noexcept override;
			bool UnloadResource(Sound &sound) noexcept override;


			//See FileResourceManager::ResourceLoaded for more details
			void ResourceLoaded(Sound &sound) noexcept override;

			//See FileResourceManager::ResourceUnloaded for more details
			void ResourceUnloaded(Sound &sound) noexcept override;

			//See FileResourceManager::ResourceFailed for more details
			void ResourceFailed(Sound &sound) noexcept override;

		public:

			//Default constructor
			SoundManager() noexcept;

			//Deleted copy constructor
			SoundManager(const SoundManager&) = delete;

			//Default move constructor
			SoundManager(SoundManager&&) = default;

			//Destructor
			~SoundManager() noexcept;


			/*
				Operators
			*/

			//Deleted copy assignment
			SoundManager& operator=(const SoundManager&) = delete;

			//Move assignment
			SoundManager& operator=(SoundManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all sounds in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Sounds() noexcept
			{
				return Resources();
			}

			//Returns an immutable range of all sounds in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Sounds() const noexcept
			{
				return Resources();
			}


			/*
				Modifiers
			*/

			//Mute all sounds in this sound manager
			void Mute(bool mute) noexcept;
			
			//Sets the relative pitch / playback rate of this sound manager
			void Pitch(real pitch) noexcept;

			//Sets the master volume of this sound manager to the given volume in range [0.0, 1.0]
			void Volume(real volume) noexcept;


			/*
				Observers
			*/

			//Returns true if this sound manager has muted all sounds
			[[nodiscard]] bool IsMuted() const noexcept;

			//Returns the relative pitch / playback rate of this sound manager
			[[nodiscard]] real Pitch() const noexcept;

			//Returns the master volume of this sound manager
			[[nodiscard]] real Volume() const noexcept;


			/*
				Sounds
				Creating
			*/

			//Create a sound with the given name and asset name
			NonOwningPtr<Sound> CreateSound(std::string name, std::string asset_name);

			//Create a sound with the given name, asset name, type, mixing, processing, orientation, rolloff and looping mode
			NonOwningPtr<Sound> CreateSound(std::string name, std::string asset_name, sound::SoundType type,
				sound::SoundMixingMode mixing_mode, sound::SoundProcessingMode processing_mode,
				sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {});

			//Create a sound with the given name, asset name, type, mixing, processing and looping mode
			NonOwningPtr<Sound> CreateSound(std::string name, std::string asset_name, sound::SoundType type,
				sound::SoundMixingMode mixing_mode, sound::SoundProcessingMode processing_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {});

			//Create a sound with the given name, asset name, type and looping mode
			NonOwningPtr<Sound> CreateSound(std::string name, std::string asset_name, sound::SoundType type,
				std::optional<sound::SoundLoopingMode> looping_mode = {});


			//Create a sound as a copy of the given sound
			NonOwningPtr<Sound> CreateSound(const Sound &sound);

			//Create a sound by moving the given sound
			NonOwningPtr<Sound> CreateSound(Sound &&sound);


			/*
				Sounds
				Retrieving
			*/

			//Gets a pointer to a mutable sound with the given name
			//Returns nullptr if sound could not be found
			[[nodiscard]] NonOwningPtr<Sound> GetSound(std::string_view name) noexcept;

			//Gets a pointer to an immutable sound with the given name
			//Returns nullptr if sound could not be found
			[[nodiscard]] NonOwningPtr<const Sound> GetSound(std::string_view name) const noexcept;


			/*
				Sounds
				Removing
			*/

			//Clear all removable sounds from this manager
			void ClearSounds() noexcept;

			//Remove a removable sound from this manager
			bool RemoveSound(Sound &sound) noexcept;

			//Remove a removable sound with the given name from this manager
			bool RemoveSound(std::string_view name) noexcept;
	};
} //ion::sounds

#endif