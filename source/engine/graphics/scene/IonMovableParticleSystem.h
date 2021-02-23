/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableParticleSystem.h
-------------------------------------------
*/

#ifndef ION_MOVABLE_PARTICLE_SYSTEM_H
#define ION_MOVABLE_PARTICLE_SYSTEM_H

#include <optional>

#include "IonMovableObject.h"
#include "graphics/particles/IonParticleSystem.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::scene
{
	namespace movable_particle_system::detail
	{
	} //movable_particle_system::detail


	//A movable particle system that can be attached to a scene node
	class MovableParticleSystem final : public MovableObject
	{
		private:

			NonOwningPtr<particles::ParticleSystem> particle_system_;
			std::optional<particles::ParticleSystem> my_particle_system_;

		public:

			//Construct a new movable particle system with the given particle system
			explicit MovableParticleSystem(NonOwningPtr<particles::ParticleSystem> particle_system);


			/*
				Modifiers
			*/

			//Revert to initial particle system setup
			void Revert();


			/*
				Observers
			*/

			//Returns a mutable reference to the particle system
			[[nodiscard]] auto& Get() noexcept
			{
				return my_particle_system_;
			}

			//Returns an immutable reference to the particle system
			[[nodiscard]] auto& Get() const noexcept
			{
				return my_particle_system_;
			}
	};
} //ion::graphics::scene

#endif