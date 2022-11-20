/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonAnimationManager.cpp
-------------------------------------------
*/

#include "IonAnimationManager.h"
#include <type_traits>

namespace ion::graphics::textures
{

using namespace animation_manager;

namespace animation_manager::detail
{

} //animation_manager::detail


//Public


/*
	Animations - Creating
*/

NonOwningPtr<Animation> AnimationManager::CreateAnimation(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
	duration cycle_duration, std::optional<int> repeat_count,
	animation::PlaybackDirection direction, real playback_rate)
{
	return Create(std::move(name), frame_sequence, cycle_duration, repeat_count, direction, playback_rate);
}

NonOwningPtr<Animation> AnimationManager::CreateAnimation(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
	duration cycle_duration, std::optional<int> repeat_count, real playback_rate)
{
	return Create(std::move(name), frame_sequence, cycle_duration, repeat_count, playback_rate);
}


NonOwningPtr<Animation> AnimationManager::CreateAnimation(const Animation &animation)
{
	return Create(animation);
}

NonOwningPtr<Animation> AnimationManager::CreateAnimation(Animation &&animation)
{
	return Create(std::move(animation));
}


/*
	Animations - Retrieving
*/

NonOwningPtr<Animation> AnimationManager::GetAnimation(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const Animation> AnimationManager::GetAnimation(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Animations - Removing
*/

void AnimationManager::ClearAnimations() noexcept
{
	Clear();
}

bool AnimationManager::RemoveAnimation(Animation &animation) noexcept
{
	return Remove(animation);
}

bool AnimationManager::RemoveAnimation(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::graphics::textures