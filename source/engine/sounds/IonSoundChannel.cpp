/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSoundChannel.cpp
-------------------------------------------
*/

#include "IonSoundChannel.h"

#include "IonSoundManager.h"
#include "Fmod/fmod.hpp"

namespace ion::sounds
{

using namespace sound_channel;

namespace sound_channel::detail
{

} //sound_channel::detail


SoundChannel::SoundChannel(NonOwningPtr<SoundChannelGroup> sound_channel_group) noexcept :
	group_{sound_channel_group}
{
	//Empty
}


/*
	Modifiers
*/

void SoundChannel::CurrentChannelGroup(NonOwningPtr<SoundChannelGroup> sound_channel_group) noexcept
{
	if (handle_ && group_ != sound_channel_group)
	{
		group_ = sound_channel_group;
		sound_manager::detail::set_channel_group(*handle_, group_ ? group_->Handle() : nullptr);	
	}
}


void SoundChannel::Mute(bool mute) noexcept
{
	if (handle_)
		sound_manager::detail::set_mute(*handle_, mute);
}
			
void SoundChannel::Pitch(real pitch) noexcept
{
	if (handle_)
		sound_manager::detail::set_pitch(*handle_, pitch);
}

void SoundChannel::Volume(real volume) noexcept
{
	if (handle_)
		sound_manager::detail::set_volume(*handle_, volume);
}


void SoundChannel::Attributes(const graphics::utilities::Vector3 &position, const graphics::utilities::Vector3 &velocity) noexcept
{
	if (handle_)
		sound_manager::detail::set_attributes(*handle_, position, velocity);
}

void SoundChannel::Distance(real min_distance, real max_distance) noexcept
{
	if (handle_)
		sound_manager::detail::set_min_max_distance(*handle_, min_distance, max_distance);
}


/*
	Observers
*/

bool SoundChannel::IsMuted() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_mute(*handle_);
	else
		return true;
}

real SoundChannel::Pitch() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_pitch(*handle_);
	else
		return 1.0_r;
}

real SoundChannel::Volume() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_volume(*handle_);
	else
		return 0.0_r;
}


std::optional<std::pair<graphics::utilities::Vector3, graphics::utilities::Vector3>> SoundChannel::Attributes() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_attributes(*handle_);
	else
		return {};
}

std::optional<std::pair<real, real>> SoundChannel::Distance() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_min_max_distance(*handle_);
	else
		return {};
}


/*
	Playback 
*/

void SoundChannel::Resume() noexcept
{
	if (handle_)
		sound_manager::detail::set_paused(*handle_, false);
}

void SoundChannel::Pause() noexcept
{
	if (handle_)
		sound_manager::detail::set_paused(*handle_, true);
}

void SoundChannel::Reset() noexcept
{
	if (handle_)
	{
		sound_manager::detail::set_paused(*handle_, true);
		sound_manager::detail::set_position(*handle_, 0);
	}
}


bool SoundChannel::IsPlaying() const noexcept
{
	if (handle_)
		return sound_manager::detail::is_playing(*handle_);
	else
		return false;
}

} //ion::sounds