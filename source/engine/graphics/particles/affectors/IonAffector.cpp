/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonDirectionRandomizer.cpp
-------------------------------------------
*/

#include "IonAffector.h"

namespace ion::graphics::particles::affectors
{

/*
	Affect particles
*/

void Affector::Affect(affector::detail::particle_range particles, duration time) noexcept
{
	if (enable_)
		DoAffect(particles, time);
}

} //ion::graphics::particles::affectors