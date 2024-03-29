/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonSuppressMove.h
-------------------------------------------
*/

#ifndef ION_SUPPRESS_MOVE_H
#define ION_SUPPRESS_MOVE_H

#include <compare>
#include <type_traits>
#include <utility>

namespace ion::types
{
	///@brief A class representing a member value that has suppressed copies and moves
	///@details Tag class members as SuppressMove to disable copying and moving of its value.
	///An object containing one or more tagged members can still be copied and moved normally.
	///A member tagged as SuppressMove will be default constructed when the copy or move constructor is called
	template <typename T>
	class SuppressMove
	{
		static_assert(std::is_default_constructible_v<T>);

		public:

			using value_type = T;

		private:

			T value_{};
		
		public:

			///@brief Default constructor
			constexpr SuppressMove() = default;

			///@brief Constructs a new value with the given value
			constexpr SuppressMove(const T &value) noexcept :
				value_{value}
			{
				//Empty
			}

			///@brief Constructs a new value with the given value
			constexpr SuppressMove(T &&value) noexcept :
				value_{std::move(value)}
			{
				//Empty
			}		

			///@brief Copy constructor
			constexpr SuppressMove(const SuppressMove<T>&) noexcept
			{
				//Suppress
			}


			/**
				@name Operators
				@{
			*/

			///@brief Copy assignment
			constexpr auto& operator=(const SuppressMove<T>&) noexcept
			{
				//Suppress
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
			[[nodiscard]] constexpr auto operator<=>(const SuppressMove<T>&) const = default;

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