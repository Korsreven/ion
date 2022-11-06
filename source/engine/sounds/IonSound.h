/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSound.h
-------------------------------------------
*/

#ifndef ION_SOUND_H
#define ION_SOUND_H

#include <optional>
#include <string>
#include <utility>

#include "IonSoundChannel.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonFileResource.h"

//Forward declarations
namespace FMOD
{
	class Sound;
};

namespace ion::sounds
{
	//Forward declarations
	class SoundChannelGroup;
	class SoundManager;
	
	namespace sound
	{
		enum class SoundType
		{		
			Sample,
			CompressedSample,
			Stream
		};

		enum class SoundProcessingMode : bool
		{
			TwoDimensional, //2D
			ThreeDimensional //3D
		};	

		enum class SoundOrientationMode : bool
		{		
			Head,
			World
		};

		enum class SoundRolloffMode
		{	
			Linear,
			LinearSquare,
			Inverse,
			InverseTapered
		};

		enum class SoundLoopingMode : bool
		{
			Forward,
			Bidirectional
		};


		namespace detail
		{
		} //detail
	} //sound


	///@brief A class representing a sound resource (usually from a physical file)
	///@details A sound can be a sample or stream, two or three-dimensional, as well as other properties
	class Sound final :
		public resources::FileResource<SoundManager>,
		public managed::ObjectManager<SoundChannel, Sound>
	{
		private:

			sound::SoundType type_ = sound::SoundType::Sample;
			sound::SoundProcessingMode processing_mode_ = sound::SoundProcessingMode::TwoDimensional;
			sound::SoundOrientationMode orientation_mode_ = sound::SoundOrientationMode::World;
			sound::SoundRolloffMode rolloff_mode_ = sound::SoundRolloffMode::Inverse;
			std::optional<sound::SoundLoopingMode> looping_mode_; //No looping

			FMOD::Sound *handle_ = nullptr;
			std::optional<std::string> stream_data_;

		protected:

			/**
				@name Events
				@{
			*/

			///@brief See ObjectManager::Removed for more details
			void Removed(SoundChannel &sound_channel) noexcept override;

			///@}

		public:

			using resources::FileResource<SoundManager>::FileResource;

			///@brief Constructs a new sound with the given name, asset name, type, processing, orientation, rolloff and looping mode
			Sound(std::string name, std::string asset_name,
				sound::SoundType type, sound::SoundProcessingMode processing_mode,
				sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {}) noexcept;

			///@brief Constructs a new sound with the given name, asset name, type, processing and looping mode
			Sound(std::string name, std::string asset_name,
				sound::SoundType type, sound::SoundProcessingMode processing_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {}) noexcept;

			///@brief Constructs a new sound with the given name, asset name, type and looping mode
			Sound(std::string name, std::string asset_name,
				sound::SoundType type, std::optional<sound::SoundLoopingMode> looping_mode = {}) noexcept;


			///@brief Deleted copy constructor
			Sound(const Sound&) = delete;

			///@brief Default move constructor
			Sound(Sound&&) = default;

			///@brief Destructor
			~Sound() noexcept;


			/**
				@name Static sound conversions
				@{
			*/

			///@brief Returns a non-positional (2D) sound with the given name, asset name, type and looping mode
			[[nodiscard]] static Sound NonPositional(std::string name, std::string asset_name,
				sound::SoundType type, std::optional<sound::SoundLoopingMode> looping_mode = {});


			///@brief Returns a positional (3D) sound with the given name, asset name, type, orientation, rolloff and looping mode
			[[nodiscard]] static Sound Positional(std::string name, std::string asset_name,
				sound::SoundType type, sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {});

			///@brief Returns a positional (3D) sound with the given name, asset name, type and looping mode
			[[nodiscard]] static Sound Positional(std::string name, std::string asset_name,
				sound::SoundType type, std::optional<sound::SoundLoopingMode> looping_mode = {});

			///@}

			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			Sound& operator=(const Sound&) = delete;

			///@brief Default move assignment
			Sound& operator=(Sound&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all sound channels playing this sound
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SoundChannels() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all sound channels playing this sound
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto SoundChannels() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the handle for the sound to the given value
			inline void Handle(FMOD::Sound *handle) noexcept
			{
				handle_ = handle;
			}

			///@brief Sets the stream data of the texture to the given data
			inline void StreamData(std::string data) noexcept
			{
				stream_data_ = std::move(data);
			}

			///@brief Resets the stream data to save some memory (if not needed anymore)
			inline void ResetStreamData() noexcept
			{
				stream_data_.reset();
			}


			///@brief Sets the min and max audible distance for the sound to the given min and max values
			///@details Increase the min distance to make the sound louder.
			///Decrease the min distance to make the sound quieter.
			///Max distance is obsolete unless you need the sound to stop fading out at a certain point
			void Distance(real min_distance, real max_distance = 10'000.0_r) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the handle for the sound
			///@details Returns nullptr if the sound is not loaded
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}

			///@brief Returns the stream data of the sound
			///@details Returns nullopt if the sound has not been prepared yet, or is not streamed
			[[nodiscard]] inline auto& StreamData() const noexcept
			{
				return stream_data_;
			}


			///@brief Returns the sound type
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			///@brief Returns the processing mode of the sound
			[[nodiscard]] inline auto ProcessingMode() const noexcept
			{
				return processing_mode_;
			}

			///@brief Returns the orientation mode of the sound
			[[nodiscard]] inline auto OrientationMode() const noexcept
			{
				return orientation_mode_;
			}

			///@brief Returns the rolloff mode of the sound
			[[nodiscard]] inline auto RolloffMode() const noexcept
			{
				return rolloff_mode_;
			}

			///@brief Returns the looping mode of the sound
			///@details Returns nullopt if the sound has no looping
			[[nodiscard]] inline auto& LoopingMode() const noexcept
			{
				return looping_mode_;
			}


			///@brief Returns the min and max audible distance for the sound
			[[nodiscard]] std::optional<std::pair<real, real>> Distance() const noexcept;
			
			///@}

			/**
				@name Sound channels
				Creating
				@{
			*/

			///@brief Plays this sound, by creating a sound channel outputting to the master channel group
			NonOwningPtr<SoundChannel> Play(bool paused = false);

			///@brief Plays this sound, by creating a sound channel outputting to the given channel group
			NonOwningPtr<SoundChannel> Play(NonOwningPtr<SoundChannelGroup> sound_channel_group, bool paused = false);

			///@brief Plays this sound, by reusing the given sound channel
			NonOwningPtr<SoundChannel> Play(NonOwningPtr<SoundChannel> sound_channel, bool paused = false);

			///@}

			/**
				@name Sound channels
				Removing
				@{
			*/

			///@brief Clears all removable sound channels from this sound
			void ClearSoundChannels() noexcept;

			///@brief Removes a removable sound channel from this sound
			bool RemoveSoundChannel(SoundChannel &sound_channel) noexcept;

			///@}
	};
} //ion::sounds

#endif