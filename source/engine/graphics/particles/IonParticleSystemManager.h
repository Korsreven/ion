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


	///@brief A class that manages and stores particle systems
	class ParticleSystemManager final :
		public managed::ObjectManager<ParticleSystem, ParticleSystemManager>
	{
		public:

			///@brief Default constructor
			ParticleSystemManager() = default;

			///@brief Deleted copy constructor
			ParticleSystemManager(const ParticleSystemManager&) = delete;

			///@brief Default move constructor
			ParticleSystemManager(ParticleSystemManager&&) = default;

			///@brief Destructor
			~ParticleSystemManager() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			ParticleSystemManager& operator=(const ParticleSystemManager&) = delete;

			///@brief Default move assignment
			ParticleSystemManager& operator=(ParticleSystemManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all particle systems in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ParticleSystems() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all particle systems in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ParticleSystems() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Particle systems - Creating
				@{
			*/

			///@brief Creates a particle system with the given name
			NonOwningPtr<ParticleSystem> CreateParticleSystem(std::string name);

			///@brief Creates a particle system with the given name and particle primitive
			NonOwningPtr<ParticleSystem> CreateParticleSystem(std::string name, particle_system::ParticlePrimitive particle_primitive);


			///@brief Creates a particle system by moving the given particle system
			NonOwningPtr<ParticleSystem> CreateParticleSystem(ParticleSystem &&particle_system);

			///@}

			/**
				@name Particle systems - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable particle system with the given name
			///@details Returns nullptr if particle system could not be found
			[[nodiscard]] NonOwningPtr<ParticleSystem> GetParticleSystem(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable particle system with the given name
			///@details Returns nullptr if particle system could not be found
			[[nodiscard]] NonOwningPtr<const ParticleSystem> GetParticleSystem(std::string_view name) const noexcept;

			///@}

			/**
				@name Particle systems - Removing
				@{
			*/

			///@brief Clears all removable particle systems from this manager
			void ClearParticleSystems() noexcept;

			///@brief Removes a removable particle system from this manager
			bool RemoveParticleSystem(ParticleSystem &particle_system) noexcept;

			///@brief Removes a removable particle system with the given name from this manager
			bool RemoveParticleSystem(std::string_view name) noexcept;

			///@}
	};
} //ion::graphics::particles

#endif