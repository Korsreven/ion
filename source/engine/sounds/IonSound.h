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

#include "memory/IonNonOwningPtr.h"
#include "resources/IonFileResource.h"

namespace FMOD
{
	class Sound; //Forward declaration
};

namespace ion::sounds
{
	//Forward declarations
	class SoundChannel; 
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


	class Sound final : public resources::FileResource<SoundManager>
	{
		private:

			sound::SoundType type_ = sound::SoundType::Sample;
			sound::SoundProcessingMode processing_mode_ = sound::SoundProcessingMode::TwoDimensional;
			sound::SoundOrientationMode orientation_mode_ = sound::SoundOrientationMode::World;
			sound::SoundRolloffMode rolloff_mode_ = sound::SoundRolloffMode::Inverse;
			std::optional<sound::SoundLoopingMode> looping_mode_; //No looping

			FMOD::Sound *handle_ = nullptr;
			std::optional<std::string> stream_data_;

		public:

			using resources::FileResource<SoundManager>::FileResource;

			//Construct a new sound with the given name, asset name, type, processing, orientation, rolloff and looping mode
			Sound(std::string name, std::string asset_name,
				sound::SoundType type, sound::SoundProcessingMode processing_mode,
				sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {});

			//Construct a new sound with the given name, asset name, type, processing and looping mode
			Sound(std::string name, std::string asset_name,
				sound::SoundType type, sound::SoundProcessingMode processing_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {});

			//Construct a new sound with the given name, asset name, type and looping mode
			Sound(std::string name, std::string asset_name,
				sound::SoundType type, std::optional<sound::SoundLoopingMode> looping_mode = {});


			/*
				Static sound conversions
			*/

			//Returns a non-positional (2D) sound with the given name, asset name, type and looping mode
			[[nodiscard]] static Sound NonPositional(std::string name, std::string asset_name,
				sound::SoundType type, std::optional<sound::SoundLoopingMode> looping_mode = {});


			//Returns a positional (3D) sound with the given name, asset name, type, orientation, rolloff and looping mode
			[[nodiscard]] static Sound Positional(std::string name, std::string asset_name,
				sound::SoundType type, sound::SoundOrientationMode orientation_mode, sound::SoundRolloffMode rolloff_mode,
				std::optional<sound::SoundLoopingMode> looping_mode = {});

			//Returns a positional (3D) sound with the given name, asset name, type and looping mode
			[[nodiscard]] static Sound Positional(std::string name, std::string asset_name,
				sound::SoundType type, std::optional<sound::SoundLoopingMode> looping_mode = {});


			/*
				Modifiers
			*/

			//Sets the handle for the sound to the given value
			inline void Handle(FMOD::Sound *handle) noexcept
			{
				handle_ = handle;
			}

			//Sets the stream data of the texture to the given data
			inline void StreamData(std::string data)
			{
				stream_data_ = std::move(data);
			}

			//Resets the stream data to save some memory (if not needed anymore)
			inline void ResetStreamData() noexcept
			{
				stream_data_.reset();
			}


			/*
				Observers
			*/

			//Returns the handle for the sound
			//Returns nullptr if the sound is not loaded
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}

			//Returns the stream data of the sound
			//Returns nullopt if the sound has not been prepared yet, or is not streamed
			[[nodiscard]] inline auto& StreamData() const noexcept
			{
				return stream_data_;
			}


			//Returns the sound type
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			//Returns the processing mode of the sound
			[[nodiscard]] inline auto ProcessingMode() const noexcept
			{
				return processing_mode_;
			}

			//Returns the orientation mode of the sound
			[[nodiscard]] inline auto OrientationMode() const noexcept
			{
				return orientation_mode_;
			}

			//Returns the rolloff mode of the sound
			[[nodiscard]] inline auto RolloffMode() const noexcept
			{
				return rolloff_mode_;
			}

			//Returns the looping mode of the sound
			//Returns nullopt if the sound has no looping
			[[nodiscard]] inline auto& LoopingMode() const noexcept
			{
				return looping_mode_;
			}
			

			/*
				Creating
				Sound channel
			*/

			//Play this sound, by creating a sound channel outputting to the master channel group
			[[nodiscard]] NonOwningPtr<SoundChannel> Play(bool paused = false) noexcept;

			//Play this sound, by creating a sound channel outputting to the given channel group
			[[nodiscard]] NonOwningPtr<SoundChannel> Play(SoundChannelGroup &channel_group, bool paused = false) noexcept;
	};
} //ion::sounds

#endif