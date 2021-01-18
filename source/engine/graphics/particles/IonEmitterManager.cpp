/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonEmitterManager.cpp
-------------------------------------------
*/

#include "IonEmitterManager.h"

namespace ion::graphics::particles
{

using namespace emitter_manager;

namespace emitter_manager::detail
{

} //emitter_manager::detail


//Public

/*
	Emitters
	Creating
*/

NonOwningPtr<Emitter> EmitterManager::CreateEmitter(std::string name)
{
	return Create(std::move(name));
}

NonOwningPtr<Emitter> EmitterManager::CreateEmitter(std::string name, emitter::EmitterType type, const Vector2 &position, const Vector2 &direction,
	const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
	std::optional<duration> emission_duration, int particle_quota)
{
	return Create(std::move(name), type, position, direction,
				  size, inner_size, emission_rate, emission_angle,
				  emission_duration, particle_quota);
}


NonOwningPtr<Emitter> EmitterManager::CreateEmitter(Emitter &&emitter)
{
	return Create(std::move(emitter));
}


/*
	Emitters
	Retrieving
*/

NonOwningPtr<Emitter> EmitterManager::GetEmitter(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const Emitter> EmitterManager::GetEmitter(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Emitters
	Removing
*/

void EmitterManager::ClearEmitters() noexcept
{
	Clear();
}

bool EmitterManager::RemoveEmitter(Emitter &Emitter) noexcept
{
	return Remove(Emitter);
}

} //ion::graphics::particles