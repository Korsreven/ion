/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonParticleSystemManager.h
-------------------------------------------
*/

#ifndef ION_PARTICLE_SYSTEM_MANAGER_H
#define ION_PARTICLE_SYSTEM_MANAGER_H

#include <string>
#include <string_view>

#include "IonParticleSystem.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::particles
{
	namespace particle_system_manager::detail
	{
	} //particle_system_manager::detail


	struct ParticleSystemManager final :
		managed::ObjectManager<ParticleSystem, ParticleSystemManager>
	{
		//Default constructor
		ParticleSystemManager() = default;

		//Deleted copy constructor
		ParticleSystemManager(const ParticleSystemManager&) = delete;

		//Default move constructor
		ParticleSystemManager(ParticleSystemManager&&) = default;

		//Destructor
		~ParticleSystemManager() = default;


		/*
			Operators
		*/

		//Deleted copy assignment
		ParticleSystemManager& operator=(const ParticleSystemManager&) = delete;

		//Move assignment
		ParticleSystemManager& operator=(ParticleSystemManager&&) = default;


		/*
			Ranges
		*/

		//Returns a mutable range of all particle systems in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto ParticleSystems() noexcept
		{
			return Objects();
		}

		//Returns an immutable range of all particle systems in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto ParticleSystems() const noexcept
		{
			return Objects();
		}


		/*
			Particle systems
			Creating
		*/

		//Create a particle system with the given name
		NonOwningPtr<ParticleSystem> CreateParticleSystem(std::string name);

		//Create a particle system with the given name and particle primitive
		NonOwningPtr<ParticleSystem> CreateParticleSystem(std::string name, particle_system::ParticlePrimitive particle_primitive);


		//Create a particle system by moving the given particle system
		NonOwningPtr<ParticleSystem> CreateParticleSystem(ParticleSystem &&particle_system);


		/*
			Particle systems
			Retrieving
		*/

		//Gets a pointer to a mutable particle system with the given name
		//Returns nullptr if particle system could not be found
		[[nodiscard]] NonOwningPtr<ParticleSystem> GetParticleSystem(std::string_view name) noexcept;

		//Gets a pointer to an immutable particle system with the given name
		//Returns nullptr if particle system could not be found
		[[nodiscard]] NonOwningPtr<const ParticleSystem> GetParticleSystem(std::string_view name) const noexcept;


		/*
			Particle systems
			Removing
		*/

		//Clear all removable particle systems from this manager
		void ClearParticleSystems() noexcept;

		//Remove a removable particle system from this manager
		bool RemoveParticleSystem(ParticleSystem &particle_system) noexcept;

		//Remove a removable particle system with the given name from this manager
		bool RemoveParticleSystem(std::string_view name) noexcept;
	};
} //ion::graphics::particles

#endif