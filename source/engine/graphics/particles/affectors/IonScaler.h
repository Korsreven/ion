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
#include "memory/IonOwningPtr.h"
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


			/**
				@name Operators
				@{
			*/

			///@brief Returns true if this percent is less than the given step percent
			[[nodiscard]] inline auto operator<(const Step &rhs) const noexcept
			{
				return Percent < rhs.Percent;
			}

			///@brief Returns true if this percent is less than the given percent
			[[nodiscard]] inline auto operator<(real percent) const noexcept
			{
				return Percent < percent;
			}

			///@}
		};


		namespace detail
		{
			///@brief Sizes are sorted by percentages in range [0.0, 1.0]
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

			std::vector<Step> normalize_steps(std::vector<Step> steps) noexcept;
			std::vector<Step> uniformly_distribute_steps(const std::vector<Vector2> &sizes, real from_percent, real to_percent);

			void affect_particles(affector::detail::particle_range particles, const size_steps &steps) noexcept;
		} //detail
	} //scaler


	///@brief A class representing an affector that can scale single particles
	class Scaler final : public Affector
	{
		private:

			scaler::detail::size_steps steps_;

		protected:

			/**
				@name Affect particles
				@{
			*/

			///@brief Elapses scaler by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void DoAffect(affector::detail::particle_range particles, duration time) noexcept override;

			///@}

		public:

			///@brief Constructs a new scaler with the given name
			explicit Scaler(std::string name) noexcept;

			///@brief Constructs a new scaler with the given name and steps
			///@details Percentages are normalized to range [0.0, 1.0]
			Scaler(std::string name, std::vector<scaler::Step> steps) noexcept;


			/**
				@name Static scaler conversions
				@{
			*/

			///@brief Returns a new scaler from the given name and sizes
			///@details Percentages are uniformly distributed in range [0.0, 1.0]
			[[nodiscard]] static Scaler UniformSteps(std::string name, const std::vector<Vector2> &sizes);

			///@brief Returns a new scaler from the given name, sizes, from percent and to percent
			///@details Percentages are uniformly distributed in range [from, to]
			[[nodiscard]] static Scaler UniformSteps(std::string name, const std::vector<Vector2> &sizes, real from_percent, real to_percent);

			///@}

			/**
				@name Cloning
				@{
			*/

			///@brief Returns an owning ptr to a clone of this affector
			[[nodiscard]] OwningPtr<Affector> Clone() const override;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns an immutable range of all size steps in this scaler
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Steps() const noexcept
			{
				return adaptors::ranges::Iterable<const scaler::detail::size_steps&>{steps_};
			}

			///@}

			/**
				@name Steps
				@{
			*/

			///@brief Adds (in sorted order) the given percentage and size to this scaler
			///@details The step is clamped to range [0.0, 1.0]
			void AddStep(real percent, std::optional<Vector2> size);

			///@brief Adds (in sorted order) the given step to this scaler
			///@details The step is clamped to range [0.0, 1.0]
			void AddStep(scaler::Step step);

			///@brief Adds (in sorted order) the given steps to the this scaler
			///@details Percentages are normalized to range [0.0, 1.0]
			void AddSteps(std::vector<scaler::Step> steps);
			

			///@brief Clears all steps from this scaler
			void ClearSteps() noexcept;

			///@}
	};
} //ion::graphics::particles::affectors

#endif