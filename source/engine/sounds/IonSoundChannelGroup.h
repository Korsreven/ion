/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSoundChannelGroup.h
-------------------------------------------
*/

#ifndef ION_SOUND_CHANNEL_GROUP_H
#define ION_SOUND_CHANNEL_GROUP_H

#include "managed/IonManagedObject.h"
#include "types/IonTypes.h"

namespace FMOD
{
	class ChannelGroup; //Forward declaration
};

namespace ion::sounds
{
	class SoundManager; //Forward declaration

	namespace sound_channel_group
	{
		namespace detail
		{
		} //detail
	} //sound_channel_group


	//A class representing a sound channel group that can be used to group sound channels together
	//A sound channel group can adjust pitch and volume relative to the master channel
	class SoundChannelGroup final : public managed::ManagedObject<SoundManager>
	{
		private:

			FMOD::ChannelGroup *handle_ = nullptr;

		public:

			using managed::ManagedObject<SoundManager>::ManagedObject;


			/*
				Modifiers
			*/

			//Sets the handle for the sound channel group to the given value
			inline void Handle(FMOD::ChannelGroup *handle) noexcept
			{
				handle_ = handle;
			}


			//Mutes all sounds in this sound channel group
			void Mute(bool mute) noexcept;
			
			//Sets the relative pitch / playback rate of this sound channel group
			void Pitch(real pitch) noexcept;

			//Sets the volume of this sound channel group to the given volume in range [0.0, 1.0]
			void Volume(real volume) noexcept;


			/*
				Observers
			*/

			//Returns the handle for the sound channel group
			//Returns nullptr if the sound channel group is not valid
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}


			//Returns true if this sound channel group has muted all sounds
			[[nodiscard]] bool IsMuted() const noexcept;

			//Returns the relative pitch / playback rate of this sound channel group
			[[nodiscard]] real Pitch() const noexcept;

			//Returns the volume of this sound channel group
			[[nodiscard]] real Volume() const noexcept;
	};
} //ion::sounds

#endif