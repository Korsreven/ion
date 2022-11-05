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
	///@brief A class representing a member value that has suppressed copies
	///@details Tag class members as SuppressCopy to disable copying of its value.
	///An object containing one or more tagged members can still be copied normally.
	///A member tagged as SuppressCopy will be default constructed when the copy constructor is called
	template <typename T>
	class SuppressCopy
	{
		static_assert(std::is_default_constructible_v<T> && std::is_move_constructible_v<T> && std::is_move_assignable_v<T>);

		public:

			using value_type = T;

		private:

			T value_{};
		
		public:

			///@brief Default constructor
			constexpr SuppressCopy() = default;

			///@brief Constructs a new value with the given value
			constexpr SuppressCopy(const T &value) noexcept :
				value_{value}
			{
				//Empty
			}

			///@brief Constructs a new value with the given value
			constexpr SuppressCopy(T &&value) noexcept :
				value_{std::move(value)}
			{
				//Empty
			}		

			///@brief Copy constructor
			constexpr SuppressCopy(const SuppressCopy<T>&) noexcept
			{
				//Suppress
			}

			///@brief Move constructor (trivially)
			constexpr SuppressCopy(SuppressCopy<T> &&rhs) noexcept
				requires std::is_trivially_move_constructible_v<T> :
				value_{std::exchange(rhs.value_, T{})}
			{
				//Empty
			}

			///@brief Move constructor (not trivially)
			constexpr SuppressCopy(SuppressCopy<T> &&rhs) noexcept
				requires !std::is_trivially_move_constructible_v<T> :
				value_{std::move(rhs.value_)}
			{
				//Empty
			}


			/**
				@name Operators
				@{
			*/

			///@brief Copy assignment
			constexpr auto& operator=(const SuppressCopy<T>&) noexcept
			{
				//Suppress
				return *this;
			}

			///@brief Move assignment (trivially)
			constexpr auto& operator=(SuppressCopy<T> &&rhs) noexcept
				requires std::is_trivially_move_constructible_v<T>
			{
				value_ = std::exchange(rhs.value_, T{});
				return *this;
			}

			///@brief Move assignment (not trivially)
			constexpr auto& operator=(SuppressCopy<T> &&rhs) noexcept
				requires !std::is_trivially_move_constructible_v<T>
			{
				value_ = std::move(rhs.value_);
				return *this;
			}

			///@brief Value copy assignment
			constexpr auto& operator=(const T &value) noexcept
			{
				value_ = value;
				return *this;
			}

			///@brief Value move assignment
			constexpr auto& operator=(T &&value) noexcept
			{
				value_ = std::move(value);
				return *this;
			}


			///@brief Combined comparison operator
			[[nodiscard]] constexpr auto operator<=>(const SuppressCopy<T>&) const = default;

			///@brief Returns true if the value is equal to the given value
			[[nodiscard]] constexpr auto operator==(const T &value) const noexcept
			{
				return value_ == value;
			}

			///@brief Returns true if the value is different from the given value
			[[nodiscard]] constexpr auto operator!=(const T &value) const noexcept
			{
				return !(*this == value);
			}


			///@brief Returns a mutable reference to the value
			[[nodiscard]] constexpr operator auto&() noexcept
			{
				return value_;
			}

			///@brief Returns an immutable reference to the value
			[[nodiscard]] constexpr operator auto&() const noexcept
			{
				return value_;
			}

			///@}

			/**
				@name Operators (for pointer types only)
				@{
			*/

			///@brief Returns true if the pointer points to something
			[[nodiscard]] explicit constexpr operator bool() const noexcept
				requires std::is_pointer_v<T>
			{
				return value_;
			}

			///@brief Returns a pointer to the value
			[[nodiscard]] constexpr auto operator->() const noexcept
				requires std::is_pointer_v<T>
			{
				return value_;
			}

			///@brief Dereferences pointer to the value
			[[nodiscard]] constexpr auto& operator*() const noexcept
				requires std::is_pointer_v<T>
			{
				return *value_;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns a mutable reference to the value
			[[nodiscard]] constexpr auto& Get() noexcept
			{
				return value_;
			}

			///@brief Returns an immutable reference to the value
			[[nodiscard]] constexpr auto& Get() const noexcept
			{
				return value_;
			}

			///@}
	};
} //ion::types

#endif