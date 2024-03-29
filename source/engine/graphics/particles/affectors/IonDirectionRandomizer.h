/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonDirectionRandomizer.h
-------------------------------------------
*/

#ifndef ION_DIRECTION_RANDOMIZER_H
#define ION_DIRECTION_RANDOMIZER_H

#include <algorithm>

#include "IonAffector.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::particles::affectors
{
	using namespace types::type_literals;

	namespace direction_randomizer::detail
	{
		void affect_particles(affector::detail::particle_range particles, duration time,
							  real angle, real scope) noexcept;
	} //direction_randomizer::detail


	///@brief A class representing an affector that can add random direction to single particles
	class DirectionRandomizer final : public Affector
	{
		private:

			real angle_ = ion::utilities::math::Pi;
			real scope_ = 1.0_r;

		protected:

			/**
				@name Affect particles
				@{
			*/

			///@brief Elapses direction randomizer by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void DoAffect(affector::detail::particle_range particles, duration time) noexcept override;

			///@}

		public:

			///@brief Constructs a new direction randomizer with the given name
			explicit DirectionRandomizer(std::string name) noexcept;

			///@brief Constructs a new direction randomizer with the given name, angle and scope
			DirectionRandomizer(std::string name, real angle, real scope) noexcept;


			/**
				@name Cloning
				@{
			*/

			///@brief Returns an owning ptr to a clone of this affector
			[[nodiscard]] OwningPtr<Affector> Clone() const override;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the angle of the direction randomizer to the given value in range [0.0, pi]
			inline void Angle(real angle) noexcept
			{
				angle_ = std::clamp(angle, 0.0_r, ion::utilities::math::Pi);
			}

			///@brief Sets the scope of the direction randomizer to the given value in range [0.0, 1.0]
			inline void Scope(real scope) noexcept
			{
				scope_ = std::clamp(scope, 0.0_r, 1.0_r);
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the angle of the direction randomizer in range [0.0, pi]
			[[nodiscard]] inline auto Angle() const noexcept
			{
				return angle_;
			}

			///@brief Returns the scope of the direction randomizer in range [0.0, 1.0]
			[[nodiscard]] inline auto Scope() const noexcept
			{
				return scope_;
			}

			///@}
	};
} //ion::graphics::particles::affectors

#endif