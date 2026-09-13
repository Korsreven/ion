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
#include <vector>

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

	namespace particle::detail
	{
		template <typename T>
		using container_type = std::vector<T>;
	} //particle::detail


	///@brief Particle class that contains data for rendering a single particle
	struct ParticleRenderData final
	{
		Vector3 Position;
		real Rotation = 0.0_r;
		Vector2 Size;
		Color FillColor;


		///@brief Default constructor
		ParticleRenderData() = default;

		///@brief Constructs a new particle render data from the given initial values
		ParticleRenderData(const Vector3 &position, real rotation, const Vector2 &size, const Color &color) noexcept;
	};

	///@brief Particle class that contains data for simulating a single particle
	struct ParticleSimulationData final
	{
		Vector2 Direction; //Length represents velocity
		real Mass = 0.0_r;
		Cumulative<duration> Lifetime;
		Vector2 InitialDirection;

		real PreviousLifetimePercent = 0.0_r;
		Color FromColor;
		Vector2 FromSize;


		///@brief Default constructor
		ParticleSimulationData() = default;

		///@brief Constructs a new particle simulation data from the given initial values
		ParticleSimulationData(const Vector2 &direction, real mass, duration lifetime, const Vector2 &initial_direction) noexcept;
	};


	///@brief Particles class that represent multiple particles that can be represented by...
	///@details 1. A single point (1 vertex).
	///2. A rectangle (4 vertices)
	class Particles final
	{
		private:

			particle::detail::container_type<ParticleRenderData> render_data_;
			particle::detail::container_type<ParticleSimulationData> simulation_data_;

		public:

			///@brief Default constructor
			Particles() = default;


			///@brief Adds a new particle with default initial values
			int AddParticle() noexcept;

			///@brief Adds a new particle from the given initial values
			int AddParticle(const Vector3 &position, const Vector2 &direction,
					 const Vector2 &size, real mass, const Color &color,
					 duration lifetime, const Vector2 &initial_direction) noexcept;


			/**
				@name Evolving
				@{
			*/

			///@brief Evolves particle with the given offset by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			inline auto Evolve(int off, duration time) noexcept
			{
				simulation_data_[off].PreviousLifetimePercent = simulation_data_[off].Lifetime.Percent();

				if (simulation_data_[off].Lifetime += time)
					return false;
				else
				{
					render_data_[off].Position += simulation_data_[off].Direction * time.count();
					return true;
				}
			}

			///@brief Evolves all particles by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			inline auto Evolve(duration time) noexcept
			{
				auto last_off = std::ssize(render_data_);

				for (auto off = 0; off < last_off;)
				{
					//Particle lifetime has ended
					if (!Evolve(off, time))
					{
						--last_off;

						//Move last active particle
						render_data_[off] = std::move(render_data_[last_off]);
						simulation_data_[off] = std::move(simulation_data_[last_off]);
					}
					else
						++off;
				}

				render_data_.erase(std::begin(render_data_) + last_off, std::end(render_data_));
				simulation_data_.erase(std::begin(simulation_data_) + last_off, std::end(simulation_data_));
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the position of the particle to the given vector
			inline void Position(int off, const Vector3 &position) noexcept
			{
				render_data_[off].Position = position;
			}

			///@brief Sets the position of the particle to the given vector
			inline void Position(int off, const Vector2 &position) noexcept
			{
				Position(off, {position.X(), position.Y(), render_data_[off].Position.Z()});
			}

			///@brief Sets the direction of the particle to the given vector
			///@details Where the length of the given vector represents velocity
			inline void Direction(int off, const Vector2 &direction) noexcept
			{
				simulation_data_[off].Direction = direction;
				render_data_[off].Rotation = direction.SignedAngleBetween(simulation_data_[off].InitialDirection); //Update rotation
			}

			///@brief Sets the velocity (direction length) of the particle to the given value
			inline void Velocity(int off, real velocity) noexcept
			{
				simulation_data_[off].Direction.Normalize();
				simulation_data_[off].Direction *= velocity;
			}

			///@brief Sets the size of the particle to the given vector
			inline void Size(int off, const Vector2 &size) noexcept
			{
				render_data_[off].Size = size;
			}

			///@brief Sets the mass of the particle to the given value
			inline void Mass(int off, real mass) noexcept
			{
				simulation_data_[off].Mass = mass;
			}

			///@brief Sets the color of the particle to the given color
			inline void FillColor(int off, const Color &color) noexcept
			{
				render_data_[off].FillColor = color;
			}

			///@brief Sets the lifetime of the particle to the given duration
			inline void Lifetime(int off, duration lifetime) noexcept
			{
				simulation_data_[off].Lifetime.Limit(lifetime);
			}


			///@brief Sets the from color of the particle to the given color
			///@details This is used for interpolation purposes by affectors::ColorFader
			inline void FromColor(int off, const Color &color) noexcept
			{
				simulation_data_[off].FromColor = color;
			}

			///@brief Sets the from size of the particle to the given vector
			///@details This is used for interpolation purposes by affectors::Scaler
			inline void FromSize(int off, const Vector2 &size) noexcept
			{
				simulation_data_[off].FromSize = size;
			}


			void Quota(int quota)
			{
				//Erase all particles that does not fit quota
				if (quota < std::ssize(render_data_))
				{
					render_data_.erase(std::begin(render_data_) + quota, std::end(render_data_));
					simulation_data_.erase(std::begin(simulation_data_) + quota, std::end(simulation_data_));
				}
			}

			void clear() noexcept
			{
				render_data_.clear();
				simulation_data_.clear();
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the position of the particle
			[[nodiscard]] inline auto& Position(int off) const noexcept
			{
				return render_data_[off].Position;
			}

			///@brief Returns the direction of the particle
			///@details Where the length of the vector represents velocity
			[[nodiscard]] inline auto& Direction(int off) const noexcept
			{
				return simulation_data_[off].Direction;
			}

			///@brief Returns the velocity (direction length) of the particle
			[[nodiscard]] inline auto Velocity(int off) const noexcept
			{
				return simulation_data_[off].Direction.Length();
			}

			///@brief Returns the angle of rotation (in radians) of the particle
			[[nodiscard]] inline auto& Rotation(int off) const noexcept
			{
				return render_data_[off].Rotation;
			}

			///@brief Returns the size of the particle
			[[nodiscard]] inline auto& Size(int off) const noexcept
			{
				return render_data_[off].Size;
			}

			///@brief Returns the mass of the particle
			[[nodiscard]] inline auto Mass(int off) const noexcept
			{
				return simulation_data_[off].Mass;
			}

			///@brief Returns the color of the particle
			[[nodiscard]] inline auto& FillColor(int off) const noexcept
			{
				return render_data_[off].FillColor;
			}

			///@brief Returns the lifetime of the particle
			[[nodiscard]] inline auto Lifetime(int off) const noexcept
			{
				return simulation_data_[off].Lifetime.Limit();
			}

			///@brief Returns the lifetime percent of the particle in range [0.0, 1.0]
			[[nodiscard]] inline auto LifetimePercent(int off) const noexcept
			{
				return simulation_data_[off].Lifetime.Percent();
			}

			///@brief Returns the previous lifetime percent of the particle in range [0.0, 1.0]
			[[nodiscard]] inline auto PreviousLifetimePercent(int off) const noexcept
			{
				return simulation_data_[off].PreviousLifetimePercent;
			}


			///@brief Returns the from color of the particle
			///@details This is used for interpolation purposes by affectors::ColorFader
			[[nodiscard]] inline auto& FromColor(int off) const noexcept
			{
				return simulation_data_[off].FromColor;
			}

			///@brief Returns the from size of the particle
			///@details This is used for interpolation purposes by affectors::Scaler
			[[nodiscard]] inline auto& FromSize(int off) const noexcept
			{
				return simulation_data_[off].FromSize;
			}


			///@brief Returns the particles render data
			///@details This is used for rendering by scene::DrawableParticleSystem
			[[nodiscard]] inline auto& RenderData() const noexcept
			{
				return render_data_;
			}

			///@brief Returns the particles simulation data
			[[nodiscard]] inline auto& SimulationData() const noexcept
			{
				return simulation_data_;
			}


			[[nodiscard]] auto size() const noexcept
			{
				return std::size(render_data_);
			}

			[[nodiscard]] auto ssize() const noexcept
			{
				return std::ssize(render_data_);
			}

			[[nodiscard]] auto empty() const noexcept
			{
				return std::empty(render_data_);
			}

			///@}
	};
} //ion::graphics::particles

#endif