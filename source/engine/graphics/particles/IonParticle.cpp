/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonParticle.cpp
-------------------------------------------
*/

#include "IonParticle.h"

namespace ion::graphics::particles
{

Particle::Particle(const Vector3 &position, const Vector2 &direction,
				   const Vector2 &size, real mass, const Color &color,
				   duration lifetime, const Vector2 &initial_direction) noexcept :

	position_{position},
	direction_{direction},
	rotation_{direction.SignedAngleBetween(initial_direction)},
	size_{size},
	mass_{mass},
	color_{color},
	lifetime_{lifetime},

	initial_direction_{initial_direction}
{
	//Empty
}

} //ion::graphics::particles