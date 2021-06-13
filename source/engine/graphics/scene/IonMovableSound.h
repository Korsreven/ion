/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableSound.h
-------------------------------------------
*/

#ifndef ION_MOVABLE_SOUND_H
#define ION_MOVABLE_SOUND_H

#include <optional>

#include "IonMovableObject.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::sounds
{
	class Sound;
	class SoundChannel;
	class SoundChannelGroup;
}

namespace ion::graphics::scene
{
	using utilities::Vector3;


	namespace movable_sound
	{
		namespace detail
		{
		} //detail
	} //movable_sound


	//A movable (positional 3D) sound that can be attached to a scene node
	class MovableSound final : public MovableObject
	{
		private:

			Vector3 position_;
			std::optional<Vector3> previous_world_position_;

			NonOwningPtr<sounds::SoundChannel> sound_channel_;
			NonOwningPtr<sounds::Sound> initial_sound_;

		public:

			//Construct a new movable sound with the given sound and pause state
			explicit MovableSound(NonOwningPtr<sounds::Sound> sound, bool paused = false);

			//Construct a new movable sound with the given podition, sound and pause state
			MovableSound(const Vector3 &position, NonOwningPtr<sounds::Sound> sound, bool paused = false);


			//Construct a new movable sound with the given sound, sound channel group and pause state
			MovableSound(NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused = false);

			//Construct a new movable sound with the given position, sound, sound channel group and pause state
			MovableSound(const Vector3 &position, NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused = false);


			/*
				Modifiers
			*/

			//Sets the position of this sound to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				position_ = position;
			}


			//Revert to the initial sound
			void Revert();


			/*
				Observers
			*/

			//Returns the position of this sound
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}


			//Returns a mutable reference to the sound channel
			[[nodiscard]] auto& Get() noexcept
			{
				return sound_channel_;
			}

			//Returns an immutable reference to the sound channel
			[[nodiscard]] auto& Get() const noexcept
			{
				return sound_channel_;
			}


			/*
				Elapse time
			*/

			//Elapse the total time for this movable sound by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;
	};
} //ion::graphics::scene

#endif