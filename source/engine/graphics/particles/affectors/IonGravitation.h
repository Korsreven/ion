/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonGravitation.h
-------------------------------------------
*/

#ifndef ION_GRAVITATION_H
#define ION_GRAVITATION_H

#include <algorithm>

#include "IonAffector.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::particles::affectors
{
	using namespace types::type_literals;
	using graphics::utilities::Vector2;

	namespace gravitation::detail
	{
		constexpr auto g = 6.67430E-11_r;

		void affect_particles(affector::detail::particle_range particles, duration time,
							  const Vector2 &position, real gravity, real mass) noexcept;
	} //gravitation::detail


	//A class representing an affector that can act as graviation on single particles
	class Gravitation final : public Affector
	{
		private:

			Vector2 position_;	
			real mass_ = 0.0_r;
			real gravity_ = gravitation::detail::g;

		protected:

			/*
				Affect particles
			*/

			//Elapse gravitation by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void DoAffect(affector::detail::particle_range particles, duration time) noexcept override;

		public:

			//Construct a new gravitation with the given name
			explicit Gravitation(std::string name);

			//Construct a new gravitation with the given name, position and mass
			Gravitation(std::string name, const Vector2 &position, real mass);

			//Construct a new gravitation with the given name, position, mass and a custom gravity
			Gravitation(std::string name, const Vector2 &position, real mass, real gravity);


			/*
				Cloning
			*/

			//Returns an owning ptr to a clone of this affector
			[[nodiscard]] OwningPtr<Affector> Clone() const override;


			/*
				Modifiers
			*/

			//Sets the position of the gravitation
			inline void Position(const Vector2 &position) noexcept
			{
				position_ = position;
			}

			//Sets the mass of the gravitation
			inline void Mass(real mass) noexcept
			{
				mass_ = mass;
			}

			//Sets the gravity of the gravitation
			inline void Gravity(real gravity) noexcept
			{
				gravity_ = gravity;
			}


			/*
				Observers
			*/

			//Returns the position of the gravitation
			[[nodiscard]] inline auto Position() const noexcept
			{
				return position_;
			}

			//Returns the mass of the gravitation
			[[nodiscard]] inline auto Mass() const noexcept
			{
				return mass_;
			}

			//Returns the gravity of the gravitation
			[[nodiscard]] inline auto Gravity() const noexcept
			{
				return gravity_;
			}
	};
} //ion::graphics::particles::affectors

#endif