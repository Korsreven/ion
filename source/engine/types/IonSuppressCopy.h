/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonSuppressCopy.h
-------------------------------------------
*/

#ifndef ION_SUPPRESS_COPY_H
#define ION_SUPPRESS_COPY_H

#include <compare>
#include <type_traits>
#include <utility>

namespace ion::types
{
	//A class representing a member value that has suppressed copies
	//Tag class members as SuppressCopy to disable copying of its value
	//An object containing one or more tagged members can still be copied normally
	//A member tagged as SuppressCopy will be default constructed when the copy constructor is called
	template <typename T>
	class SuppressCopy
	{
		static_assert(std::is_default_constructible_v<T> && std::is_move_constructible_v<T> && std::is_move_assignable_v<T>);

		public:

			using value_type = T;

		private:

			T value_{};
		
		public:

			//Default constructor
			constexpr SuppressCopy() = default;

			//Construct a new value with the given value
			constexpr SuppressCopy(const T &value) noexcept :
				value_{value}
			{
				//Empty
			}

			//Construct a new value with the given value
			constexpr SuppressCopy(T &&value) noexcept :
				value_{std::move(value)}
			{
				//Empty
			}		

			//Copy constructor
			constexpr SuppressCopy(const SuppressCopy<T>&) noexcept
			{
				//Suppress
			}

			//Move constructor (trivially)
			constexpr SuppressCopy(SuppressCopy<T> &&rhs) noexcept
				requires std::is_trivially_move_constructible_v<T> :
				value_{std::exchange(rhs.value_, T{})}
			{
				//Empty
			}

			//Move constructor (not trivially)
			constexpr SuppressCopy(SuppressCopy<T> &&rhs) noexcept
				requires !std::is_trivially_move_constructible_v<T> :
				value_{std::move(rhs.value_)}
			{
				//Empty
			}


			/*
				Operators
			*/

			//Copy assignment
			constexpr auto& operator=(const SuppressCopy<T>&) noexcept
			{
				//Suppress
				return *this;
			}

			//Move assignment (trivially)
			constexpr auto& operator=(SuppressCopy<T> &&rhs) noexcept
				requires std::is_trivially_move_constructible_v<T>
			{
				value_ = std::exchange(rhs.value_, T{});
				return *this;
			}

			//Move assignment (not trivially)
			constexpr auto& operator=(SuppressCopy<T> &&rhs) noexcept
				requires !std::is_trivially_move_constructible_v<T>
			{
				value_ = std::move(rhs.value_);
				return *this;
			}

			//Value copy assignment
			constexpr auto& operator=(const T &value) noexcept
			{
				value_ = value;
				return *this;
			}

			//Value move assignment
			constexpr auto& operator=(T &&value) noexcept
			{
				value_ = std::move(value);
				return *this;
			}


			//Combined comparison operator
			[[nodiscard]] constexpr auto operator<=>(const SuppressCopy<T>&) const = default;

			//Returns true if the value is equal to the given value
			[[nodiscard]] constexpr auto operator==(const T &value) const noexcept
			{
				return value_ == value;
			}

			//Returns true if the value is different from the given value
			[[nodiscard]] constexpr auto operator!=(const T &value) const noexcept
			{
				return !(*this == value);
			}


			//Returns a mutable reference to the value
			[[nodiscard]] constexpr operator auto&() noexcept
			{
				return value_;
			}

			//Returns an immutable reference to the value
			[[nodiscard]] constexpr operator auto&() const noexcept
			{
				return value_;
			}


			/*
				Operators
				For pointer types only
			*/

			//Returns true if the pointer points to something
			[[nodiscard]] explicit constexpr operator bool() const noexcept
				requires std::is_pointer_v<T>
			{
				return value_;
			}

			//Returns a pointer to the value
			[[nodiscard]] constexpr auto operator->() const noexcept
				requires std::is_pointer_v<T>
			{
				return value_;
			}

			//Dereferences pointer to the value
			[[nodiscard]] constexpr auto& operator*() const noexcept
				requires std::is_pointer_v<T>
			{
				return *value_;
			}


			/*
				Observers
			*/

			//Returns a mutable reference to the value
			[[nodiscard]] constexpr auto& Get() noexcept
			{
				return value_;
			}

			//Returns an immutable reference to the value
			[[nodiscard]] constexpr auto& Get() const noexcept
			{
				return value_;
			}
	};
} //ion::types

#endif