/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableSoundListener.cpp
-------------------------------------------
*/

#include "IonMovableSoundListener.h"

#include "sounds/IonSoundManager.h"

namespace ion::graphics::scene
{

using namespace movable_sound_listener;

namespace movable_sound_listener::detail
{
} //movable_sound_listener::detail


MovableSoundListener::MovableSoundListener(NonOwningPtr<sounds::SoundManager> sound_manager) :
	sound_manager_{sound_manager}
{
	//Empty
}


/*
	Modifiers
*/




/*
	Elapse time
*/

void MovableSoundListener::Elapse(duration time) noexcept
{

}

} //ion::graphics::scene