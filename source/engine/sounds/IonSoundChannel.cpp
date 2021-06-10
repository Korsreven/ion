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

#include <cassert>

#include "IonSoundManager.h"
#include "Fmod/fmod.hpp"

namespace ion::sounds
{

using namespace sound_channel;
using namespace types::type_literals;

namespace sound_channel::detail
{

} //sound_channel::detail


SoundChannel::SoundChannel(NonOwningPtr<Sound> sound) noexcept :
	sound_{sound}
{
	//Empty
}

SoundChannel::SoundChannel(NonOwningPtr<Sound> sound, NonOwningPtr<SoundChannelGroup> group) noexcept :

	sound_{sound},
	group_{group}
{
	//Empty
}


/*
	Modifiers
*/

void SoundChannel::CurrentChannelGroup(NonOwningPtr<SoundChannelGroup> group) noexcept
{
	if (sound_ && group_ != group)
	{
		assert(handle_);
		sound_manager::detail::set_channel_group(*handle_, group ? group->Handle() : nullptr);
	}
}


void SoundChannel::Mute(bool mute) noexcept
{
	if (sound_)
	{
		assert(handle_);
		sound_manager::detail::set_mute(*handle_, mute);
	}
}
			
void SoundChannel::Pitch(real pitch) noexcept
{
	if (sound_)
	{
		assert(handle_);
		sound_manager::detail::set_pitch(*handle_, pitch);
	}
}

void SoundChannel::Volume(real volume) noexcept
{
	if (sound_)
	{
		assert(handle_);
		sound_manager::detail::set_volume(*handle_, volume);
	}
}


/*
	Observers
*/

bool SoundChannel::IsMuted() const noexcept
{
	if (sound_)
	{
		assert(handle_);
		return sound_manager::detail::get_mute(*handle_);
	}
	else
		return true;
}

real SoundChannel::Pitch() const noexcept
{
	if (sound_)
	{
		assert(handle_);
		return sound_manager::detail::get_pitch(*handle_);
	}
	else
		return 1.0_r;
}

real SoundChannel::Volume() const noexcept
{
	if (sound_)
	{
		assert(handle_);
		return sound_manager::detail::get_volume(*handle_);
	}
	else
		return 0.0_r;
}


/*
	Playback 
*/

void SoundChannel::Resume() noexcept
{
	if (sound_)
	{
		assert(handle_);
		sound_manager::detail::set_paused(*handle_, false);
	}
}

void SoundChannel::Pause() noexcept
{
	if (sound_)
	{
		assert(handle_);
		sound_manager::detail::set_paused(*handle_, true);
	}
}

void SoundChannel::Reset() noexcept
{
	if (sound_)
	{
		assert(handle_);
		sound_manager::detail::set_paused(*handle_, true);
		sound_manager::detail::set_position(*handle_, 0);
	}
}


bool SoundChannel::IsPlaying() const noexcept
{
	if (sound_)
	{
		assert(handle_);
		return sound_manager::detail::is_playing(*handle_);
	}
	else
		return false;
}

} //ion::sounds