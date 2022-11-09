/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonParticle.h
-------------------------------------------
*/

#ifndef ION_PARTICLE_H
#define ION_PARTICLE_H

#include <utility>

#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonCumulative.h"
#include "types/IonTypes.h"

namespace ion::graphics::particles
{
	using namespace types::type_literals;

	using types::Cumulative;
	using utilities::Color;
	using utilities::Vector2;
	using utilities::Vector3;


	///@brief Particle class that can be represented by...
	///@details 1. A single point (1 vertex).
	///2. A rectangle (4 vertices)
	class Particle final
	{
		private:

			Vector3 position_;
			Vector2 direction_; ///<Length represents velocity		
			real rotation_ = 0.0_r;
			Vector2 size_;
			real mass_ = 0.0_r;
			Color color_;
			Cumulative<duration> lifetime_;

			Vector2 initial_direction_;

		public:

			///@brief Default constructor
			Particle() = default;

			///@brief Constructs a new particle from the given initial values
			Particle(const Vector3 &position, const Vector2 &direction,
					 const Vector2 &size, real mass, const Color &color,
					 duration lifetime, const Vector2 &initial_direction) noexcept;


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the position of the particle to the given vector
			inline void Position(const Vector3 &position) noexcept
			{
				position_ = position;
			}

			///@brief Sets the position of the particle to the given vector
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}

			///@brief Sets the direction of the particle to the given vector
			///@details Where the length of the given vector represents velocity
			inline void Direction(const Vector2 &direction) noexcept
			{
				direction_ = direction;
				rotation_ = direction.SignedAngleBetween(initial_direction_); //Update rotation
			}

			///@brief Sets the velocity (direction length) of the particle to the given value
			inline void Velocity(real velocity) noexcept
			{
				direction_.Normalize();
				direction_ *= velocity;
			}

			///@brief Sets the size of the particle to the given vector
			inline void Size(const Vector2 &size) noexcept
			{
				size_ = size;
			}

			///@brief Sets the mass of the particle to the given value
			inline void Mass(real mass) noexcept
			{
				mass_ = mass;
			}

			///@brief Sets the color of the particle to the given color
			inline void FillColor(const Color &color) noexcept
			{
				color_ = color;
			}

			///@brief Sets the lifetime of the particle to the given duration
			inline void Lifetime(duration lifetime) noexcept
			{
				lifetime_.Limit(lifetime);
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the position of the particle
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			///@brief Returns the direction of the particle
			///@details Where the length of the vector represents velocity
			[[nodiscard]] inline auto& Direction() const noexcept
			{
				return direction_;
			}

			///@brief Returns the velocity (direction length) of the particle
			[[nodiscard]] inline auto Velocity() const noexcept
			{
				return direction_.Length();
			}

			///@brief Returns the angle of rotation (in radians) of the particle
			[[nodiscard]] inline auto& Rotation() const noexcept
			{
				return rotation_;
			}

			///@brief Returns the size of the particle
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}

			///@brief Returns the mass of the particle
			[[nodiscard]] inline auto Mass() const noexcept
			{
				return mass_;
			}

			///@brief Returns the color of the particle
			[[nodiscard]] inline auto& FillColor() const noexcept
			{
				return color_;
			}

			///@brief Returns the lifetime of the particle
			[[nodiscard]] inline auto Lifetime() const noexcept
			{
				return lifetime_.Limit();
			}

			///@brief Returns the lifetime percent of the particle in range [0.0, 1.0]
			[[nodiscard]] inline auto LifetimePercent() const noexcept
			{
				return lifetime_.Percent();
			}

			///@}

			/**
				@name Evolving
				@{
			*/

			///@brief Evolves particle by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			inline auto Evolve(duration time) noexcept
			{
				if (lifetime_ += time)
					return false;
				else
				{
					position_ += direction_ * time.count();
					return true;
				}
			}

			///@}
	};
} //ion::graphics::particles

#endif