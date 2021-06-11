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

#include "IonSoundManager.h"
#include "Fmod/fmod.hpp"

namespace ion::sounds
{

using namespace sound_channel_group;
using namespace types::type_literals;

namespace sound_channel_group::detail
{

} //sound_channel_group::detail


/*
	Modifiers
*/

void SoundChannelGroup::Mute(bool mute) noexcept
{
	if (handle_)
		sound_manager::detail::set_mute(*handle_, mute);
}
			
void SoundChannelGroup::Pitch(real pitch) noexcept
{
	if (handle_)
		sound_manager::detail::set_pitch(*handle_, pitch);
}

void SoundChannelGroup::Volume(real volume) noexcept
{
	if (handle_)
		sound_manager::detail::set_volume(*handle_, volume);
}


/*
	Observers
*/

bool SoundChannelGroup::IsMuted() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_mute(*handle_);
	else
		return true;
}

real SoundChannelGroup::Pitch() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_pitch(*handle_);
	else
		return 1.0_r;
}

real SoundChannelGroup::Volume() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_volume(*handle_);
	else
		return 0.0_r;
}

} //ion::sounds