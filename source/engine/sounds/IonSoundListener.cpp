/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSoundListener.cpp
-------------------------------------------
*/

#include "IonSoundListener.h"

#include "IonSoundManager.h"
#include "Fmod/fmod.hpp"

namespace ion::sounds
{

using namespace sound_listener;
using namespace types::type_literals;

namespace sound_listener::detail
{

} //sound_listener::detail


/*
	Modifiers
*/

void SoundListener::Attributes(const graphics::utilities::Vector3 &position, const graphics::utilities::Vector3 &velocity) noexcept
{
	if (handle_)
		sound_manager::detail::set_listener_attributes(*handle_, position, velocity);
}


/*
	Observers
*/

std::optional<std::pair<graphics::utilities::Vector3, graphics::utilities::Vector3>> SoundListener::Attributes() const noexcept
{
	if (handle_)
		return sound_manager::detail::get_listener_attributes(*handle_);
	else
		return {};
}

} //ion::sounds