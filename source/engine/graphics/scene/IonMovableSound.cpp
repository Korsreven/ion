/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableSound.cpp
-------------------------------------------
*/

#include "IonMovableSound.h"

#include "sounds/IonSound.h"

namespace ion::graphics::scene
{

using namespace movable_sound;

namespace movable_sound::detail
{
} //movable_sound::detail


MovableSound::MovableSound(NonOwningPtr<sounds::Sound> sound, bool paused) :

	sound_channel_{sound ? sound->Play(paused) : nullptr},
	initial_sound_{sound}
{
	//Empty
}

MovableSound::MovableSound(NonOwningPtr<sounds::Sound> sound, NonOwningPtr<sounds::SoundChannelGroup> sound_channel_group, bool paused) :

	sound_channel_{sound ? sound->Play(sound_channel_group, paused) : nullptr},
	initial_sound_{sound}
{
	//Empty
}


/*
	Modifiers
*/

void MovableSound::Revert()
{
	if (initial_sound_)
		sound_channel_ = initial_sound_->Play(sound_channel_, !sound_channel_->IsPlaying());
}


/*
	Elapse time
*/

void MovableSound::Elapse(duration time) noexcept
{

}

} //ion::graphics::scene