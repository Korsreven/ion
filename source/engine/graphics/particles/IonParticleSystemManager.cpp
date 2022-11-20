/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonParticleSystemManager.cpp
-------------------------------------------
*/

#include "IonParticleSystemManager.h"

namespace ion::graphics::particles
{

using namespace particle_system_manager;

namespace particle_system_manager::detail
{

} //particle_system_manager::detail


//Public


/*
	Particle systems - Creating
*/

NonOwningPtr<ParticleSystem> ParticleSystemManager::CreateParticleSystem(std::string name)
{
	return Create(std::move(name));
}

NonOwningPtr<ParticleSystem> ParticleSystemManager::CreateParticleSystem(std::string name, particle_system::ParticlePrimitive particle_primitive)
{
	return Create(std::move(name), particle_primitive);
}


NonOwningPtr<ParticleSystem> ParticleSystemManager::CreateParticleSystem(ParticleSystem &&particle_system)
{
	return Create(std::move(particle_system));
}


/*
	Particle systems - Retrieving
*/

NonOwningPtr<ParticleSystem> ParticleSystemManager::GetParticleSystem(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const ParticleSystem> ParticleSystemManager::GetParticleSystem(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Particle systems - Removing
*/

void ParticleSystemManager::ClearParticleSystems() noexcept
{
	Clear();
}

bool ParticleSystemManager::RemoveParticleSystem(ParticleSystem &particle_system) noexcept
{
	return Remove(particle_system);
}

bool ParticleSystemManager::RemoveParticleSystem(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::graphics::particles