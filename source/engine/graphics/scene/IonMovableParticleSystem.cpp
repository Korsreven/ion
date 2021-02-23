/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableParticleSystem.cpp
-------------------------------------------
*/

#include "IonMovableParticleSystem.h"

namespace ion::graphics::scene
{

using namespace movable_particle_system;

namespace movable_particle_system::detail
{
} //movable_particle_system::detail


MovableParticleSystem::MovableParticleSystem(NonOwningPtr<particles::ParticleSystem> particle_system) :
	particle_system_{particle_system},
	my_particle_system_{particle_system ? std::make_optional(particle_system->Clone()) : std::nullopt}
{
	//Empty
}


/*
	Modifiers
*/

void MovableParticleSystem::Revert()
{
	if (particle_system_)
		my_particle_system_ = particle_system_->Clone();
}

} //ion::graphics::scene