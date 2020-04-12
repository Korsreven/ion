/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonEmitter.cpp
-------------------------------------------
*/

#include "IonEmitter.h"

namespace ion::graphics::particles
{

using namespace emitter;
using namespace ion::utilities;

namespace emitter::detail
{

void evolve_particles(container_type<Particle> &particles, duration time) noexcept
{
	auto end = std::end(particles);
	auto last = end;

	for (auto iter = std::begin(particles); iter != last;)
	{
		//Particle life time has ended
		if (!iter->Evolve(time))
			*iter = std::move(*--last); //Move last active particle
		else
			++iter;
	}

	particles.erase(last, end);
}

} //emitter::detail


Emitter::Emitter(EmitterType type, const Vector2 &position, const Vector2 &direction,
	const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
	std::optional<duration> emission_duration, int particle_quota) :

	type_{type},
	position_{position},
	direction_{direction},
	size_{size},
	inner_size_{inner_size},

	emission_rate_{emission_rate},
	emission_angle_{emission_angle},
	emission_duration_{emission_duration},
	particle_quota_{particle_quota}
{
	//Empty
}


/*
	Static emitter conversions
*/

Emitter Emitter::Point(const Vector2 &position, const Vector2 &direction,
	real emission_rate, real emission_angle, std::optional<duration> emission_duration,
	int particle_quota)
{
	return {EmitterType::Point, position, direction, {}, {}, emission_rate, emission_angle, emission_duration, particle_quota};
}

Emitter Emitter::Box(const Vector2 &position, const Vector2 &direction,
	const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
	std::optional<duration> emission_duration, int particle_quota)
{
	return {EmitterType::Box, position, direction, size, inner_size, emission_rate, emission_angle, emission_duration, particle_quota};
}

Emitter Emitter::Ring(const Vector2 &position, const Vector2 &direction,
	const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
	std::optional<duration> emission_duration, int particle_quota)
{
	return {EmitterType::Ring, position, direction, size, inner_size, emission_rate, emission_angle, emission_duration, particle_quota};
}


/*
	Spawn modifiers
*/

void Emitter::ParticleTexture(textures::Texture &texture)
{
	particle_texture_.Observe(texture);
}

void Emitter::ParticleTexture(std::nullptr_t) noexcept
{
	particle_texture_.Release();
}


/*
	Common functions for controlling the emitter
*/

void Emitter::Start() noexcept
{
	emitting_ = true;
}

void Emitter::Stop() noexcept
{
	emitting_ = false;
}

void Emitter::Reset() noexcept
{
	Stop();

	if (emission_duration_)
		emission_duration_->Reset();

	emission_amount_.Reset();
}

void Emitter::Restart() noexcept
{
	Reset();
	Start();
}


/*
	Elapse time
*/

void Emitter::Elapse(duration time) noexcept
{
	if (emitting_)
	{
		if (emission_duration_ && (*emission_duration_ += time))
			Reset();

		else if (emission_amount_ += emission_rate_ * time.count())
		{
			Emit(static_cast<int>(emission_amount_.Total())); //Emit total number of particles (whole part)
			emission_amount_.Total(math::Fraction(emission_amount_.Total())); //Keep fractional part for next emission
		}
	}

	//Evolve particles (even when not emitting)
	if (!std::empty(particles_))
		detail::evolve_particles(particles_, time);

	//Affect particles (even when not emitting)
	for (auto &affector : Affectors())
		affector.Affect(Particles(), time);
}


/*
	Emitting
*/

void Emitter::Emit(int particle_count) noexcept
{
	for (particle_count = std::min(particle_count, particle_quota_ - static_cast<int>(std::size(particles_)));
		particle_count > 0; --particle_count)

		particles_.emplace_back(
			position_ + detail::particle_position(type_, size_ * 0.5_r, inner_size_ * 0.5_r), //Position
			detail::particle_direction(direction_, emission_angle_, particle_velocity_.first, particle_velocity_.second), //Direction + velocity
			detail::particle_size(particle_size_.first, particle_size_.second), //Size
			detail::particle_mass(particle_mass_.first, particle_mass_.second), //Mass
			detail::particle_solid_color(particle_solid_color_.first, particle_solid_color_.second), //Solid color
			detail::particle_life_time(particle_life_time_.first, particle_life_time_.second) //Life time
		);
}

/*
	Particles
*/

void Emitter::ClearParticles() noexcept
{
	particles_.clear();
}

} //ion::graphics::particles