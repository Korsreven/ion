/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonSineForce.h
-------------------------------------------
*/

#ifndef ION_SINE_FORCE_H
#define ION_SINE_FORCE_H

#include <algorithm>
#include <utility>

#include "IonAffector.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"
#include "types/IonCumulative.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"
#include "utilities/IonRandom.h"

namespace ion::graphics::particles::affectors
{
	using namespace types::type_literals;
	using graphics::utilities::Vector2;
	using types::Cumulative;

	namespace sine_force
	{
		enum class ForceType : bool
		{
			Add,
			Average
		};


		namespace detail
		{
			void affect_particles(affector::detail::particle_range particles, duration time,
								  ForceType type, const Vector2 &force, real angle) noexcept;
		} //detail
	} //sine_force


	///@brief A class representing an affector that can add a sine force to single particles
	class SineForce final : public Affector
	{
		private:

			sine_force::ForceType type_ = sine_force::ForceType::Add;
			Vector2 force_;

			std::pair<real, real> frequency_;			
			real current_frequency_ = 0.0_r;
			Cumulative<real> current_angle_{ion::utilities::math::TwoPi};

		protected:

			/**
				@name Affect particles
				@{
			*/

			///@brief Elapses sine force by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void DoAffect(affector::detail::particle_range particles, duration time) noexcept override;

			///@}

		public:

			///@brief Constructs a new sine force with the given name
			explicit SineForce(std::string name) noexcept;

			///@brief Constructs a new sine force with the given name, type, force and frequency
			SineForce(std::string name, sine_force::ForceType type, const Vector2 &force, real frequency) noexcept;

			///@brief Constructs a new sine force with the given name, type, force, min and max frequency
			SineForce(std::string name, sine_force::ForceType type, const Vector2 &force, real min_frequency, real max_frequency) noexcept;


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

			///@brief Sets the force type of the sine force
			inline void Type(sine_force::ForceType type) noexcept
			{
				type_ = type;
			}

			///@brief Sets the force of the sine force
			inline void Force(const Vector2 &force) noexcept
			{
				force_ = force;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the force type of the sine force
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			///@brief Returns the force of the sine force
			[[nodiscard]] inline auto Force() const noexcept
			{
				return force_;
			}

			///@brief Sets the frequency range of the sine force to the given range
			inline void Frequency(real frequency) noexcept
			{
				frequency_ = std::pair(frequency, frequency);
				current_frequency_ = frequency;
			}

			///@brief Sets the frequency range of the sine force to the given range
			inline void Frequency(real min_frequency, real max_frequency) noexcept
			{
				frequency_ = std::minmax(min_frequency, max_frequency);
				current_frequency_ = ion::utilities::random::Number(frequency_.first, frequency_.second);
			}

			///@}
	};
} //ion::graphics::particles::affectors

#endif