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

#include <chrono>

#include "types/IonTypes.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::particles
{
	using namespace types::type_literals;

	using utilities::Color;
	using utilities::Vector2;

	namespace particle::detail
	{
	} //particle::detail


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
			Color solid_color_ = utilities::color::White;

			std::chrono::duration<real> life_time_{};
			std::chrono::duration<real> total_life_time_{};	

		public:

			Particle() = default;

			//Constructs a new particle from the given initial values
			Particle(const Vector2 &position, const Vector2 &direction,
					 const Vector2 &size, real mass, const Color &solid_color,
					 std::chrono::duration<real> life_time) noexcept;


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

			//Sets the life time of the particle to the given value
			inline void LifeTime(std::chrono::duration<real> life_time) noexcept
			{
				if (life_time_.count() <= 0.0_r)
					total_life_time_ = life_time;

				life_time_ = life_time;
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

			//Return the life time and total life time of the particle
			[[nodiscard]] inline auto LifeTime() const noexcept
			{
				return std::pair{life_time_, total_life_time_};
			}


			/*
				Evolving
			*/

			//Evolve particle by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			inline auto Evolve(std::chrono::duration<real> time) noexcept
			{
				if ((life_time_ -= time) > std::chrono::duration<real>::zero())
				{
					position_ += direction_ * time.count();
					return true;
				}
				else
					return false;
			}
	};
} //ion::graphics::particles

#endif