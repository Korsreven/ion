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
#include "affectors/IonAffectorManager.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonCumulative.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"
#include "utilities/IonRandom.h"

namespace ion::graphics::materials
{
	class Material; //Forward declaration
}

namespace ion::graphics::particles
{
	using namespace types::type_literals;

	using types::Cumulative;
	using utilities::Color;
	using utilities::Vector2;
	using utilities::Vector3;

	class EmitterManager; //Forward declaration

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

			inline auto particle_color(const Color &from_color, const Color &to_color)
			{
				return from_color.MixCopy(to_color, ion::utilities::random::Number());
			}

			inline auto particle_lifetime(duration min_lifetime, duration max_lifetime)
			{
				return duration{ion::utilities::random::Number(min_lifetime.count(), max_lifetime.count())};
			}


			void evolve_particles(container_type<Particle> &particles, duration time) noexcept;
		} //detail
	} //emitter


	///@brief A class representing an emitter that can emit multiple particles and contain multiple affectors
	class Emitter final :
		public managed::ManagedObject<EmitterManager>,
		public affectors::AffectorManager
	{
		private:

			emitter::EmitterType type_ = emitter::EmitterType::Point;
			Vector3 position_;
			Vector2 direction_;
			Vector2 size_;
			Vector2 inner_size_;

			real emission_rate_ = 0.0_r;
			real emission_angle_ = 0.0_r;
			std::optional<Cumulative<duration>> emission_duration_;			

			int particle_quota_ = 100;
			bool emitting_ = false;
			Cumulative<real> emission_amount_{1.0_r};

			emitter::detail::container_type<Particle> particles_;


			//Initial spawn values for each new particle, in range [first, second]
			
			std::pair<real, real> particle_velocity_;
			std::pair<Vector2, Vector2> particle_size_;		
			std::pair<real, real> particle_mass_;
			std::pair<Color, Color> particle_color_;
			std::pair<duration, duration> particle_lifetime_;
			NonOwningPtr<materials::Material> particle_material_;

		public:

			///@brief Constructs a new emitter with the given name
			explicit Emitter(std::string name) noexcept;

			///@brief Constructs a new emitter with the given name and initial values
			Emitter(std::string name, emitter::EmitterType type, const Vector3 &position, const Vector2 &direction,
				const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
				std::optional<duration> emission_duration, int particle_quota = 100) noexcept;


			/**
				@name Static emitter conversions
				@{
			*/

			///@brief Returns a new point emitter from the given name and initial values
			[[nodiscard]] static Emitter Point(std::string name, const Vector3 &position, const Vector2 &direction,
				real emission_rate, real emission_angle, std::optional<duration> emission_duration,
				int particle_quota = 100);

			///@brief Returns a new box emitter from the given name and initial values
			[[nodiscard]] static Emitter Box(std::string name, const Vector3 &position, const Vector2 &direction,
				const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
				std::optional<duration> emission_duration, int particle_quota = 100);

			///@brief Returns a new ring emitter from the given name and initial values
			[[nodiscard]] static Emitter Ring(std::string name, const Vector3 &position, const Vector2 &direction,
				const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
				std::optional<duration> emission_duration, int particle_quota = 100);

			///@}

			/**
				@name Cloning
				@{
			*/

			///@brief Returns an owning ptr to a clone of this emitter
			[[nodiscard]] OwningPtr<Emitter> Clone() const;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the type of the emitter to the given value
			inline void Type(emitter::EmitterType type) noexcept
			{
				type_ = type;
			}

			///@brief Sets the position of the emitter to the given vector
			inline void Position(const Vector3 &position) noexcept
			{
				position_ = position;
			}

			///@brief Sets the position of the emitter to the given vector
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}

			///@brief Sets the direction of the emitter to the given vector
			inline void Direction(const Vector2 &direction) noexcept
			{
				direction_ = direction;
			}

			///@brief Sets the size of the emitter to the given vector
			///@details Only in use if the emitter type is either a box or a ring
			inline void Size(const Vector2 &size) noexcept
			{
				size_ = size;
			}

			///@brief Sets the inner size of the emitter to the given vector
			///@details Only in use if the emitter type is either a box or a ring
			inline void InnerSize(const Vector2 &inner_size) noexcept
			{
				inner_size_ = inner_size;
			}


			///@brief Sets the emission rate of the emitter to the given value
			inline void EmissionRate(real rate) noexcept
			{
				emission_rate_ = rate;
			}

			///@brief Sets the emission angle of the emitter to the given value in range [0.0, pi]
			inline void EmissionAngle(real angle) noexcept
			{
				emission_angle_ = std::clamp(angle, 0.0_r, ion::utilities::math::Pi);
			}

			///@brief Sets the emission duration of the emitter to the given amount
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


			///@brief Sets the particle quota to the given value
			///@details Particle quota is the max number of particles that can simultaneously be active 
			inline void ParticleQuota(int quota)
			{
				particle_quota_ = quota > 0 ? quota : 0;

				//Erase all particles that does not fit quota
				if (particle_quota_ < std::ssize(particles_))
					particles_.erase(std::begin(particles_) + particle_quota_, std::end(particles_));
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the type of the emitter
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			///@brief Returns the position of the emission
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			///@brief Returns the direction of the emission
			[[nodiscard]] inline auto& Direction() const noexcept
			{
				return direction_;
			}

			///@brief Returns the size of the emitter
			///@details Only in use if the emitter type is either a box or a ring
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}

			///@brief Returns the inner size of the emitter
			///@details Only in use if the emitter type is either a box or a ring
			[[nodiscard]] inline auto& InnerSize() const noexcept
			{
				return inner_size_;
			}


			///@brief Returns the emission rate of the emitter
			[[nodiscard]] inline auto EmissionRate() const noexcept
			{
				return emission_rate_;
			}

			//Returns the emission angle of the emitter in range [0.0, pi]
			[[nodiscard]] inline auto EmissionAngle() const noexcept
			{
				return emission_angle_;
			}

			///@brief Returns the emission duration of the emitter
			///@details Returns nullopt if no duration is set
			[[nodiscard]] inline auto EmissionDuration() const noexcept
			{
				return emission_duration_;
			}

			///@brief Returns the emission duration percent of the emitter in range [0.0, 1.0]
			[[nodiscard]] inline auto EmissionDurationPercent() const noexcept
			{
				return emission_duration_ ?
					emission_duration_->Percent() :
					0.0_r;
			}


			///@brief Returns the particle quota of the emitter
			///@details The max number of simultaneous particles allowed
			[[nodiscard]] inline auto ParticleQuota() const noexcept
			{
				return particle_quota_;
			}

			///@brief Returns true if the emitter is emitting particles
			[[nodiscard]] inline auto IsEmitting() const noexcept
			{
				return emitting_;
			}

			///@}

			/**
				@name Spawn modifiers
				@{
			*/

			///@brief Sets the velocity of each new particle to the given value
			inline void ParticleVelocity(real velocity) noexcept
			{
				particle_velocity_ = std::pair{velocity, velocity};
			}

			///@brief Sets the velocity range of each new particle to the given range
			inline void ParticleVelocity(real min_velocity, real max_velocity) noexcept
			{
				particle_velocity_ = std::minmax(min_velocity, max_velocity);
			}

			///@brief Sets the size of each new particle to the given value
			inline void ParticleSize(const Vector2 &size) noexcept
			{
				particle_size_ = std::pair{size, size};
			}

			///@brief Sets the size range of each new particle to the given range
			inline void ParticleSize(const Vector2 &min_size, const Vector2 &max_size) noexcept
			{
				particle_size_ = std::minmax(min_size, max_size);
			}

			///@brief Sets the mass of each new particle to the given value
			inline void ParticleMass(real mass) noexcept
			{
				particle_mass_ = std::pair{mass, mass};
			}

			///@brief Sets the mass range of each new particle to the given range
			inline void ParticleMass(real min_mass, real max_mass) noexcept
			{
				particle_mass_ = std::minmax(min_mass, max_mass);
			}

			///@brief Sets the color of each new particle to the given value
			inline void ParticleColor(const Color &color) noexcept
			{
				particle_color_ = std::pair(color, color);
			}

			///@brief Sets the color range of each new particle to the given range
			///@details The final color of the new particle is determined by mixing both colors with a random percentage
			inline void ParticleColor(const Color &from_color, const Color &to_color) noexcept
			{
				particle_color_ = std::pair(from_color, to_color); //minmax not needed
			}

			///@brief Sets the lifetime of each new particle to the given value
			inline void ParticleLifetime(duration lifetime) noexcept
			{
				particle_lifetime_ = std::pair(lifetime, lifetime);
			}

			///@brief Sets the lifetime range of each new particle to the given range
			inline void ParticleLifetime(duration min_lifetime, duration max_lifetime) noexcept
			{
				particle_lifetime_ = std::minmax(min_lifetime, max_lifetime);
			}

			///@brief Sets the material of each new particle to the given material
			inline void ParticleMaterial(NonOwningPtr<materials::Material> particle_material) noexcept
			{
				particle_material_ = particle_material;
			}

			///@}

			/**
				@name Spawn observers
				@{
			*/

			///@brief Returns the velocity of each new particle in range [min, max]
			[[nodiscard]] inline auto& ParticleVelocity() const noexcept
			{
				return particle_velocity_;
			}

			///@brief Returns the size of each new particle in range [min, max]
			[[nodiscard]] inline auto& ParticleSize() const noexcept
			{
				return particle_size_;
			}

			///@brief Returns the mass of each new particle in range [min, max]
			[[nodiscard]] inline auto& ParticleMass() const noexcept
			{
				return particle_mass_;
			}

			///@brief Returns the color of each new particle in range [from, to]
			[[nodiscard]] inline auto& ParticleColor() const noexcept
			{
				return particle_color_;
			}

			///@brief Returns the lifetime of each new particle in range [min, max]
			[[nodiscard]] inline auto& ParticleLifetime() const noexcept
			{
				return particle_lifetime_;
			}

			///@brief Returns the material of each new particle
			[[nodiscard]] inline auto ParticleMaterial() const noexcept
			{
				return particle_material_;
			}

			///@}

			/**
				@name Common functions for controlling the emitter
				@{
			*/

			///@brief Starts or resumes emitter
			void Start() noexcept;

			///@brief Stops emitter
			void Stop() noexcept;

			///@brief Stops and reset emission duration to zero
			void Reset() noexcept;

			///@brief Stops, resets and starts emitter
			void Restart() noexcept;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses emitter by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;

			///@}

			/**
				@name Emitting
				@{
			*/

			///@brief Emits the given particle count with the current emission rate
			void Emit(int particle_count) noexcept;

			///@}

			/**
				@name Particles
				@{
			*/

			///@brief Clears all particles emitted by this emitter
			void ClearParticles() noexcept;

			///@brief Returns true if this emitter has any active particles
			[[nodiscard]] inline auto HasActiveParticles() const noexcept
			{
				return !std::empty(particles_);
			}

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all particles in this emitter
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Particles() noexcept
			{
				return adaptors::ranges::Iterable<emitter::detail::container_type<Particle>&>{particles_};
			}

			///@brief Returns an immutable range of all particles in this emitter
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Particles() const noexcept
			{
				return adaptors::ranges::Iterable<const emitter::detail::container_type<Particle>&>{particles_};
			}

			///@}
	};
} //ion::graphics::particles

#endif