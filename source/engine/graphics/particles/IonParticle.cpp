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

namespace particle::detail
{
} //particle::detail


Particle::Particle(const Vector2 &position, const Vector2 &direction,
				   const Vector2 &size, real mass, const Color &solid_color,
				   std::chrono::duration<real> life_time) noexcept :
	position_{position},
	direction_{direction},
	size_{size},
	mass_{mass},
	solid_color_{solid_color},

	life_time_{life_time},
	total_life_time_{life_time}
{
	//Empty
}

} //ion::graphics::particles