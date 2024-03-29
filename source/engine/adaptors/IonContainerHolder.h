/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors
File:	IonContainerHolder.h
-------------------------------------------
*/

#ifndef ION_CONTAINER_HOLDER_H
#define ION_CONTAINER_HOLDER_H

#include <iterator>
#include <type_traits>

#include "IonBasicContainerHolder.h"

namespace ion::adaptors
{
	///@brief A class that provides a basic interface on top of a wrapped container
	template <typename Container>
	struct Holder : BasicHolder<Container>
	{
		using BasicHolder<Container>::BasicHolder;


		//The container type that this holder holds
		//Make sure that container_type is not a reference type
		using container_type = std::remove_reference_t<Container>;

		using value_type = typename container_type::value_type;
		using allocator_type = typename container_type::allocator_type;
		using size_type = typename container_type::size_type;
		using difference_type = typename container_type::difference_type;

		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using pointer = typename container_type::pointer;
		using const_pointer = typename container_type::const_pointer;


		/**
			@name Observers
			@{
		*/

		///@brief Returns a pointer to the data of this holder
		[[nodiscard]] constexpr auto data() const noexcept
		{
			return std::data(this->container_);
		}

		///@brief Returns true if this holders holds an empty container
		[[nodiscard]] constexpr auto empty() const noexcept
		{
			return std::empty(this->container_);
		}

		///@brief Returns the size of the container this holder holds
		[[nodiscard]] constexpr auto size() const noexcept
		{
			return std::size(this->container_);
		}

		///@brief Returns the max size of the container this holder holds
		[[nodiscard]] constexpr auto max_size() const noexcept
		{
			return this->container_.max_size();
		}

		///@}
	};
} //ion::adaptors

#endif