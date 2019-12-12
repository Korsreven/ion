/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonGravitation.cpp
-------------------------------------------
*/

#include "IonGravitation.h"

#include "graphics/particles/IonParticle.h"

namespace ion::graphics::particles::affectors
{

using namespace gravitation;
using namespace ion::utilities;

namespace gravitation::detail
{

void affect_particles(affector::detail::particle_range particles, duration time,
					  const Vector2 &position, real gravity, real mass) noexcept
{
	for (auto &particle : particles)
	{
		//Newton's law of universal gravitation
		auto distance = position - particle.Position();
		auto length = distance.SquaredLength();

		if (length > 0.0_r)
		{
			auto force = gravity * mass * particle.Mass() / length;
			particle.Direction(particle.Direction() + distance * force * time.count());
		}
	}
}

} //gravitation::detail


//Protected

/*
	Affect particles
*/

void Gravitation::DoAffect(affector::detail::particle_range particles, duration time) noexcept
{
	detail::affect_particles(particles, time, position_, gravity_, mass_);
}


//Public

Gravitation::Gravitation(const Vector2 &position, real gravity, real mass) noexcept :
	position_{position},
	gravity_{gravity},
	mass_{mass}
{
	//Empty
}

} //ion::graphics::particles::affectors