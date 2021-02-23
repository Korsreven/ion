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

#include "IonDirectionRandomizer.h"

#include "graphics/particles/IonParticle.h"
#include "graphics/utilities/IonVector2.h"
#include "utilities/IonRandom.h"

namespace ion::graphics::particles::affectors
{

using namespace direction_randomizer;
using namespace ion::utilities;
using graphics::utilities::Vector2;

namespace direction_randomizer::detail
{

void affect_particles(affector::detail::particle_range particles, duration time,
					  real angle, real scope) noexcept
{
	for (auto &particle : particles)
	{
		if (scope > random::Number())
		{
			auto velocity = particle.Velocity();
			particle.Direction(particle.Direction() + particle.Direction().RandomDeviant(angle) * time.count());
			auto new_velocity = particle.Velocity();

			//Keep velocity
			if (new_velocity != 0.0_r)
				particle.Direction(particle.Direction() * (velocity / new_velocity));
		}
	}
}

} //direction_randomizer::detail


//Protected

/*
	Affect particles
*/

void DirectionRandomizer::DoAffect(affector::detail::particle_range particles, duration time) noexcept
{
	detail::affect_particles(particles, time, angle_, scope_);
}


//Public

DirectionRandomizer::DirectionRandomizer(std::string name) :
	Affector{std::move(name)}
{
	//Empty
}

DirectionRandomizer::DirectionRandomizer(std::string name, real angle, real scope) :

	Affector{std::move(name)},

	angle_{std::clamp(angle, 0.0_r, math::Pi)},
	scope_{std::clamp(scope, 0.0_r, 1.0_r)}
{
	//Empty
}


/*
	Cloning
*/

OwningPtr<Affector> DirectionRandomizer::Clone() const
{
	return make_owning<DirectionRandomizer>(*this);
}

} //ion::graphics::particles::affectors