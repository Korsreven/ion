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

#include <optional>
#include <string>

#include "IonMovableObject.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"

//Forward declarations
namespace ion::sounds
{
	class SoundListener;
}

namespace ion::graphics::scene
{
	using utilities::Vector2;
	using utilities::Vector3;


	namespace movable_sound_listener
	{
		namespace detail
		{
		} //detail
	} //movable_sound_listener


	///@brief A class representing a movable sound listener that can be attached to a scene node
	class MovableSoundListener final : public MovableObject
	{
		private:

			Vector3 position_;
			std::optional<Vector3> previous_world_position_;

			NonOwningPtr<sounds::SoundListener> sound_listener_;

		public:

			///@brief Constructs a new movable sound listener with the given name and sound listener
			MovableSoundListener(std::optional<std::string> name,
				NonOwningPtr<sounds::SoundListener> sound_listener) noexcept;

			///@brief Constructs a new movable sound listener with the given name, position and sound listener
			MovableSoundListener(std::optional<std::string> name, const Vector3 &position,
				NonOwningPtr<sounds::SoundListener> sound_listener) noexcept;


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the position of this sound listener to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				position_ = position;
			}

			///@brief Sets the position of this sound listener to the given position
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the position of this sound listener
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}


			///@brief Returns a mutable reference to the sound listener
			[[nodiscard]] auto& Get() noexcept
			{
				return sound_listener_;
			}

			///@brief Returns an immutable reference to the sound listener
			[[nodiscard]] auto& Get() const noexcept
			{
				return sound_listener_;
			}

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this movable sound listener by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;

			///@}
	};
} //ion::graphics::scene

#endif