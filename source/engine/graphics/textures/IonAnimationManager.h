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


	///@brief A class that manages and stores animations
	class AnimationManager final :
		public managed::ObjectManager<Animation, AnimationManager>
	{
		public:

			///@brief Default constructor
			AnimationManager() = default;

			///@brief Deleted copy constructor
			AnimationManager(const AnimationManager&) = delete;

			///@brief Default move constructor
			AnimationManager(AnimationManager&&) = default;

			///@brief Destructor
			~AnimationManager() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			AnimationManager& operator=(const AnimationManager&) = delete;

			///@brief Default move assignment
			AnimationManager& operator=(AnimationManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all animations in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Animations() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all animations in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Animations() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Animations - Creating
				@{
			*/

			///@brief Creates an animation with the given name, frames, cycle duration, repeat count, playback direction and rate
			NonOwningPtr<Animation> CreateAnimation(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
				duration cycle_duration, std::optional<int> repeat_count = std::nullopt,
				animation::PlaybackDirection direction = animation::PlaybackDirection::Normal, real playback_rate = 1.0_r);

			///@brief Creates an animation (in normal direction) with the given name, frames, cycle duration, repeat count and playback rate
			NonOwningPtr<Animation> CreateAnimation(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
				duration cycle_duration, std::optional<int> repeat_count, real playback_rate);


			///@brief Creates an animation as a copy of the given animation
			NonOwningPtr<Animation> CreateAnimation(const Animation &animation);

			///@brief Creates an animation by moving the given animation
			NonOwningPtr<Animation> CreateAnimation(Animation &&animation);

			///@}

			/**
				@name Animations - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable animation with the given name
			///@details Returns nullptr if animation could not be found
			[[nodiscard]] NonOwningPtr<Animation> GetAnimation(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable animation with the given name
			///@details Returns nullptr if animation could not be found
			[[nodiscard]] NonOwningPtr<const Animation> GetAnimation(std::string_view name) const noexcept;

			///@}

			/**
				@name Animations - Removing
				@{
			*/

			///@brief Clears all removable animations from this manager
			void ClearAnimations() noexcept;

			///@brief Removes a removable animation from this manager
			bool RemoveAnimation(Animation &animation) noexcept;

			///@brief Removes a removable animation with the given name from this manager
			bool RemoveAnimation(std::string_view name) noexcept;

			///@}
	};
} //ion::graphics::textures

#endif