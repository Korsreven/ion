/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonScaler.cpp
-------------------------------------------
*/

#include "IonScaler.h"

#include <algorithm>
#include <utility>

#include "graphics/particles/IonParticle.h"

namespace ion::graphics::particles::affectors
{

using namespace scaler;

namespace scaler::detail
{

std::vector<Step> normalize_steps(std::vector<Step> steps)
{
	if (!std::empty(steps))
	{
		auto [min, max] = std::minmax_element(std::begin(steps), std::end(steps));

		//Normalize (scale) values to...
		if (min->Percent != max->Percent)
		{
			auto min_percent = min->Percent;
			auto max_percent = max->Percent;

			//range [0.0, 1.0]
			if (min_percent < 0.0_r && max_percent > 1.0_r)
			{
				for (auto &step : steps)
					step.Percent = ion::utilities::math::Normalize(step.Percent, min_percent, max_percent);
			}
			//range [0.0, max]
			else if (min_percent < 0.0_r)
			{
				for (auto &step : steps)
					step.Percent = ion::utilities::math::Normalize(step.Percent, min_percent, max_percent,
																   0.0_r, clamp_percent(max_percent, 1.0_r));
			}
			//range [min, 1.0]
			else if (max_percent > 1.0_r)
			{
				for (auto &step : steps)
					step.Percent = ion::utilities::math::Normalize(step.Percent, min_percent, max_percent,
																   clamp_percent(min_percent, 0.0_r), 1.0_r);
			}
		}
		//Clamp values to range [0.0, 1.0]
		else
		{
			for (auto &step : steps)
				step.Percent = ion::utilities::math::Saturate(step.Percent);
		}
	}

	return steps;
}

std::vector<Step> uniformly_distribute_steps(const std::vector<Vector2> &sizes, real from_percent, real to_percent)
{
	if (!std::empty(sizes))
	{
		auto idle_step = from_percent > 0.0_r && from_percent < to_percent;

		std::vector<Step> steps;
		steps.reserve(std::size(sizes) + idle_step);

		if (idle_step)
			steps.push_back({from_percent, {}});

		auto stride = (to_percent - from_percent) / std::size(sizes);
		for (auto &size : sizes)
			steps.push_back({from_percent += stride, size});
		steps.back().Percent = to_percent; //Set exact

		return steps;
	}
	else
		return {};
}

void affect_particles(affector::detail::particle_range particles, const size_steps &steps) noexcept
{
	static const Step first{0.0_r, {}};
	static const Step last{1.0_r, {}};

	for (auto &particle : particles)
	{
		auto percent = particle.LifeTimePercent();
		auto [from, to] =
			[&]() noexcept
			{
				if (auto iter = steps.lower_bound(percent); iter != std::end(steps))
					return std::pair{iter != std::begin(steps) ? &*(iter - 1) : &first, &*iter};
				else
					return std::pair{&*(iter - 1), &last};
			}();

		//Scale
		if (to->Size)
		{
			percent = (1.0_r - percent) / (to->Percent - from->Percent);

			//from -> to
			if (from->Size)
				particle.Size(from->Size->Lerp(*(to->Size), percent));
			//current -> to
			else
				particle.Size(particle.Size().Lerp(*(to->Size), percent));
		}
	}
}

} //scaler::detail


//Protected

/*
	Affect particles
*/

void Scaler::DoAffect(affector::detail::particle_range particles, [[maybe_unused]] duration time) noexcept
{
	if (!std::empty(steps_))
		detail::affect_particles(particles, steps_);
}


//Public

Scaler::Scaler(std::string name) :
	Affector{std::move(name)}
{
	//Empty
}

Scaler::Scaler(std::string name, const std::vector<scaler::Step> &steps) :

	Affector{std::move(name)},
	steps_{detail::normalize_steps(std::move(steps))}
{
	//Empty
}


/*
	Static scaler conversions
*/

Scaler Scaler::UniformSteps(std::string name, const std::vector<Vector2> &sizes)
{
	return {std::move(name), detail::uniformly_distribute_steps(sizes, 0.0_r, 1.0_r)};
}

Scaler Scaler::UniformSteps(std::string name, const std::vector<Vector2> &sizes, real from_percent, real to_percent)
{
	auto [min, max] = std::minmax(from_percent, to_percent);
	auto [from, to] = detail::clamp_range(min, max);
	return {std::move(name), detail::uniformly_distribute_steps(sizes, from, to)};
}


/*
	Cloning
*/

OwningPtr<Affector> Scaler::Clone() const
{
	return make_owning<Scaler>(*this);
}


/*
	Steps
*/

void Scaler::AddStep(real percent, std::optional<Vector2> size)
{
	AddStep({percent, std::move(size)});
}

void Scaler::AddStep(scaler::Step step)
{
	AddSteps({std::move(step)});
}

void Scaler::AddSteps(std::vector<scaler::Step> steps)
{
	steps_.insert(detail::normalize_steps(std::move(steps)));
}


void Scaler::ClearSteps() noexcept
{
	steps_.clear();
}

} //ion::graphics::particles::affectors