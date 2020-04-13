/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonEmitter.h
-------------------------------------------
*/

#ifndef ION_EMITTER_H
#define ION_EMITTER_H

#include <algorithm>
#include <cmath>
#include <optional>
#include <utility>
#include <vector>

#include "IonParticle.h"
#include "adaptors/ranges/IonIterable.h"
#include "affectors/IonAffectorFactory.h"
#include "graphics/textures/IonTextureManager.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonObservedObject.h"
#include "types/IonCumulative.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"
#include "utilities/IonRandom.h"

namespace ion::graphics::particles
{
	using namespace types::type_literals;

	using types::Cumulative;
	using utilities::Color;
	using utilities::Vector2;


	namespace emitter
	{
		enum class EmitterType
		{
			Point,
			Box,
			Ring
		};

		namespace detail
		{
			template <typename T>
			using container_type = std::vector<T>;


			inline auto box_coordinate(real coord, real rand, real half_size, real half_inner_size1, real half_inner_size2) noexcept
			{
				return coord > -half_inner_size1 && coord < half_inner_size1 ?
						(half_size - half_inner_size2) * rand + (rand < 0.0_r ? -half_inner_size2 : half_inner_size2) :
						half_size * rand;
			}

			inline auto position_inside_box(const Vector2 &half_size, const Vector2 &half_inner_size) noexcept
			{
				using namespace ion::utilities;

				if (random::Number(0, 1))
				{
					auto x = half_size.X() * random::Number(-1.0_r, 1.0_r);
					return Vector2{x, box_coordinate(x, random::Number(-1.0_r, 1.0_r), half_size.Y(), half_inner_size.X(), half_inner_size.Y())};
				}
				else
				{
					auto y = half_size.Y() * random::Number(-1.0_r, 1.0_r);
					return Vector2{box_coordinate(y, random::Number(-1.0_r, 1.0_r), half_size.X(), half_inner_size.Y(), half_inner_size.X()), y};
				}
			}

			inline auto position_inside_ring(const Vector2 &half_size, const Vector2 &half_inner_size) noexcept
			{
				using namespace ion::utilities;

				auto theta = math::TwoPi * random::Number();
				auto radius = (half_size - half_inner_size) * random::Number() + half_inner_size;
				return Vector2{radius.X() * math::Cos(theta),
							   radius.Y() * math::Sin(theta)};
			}


			inline auto particle_position(EmitterType emitter_type, const Vector2 &half_size, const Vector2 &half_inner_size) noexcept
			{
				switch (emitter_type)
				{
					case EmitterType::Box:
					return position_inside_box(half_size, half_inner_size);

					case EmitterType::Ring:
					return position_inside_ring(half_size, half_inner_size);

					default: //Point
					return utilities::vector2::Zero;
				}
			}

			inline auto particle_velocity(real min_velocity, real max_velocity)
			{
				return ion::utilities::random::Number(min_velocity, max_velocity);
			}

			inline auto particle_direction(const Vector2 &direction, real angle)
			{
				return direction.RandomDeviant(angle);
			}

			inline auto particle_direction(const Vector2 &direction, real angle, real min_velocity, real max_velocity)
			{
				return particle_direction(direction, angle) * particle_velocity(min_velocity, max_velocity);
			}

			inline auto particle_size(const Vector2 &min_size, const Vector2 &max_size)
			{
				using namespace ion::utilities;

				auto [min_x, min_y] = min_size.XY();
				auto [max_x, max_y] = max_size.XY();
				return Vector2{random::Number(min_x, max_x),
							   random::Number(min_y, max_y)};
			}

			inline auto particle_mass(real min_mass, real max_mass)
			{
				return ion::utilities::random::Number(min_mass, max_mass);
			}

			inline auto particle_solid_color(const Color &from_solid_color, const Color &to_solid_color)
			{
				return from_solid_color.MixCopy(to_solid_color, ion::utilities::random::Number());
			}

			inline auto particle_life_time(duration min_life_time, duration max_life_time)
			{
				return duration{ion::utilities::random::Number(min_life_time.count(), max_life_time.count())};
			}


			void evolve_particles(container_type<Particle> &particles, duration time) noexcept;
		} //detail
	} //emitter


	//An emitter class that can emit multiple particles
	class Emitter final : public affectors::AffectorFactory
	{
		private:

			emitter::EmitterType type_ = emitter::EmitterType::Point;
			Vector2 position_;
			Vector2 direction_;
			Vector2 size_;
			Vector2 inner_size_;

			real emission_rate_ = 0.0_r;
			real emission_angle_ = 0.0_r;
			std::optional<Cumulative<duration>> emission_duration_;			

			int particle_quota_ = 0;
			bool emitting_ = false;
			Cumulative<real> emission_amount_{1.0_r};

			emitter::detail::container_type<Particle> particles_;


			//Initial spawn values for each new particle, in range [first, second]
			
			std::pair<real, real> particle_velocity_;
			std::pair<Vector2, Vector2> particle_size_;		
			std::pair<real, real> particle_mass_;
			std::pair<Color, Color> particle_solid_color_;
			std::pair<duration, duration> particle_life_time_;
			managed::ObservedObject<textures::Texture> particle_texture_;

		public:

			//Default constructor
			Emitter() = default;

			//Construct a new emitter from the given initial values
			Emitter(emitter::EmitterType type, const Vector2 &position, const Vector2 &direction,
				const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
				std::optional<duration> emission_duration, int particle_quota = 100);


			/*
				Static emitter conversions
			*/

			//Returns a new point emitter from the given initial values
			[[nodiscard]] static Emitter Point(const Vector2 &position, const Vector2 &direction,
				real emission_rate, real emission_angle, std::optional<duration> emission_duration,
				int particle_quota = 100);

			//Returns a new box emitter from the given initial values
			[[nodiscard]] static Emitter Box(const Vector2 &position, const Vector2 &direction,
				const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
				std::optional<duration> emission_duration, int particle_quota = 100);

			//Returns a new ring emitter from the given initial values
			[[nodiscard]] static Emitter Ring(const Vector2 &position, const Vector2 &direction,
				const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
				std::optional<duration> emission_duration, int particle_quota = 100);


			/*
				Modifiers
			*/

			//Sets the type of the emitter to the given value
			inline void Type(emitter::EmitterType type) noexcept
			{
				type_ = type;
			}

			//Sets the position of the emitter to the given vector
			inline void Position(const Vector2 &position) noexcept
			{
				position_ = position;
			}

			//Sets the direction of the emitter to the given vector
			inline void Direction(const Vector2 &direction) noexcept
			{
				direction_ = direction;
			}

			//Sets the size of the emitter to the given vector
			//Only in use if the emitter type is either a box or a ring
			inline void Size(const Vector2 &size) noexcept
			{
				size_ = size;
			}

			//Sets the inner size of the emitter to the given vector
			//Only in use if the emitter type is either a box or a ring
			inline void InnerSize(const Vector2 &inner_size) noexcept
			{
				inner_size_ = inner_size;
			}


			//Sets the emission rate of the emitter to the given value
			inline void EmissionRate(real rate) noexcept
			{
				emission_rate_ = rate;
			}

			//Sets the emission angle of the emitter to the given value in range [0.0, pi]
			inline void EmissionAngle(real angle) noexcept
			{
				emission_angle_ = std::clamp(angle, 0.0_r, ion::utilities::math::Pi);
			}

			//Sets the emission duration of the emitter to the given amount
			inline void EmissionDuration(std::optional<duration> amount) noexcept
			{
				if (amount)
				{
					if (emission_duration_)
						emission_duration_->Limit(*amount);
					else
						emission_duration_.emplace(*amount);
				}
				else
					emission_duration_.reset();
			}


			//Sets the particle quota to the given value
			//Particle quota is the max number of particles that can simultaneously be active 
			inline void ParticleQuota(int quota)
			{
				particle_quota_ = quota > 0 ? quota : 0;

				//Erase all particles that does not fit quota
				if (particle_quota_ < static_cast<int>(std::size(particles_)))
					particles_.erase(std::begin(particles_) + particle_quota_, std::end(particles_));
			}


			/*
				Observers
			*/

			//Returns the type of the emitter
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			//Returns the position of the emission
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the direction of the emission
			[[nodiscard]] inline auto& Direction() const noexcept
			{
				return direction_;
			}

			//Returns the size of the emitter
			//Only in use if the emitter type is either a box or a ring
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}

			//Returns the inner size of the emitter
			//Only in use if the emitter type is either a box or a ring
			[[nodiscard]] inline auto& InnerSize() const noexcept
			{
				return inner_size_;
			}


			//Returns the emission rate of the emitter
			[[nodiscard]] inline auto EmissionRate() const noexcept
			{
				return emission_rate_;
			}

			//Returns the emission angle of the emitter in range [0.0, pi]
			[[nodiscard]] inline auto EmissionAngle() const noexcept
			{
				return emission_angle_;
			}

			//Returns the emission duration of the emitter
			//Returns nullopt if no duration is set
			[[nodiscard]] inline auto EmissionDuration() const noexcept
			{
				return emission_duration_;
			}

			//Returns the emission duration percent of the emitter in range [0.0, 1.0]
			[[nodiscard]] inline auto EmissionDurationPercent() const noexcept
			{
				return emission_duration_ ?
					emission_duration_->Percent() :
					0.0_r;
			}


			//Returns the particle quota of the emitter
			//The max number of simultaneous particles allowed
			[[nodiscard]] inline auto ParticleQuota() const noexcept
			{
				return particle_quota_;
			}

			//Returns true if the emitter is emitting particles
			[[nodiscard]] inline auto IsEmitting() const noexcept
			{
				return emitting_;
			}


			/*
				Spawn modifiers
			*/

			//Sets the velocity of each new particle to the given value
			inline void ParticleVelocity(real velocity) noexcept
			{
				particle_velocity_ = std::pair{velocity, velocity};
			}

			//Sets the velocity range of each new particle to the given range
			inline void ParticleVelocity(real min_velocity, real max_velocity) noexcept
			{
				particle_velocity_ = std::minmax(min_velocity, max_velocity);
			}

			//Sets the size of each new particle to the given value
			inline void ParticleSize(const Vector2 &size) noexcept
			{
				particle_size_ = std::pair{size, size};
			}

			//Sets the size range of each new particle to the given range
			inline void ParticleSize(const Vector2 &min_size, const Vector2 &max_size) noexcept
			{
				particle_size_ = std::minmax(min_size, max_size);
			}

			//Sets the mass of each new particle to the given value
			inline void ParticleMass(real mass) noexcept
			{
				particle_mass_ = std::pair{mass, mass};
			}

			//Sets the mass range of each new particle to the given range
			inline void ParticleMass(real min_mass, real max_mass) noexcept
			{
				particle_mass_ = std::minmax(min_mass, max_mass);
			}

			//Sets the solid color of each new particle to the given value
			inline void ParticleSolidColor(const Color &solid_color) noexcept
			{
				particle_solid_color_ = std::pair(solid_color, solid_color);
			}

			//Sets the solid color range of each new particle to the given range
			//The final color of the new particle is determined by mixing both colors with a random percentage
			inline void ParticleSolidColor(const Color &from_solid_color, const Color &to_solid_color) noexcept
			{
				particle_solid_color_ = std::pair(from_solid_color, to_solid_color); //minmax not needed
			}

			//Sets the life time of each new particle to the given value
			inline void ParticleLifeTime(duration life_time) noexcept
			{
				particle_life_time_ = std::pair(life_time, life_time);
			}

			//Sets the life time range of each new particle to the given range
			inline void ParticleLifeTime(duration min_life_time, duration max_life_time) noexcept
			{
				particle_life_time_ = std::minmax(min_life_time, max_life_time);
			}
			
			//Attach the given texture to all active and new particles
			void ParticleTexture(textures::Texture &texture);

			//Detach (if any) particle texture from all active and new particles
			void ParticleTexture(std::nullptr_t) noexcept;


			/*
				Spawn observers
			*/

			//Returns the velocity of each new particle in range [min, max]
			[[nodiscard]] inline auto& ParticleVelocity() const noexcept
			{
				return particle_velocity_;
			}

			//Returns the size of each new particle in range [min, max]
			[[nodiscard]] inline auto& ParticleSize() const noexcept
			{
				return particle_size_;
			}

			//Returns the mass of each new particle in range [min, max]
			[[nodiscard]] inline auto& ParticleMass() const noexcept
			{
				return particle_mass_;
			}

			//Returns the solid color of each new particle in range [from, to]
			[[nodiscard]] inline auto& ParticleSolidColor() const noexcept
			{
				return particle_solid_color_;
			}

			//Returns the life time of each new particle in range [min, max]
			[[nodiscard]] inline auto& ParticleLifeTime() const noexcept
			{
				return particle_life_time_;
			}

			//Returns the attached particle texture
			//Returns nullptr if no particle texture is attached
			[[nodiscard]] inline auto ParticleTexture() const noexcept
			{
				return particle_texture_.Object();
			}


			/*
				Common functions for controlling the emitter
			*/

			//Starts or resumes emitter
			void Start() noexcept;

			//Stops emitter
			void Stop() noexcept;

			//Stops and reset emission duration to zero
			void Reset() noexcept;

			//Stops, resets and starts emitter
			void Restart() noexcept;


			/*
				Elapse time
			*/

			//Elapse emitter by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;


			/*
				Emitting
			*/

			//Emit the given particle count with the current emission rate
			void Emit(int particle_count) noexcept;


			/*
				Particles
			*/

			//Clears all particles emitted by this emitter
			void ClearParticles() noexcept;

			//Returns true if this emitter has any active particles
			[[nodiscard]] inline auto HasActiveParticles() const noexcept
			{
				return !std::empty(particles_);
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all particles in this emitter
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Particles() noexcept
			{
				return adaptors::ranges::Iterable<emitter::detail::container_type<Particle>&>{particles_};
			}

			//Returns an immutable range of all particles in this emitter
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Particles() const noexcept
			{
				return adaptors::ranges::Iterable<const emitter::detail::container_type<Particle>&>{particles_};
			}
	};
} //ion::graphics::particles

#endif