/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonParticleSystem.h
-------------------------------------------
*/

#ifndef ION_PARTICLE_SYSTEM_H
#define ION_PARTICLE_SYSTEM_H

#include "IonEmitterManager.h"
#include "affectors/IonAffectorManager.h"
#include "managed/IonManagedObject.h"

namespace ion::graphics::particles
{
	class ParticleSystemManager; //Forward declaration

	namespace particle_system
	{
		enum class ParticlePrimitive : bool
		{
			Point,
			Rectangle
		};

		namespace detail
		{
		} //detail
	} //particle_system


	///@brief A class representing a particle system that can contain multiple emitters and affectors
	class ParticleSystem final :
		public managed::ManagedObject<ParticleSystemManager>,
		public EmitterManager,
		public affectors::AffectorManager
	{
		private:

			particle_system::ParticlePrimitive particle_primitive_ = particle_system::ParticlePrimitive::Point;

		public:

			///@brief Constructs a new particle system with the given name
			explicit ParticleSystem(std::string name) noexcept;

			///@brief Constructs a new particle system with the given name and particle primitive
			ParticleSystem(std::string name, particle_system::ParticlePrimitive particle_primitive) noexcept;


			/**
				@name Cloning
				@{
			*/

			///@brief Returns a clone of this particle system
			[[nodiscard]] ParticleSystem Clone() const;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the render primitive used by this particle system to the given value
			inline void RenderPrimitive(particle_system::ParticlePrimitive particle_primitive) noexcept
			{
				particle_primitive_ = particle_primitive;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the render primitive used by this particle system
			[[nodiscard]] inline auto RenderPrimitive() const noexcept
			{
				return particle_primitive_;
			}

			///@}

			/**
				@name Common functions for controlling the particle system
				@{
			*/

			///@brief Starts or resumes all emitters
			void StartAll() noexcept;

			///@brief Stops all emitters
			void StopAll() noexcept;

			///@brief Stops and reset all emitters
			void ResetAll() noexcept;

			///@brief Stops, resets and starts all emitters
			void RestartAll() noexcept;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses particle system by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;

			///@}

			/**
				@name Particles
				@{
			*/

			///@brief Returns true if this emitter has any active particles
			[[nodiscard]] bool HasActiveParticles() const noexcept;

			///@}
	};
} //ion::graphics::particles

#endif