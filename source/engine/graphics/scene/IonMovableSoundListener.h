/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableSoundListener.h
-------------------------------------------
*/

#ifndef ION_MOVABLE_SOUND_LISTENER_H
#define ION_MOVABLE_SOUND_LISTENER_H

#include "IonMovableObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::sounds
{
	class SoundManager;
}

namespace ion::graphics::scene
{
	namespace movable_sound_listener
	{
		namespace detail
		{
		} //detail
	} //movable_sound_listener


	//A movable sound listener that can be attached to a scene node
	class MovableSoundListener final : public MovableObject
	{
		private:

			NonOwningPtr<sounds::SoundManager> sound_manager_;

		public:

			//Construct a new movable sound listener with the given sound manager
			explicit MovableSoundListener(NonOwningPtr<sounds::SoundManager> sound_manager);


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			//Returns a mutable reference to the sound manager
			[[nodiscard]] auto& Get() noexcept
			{
				return sound_manager_;
			}

			//Returns an immutable reference to the sound manager
			[[nodiscard]] auto& Get() const noexcept
			{
				return sound_manager_;
			}


			/*
				Elapse time
			*/

			//Elapse the total time for this movable sound listener by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;
	};
} //ion::graphics::scene

#endif