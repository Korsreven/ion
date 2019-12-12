/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonColorFader.h
-------------------------------------------
*/

#ifndef ION_COLOR_FADER_H
#define ION_COLOR_FADER_H

#include <optional>
#include <vector>

#include "IonAffector.h"
#include "adaptors/IonFlatSet.h"
#include "graphics/utilities/IonColor.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::particles::affectors
{
	using graphics::utilities::Color;

	namespace color_fader
	{
		using namespace types::type_literals;

		struct Step
		{
			real Percent = 0.0_r;
			std::optional<Color> ToColor;


			/*
				Operators
			*/

			//Returns true if this percent is less than the given step percent
			[[nodiscard]] inline auto operator<(const Step &rhs) const noexcept
			{
				return Percent < rhs.Percent;
			}

			//Returns true if this percent is less than the given percent
			[[nodiscard]] inline auto operator<(real percent) const noexcept
			{
				return Percent < percent;
			}
		};


		namespace detail
		{
			//Colors are sorted by percentages in range [0.0, 1.0]
			using color_steps = adaptors::FlatSet<Step>;


			constexpr auto clamp_percent(real percent, real else_percent) noexcept
			{
				return percent >= 0.0_r && percent <= 1.0_r ?
					percent : //Inside range
					else_percent; //Outside range
			}

			constexpr auto clamp_range(real from_percent, real to_percent) noexcept
			{
				//Different, clamp if outside range
				if (from_percent != to_percent)
					return std::pair{clamp_percent(from_percent, 0.0_r),
									 clamp_percent(to_percent, 1.0_r)};
				//Equal, clamp to range [0.0, 1.0]
				else
				{
					auto percent = ion::utilities::math::Saturate(from_percent);
					return std::pair{percent, percent};
				}
			}

			std::vector<Step> normalize_steps(std::vector<Step> steps);
			std::vector<Step> uniformly_distribute_steps(const std::vector<Color> &colors, real from_percent, real to_percent);

			void affect_particles(affector::detail::particle_range particles, const color_steps &steps) noexcept;
		} //detail
	} //color_fader


	class ColorFader final : public Affector
	{
		private:

			color_fader::detail::color_steps steps_;

		protected:

			/*
				Affect particles
			*/

			//Elapse color fader by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void DoAffect(affector::detail::particle_range particles, duration time) noexcept override;

		public:

			//Default constructor
			ColorFader() = default;

			//Construct a new color fader with the given steps
			//Percentages are normalized to range [0.0, 1.0]
			ColorFader(const std::vector<color_fader::Step> &steps);


			/*
				Static color fader conversions
			*/

			//Returns a new color fader from the given colors
			//Percentages are uniformly distributed in range [0.0, 1.0]
			[[nodiscard]] static ColorFader UniformSteps(const std::vector<Color> &colors);

			//Returns a new color fader from the given colors, from percent and to percent
			//Percentages are uniformly distributed in range [from, to]
			[[nodiscard]] static ColorFader UniformSteps(const std::vector<Color> &colors, real from_percent, real to_percent);


			/*
				Ranges
			*/

			//Returns an immutable range of all fading steps in this color fader
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Steps() const noexcept
			{
				return adaptors::ranges::Iterable<const color_fader::detail::color_steps&>{steps_};
			}


			/*
				Steps
			*/

			//Add (in sorted order) the given percentage and color to this color fader
			//The step is clamped to range [0.0, 1.0]
			void AddStep(real percent, std::optional<Color> color);

			//Add (in sorted order) the given step to this color fader
			//The step is clamped to range [0.0, 1.0]
			void AddStep(color_fader::Step step);

			//Add (in sorted order) the given steps to the this color fader
			//Percentages are normalized to range [0.0, 1.0]
			void AddSteps(std::vector<color_fader::Step> steps);
			

			//Clear all steps from this color fader
			void ClearSteps() noexcept;
	};
} //ion::graphics::particles::affectors

#endif