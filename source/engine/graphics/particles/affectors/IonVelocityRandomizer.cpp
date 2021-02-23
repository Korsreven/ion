/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonVelocityRandomizer.cpp
-------------------------------------------
*/

#include "IonVelocityRandomizer.h"

#include "graphics/particles/IonParticle.h"
#include "graphics/utilities/IonVector2.h"
#include "utilities/IonRandom.h"

namespace ion::graphics::particles::affectors
{

using namespace velocity_randomizer;
using namespace ion::utilities;
using graphics::utilities::Vector2;

namespace velocity_randomizer::detail
{

void affect_particles(affector::detail::particle_range particles, duration time,
					  const std::pair<real, real> &velocity, real scope) noexcept
{
	for (auto &particle : particles)
	{
		if (scope > random::Number())
			particle.Velocity(particle.Velocity() * random::Number(velocity.first, velocity.second) * time.count());
	}
}

} //velocity_randomizer::detail


//Protected

/*
	Affect particles
*/

void VelocityRandomizer::DoAffect(affector::detail::particle_range particles, duration time) noexcept
{
	detail::affect_particles(particles, time, velocity_, scope_);
}


//Public

VelocityRandomizer::VelocityRandomizer(std::string name) :
	Affector{std::move(name)}
{
	//Empty
}

VelocityRandomizer::VelocityRandomizer(std::string name, real velocity, real scope) :
	VelocityRandomizer(std::move(name), 0.0_r, velocity, scope)
{
	//Empty
}

VelocityRandomizer::VelocityRandomizer(std::string name, real min_velocity, real max_velocity, real scope) :

	Affector{std::move(name)},

	velocity_{std::minmax(min_velocity, max_velocity)},
	scope_{std::clamp(scope, 0.0_r, 1.0_r)}
{
	//Empty
}


/*
	Cloning
*/

OwningPtr<Affector> VelocityRandomizer::Clone() const
{
	return make_owning<VelocityRandomizer>(*this);
}

} //ion::graphics::particles::affectors