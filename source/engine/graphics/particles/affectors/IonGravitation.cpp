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

void affect_particles(Particles &particles, duration time,
					  const Vector2 &position, real gravity, real mass) noexcept
{
	for (auto i = 0; i < std::ssize(particles); ++i)
	{
		//Newton's law of universal gravitation
		auto distance = position - particles.Position(i);
		auto length = distance.SquaredLength();

		if (length > 0.0_r)
		{
			auto force = gravity * mass * particles.Mass(i) / length;
			particles.Direction(i, particles.Direction(i) + distance * force * time.count());
		}
	}
}

} //gravitation::detail


//Protected

/*
	Affect particles
*/

void Gravitation::DoAffect(Particles &particles, duration time) noexcept
{
	detail::affect_particles(particles, time, position_, gravity_, mass_);
}


//Public

Gravitation::Gravitation(std::string name) noexcept :
	Affector{std::move(name)}
{
	//Empty
}

Gravitation::Gravitation(std::string name, const Vector2 &position, real mass) noexcept :
	Gravitation{std::move(name), position, mass, detail::g}
{
	//Empty
}

Gravitation::Gravitation(std::string name, const Vector2 &position, real mass, real gravity) noexcept :

	Affector{std::move(name)},

	position_{position},
	mass_{mass},
	gravity_{gravity}	
{
	//Empty
}


/*
	Cloning
*/

OwningPtr<Affector> Gravitation::Clone() const
{
	return make_owning<Gravitation>(*this);
}

} //ion::graphics::particles::affectors