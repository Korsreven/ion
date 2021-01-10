/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonScaler.h
-------------------------------------------
*/

#ifndef ION_SCALER_H
#define ION_SCALER_H

#include <optional>
#include <vector>

#include "IonAffector.h"
#include "adaptors/IonFlatSet.h"
#include "graphics/utilities/IonVector2.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::particles::affectors
{
	using graphics::utilities::Vector2;

	namespace scaler
	{
		using namespace types::type_literals;

		struct Step
		{
			real Percent = 0.0_r;
			std::optional<Vector2> Size;


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
			//Sizes are sorted by percentages in range [0.0, 1.0]
			using size_steps = adaptors::FlatSet<Step>;


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
			std::vector<Step> uniformly_distribute_steps(const std::vector<Vector2> &sizes, real from_percent, real to_percent);

			void affect_particles(affector::detail::particle_range particles, const size_steps &steps) noexcept;
		} //detail
	} //scaler


	class Scaler final : public Affector
	{
		private:

			scaler::detail::size_steps steps_;

		protected:

			/*
				Affect particles
			*/

			//Elapse scaler by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void DoAffect(affector::detail::particle_range particles, duration time) noexcept override;

		public:

			//Construct a new scaler with the given name
			explicit Scaler(std::string name);

			//Construct a new scaler with the given name and steps
			//Percentages are normalized to range [0.0, 1.0]
			Scaler(std::string name, const std::vector<scaler::Step> &steps);


			/*
				Static scaler conversions
			*/

			//Returns a new scaler from the given name and sizes
			//Percentages are uniformly distributed in range [0.0, 1.0]
			[[nodiscard]] static Scaler UniformSteps(std::string name, const std::vector<Vector2> &sizes);

			//Returns a new scaler from the given name, sizes, from percent and to percent
			//Percentages are uniformly distributed in range [from, to]
			[[nodiscard]] static Scaler UniformSteps(std::string name, const std::vector<Vector2> &sizes, real from_percent, real to_percent);


			/*
				Ranges
			*/

			//Returns an immutable range of all size steps in this scaler
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Steps() const noexcept
			{
				return adaptors::ranges::Iterable<const scaler::detail::size_steps&>{steps_};
			}


			/*
				Steps
			*/

			//Add (in sorted order) the given percentage and size to this scaler
			//The step is clamped to range [0.0, 1.0]
			void AddStep(real percent, std::optional<Vector2> size);

			//Add (in sorted order) the given step to this scaler
			//The step is clamped to range [0.0, 1.0]
			void AddStep(scaler::Step step);

			//Add (in sorted order) the given steps to the this scaler
			//Percentages are normalized to range [0.0, 1.0]
			void AddSteps(std::vector<scaler::Step> steps);
			

			//Clear all steps from this scaler
			void ClearSteps() noexcept;
	};
} //ion::graphics::particles::affectors

#endif