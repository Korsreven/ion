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

		//Enable support for chrono::duration
		constexpr auto underlying_value(duration value) noexcept
		{
			return value.count();
		}
	} //cumulative::detail
	

	//Cumulative sum (running total) in range [0, limit]
	//Total is allowed to accumulate higher than the limit (before resetting)
	template <typename T>
	class Cumulative
	{
		static_assert(std::is_arithmetic_v<representation_t<T>>);

		private:

			T total_{};
			T limit_{};
		
		public:

			//Default constructor
			constexpr Cumulative() = default;

			//Construct a new cumulative with the given limit
			constexpr explicit Cumulative(T limit) noexcept :
				limit_{limit > T{} ? limit : T{}}
			{
				//Empty
			}


			/*
				Modifiers
			*/

			//Sets the running total to the given amount
			constexpr void Total(T amount) noexcept
			{
				total_ = amount;
			}

			//Sets the limit to the given amount
			constexpr void Limit(T amount) noexcept
			{
				limit_ = amount > T{} ? amount : T{};
			}


			/*
				Observers
			*/

			//Returns the running total
			[[nodiscard]] constexpr auto Total() const noexcept
			{
				return total_;
			}

			//Returns the limit
			[[nodiscard]] constexpr auto Limit() const noexcept
			{
				return limit_;
			}


			/*
				Accumulating
			*/

			//Returns true if running total has reached the limit
			[[nodiscard]] constexpr auto IsComplete() const noexcept
			{
				return total_ >= limit_;
			}


			//Add the given amount to the running total
			//Resets and carries over the remainder if accumulation is complete
			constexpr auto& Add(T amount) noexcept
			{
				if (IsComplete())
					ResetWithCarry();

				total_ += amount;
				return *this;
			}

			//Add the given percentage (of the limit) to the running total
			constexpr auto& AddPercent(real percent) noexcept
			{
				return Add(T(cumulative::detail::underlying_value(limit_) * percent));
			}


			/*
				Clamping
			*/

			//Clamp running total to range [0, limit]
			constexpr void Clamp() noexcept
			{
				total_ = std::clamp(total_, T{}, limit_);
			}


			/*
				Percentage
			*/

			//Sets the total to the given percentage (of the limit)
			constexpr void Percent(real percent) noexcept
			{
				total_ = T(cumulative::detail::underlying_value(limit_) * percent);
			}

			//Returns the total as a percentage (of the limit)
			[[nodiscard]] constexpr auto Percent() const noexcept
			{
				return limit_ > T{} ?
					cumulative::detail::underlying_value(total_) /
					static_cast<real>(cumulative::detail::underlying_value(limit_)) :
					1.0_r;
			}


			/*
				Remaining
			*/

			//Sets the remaining total to the given amount
			constexpr void Remaining(T amount) noexcept
			{
				total_ = limit_ - amount;
			}

			//Returns the remaining total
			[[nodiscard]] constexpr T Remaining() const noexcept
			{
				return limit_ - total_;
			}


			/*
				Resetting
			*/

			//Returns the amount that has gone above the limit
			[[nodiscard]] constexpr T Remainder() const noexcept
			{
				return total_ > limit_ ? total_ - limit_ : T{};
			}

			//Resets the running total to 0
			constexpr void Reset() noexcept
			{
				total_ = T{};
			}

			//Resets the running total to 0, but carries over the amount that has gone above the limit (remainder)
			constexpr void ResetWithCarry() noexcept
			{
				total_ = Remainder();
			}


			/*
				Operators
				For convenience
			*/

			//Returns true if running total has reached the limit
			constexpr operator bool() noexcept
			{
				return IsComplete();
			}


			//Pre-increment the running total with 1
			constexpr auto& operator++() noexcept
			{
				return Add(T{1});
			}

			//Post-increment the running total with 1
			constexpr auto operator++(int) noexcept
			{
				auto temp = *this;
				++*this;
				return temp;
			}

			//Increment the running total with the given amount
			constexpr auto& operator+=(T amount) noexcept
			{
				return Add(amount);
			}
	};
} //ion::types

#endif