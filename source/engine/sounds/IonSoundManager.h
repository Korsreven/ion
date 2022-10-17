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
#include <tuple>

#include "IonSound.h"
#include "IonSoundChannelGroup.h"
#include "IonSoundListener.h"
#include "assets/repositories/IonAudioRepository.h"
#include "graphics/utilities/IonVector3.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonFileResourceManager.h"

//Forward declarations
namespace FMOD
{
	class Channel;
	class ChannelControl;
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
			void update_sound_system(FMOD::System &system) noexcept;

			FMOD::Sound* load_sound(
				FMOD::System &system,
				const std::string &file_data, const std::optional<std::string> &stream_data,
				sound::SoundType type, sound::SoundProcessingMode processing_mode,
				sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
				const std::optional<sound::SoundLoopingMode> &looping_mode) noexcept;
			void unload_sound(FMOD::Sound *sound_handle) noexcept;

			FMOD::Channel* play_sound(
				FMOD::System &system, FMOD::Sound &sound,
				FMOD::ChannelGroup *channel_group, FMOD::Channel *channel,
				bool paused);

			FMOD::ChannelGroup* create_channel_group(FMOD::System &system) noexcept;
			void release_channel_group(FMOD::ChannelGroup *channel_group) noexcept;
			
			FMOD::System* get_system(FMOD::Sound &sound) noexcept;
			FMOD::ChannelGroup* get_master_channel_group(FMOD::System &system) noexcept;
			void set_channel_group(FMOD::Channel &channel, FMOD::ChannelGroup *group) noexcept;

			void stop(FMOD::ChannelControl &control) noexcept;
			void set_mute(FMOD::ChannelControl &control, bool mute) noexcept;
			void set_paused(FMOD::ChannelControl &control, bool paused) noexcept;
			void set_pitch(FMOD::ChannelControl &control, real pitch) noexcept;
			void set_volume(FMOD::ChannelControl &control, real volume) noexcept;
			bool get_mute(FMOD::ChannelControl &control) noexcept;
			real get_pitch(FMOD::ChannelControl &control) noexcept;
			real get_volume(FMOD::ChannelControl &control) noexcept;
			bool is_playing(FMOD::ChannelControl &control) noexcept;

			void set_position(FMOD::Channel &channel, int position) noexcept;


			/*
				Positional (3D) functionality
			*/

			void set_settings(FMOD::System &system, real doppler_scale, real distance_factor, real rolloff_scale) noexcept;

			void set_listener_attributes(FMOD::System &system,
				const graphics::utilities::Vector3 &position, const graphics::utilities::Vector3 &velocity) noexcept;
			std::pair<graphics::utilities::Vector3, graphics::utilities::Vector3> get_listener_attributes(FMOD::System &system) noexcept;

			void set_attributes(FMOD::Channel &channel,
				const graphics::utilities::Vector3 &position, const graphics::utilities::Vector3 &velocity) noexcept;
			void set_min_max_distance(FMOD::Sound &sound, real min_distance, real max_distance) noexcept;
			void set_min_max_distance(FMOD::Channel &channel, real min_distance, real max_distance) noexcept;

			std::tuple<real, real, real> get_settings(FMOD::System &system) noexcept;
			std::pair<graphics::utilities::Vector3, graphics::utilities::Vector3> get_attributes(FMOD::Channel &channel) noexcept;
			std::pair<real, real> get_min_max_distance(FMOD::Sound &sound) noexcept;
			std::pair<real, real> get_min_max_distance(FMOD::Channel &channel) noexcept;
		} //detail
	} //sound_manager


	//A class that manages sounds, sound channels, sound listeners and audio repositories
	class SoundManager final :
		public resources::FileResourceManager<Sound, SoundManager, assets::repositories::AudioRepository>,
		public managed::ObjectManager<SoundChannelGroup, SoundManager>,
		public managed::ObjectManager<SoundListener, SoundManager>
	{
		private:

			FMOD::System *sound_system_ = nullptr;

		protected:

			using SoundBase = FileResourceManager<Sound, SoundManager, assets::repositories::AudioRepository>;	
			using SoundChannelGroupBase = managed::ObjectManager<SoundChannelGroup, SoundManager>;
			using SoundListenerBase = managed::ObjectManager<SoundListener, SoundManager>;


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


			//See ObjectManager::Created for more details
			void Created(SoundChannelGroup &sound_channel_group) noexcept override;
			void Created(SoundListener &sound_listener) noexcept override;

			//See ObjectManager::Removed for more details
			void Removed(SoundChannelGroup &sound_channel_group) noexcept override;
			void Removed(SoundListener &sound_listener) noexcept override;

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

			//Default move assignment
			SoundManager& operator=(SoundManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all sounds in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Sounds() noexcept
			{
				return SoundBase::Resources();
			}

			//Returns an immutable range of all sounds in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Sounds() const noexcept
			{
				return SoundBase::Resources();
			}


			//Returns a mutable range of all sound channel groups in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SoundChannelGroups() noexcept
			{
				return SoundChannelGroupBase::Objects();
			}

			//Returns an immutable range of all sound channel groups in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SoundChannelGroups() const noexcept
			{
				return SoundChannelGroupBase::Objects();
			}


			//Returns a mutable range of all sound listeners in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SoundListeners() noexcept
			{
				return SoundListenerBase::Objects();
			}

			//Returns an immutable range of all sound listeners in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SoundListeners() const noexcept
			{
				return SoundListenerBase::Objects();
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


			//Sets the settings in use by the sound manager to the given doppler scale, distance factor and rolloff scale
			//This function will only have effects on positional (3D) sounds
			//Doppler scale is how much the pitch varies due to doppler shifting
			//Distance factor is the relative distance factor, compared to 1.0 meters
			//Rolloff scale makes the sound drop off faster or slower (only for sound::SoundRolloffMode::Inverse and InverseTapered)
			void Settings(real doppler_scale, real distance_factor, real rolloff_scale) noexcept;


			/*
				Observers
			*/

			//Returns true if this sound manager has muted all sounds
			[[nodiscard]] bool IsMuted() const noexcept;

			//Returns the relative pitch / playback rate of this sound manager
			[[nodiscard]] real Pitch() const noexcept;

			//Returns the master volume of this sound manager
			[[nodiscard]] real Volume() const noexcept;


			//Returns the settings in use by the sound manager
			//This function will only have effects on positional (3D) sounds
			[[nodiscard]] std::tuple<real, real, real> Settings() const noexcept;


			/*
				Updating
			*/

			//Should be called once per frame
			//Mostly needed for positional (3D) sounds to work properly
			void Update() noexcept;


			/*
				Sounds
				Creating
			*/

			//Create a sound with the given name and asset name
			NonOwningPtr<Sound> CreateSound(std::string name, std::string asset_name);

			//Create a sound with the given name, asset name, type, processing, orientation, rolloff and looping mode
			NonOwningPtr<Sound> CreateSound(std::string name, std::string asset_name,
				sound::SoundType type, sound::SoundProcessingMode processing_mode,
				sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {});

			//Create a sound with the given name, asset name, type, processing and looping mode
			NonOwningPtr<Sound> CreateSound(std::string name, std::string asset_name,
				sound::SoundType type, sound::SoundProcessingMode processing_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {});

			//Create a sound with the given name, asset name, type and looping mode
			NonOwningPtr<Sound> CreateSound(std::string name, std::string asset_name, sound::SoundType type,
				std::optional<sound::SoundLoopingMode> looping_mode = {});


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


			/*
				Sound channel groups
				Creating
			*/

			//Create a sound channel group with the given name
			NonOwningPtr<SoundChannelGroup> CreateSoundChannelGroup(std::string name);


			//Create a sound channel group as a copy of the given sound channel group
			NonOwningPtr<SoundChannelGroup> CreateSoundChannelGroup(const SoundChannelGroup &sound_channel_group);

			//Create a sound channel group by moving the given sound channel group
			NonOwningPtr<SoundChannelGroup> CreateSoundChannelGroup(SoundChannelGroup &&sound_channel_group);


			/*
				Sound channel groups
				Retrieving
			*/

			//Gets a pointer to a mutable sound channel group with the given name
			//Returns nullptr if sound channel group could not be found
			[[nodiscard]] NonOwningPtr<SoundChannelGroup> GetSoundChannelGroup(std::string_view name) noexcept;

			//Gets a pointer to an immutable sound channel group with the given name
			//Returns nullptr if sound channel group could not be found
			[[nodiscard]] NonOwningPtr<const SoundChannelGroup> GetSoundChannelGroup(std::string_view name) const noexcept;


			/*
				Sound channel groups
				Removing
			*/

			//Clear all removable sound channel groups from this manager
			void ClearSoundChannelGroups() noexcept;

			//Remove a removable sound channel group from this manager
			bool RemoveSoundChannelGroup(SoundChannelGroup &sound_channel_group) noexcept;

			//Remove a removable sound channel group with the given name from this manager
			bool RemoveSoundChannelGroup(std::string_view name) noexcept;


			/*
				Sound listeners
				Creating
			*/

			//Create a sound listener with the given name
			NonOwningPtr<SoundListener> CreateSoundListener(std::string name);


			/*
				Sound listeners
				Retrieving
			*/

			//Gets a pointer to a mutable sound listener with the given name
			//Returns nullptr if sound listener could not be found
			[[nodiscard]] NonOwningPtr<SoundListener> GetSoundListener(std::string_view name) noexcept;

			//Gets a pointer to an immutable sound listener with the given name
			//Returns nullptr if sound listener could not be found
			[[nodiscard]] NonOwningPtr<const SoundListener> GetSoundListener(std::string_view name) const noexcept;


			/*
				Sound listeners
				Removing
			*/

			//Clear all removable sound listeners from this manager
			void ClearSoundListeners() noexcept;

			//Remove a removable sound listener from this manager
			bool RemoveSoundListener(SoundListener &sound_listener) noexcept;

			//Remove a removable sound listener with the given name from this manager
			bool RemoveSoundListener(std::string_view name) noexcept;
	};
} //ion::sounds

#endif