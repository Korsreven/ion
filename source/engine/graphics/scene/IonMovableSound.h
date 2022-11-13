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
#include <string>

#include "IonMovableObject.h"
#include "graphics/utilities/IonVector2.h"
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
	using utilities::Vector2;
	using utilities::Vector3;


	namespace movable_sound
	{
		namespace detail
		{
		} //detail
	} //movable_sound


	///@brief A class representing a movable (positional 3D) sound that can be attached to a scene node
	class MovableSound final : public MovableObject
	{
		private:

			Vector3 position_;
			std::optional<Vector3> previous_world_position_;

			NonOwningPtr<sounds::SoundChannel> sound_channel_;
			NonOwningPtr<sounds::Sound> initial_sound_;

		public:

			///@brief Constructs a new movable sound with the given name, sound and pause state
			MovableSound(std::optional<std::string> name,
				NonOwningPtr<sounds::Sound> sound, bool paused = false);

			///@brief Constructs a new movable sound with the given name, position, sound and pause state
			MovableSound(std::optional<std::string> name, const Vector3 &position,
				NonOwningPtr<sounds::Sound> sound, bool paused = false);


			///@brief Constructs a new movable sound with the given name, sound, sound channel group and pause state
			MovableSound(std::optional<std::string> name,
				NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused = false);

			///@brief Constructs a new movable sound with the given name, position, sound, sound channel group and pause state
			MovableSound(std::optional<std::string> name, const Vector3 &position,
				NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused = false);


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the position of this sound to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				position_ = position;
			}

			///@brief Sets the position of this sound to the given position
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}


			///@brief Reverts to the initial sound
			void Revert();

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the position of this sound
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}


			///@brief Returns a mutable reference to the sound channel
			[[nodiscard]] auto& Get() noexcept
			{
				return sound_channel_;
			}

			///@brief Returns an immutable reference to the sound channel
			[[nodiscard]] auto& Get() const noexcept
			{
				return sound_channel_;
			}

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this movable sound by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;

			///@}
	};
} //ion::graphics::scene

#endif