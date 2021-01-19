/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonAnimationManager.h
-------------------------------------------
*/

#ifndef ION_ANIMATION_MANAGER_H
#define ION_ANIMATION_MANAGER_H

#include <string>
#include <string_view>

#include "IonAnimation.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::textures
{
	namespace animation_manager::detail
	{
	} //animation_manager::detail


	struct AnimationManager final :
		managed::ObjectManager<Animation, AnimationManager>
	{
		//Default constructor
		AnimationManager() = default;

		//Deleted copy constructor
		AnimationManager(const AnimationManager&) = delete;

		//Default move constructor
		AnimationManager(AnimationManager&&) = default;

		//Destructor
		~AnimationManager() = default;


		/*
			Operators
		*/

		//Deleted copy assignment
		AnimationManager& operator=(const AnimationManager&) = delete;

		//Move assignment
		AnimationManager& operator=(AnimationManager&&) = default;


		/*
			Ranges
		*/

		//Returns a mutable range of all animations in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto Animations() noexcept
		{
			return Objects();
		}

		//Returns an immutable range of all animations in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto Animations() const noexcept
		{
			return Objects();
		}


		/*
			Animations
			Creating
		*/

		//Create an animation with the given name, frames, cycle duration, repeat count, playback direction and rate
		NonOwningPtr<Animation> CreateAnimation(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
			duration cycle_duration, std::optional<int> repeat_count = std::nullopt,
			animation::PlaybackDirection direction = animation::PlaybackDirection::Normal, real playback_rate = 1.0_r);

		//Create an animation (in normal direction) with the given name, frames, cycle duration, repeat count and playback rate
		NonOwningPtr<Animation> CreateAnimation(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
			duration cycle_duration, std::optional<int> repeat_count, real playback_rate);


		//Create an animation as a copy of the given animation
		NonOwningPtr<Animation> CreateAnimation(const Animation &animation);

		//Create an animation by moving the given animation
		NonOwningPtr<Animation> CreateAnimation(Animation &&animation);


		/*
			Animations
			Retrieving
		*/

		//Gets a pointer to a mutable animation with the given name
		//Returns nullptr if animation could not be found
		[[nodiscard]] NonOwningPtr<Animation> GetAnimation(std::string_view name) noexcept;

		//Gets a pointer to an immutable animation with the given name
		//Returns nullptr if animation could not be found
		[[nodiscard]] NonOwningPtr<const Animation> GetAnimation(std::string_view name) const noexcept;


		/*
			Animations
			Removing
		*/

		//Clear all removable animations from this manager
		void ClearAnimations() noexcept;

		//Remove a removable animation from this manager
		bool RemoveAnimation(Animation &animation) noexcept;

		//Remove a removable animation with the given name from this manager
		bool RemoveAnimation(std::string_view name) noexcept;
	};
} //ion::graphics::textures

#endif