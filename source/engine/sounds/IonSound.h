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

#include "resources/IonFileResource.h"

namespace ion::sounds
{
	class SoundManager; //Forward declaration
	
	namespace sound
	{
		enum class SoundCreateMode
		{		
			Sample,
			CompressedSample,
			Stream
		};

		enum class SoundMixingMode : bool
		{		
			Hardware,
			Software
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
			Inverse,
			Linear,
			LinearSquare
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

			sound::SoundCreateMode create_mode_ = sound::SoundCreateMode::Sample;
			sound::SoundMixingMode mixing_mode_ = sound::SoundMixingMode::Hardware;
			sound::SoundProcessingMode processing_mode_ = sound::SoundProcessingMode::TwoDimensional;
			sound::SoundOrientationMode orientation_mode_ = sound::SoundOrientationMode::World;
			sound::SoundRolloffMode rolloff_mode_ = sound::SoundRolloffMode::Inverse;
			std::optional<sound::SoundLoopingMode> looping_mode_; //No looping

			std::optional<std::string> stream_data_;

		public:

			using resources::FileResource<SoundManager>::FileResource;

			//Construct a new sound with the given name and asset name
			Sound(std::string name, std::string asset_name);


			/*
				Modifiers
			*/

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

			//Returns the create mode of the sound
			[[nodiscard]] inline auto CreateMode() const noexcept
			{
				return create_mode_;
			}

			//Returns the mixing mode of the sound
			[[nodiscard]] inline auto MixingMode() const noexcept
			{
				return mixing_mode_;
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


			//Returns the stream data of the sound
			//Returns nullopt if the sound has not been prepared yet, or is not streamed
			[[nodiscard]] inline auto& StreamData() const noexcept
			{
				return stream_data_;
			}
	};
} //ion::sounds

#endif