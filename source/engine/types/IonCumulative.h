/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonCumulative.h
-------------------------------------------
*/

#ifndef ION_CUMULATIVE_H
#define ION_CUMULATIVE_H

#include <algorithm>
#include <type_traits>

#include "IonTypes.h"
#include "IonTypeTraits.h"

namespace ion::types
{
	namespace cumulative::detail
	{
		template <typename T>
		constexpr auto underlying_value(T value) noexcept
		{
			return value;
		}

		///@brief Enables support for chrono::duration
		constexpr auto underlying_value(duration value) noexcept
		{
			return value.count();
		}
	} //cumulative::detail
	

	///@brief A class representing a cumulative sum (running total) in range [0, limit]
	///@details Total is allowed to accumulate higher than the limit (before resetting)
	template <typename T>
	class Cumulative
	{
		static_assert(std::is_arithmetic_v<representation_t<T>>);

		private:

			T total_{};
			T limit_{};
		
		public:

			///@brief Default constructor
			constexpr Cumulative() = default;

			///@brief Constructs a new cumulative with the given limit
			constexpr explicit Cumulative(T limit) noexcept :
				limit_{limit > T{} ? limit : T{}}
			{
				//Empty
			}


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the running total to the given amount
			constexpr void Total(T amount) noexcept
			{
				total_ = amount;
			}

			///@brief Sets the limit to the given amount
			constexpr void Limit(T amount) noexcept
			{
				limit_ = amount > T{} ? amount : T{};
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the running total
			[[nodiscard]] constexpr auto Total() const noexcept
			{
				return total_;
			}

			///@brief Returns the limit
			[[nodiscard]] constexpr auto Limit() const noexcept
			{
				return limit_;
			}

			///@}

			/**
				@name Accumulating
				@{
			*/

			///@brief Returns true if running total has reached the limit
			[[nodiscard]] constexpr auto IsComplete() const noexcept
			{
				return total_ >= limit_;
			}


			///@brief Adds the given amount to the running total
			///@details Resets and carries over the remainder if accumulation is complete
			constexpr auto& Add(T amount) noexcept
			{
				if (IsComplete())
					ResetWithCarry();

				total_ += amount;
				return *this;
			}

			///@brief Adds the given percentage (of the limit) to the running total
			constexpr auto& AddPercent(real percent) noexcept
			{
				return Add(T(cumulative::detail::underlying_value(limit_) * percent));
			}

			///@}

			/**
				@name Clamping
				@{
			*/

			///@brief Clamps running total to range [0, limit]
			constexpr void Clamp() noexcept
			{
				total_ = std::clamp(total_, T{}, limit_);
			}

			///@}

			/**
				@name Percentage
				@{
			*/

			///@brief Sets the total to the given percentage (of the limit)
			constexpr void Percent(real percent) noexcept
			{
				total_ = T(cumulative::detail::underlying_value(limit_) * percent);
			}

			///@brief Returns the total as a percentage (of the limit)
			[[nodiscard]] constexpr auto Percent() const noexcept
			{
				return limit_ > T{} ?
					cumulative::detail::underlying_value(total_) /
					static_cast<real>(cumulative::detail::underlying_value(limit_)) :
					1.0_r;
			}

			///@}

			/**
				@name Remaining
				@{
			*/

			///@brief Sets the remaining total to the given amount
			constexpr void Remaining(T amount) noexcept
			{
				total_ = limit_ - amount;
			}

			///@brief Returns the remaining total
			[[nodiscard]] constexpr T Remaining() const noexcept
			{
				return limit_ - total_;
			}

			///@}

			/**
				@name Resetting
				@{
			*/

			///@brief Returns the amount that has gone above the limit
			[[nodiscard]] constexpr T Remainder() const noexcept
			{
				return total_ > limit_ ? total_ - limit_ : T{};
			}

			///@brief Resets the running total to 0
			constexpr void Reset() noexcept
			{
				total_ = T{};
			}

			///@brief Resets the running total to 0, but carries over the amount that has gone above the limit (remainder)
			constexpr void ResetWithCarry() noexcept
			{
				total_ = Remainder();
			}

			///@}

			/**
				@name Operators - For convenience
				@{
			*/

			///@brief Returns true if running total has reached the limit
			constexpr operator bool() noexcept
			{
				return IsComplete();
			}


			///@brief Pre-increments the running total with 1
			constexpr auto& operator++() noexcept
			{
				return Add(T{1});
			}

			///@brief Post-increments the running total with 1
			constexpr auto operator++(int) noexcept
			{
				auto temp = *this;
				++*this;
				return temp;
			}

			///@brief Increments the running total with the given amount
			constexpr auto& operator+=(T amount) noexcept
			{
				return Add(amount);
			}

			///@}
	};
} //ion::types

#endif