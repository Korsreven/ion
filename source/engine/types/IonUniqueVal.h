/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonUniqueVal.h
-------------------------------------------
*/

#ifndef ION_UNIQUE_VAL_H
#define ION_UNIQUE_VAL_H

#include <algorithm>
#include <compare>
#include <type_traits>
#include <utility>

namespace ion::types
{
	//A class representing a unique value
	//UniqueVal is similar to unique_ptr in that a value can only be moved not copied
	template <typename T>
	class UniqueVal
	{
		static_assert(std::is_default_constructible_v<T> && std::is_swappable_v<T>);

		public:

			using value_type = T;

		private:

			T value_{};
		
		public:

			//Default constructor
			constexpr UniqueVal() = default;

			//Constructs a new unique value with the given value
			constexpr explicit UniqueVal(T value) noexcept :
				value_{std::move(value)}
			{
				//Empty
			}

			//Copy constructor
			constexpr UniqueVal(const UniqueVal<T>&) noexcept
			{
				//Empty
			}

			//Move constructor
			constexpr UniqueVal(UniqueVal<T> &&rhs) noexcept :
				value_{std::exchange(rhs.value_, T{})}
			{
				//Empty
			}


			/*
				Operators
			*/

			//Copy assignment
			constexpr auto& operator=(const UniqueVal<T>&) noexcept
			{
				//Do nothing
				return *this;
			}

			//Move assignment
			constexpr auto& operator=(UniqueVal<T> &&rhs) noexcept
			{
				value_ = std::exchange(rhs.value_, T{});
				return *this;
			}

			//Combined comparison operator
			[[nodiscard]] constexpr auto operator<=>(const UniqueVal<T>&) const = default;


			/*
				Modifiers
			*/

			//Releases the unique value
			[[nodiscard]] constexpr auto Release() noexcept
			{
				auto value = std::move(value_);
				Reset();
				return value;
			}

			//Replaces the unique value
			constexpr void Reset(T value = T{}) noexcept
			{
				value_ = std::move(value);
			}

			//Swaps the unique values
			constexpr void Swap(UniqueVal<T> &rhs) noexcept
			{
				std::swap(value_, rhs.value_);
			}


			/*
				Observers
			*/

			//Returns an immutable reference to the unique value
			[[nodiscard]] constexpr auto& Get() const noexcept
			{
				return value_;
			}
	};
} //ion::types

#endif