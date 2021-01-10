/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonSineForce.cpp
-------------------------------------------
*/

#include "IonSineForce.h"

#include "graphics/particles/IonParticle.h"

namespace ion::graphics::particles::affectors
{

using namespace sine_force;
using namespace ion::utilities;

namespace sine_force::detail
{

void affect_particles(affector::detail::particle_range particles, duration time,
					  ForceType type, const Vector2 &force, real angle) noexcept
{
	auto scaled_force = force * (math::Sin(angle) * time.count());

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

} //sine_force::detail


//Protected

/*
	Affect particles
*/

void SineForce::DoAffect(affector::detail::particle_range particles, duration time) noexcept
{
	if (current_angle_ += current_frequency_ * time.count())
	{
		current_frequency_ = random::Number(frequency_.first, frequency_.second); //Change frequency
		current_angle_.ResetWithCarry();
	}

	detail::affect_particles(particles, time, type_, force_, current_angle_);
}


//Public

SineForce::SineForce(std::string name) :
	Affector{std::move(name)}
{
	//Empty
}

SineForce::SineForce(std::string name, ForceType type, const Vector2 &force, real frequency) :
	SineForce(std::move(name), type, force, frequency, frequency)
{
	//Empty
}

SineForce::SineForce(std::string name, ForceType type, const Vector2 &force, real min_frequency, real max_frequency) :

	Affector{std::move(name)},

	type_{type},
	force_{force},

	frequency_{std::minmax(min_frequency, max_frequency)},
	current_frequency_{random::Number(frequency_.first, frequency_.second)} //Set frequency
{
	//Empty
}

} //ion::graphics::particles::affectors