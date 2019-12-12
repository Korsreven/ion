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
#include "types/IonCumulative.h"
#include "types/IonTypes.h"

namespace ion::graphics::particles
{
	using namespace types::type_literals;

	using types::Cumulative;
	using utilities::Color;
	using utilities::Vector2;


	//Particle class that can be represented by...
	//1. A single point (1 vertex)
	//2. A rectangle (4 vertices)
	class Particle final
	{
		private:

			Vector2 position_;
			Vector2 direction_; //Length represents velocity
			Vector2 size_;
			real mass_ = 0.0_r;
			Color solid_color_;
			Cumulative<duration> life_time_;

		public:

			Particle() = default;

			//Constructs a new particle from the given initial values
			Particle(const Vector2 &position, const Vector2 &direction,
					 const Vector2 &size, real mass, const Color &solid_color,
					 duration life_time) noexcept;


			/*
				Modifiers
			*/

			//Sets the position of the particle to the given vector
			inline void Position(const Vector2 &position) noexcept
			{
				position_ = position;
			}

			//Sets the direction of the particle to the given vector
			//Where the length of the given vector represents velocity
			inline void Direction(const Vector2 &direction) noexcept
			{
				direction_ = direction;
			}

			//Sets the velocity (direction length) of the particle to the given value
			inline void Velocity(real velocity) noexcept
			{
				direction_.Normalize();
				direction_ *= velocity;
			}

			//Sets the size of the particle to the given vector
			inline void Size(const Vector2 &size) noexcept
			{
				size_ = size;
			}

			//Sets the mass of the particle to the given value
			inline void Mass(real mass) noexcept
			{
				mass_ = mass;
			}

			//Sets the solid color of the particle to the given color
			inline void SolidColor(const Color &color) noexcept
			{
				solid_color_ = color;
			}

			//Sets the life time of the particle to the given duration
			inline void LifeTime(duration life_time) noexcept
			{
				life_time_.Limit(life_time);
			}


			/*
				Observers
			*/

			//Returns the position of the particle
			[[nodiscard]] inline const auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the direction of the particle
			//Where the length of the vector represents velocity
			[[nodiscard]] inline const auto& Direction() const noexcept
			{
				return direction_;
			}

			//Returns the velocity (direction length) of the particle
			[[nodiscard]] inline auto Velocity() const noexcept
			{
				return direction_.Length();
			}

			//Returns the size of the particle
			[[nodiscard]] inline const auto& Size() const noexcept
			{
				return size_;
			}

			//Returns the mass of the particle
			[[nodiscard]] inline auto Mass() const noexcept
			{
				return mass_;
			}

			//Returns the solid color of the particle
			[[nodiscard]] inline const auto& SolidColor() const noexcept
			{
				return solid_color_;
			}

			//Returns the life time of the particle
			[[nodiscard]] inline auto LifeTime() const noexcept
			{
				return life_time_.Limit();
			}

			//Returns the life time percent of the particle in range [0.0, 1.0]
			[[nodiscard]] inline auto LifeTimePercent() const noexcept
			{
				return life_time_.Percent();
			}


			/*
				Evolving
			*/

			//Evolve particle by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			inline auto Evolve(duration time) noexcept
			{
				if (life_time_ += time)
					return false;
				else
				{
					position_ += direction_ * time.count();
					return true;
				}
			}
	};
} //ion::graphics::particles

#endif