/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSoundChannelGroup.cpp
-------------------------------------------
*/

#include "IonSoundChannelGroup.h"

#include <cassert>
#include "IonSoundManager.h"

namespace ion::sounds
{

using namespace sound_channel_group;

namespace sound_channel_group::detail
{

} //sound_channel_group::detail


/*
	Modifiers
*/

void SoundChannelGroup::Mute(bool mute) noexcept
{
	assert(handle_);
	sound_manager::detail::set_mute(*handle_, mute);
}
			
void SoundChannelGroup::Pitch(real pitch) noexcept
{
	assert(handle_);
	sound_manager::detail::set_pitch(*handle_, pitch);
}

void SoundChannelGroup::Volume(real volume) noexcept
{
	assert(handle_);
	sound_manager::detail::set_volume(*handle_, volume);
}


/*
	Observers
*/

bool SoundChannelGroup::IsMuted() const noexcept
{
	assert(handle_);
	return sound_manager::detail::get_mute(*handle_);
}

real SoundChannelGroup::Pitch() const noexcept
{
	assert(handle_);
	return sound_manager::detail::get_pitch(*handle_);
}

real SoundChannelGroup::Volume() const noexcept
{
	assert(handle_);
	return sound_manager::detail::get_volume(*handle_);
}

} //ion::sounds