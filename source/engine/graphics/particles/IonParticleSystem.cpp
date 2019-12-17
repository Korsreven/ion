/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonParticleSystem.cpp
-------------------------------------------
*/

#include "IonParticleSystem.h"

namespace ion::graphics::particles
{

using namespace particle_system;

namespace particle_system::detail
{

} //particle_system::detail


ParticleSystem::ParticleSystem(ParticlePrimitive particle_primitive) noexcept :
	particle_primitive_{particle_primitive}
{
	//Empty
}


/*
	Common functions for controlling the particle system
*/

void ParticleSystem::StartAll() noexcept
{
	for (auto &emitter : emitters_)
		emitter->Start();
}

void ParticleSystem::StopAll() noexcept
{
	for (auto &emitter : emitters_)
		emitter->Stop();
}

void ParticleSystem::ResetAll() noexcept
{
	for (auto &emitter : emitters_)
		emitter->Reset();
}

void ParticleSystem::RestartAll() noexcept
{
	for (auto &emitter : emitters_)
		emitter->Restart();
}


/*
	Elapse time
*/

void ParticleSystem::Elapse(duration time) noexcept
{
	//Elapse emitters
	for (auto &emitter : emitters_)
	{
		emitter->Elapse(time);

		//Affect particles (in emitter)
		for (auto &affector : Affectors())
			affector.Affect(emitter->Particles(), time);
	}
}


/*
	Creating
*/

Emitter& ParticleSystem::CreateEmitter()
{
	auto &emitter = emitters_.emplace_back(
		std::make_unique<Emitter>());
	return *emitter;
}

Emitter& ParticleSystem::CreateEmitter(Emitter emitter)
{
	auto &ref = emitters_.emplace_back(
		std::make_unique<Emitter>(std::move(emitter)));
	return *ref;
}


/*
	Removing
*/

void ParticleSystem::ClearEmitters() noexcept
{
	emitters_.clear();
	emitters_.shrink_to_fit();
}

bool ParticleSystem::RemoveEmitter(Emitter &emitter) noexcept
{
	auto iter =
		std::find_if(std::begin(emitters_), std::end(emitters_),
			[&](auto &x) noexcept
			{
				return x.get() == &emitter;
			});

	//Affector found
	if (iter != std::end(emitters_))
	{
		emitters_.erase(iter);
		return true;
	}
	else
		return false;
}


/*
	Particles
*/

bool ParticleSystem::HasActiveParticles() const noexcept
{
	for (auto &emitter : emitters_)
	{
		if (emitter->HasActiveParticles())
			return true;
	}

	return false;
}

} //ion::graphics::particles