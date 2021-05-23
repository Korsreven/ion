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


Emitter::Emitter(std::string name) :
	managed::ManagedObject<EmitterManager>{std::move(name)}
{
	//Empty
}

Emitter::Emitter(std::string name, EmitterType type, const Vector3 &position, const Vector2 &direction,
	const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
	std::optional<duration> emission_duration, int particle_quota) :

	managed::ManagedObject<EmitterManager>{std::move(name)},

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

Emitter Emitter::Point(std::string name, const Vector3 &position, const Vector2 &direction,
	real emission_rate, real emission_angle, std::optional<duration> emission_duration,
	int particle_quota)
{
	return {std::move(name), EmitterType::Point, position, direction, {}, {}, emission_rate, emission_angle, emission_duration, particle_quota};
}

Emitter Emitter::Box(std::string name, const Vector3 &position, const Vector2 &direction,
	const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
	std::optional<duration> emission_duration, int particle_quota)
{
	return {std::move(name), EmitterType::Box, position, direction, size, inner_size, emission_rate, emission_angle, emission_duration, particle_quota};
}

Emitter Emitter::Ring(std::string name, const Vector3 &position, const Vector2 &direction,
	const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
	std::optional<duration> emission_duration, int particle_quota)
{
	return {std::move(name), EmitterType::Ring, position, direction, size, inner_size, emission_rate, emission_angle, emission_duration, particle_quota};
}


/*
	Cloning
*/

OwningPtr<Emitter> Emitter::Clone() const
{
	auto emitter = make_owning<Emitter>(*name_);
	emitter->type_ = type_;
	emitter->position_ = position_;
	emitter->direction_ = direction_;
	emitter->size_ = size_;
	emitter->inner_size_ = inner_size_;

	emitter->emission_rate_ = emission_rate_;
	emitter->emission_angle_ = emission_angle_;
	emitter->emission_duration_ = emission_duration_;			

	emitter->particle_quota_ = particle_quota_;
	emitter->particle_velocity_ = particle_velocity_;
	emitter->particle_size_ = particle_size_;		
	emitter->particle_mass_ = particle_mass_;
	emitter->particle_color_ = particle_color_;
	emitter->particle_life_time_ = particle_life_time_;
	emitter->particle_material_ = particle_material_;

	//Clone local affectors
	for (auto &affector : Affectors())
		emitter->AffectorManager::Adopt(affector.Clone());

	return emitter;
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
	for (particle_count = std::min(particle_count, particle_quota_ - std::ssize(particles_));
		particle_count > 0; --particle_count)

		particles_.emplace_back(
			position_ + detail::particle_position(type_, size_ * 0.5_r, inner_size_ * 0.5_r), //Position
			detail::particle_direction(direction_, emission_angle_, particle_velocity_.first, particle_velocity_.second), //Direction + velocity
			detail::particle_size(particle_size_.first, particle_size_.second), //Size
			detail::particle_mass(particle_mass_.first, particle_mass_.second), //Mass
			detail::particle_color(particle_color_.first, particle_color_.second), //Color
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