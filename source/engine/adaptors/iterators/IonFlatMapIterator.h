/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors/iterators
File:	IonFlatMapIterator.h
-------------------------------------------
*/

#ifndef ION_FLAT_MAP_ITERATOR_H
#define ION_FLAT_MAP_ITERATOR_H

#include <cstddef>
#include <iterator>
#include <utility>

#include "types/IonTypeTraits.h"

namespace ion::adaptors::iterators
{
	template <typename Iterator>
	struct FlatMapIterator;

	template <typename Iterator>
	struct ConstFlatMapIterator;


	namespace flat_map_iterator::detail
	{
		/*
			Get flat map iterator type trait
			Type is either FlatMapIterator or ConstFlatMapIterator based on Iterator
		*/

		template <typename Iterator, typename = void>
		struct get_flat_map_iterator
		{
			using type = FlatMapIterator<Iterator>;
		};

		template <typename Iterator>
		struct get_flat_map_iterator<Iterator,
			std::enable_if_t<types::is_const_iterator_v<Iterator>>>
		{
			using type = ConstFlatMapIterator<Iterator>;
		};

		template <typename Iterator>
		using get_flat_map_iterator_t = typename get_flat_map_iterator<Iterator>::type;


		template <typename T>
		using add_const_to_first_t = std::pair<const typename T::first_type, typename T::second_type>;
	} //flat_map_iterator::detail


	//A const iterator adaptor class for flat map, that makes the dereferenced first value of the pair const
	template <typename Iterator>
	struct ConstFlatMapIterator : Iterator
	{
		using iterator_category = std::random_access_iterator_tag;

		using value_type = typename Iterator::value_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = const flat_map_iterator::detail::add_const_to_first_t<value_type>*;
		using reference = const flat_map_iterator::detail::add_const_to_first_t<value_type>&;

		static_assert(types::is_pair_v<value_type> &&
					  offsetof(value_type, second) == offsetof(flat_map_iterator::detail::add_const_to_first_t<value_type>, second));


		constexpr ConstFlatMapIterator() = default;

		//Construct const flat map iterator from underlying iterator type
		constexpr ConstFlatMapIterator(const Iterator &iterator) noexcept :
			Iterator{iterator}
		{
			//Empty
		}


		/*
			Operators
			Modifiers
		*/

		//Post increment iterator
		[[nodiscard]] constexpr ConstFlatMapIterator operator++(int) noexcept
		{
			auto iter = *this;
			++*this;
			return iter;
		}

		//Pre increment iterator
		constexpr ConstFlatMapIterator &operator++() noexcept
		{
			Iterator::operator++();
			return *this;
		}

		//Post decrement iterator
		[[nodiscard]] constexpr ConstFlatMapIterator operator--(int) noexcept
		{
			auto iter = *this;
			--*this;
			return iter;
		}

		//Pre decrement iterator
		constexpr ConstFlatMapIterator &operator--() noexcept
		{
			Iterator::operator--();
			return *this;
		}


		//Increment iterator by the given offset
		//Returns the result as a copy
		[[nodiscard]] constexpr ConstFlatMapIterator operator+(int off) const noexcept
		{
			auto iter = *this;
			iter.operator+=(off);
			return iter;
		}

		//Decrement iterator by the given offset
		//Returns the result as a copy
		[[nodiscard]] constexpr ConstFlatMapIterator operator-(int off) const noexcept
		{
			auto iter = *this;
			iter.operator-=(off);
			return iter;
		}

		//Increment iterator by the given offset
		constexpr ConstFlatMapIterator &operator+=(int off) noexcept
		{
			Iterator::operator+=(off);
			return *this;
		}

		//Decrement iterator by the given offset
		constexpr ConstFlatMapIterator &operator-=(int off) noexcept
		{
			Iterator::operator-=(off);
			return *this;
		}


		//Returns the difference (distance) between this and the given iterator
		[[nodiscard]] constexpr difference_type operator-(const ConstFlatMapIterator &rhs) const noexcept
		{
			return Iterator::operator-(rhs);
		}


		/*
			Observers
		*/

		//Returns the dereferenced underlying value
		[[nodiscard]] constexpr reference operator*() const noexcept
		{
			return reinterpret_cast<reference>(Iterator::operator*());
		}

		//Returns a pointer to the dereferenced underlying value
		[[nodiscard]] constexpr pointer operator->() const noexcept
		{
			return &**this;
		}

		//Returns the dereferenced underlying value at the given offset
		[[nodiscard]] constexpr reference operator[](difference_type off) const noexcept
		{
			return reinterpret_cast<reference>(Iterator::operator[](off));
		}
	};


	//An iterator adaptor class for flat map, that makes the dereferenced first value of the pair const
	template <typename Iterator>
	struct FlatMapIterator final : ConstFlatMapIterator<Iterator>
	{
		using iterator_category = typename ConstFlatMapIterator<Iterator>::iterator_category;

		using value_type = typename ConstFlatMapIterator<Iterator>::value_type;
		using difference_type = typename ConstFlatMapIterator<Iterator>::difference_type;
		using pointer = flat_map_iterator::detail::add_const_to_first_t<value_type>*;
		using reference = flat_map_iterator::detail::add_const_to_first_t<value_type>&;


		constexpr FlatMapIterator() = default;

		//Construct flat map iterator from underlying iterator type
		constexpr FlatMapIterator(const Iterator &iterator) noexcept :
			ConstFlatMapIterator<Iterator>{iterator}
		{
			//Empty
		}


		/*
			Operators
			Modifiers
		*/

		//Post increment iterator
		[[nodiscard]] constexpr FlatMapIterator operator++(int) noexcept
		{
			auto iter = *this;
			++*this;
			return iter;
		}

		//Pre increment iterator
		constexpr FlatMapIterator &operator++() noexcept
		{
			Iterator::operator++();
			return *this;
		}

		//Post decrement iterator
		[[nodiscard]] constexpr FlatMapIterator operator--(int) noexcept
		{
			auto iter = *this;
			--*this;
			return iter;
		}

		//Pre decrement iterator
		constexpr FlatMapIterator &operator--() noexcept
		{
			Iterator::operator--();
			return *this;
		}


		//Increment iterator by the given offset
		//Returns the result as a copy
		[[nodiscard]] constexpr FlatMapIterator operator+(int off) const noexcept
		{
			auto iter = *this;
			iter.operator+=(off);
			return iter;
		}

		//Decrement iterator by the given offset
		//Returns the result as a copy
		[[nodiscard]] constexpr FlatMapIterator operator-(int off) const noexcept
		{
			auto iter = *this;
			iter.operator-=(off);
			return iter;
		}

		//Increment iterator by the given offset
		constexpr FlatMapIterator &operator+=(int off) noexcept
		{
			Iterator::operator+=(off);
			return *this;
		}

		//Decrement iterator by the given offset
		constexpr FlatMapIterator &operator-=(int off) noexcept
		{
			Iterator::operator-=(off);
			return *this;
		}


		//Returns the difference (distance) between this and the given iterator
		[[nodiscard]] constexpr difference_type operator-(const FlatMapIterator &rhs) const noexcept
		{
			return Iterator::operator-(rhs);
		}


		/*
			Observers
		*/

		//Returns the dereferenced underlying value
		[[nodiscard]] constexpr reference operator*() const noexcept
		{
			return reinterpret_cast<reference>(Iterator::operator*());
		}

		//Returns a pointer to the dereferenced underlying value
		[[nodiscard]] constexpr pointer operator->() const noexcept
		{
			return &**this;
		}

		//Returns the dereferenced underlying value at the given offset
		[[nodiscard]] constexpr reference operator[](difference_type off) const noexcept
		{
			return reinterpret_cast<reference>(Iterator::operator[](off));
		}
	};
} //ion::adaptors::iterators

#endif