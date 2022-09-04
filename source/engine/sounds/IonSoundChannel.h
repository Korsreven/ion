/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSoundChannel.h
-------------------------------------------
*/

#ifndef ION_SOUND_CHANNEL_H
#define ION_SOUND_CHANNEL_H

#include <optional>
#include <utility>

#include "graphics/utilities/IonVector3.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace FMOD
{
	class Channel; //Forward declaration
};

namespace ion::sounds
{
	//Forward declarations
	class Sound;
	class SoundChannelGroup;

	using namespace types::type_literals;

	namespace sound_channel
	{
		namespace detail
		{
		} //detail
	} //sound_channel


	//A class representing a sound channel that can be part of a group or not
	//A sound channel is an instance of a sound, and is created when a sound is played
	class SoundChannel final : public managed::ManagedObject<Sound>
	{
		private:
		
			FMOD::Channel *handle_ = nullptr;
			NonOwningPtr<SoundChannelGroup> group_;

		public:

			//Default constructor
			SoundChannel() = default;

			//Construct a new sound channel with the given sound channel group
			explicit SoundChannel(NonOwningPtr<SoundChannelGroup> sound_channel_group) noexcept;


			/*
				Modifiers
			*/

			//Sets the handle for the sound channel to the given value
			inline void Handle(FMOD::Channel *handle) noexcept
			{
				handle_ = handle;
			}

			//Sets the current channel group for this sound channel
			void CurrentChannelGroup(NonOwningPtr<SoundChannelGroup> sound_channel_group) noexcept;


			//Mute this sound channel
			void Mute(bool mute) noexcept;
			
			//Sets the relative pitch / playback rate of this sound channel
			void Pitch(real pitch) noexcept;

			//Sets the volume of this sound channel to the given volume in range [0.0, 1.0]
			void Volume(real volume) noexcept;


			//Sets the position and velocity attributes in use by the sound channel to the given position and velocity
			//This is automatically set when a sound channel is attached to a movable sound
			void Attributes(const graphics::utilities::Vector3 &position, const graphics::utilities::Vector3 &velocity) noexcept;

			//Sets the min and max audible distance for the sound channel to the given min and max values
			//Increase the min distance to make the sound louder
			//Decrease the min distance to make the sound quieter
			//Max distance is obsolete unless you need the sound to stop fading out at a certain point
			void Distance(real min_distance, real max_distance = 10'000.0_r) noexcept;


			/*
				Observers
			*/

			//Returns the handle for the sound channel
			//Returns nullptr if the sound channel is not valid
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}

			//Returns a pointer to the current channel group for this sound channel
			//Returns nullptr if this sound channel is currently not in a channel group
			[[nodiscard]] inline auto CurrentChannelGroup() const noexcept
			{
				return group_;
			}


			//Returns true if this sound channel is muted
			[[nodiscard]] bool IsMuted() const noexcept;

			//Returns the relative pitch / playback rate of this sound channel
			[[nodiscard]] real Pitch() const noexcept;

			//Returns the volume of this sound channel
			[[nodiscard]] real Volume() const noexcept;


			//Returns the position and velocity attributes in use by the sound channel
			[[nodiscard]] std::optional<std::pair<graphics::utilities::Vector3, graphics::utilities::Vector3>> Attributes() const noexcept;

			//Returns the min and max audible distance for the sound channel
			[[nodiscard]] std::optional<std::pair<real, real>> Distance() const noexcept;


			/*
				Playback 
			*/

			//Resumes sound channel playback
			void Resume() noexcept;

			//Pause sound channel playback
			void Pause() noexcept;

			//Pauses sound channel playback and reset position to zero
			void Reset() noexcept;


			//Returns true if this sound channel is playing
			[[nodiscard]] bool IsPlaying() const noexcept;
	};
} //ion::sounds

#endif