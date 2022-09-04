/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSoundListener.h
-------------------------------------------
*/

#ifndef ION_SOUND_LISTENER_H
#define ION_SOUND_LISTENER_H

#include <optional>
#include <utility>

#include "graphics/utilities/IonVector3.h"
#include "managed/IonManagedObject.h"
#include "types/IonTypes.h"

namespace FMOD
{
	class System; //Forward declaration
};

namespace ion::sounds
{
	class SoundManager; //Forward declaration

	namespace sound_listener
	{
		namespace detail
		{
		} //detail
	} //sound_listener


	//A class representing a sound listener (not to be confused with events::listeners)
	//A sound listener represent the ears in a scene, with a position and velocity (doppler effect)
	//Sounds that are three-dimensional will be heared from the sound listeners location
	//A scene will typically only have one active sound listener at once
	class SoundListener final : public managed::ManagedObject<SoundManager>
	{
		private:

			FMOD::System *handle_ = nullptr;

		public:

			using managed::ManagedObject<SoundManager>::ManagedObject;


			/*
				Modifiers
			*/

			//Sets the handle for the sound listener to the given value
			inline void Handle(FMOD::System *handle) noexcept
			{
				handle_ = handle;
			}

			//Sets the position and velocity attributes in use by the sound listener to the given position and velocity
			//This is automatically set when a sound listener is attached to a movable sound listener
			void Attributes(const graphics::utilities::Vector3 &position, const graphics::utilities::Vector3 &velocity) noexcept;


			/*
				Observers
			*/

			//Returns the handle for the sound listener
			//Returns nullptr if the sound listener is not valid
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}

			//Returns the position and velocity attributes in use by the sound listener
			[[nodiscard]] std::optional<std::pair<graphics::utilities::Vector3, graphics::utilities::Vector3>> Attributes() const noexcept;
	};
} //ion::sounds

#endif