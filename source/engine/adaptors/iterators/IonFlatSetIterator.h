/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors/iterators
File:	IonFlatSetIterator.h
-------------------------------------------
*/

#ifndef ION_FLAT_SET_ITERATOR_H
#define ION_FLAT_SET_ITERATOR_H

#include <iterator>
#include "types/IonTypeTraits.h"

namespace ion::adaptors::iterators
{
	template <typename Iterator>
	struct FlatSetIterator;

	template <typename Iterator>
	struct ConstFlatSetIterator;


	namespace flat_set_iterator::detail
	{
		/*
			Get flat set iterator type trait
			Type is either FlatSetIterator or ConstFlatSetIterator based on Iterator
		*/

		template <typename Iterator, typename = void>
		struct get_flat_set_iterator
		{
			using type = FlatSetIterator<Iterator>;
		};

		template <typename Iterator>
		struct get_flat_set_iterator<Iterator,
			std::enable_if_t<types::is_const_iterator_v<Iterator>>>
		{
			using type = ConstFlatSetIterator<Iterator>;
		};

		template <typename Iterator>
		using get_flat_set_iterator_t = typename get_flat_set_iterator<Iterator>::type;
	} //flat_set_iterator::detail


	template <typename Iterator>
	struct ConstFlatSetIterator : Iterator
	{
		using iterator_category = std::random_access_iterator_tag;

		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = const value_type*;
		using reference = const value_type&;


		constexpr ConstFlatSetIterator() = default;

		//Construct const flat set iterator from underlying iterator type
		constexpr ConstFlatSetIterator(const Iterator &iterator) noexcept :
			Iterator{iterator}
		{
			//Empty
		}


		/*
			Operators
			Modifiers
		*/

		//Post increment iterator
		[[nodiscard]] constexpr ConstFlatSetIterator operator++(int) noexcept
		{
			auto iter = *this;
			++*this;
			return iter;
		}

		//Pre increment iterator
		constexpr ConstFlatSetIterator &operator++() noexcept
		{
			Iterator::operator++();
			return *this;
		}

		//Post decrement iterator
		[[nodiscard]] constexpr ConstFlatSetIterator operator--(int) noexcept
		{
			auto iter = *this;
			--*this;
			return iter;
		}

		//Pre decrement iterator
		constexpr ConstFlatSetIterator &operator--() noexcept
		{
			Iterator::operator--();
			return *this;
		}


		//Increment iterator by the given offset
		//Returns the result as a copy
		[[nodiscard]] constexpr ConstFlatSetIterator operator+(int off) const noexcept
		{
			auto iter = *this;
			iter.operator+=(off);
			return iter;
		}

		//Decrement iterator by the given offset
		//Returns the result as a copy
		[[nodiscard]] constexpr ConstFlatSetIterator operator-(int off) const noexcept
		{
			auto iter = *this;
			iter.operator-=(off);
			return iter;
		}

		//Increment iterator by the given offset
		constexpr ConstFlatSetIterator &operator+=(int off) noexcept
		{
			Iterator::operator+=(off);
			return *this;
		}

		//Decrement iterator by the given offset
		constexpr ConstFlatSetIterator &operator-=(int off) noexcept
		{
			Iterator::operator-=(off);
			return *this;
		}


		//Returns the difference (distance) between this and the given iterator
		[[nodiscard]] constexpr difference_type operator-(const ConstFlatSetIterator &rhs) const noexcept
		{
			return Iterator::operator-(rhs);
		}


		/*
			Observers
		*/

		//Returns the dereferenced underlying value
		[[nodiscard]] constexpr reference operator*() const noexcept
		{
			return Iterator::operator*();
		}

		//Returns a pointer to the dereferenced underlying value
		[[nodiscard]] constexpr pointer operator->() const noexcept
		{
			return &**this;
		}

		//Returns the dereferenced underlying value at the given offset
		[[nodiscard]] constexpr reference operator[](difference_type off) const noexcept
		{
			return Iterator::operator[](off);
		}
	};


	template <typename Iterator>
	struct FlatSetIterator final : ConstFlatSetIterator<Iterator>
	{
		using iterator_category = typename ConstFlatSetIterator<Iterator>::iterator_category;

		using value_type = typename ConstFlatSetIterator<Iterator>::value_type;
		using difference_type = typename ConstFlatSetIterator<Iterator>::difference_type;
		using pointer = typename ConstFlatSetIterator<Iterator>::pointer;
		using reference = typename ConstFlatSetIterator<Iterator>::reference;


		constexpr FlatSetIterator() = default;

		//Construct flat set iterator from underlying iterator type
		constexpr FlatSetIterator(const Iterator &iterator) noexcept :
			ConstFlatSetIterator<Iterator>{iterator}
		{
			//Empty
		}


		/*
			Operators
			Modifiers
		*/

		//Post increment iterator
		[[nodiscard]] constexpr FlatSetIterator operator++(int) noexcept
		{
			auto iter = *this;
			++*this;
			return iter;
		}

		//Pre increment iterator
		constexpr FlatSetIterator &operator++() noexcept
		{
			Iterator::operator++();
			return *this;
		}

		//Post decrement iterator
		[[nodiscard]] constexpr FlatSetIterator operator--(int) noexcept
		{
			auto iter = *this;
			--*this;
			return iter;
		}

		//Pre decrement iterator
		constexpr FlatSetIterator &operator--() noexcept
		{
			Iterator::operator--();
			return *this;
		}


		//Increment iterator by the given offset
		//Returns the result as a copy
		[[nodiscard]] constexpr FlatSetIterator operator+(int off) const noexcept
		{
			auto iter = *this;
			iter.operator+=(off);
			return iter;
		}

		//Decrement iterator by the given offset
		//Returns the result as a copy
		[[nodiscard]] constexpr FlatSetIterator operator-(int off) const noexcept
		{
			auto iter = *this;
			iter.operator-=(off);
			return iter;
		}

		//Increment iterator by the given offset
		constexpr FlatSetIterator &operator+=(int off) noexcept
		{
			Iterator::operator+=(off);
			return *this;
		}

		//Decrement iterator by the given offset
		constexpr FlatSetIterator &operator-=(int off) noexcept
		{
			Iterator::operator-=(off);
			return *this;
		}


		//Returns the difference (distance) between this and the given iterator
		[[nodiscard]] constexpr difference_type operator-(const FlatSetIterator &rhs) const noexcept
		{
			return Iterator::operator-(rhs);
		}


		/*
			Observers
		*/

		//Returns the dereferenced underlying value
		[[nodiscard]] constexpr reference operator*() const noexcept
		{
			return Iterator::operator*();
		}

		//Returns a pointer to the dereferenced underlying value
		[[nodiscard]] constexpr pointer operator->() const noexcept
		{
			return &**this;
		}

		//Returns the dereferenced underlying value at the given offset
		[[nodiscard]] constexpr reference operator[](difference_type off) const noexcept
		{
			return Iterator::operator[](off);
		}
	};
} //ion::adaptors::iterators

#endif