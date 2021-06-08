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

#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace FMOD
{
	class Channel; //Forward declaration
};

namespace ion::sounds
{
	class Sound; //Forward declaration
	class SoundChannelGroup; //Forward declaration
	
	namespace sound_channel
	{
		namespace detail
		{
		} //detail
	} //sound_channel


	class SoundChannel final
	{
		private:

			NonOwningPtr<Sound> sound_;
			NonOwningPtr<SoundChannelGroup> group_;
			FMOD::Channel *handle_ = nullptr;

		public:

			//Construct a new sound channel with the given sound
			explicit SoundChannel(NonOwningPtr<Sound> sound) noexcept;

			//Construct a new sound channel with the given sound and sound channel group
			SoundChannel(NonOwningPtr<Sound> sound, NonOwningPtr<SoundChannelGroup> group) noexcept;


			/*
				Modifiers
			*/

			//Sets the handle for the sound channel to the given value
			inline void Handle(FMOD::Channel *handle) noexcept
			{
				handle_ = handle;
			}


			//Mute this sound channel
			void Mute(bool mute) noexcept;
			
			//Sets the relative pitch / playback rate of this sound channel
			void Pitch(real pitch) noexcept;

			//Sets the volume of this sound channel to the given volume in range [0.0, 1.0]
			void Volume(real volume) noexcept;


			/*
				Observers
			*/

			//Returns the handle for the sound channel
			//Returns nullptr if the sound channel is not valid
			[[nodiscard]] inline auto Handle() const noexcept -> FMOD::Channel*
			{
				if (sound_)
					return handle_;
				else
					return nullptr;
			}


			//Returns true if this sound channel is muted
			[[nodiscard]] bool IsMuted() const noexcept;

			//Returns the relative pitch / playback rate of this sound channel
			[[nodiscard]] real Pitch() const noexcept;

			//Returns the volume of this sound channel
			[[nodiscard]] real Volume() const noexcept;
	};
} //ion::sounds

#endif