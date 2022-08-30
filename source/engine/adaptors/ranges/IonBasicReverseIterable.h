/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors/ranges
File:	IonBasicReverseIterable.h
-------------------------------------------
*/

#ifndef ION_BASIC_REVERSE_ITERABLE_H
#define ION_BASIC_REVERSE_ITERABLE_H

#include <type_traits>

#include "adaptors/IonContainerHolder.h"
#include "types/IonTypeTraits.h"

namespace ion::adaptors::ranges
{
	//A class that provides a basic interface for an reverse iterable range
	template <typename Container,
		template <typename...> typename IteratorAdaptor = types::identity>
	struct BasicReverseIterable : Holder<Container>
	{
		using my_base = Holder<Container>;
		using my_base::my_base;


		using iterator = typename IteratorAdaptor<typename my_base::container_type::reverse_iterator>::type;
		using const_iterator = typename IteratorAdaptor<typename my_base::container_type::const_reverse_iterator>::type;

		using reverse_iterator = typename IteratorAdaptor<typename my_base::container_type::iterator>::type;
		using const_reverse_iterator = typename IteratorAdaptor<typename my_base::container_type::const_iterator>::type;


		/*
			Iterators
		*/

		//Returns an iterator to the beginning of the range
		[[nodiscard]] constexpr auto begin() noexcept
		{
			if constexpr (std::is_const_v<std::remove_reference_t<Container>>)
				return const_iterator{std::rbegin(this->container_)};
			else
				return iterator{std::rbegin(this->container_)};
		}

		//Returns an iterator to the end of the range
		[[nodiscard]] constexpr auto end() noexcept
		{
			if constexpr (std::is_const_v<std::remove_reference_t<Container>>)
				return const_iterator{std::rend(this->container_)};
			else
				return iterator{std::rend(this->container_)};
		}

		//Returns a const iterator to the beginning of the range
		[[nodiscard]] constexpr auto begin() const noexcept
		{
			return const_iterator{std::rbegin(this->container_)};
		}

		//Returns a const iterator to the end of the range
		[[nodiscard]] constexpr auto end() const noexcept
		{
			return const_iterator{std::rend(this->container_)};
		}


		//Returns a const iterator to the beginning of the range
		[[nodiscard]] constexpr auto cbegin() const noexcept
		{
			return const_iterator{std::crbegin(this->container_)};
		}

		//Returns a const iterator to the end of the range
		[[nodiscard]] constexpr auto cend() const noexcept
		{
			return const_iterator{std::crend(this->container_)};
		}


		/*
			Reverse iterators
		*/

		//Returns a reverse iterator to the end of the range
		[[nodiscard]] constexpr auto rbegin() noexcept
		{
			if constexpr (std::is_const_v<std::remove_reference_t<Container>>)
				return const_reverse_iterator{std::begin(this->container_)};
			else
				return reverse_iterator{std::begin(this->container_)};
		}

		//Returns a reverse iterator to the beginning of the range
		[[nodiscard]] constexpr auto rend() noexcept
		{
			if constexpr (std::is_const_v<std::remove_reference_t<Container>>)
				return const_reverse_iterator{std::end(this->container_)};
			else
				return reverse_iterator{std::end(this->container_)};
		}

		//Returns a const reverse iterator to the end of the range
		[[nodiscard]] constexpr auto rbegin() const noexcept
		{
			return const_reverse_iterator{std::begin(this->container_)};
		}

		//Returns a const reverse iterator to the beginning of the range
		[[nodiscard]] constexpr auto rend() const noexcept
		{
			return const_reverse_iterator{std::end(this->container_)};
		}


		//Returns a const reverse iterator to the end of the range
		[[nodiscard]] constexpr auto crbegin() const noexcept
		{
			return const_reverse_iterator{std::cbegin(this->container_)};
		}

		//Returns a const reverse iterator to the beginning of the range
		[[nodiscard]] constexpr auto crend() const noexcept
		{
			return const_reverse_iterator{std::cend(this->container_)};
		}
	};
} //ion::adaptors::ranges

#endif