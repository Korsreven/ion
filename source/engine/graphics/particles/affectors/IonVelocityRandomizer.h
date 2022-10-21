/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonVelocityRandomizer.h
-------------------------------------------
*/

#ifndef ION_VELOCITY_RANDOMIZER_H
#define ION_VELOCITY_RANDOMIZER_H

#include <algorithm>
#include <utility>

#include "IonAffector.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::particles::affectors
{
	using namespace types::type_literals;

	namespace velocity_randomizer::detail
	{
		void affect_particles(affector::detail::particle_range particles, duration time,
							  const std::pair<real, real> &velocity, real scope) noexcept;
	} //velocity_randomizer::detail


	//A class representing an affector that can add random velocity to single particles
	class VelocityRandomizer final : public Affector
	{
		private:

			std::pair<real, real> velocity_;
			real scope_ = 1.0_r;

		protected:

			/*
				Affect particles
			*/

			//Elapse velocity randomizer by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void DoAffect(affector::detail::particle_range particles, duration time) noexcept override;

		public:

			//Construct a new velocity randomizer with the given name
			explicit VelocityRandomizer(std::string name) noexcept;

			//Construct a new velocity randomizer with the given name, velocity and scope
			VelocityRandomizer(std::string name, real velocity, real scope) noexcept;

			//Construct a new velocity randomizer with the  given name, min/max velocity and scope
			VelocityRandomizer(std::string name, real min_velocity, real max_velocity, real scope) noexcept;


			/*
				Cloning
			*/

			//Returns an owning ptr to a clone of this affector
			[[nodiscard]] OwningPtr<Affector> Clone() const override;


			/*
				Modifiers
			*/

			//Sets the velocity range of the velocity randomizer to the given range
			inline void Velocity(real velocity) noexcept
			{
				velocity_ = std::minmax(0.0_r, velocity);
			}

			//Sets the velocity range of the velocity randomizer to the given range
			inline void Velocity(real min_velocity, real max_velocity) noexcept
			{
				velocity_ = std::minmax(min_velocity, max_velocity);
			}

			//Sets the scope of the velocity randomizer to the given value in range [0.0, 1.0]
			inline void Scope(real scope) noexcept
			{
				scope_ = std::clamp(scope, 0.0_r, 1.0_r);
			}


			/*
				Observers
			*/

			//Returns the velocity range of the velocity randomizer
			[[nodiscard]] inline auto Velocity() const noexcept
			{
				return velocity_;
			}

			//Returns the scope of the velocity randomizer in range [0.0, 1.0]
			[[nodiscard]] inline auto Scope() const noexcept
			{
				return scope_;
			}
	};
} //ion::graphics::particles::affectors

#endif