/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonColorFader.cpp
-------------------------------------------
*/

#include "IonColorFader.h"

#include <algorithm>
#include <utility>

#include "graphics/particles/IonParticle.h"

namespace ion::graphics::particles::affectors
{

using namespace color_fader;

namespace color_fader::detail
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

std::vector<Step> uniformly_distribute_steps(const std::vector<Color> &colors, real from_percent, real to_percent)
{
	if (!std::empty(colors))
	{
		auto idle_step = from_percent > 0.0_r && from_percent < to_percent;

		std::vector<Step> steps;
		steps.reserve(std::size(colors) + idle_step);

		if (idle_step)
			steps.push_back({from_percent, {}});

		auto stride = (to_percent - from_percent) / std::size(colors);
		for (auto &color : colors)
			steps.push_back({from_percent += stride, color});
		steps.back().Percent = to_percent; //Set exact

		return steps;
	}
	else
		return {};
}

void affect_particles(affector::detail::particle_range particles, const color_steps &steps) noexcept
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

		//Fade
		if (to->ToColor)
		{
			percent = (1.0_r - percent) / (to->Percent - from->Percent);

			//from -> to
			if (from->ToColor)
				particle.FillColor(from->ToColor->MixCopy(*(to->ToColor), percent));
			//current -> to
			else
				particle.FillColor(particle.FillColor().MixCopy(*(to->ToColor), percent));
		}
	}
}

} //color_fader::detail


//Protected

/*
	Affect particles
*/

void ColorFader::DoAffect(affector::detail::particle_range particles, [[maybe_unused]] duration time) noexcept
{
	if (!std::empty(steps_))
		detail::affect_particles(particles, steps_);
}


//Public

ColorFader::ColorFader(std::string name) :
	Affector{std::move(name)}
{
	//Empty
}

ColorFader::ColorFader(std::string name, const std::vector<color_fader::Step> &steps) :

	Affector{std::move(name)},
	steps_{detail::normalize_steps(std::move(steps))}
{
	//Empty
}


/*
	Static color fader conversions
*/

ColorFader ColorFader::UniformSteps(std::string name, const std::vector<Color> &colors)
{
	return {std::move(name), detail::uniformly_distribute_steps(colors, 0.0_r, 1.0_r)};
}

ColorFader ColorFader::UniformSteps(std::string name, const std::vector<Color> &colors, real from_percent, real to_percent)
{
	auto [min, max] = std::minmax(from_percent, to_percent);
	auto [from, to] = detail::clamp_range(min, max);
	return {std::move(name), detail::uniformly_distribute_steps(colors, from, to)};
}


/*
	Cloning
*/

OwningPtr<Affector> ColorFader::Clone() const
{
	return make_owning<ColorFader>(*this);
}


/*
	Steps
*/

void ColorFader::AddStep(real percent, std::optional<Color> color)
{
	AddStep({percent, std::move(color)});
}

void ColorFader::AddStep(color_fader::Step step)
{
	AddSteps({std::move(step)});
}

void ColorFader::AddSteps(std::vector<color_fader::Step> steps)
{
	steps_.insert(detail::normalize_steps(std::move(steps)));
}


void ColorFader::ClearSteps() noexcept
{
	steps_.clear();
}

} //ion::graphics::particles::affectors