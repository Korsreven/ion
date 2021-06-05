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

namespace ion::sounds
{
	namespace sound_manager
	{
		namespace detail
		{
			std::optional<int> load_sound(
				const std::string &file_data, sound::SoundType type,
				sound::SoundMixingMode mixing_mode, sound::SoundProcessingMode processing_mode,
				sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
				const std::optional<sound::SoundLoopingMode> &looping_mode) noexcept;
			void unload_sound(int sound_handle) noexcept;
		} //detail
	} //sound_manager


	class SoundManager final :
		public resources::FileResourceManager<Sound, SoundManager, assets::repositories::AudioRepository>
	{
		private:


		protected:

			/*
				Events
			*/

			bool LoadResource(Sound &sound) noexcept override;
			bool UnloadResource(Sound &sound) noexcept override;


			//See FileResourceManager::ResourceLoaded for more details
			void ResourceLoaded(Sound &sound) noexcept override;

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




			/*
				Observers
			*/




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