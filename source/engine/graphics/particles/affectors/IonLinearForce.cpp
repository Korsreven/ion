/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonLinearForce.cpp
-------------------------------------------
*/

#include "IonLinearForce.h"

#include "graphics/particles/IonParticle.h"

namespace ion::graphics::particles::affectors
{

using namespace linear_force;
using namespace ion::utilities;

namespace linear_force::detail
{

void affect_particles(affector::detail::particle_range particles, duration time,
					  ForceType type, const Vector2 &force) noexcept
{
	auto scaled_force = force * time.count();

	//Add
	if (type == ForceType::Add)
	{
		for (auto &particle : particles)
			particle.Direction(particle.Direction() + scaled_force);
	}
	//Average
	else
	{
		for (auto &particle : particles)
			particle.Direction((particle.Direction() + scaled_force) * 0.5_r);
	}
}

} //linear_force::detail


//Protected

/*
	Affect particles
*/

void LinearForce::DoAffect(affector::detail::particle_range particles, duration time) noexcept
{
	detail::affect_particles(particles, time, type_, force_);
}


//Public

LinearForce::LinearForce(std::string name) noexcept :
	Affector{std::move(name)}
{
	//Empty
}

LinearForce::LinearForce(std::string name, ForceType type, const Vector2 &force) noexcept :

	Affector{std::move(name)},

	type_{type},
	force_{force}
{
	//Empty
}


/*
	Cloning
*/

OwningPtr<Affector> LinearForce::Clone() const
{
	return make_owning<LinearForce>(*this);
}

} //ion::graphics::particles::affectors