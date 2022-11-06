/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors/iterators
File:	IonDereferenceIterator.h
-------------------------------------------
*/

#ifndef ION_DEREFERENCE_ITERATOR_H
#define ION_DEREFERENCE_ITERATOR_H

#include <iterator>
#include <memory>

#include "types/IonTypeTraits.h"

namespace ion::adaptors::iterators
{
	template <typename Iterator>
	struct DereferenceIterator;

	template <typename Iterator>
	struct ConstDereferenceIterator;


	namespace dereference_iterator::detail
	{
		/**
			@name Get dereference iterator type trait
			Type is either DereferenceIterator or ConstDereferenceIterator based on Iterator
			@{
		*/

		template <typename Iterator, typename = void>
		struct get_dereference_iterator
		{
			using type = DereferenceIterator<Iterator>;
		};

		template <typename Iterator>
		struct get_dereference_iterator<Iterator,
			std::enable_if_t<types::is_const_iterator_v<Iterator>>>
		{
			using type = ConstDereferenceIterator<Iterator>;
		};

		template <typename Iterator>
		using get_dereference_iterator_t = typename get_dereference_iterator<Iterator>::type;

		///@}
	} //dereference_iterator::detail


	///@brief A const iterator adaptor class that automatically dereferences the iterator value
	template <typename Iterator>
	struct ConstDereferenceIterator : Iterator
	{
		using iterator_category = std::random_access_iterator_tag;

		using value_type = typename std::pointer_traits<typename Iterator::value_type>::element_type;
		using difference_type = typename Iterator::difference_type;
		using pointer = const value_type*;
		using reference = const value_type&;


		///@brief Default constructor
		constexpr ConstDereferenceIterator() = default;

		///@brief Constructs a const dereference iterator from underlying iterator type
		constexpr ConstDereferenceIterator(const Iterator &iterator) noexcept :
			Iterator{iterator}
		{
			//Empty
		}


		/**
			@name Operators
			@{
		*/

		///@brief Post-increments iterator
		[[nodiscard]] constexpr ConstDereferenceIterator operator++(int) noexcept
		{
			auto iter = *this;
			++*this;
			return iter;
		}

		///@brief Pre-increments iterator
		constexpr ConstDereferenceIterator &operator++() noexcept
		{
			Iterator::operator++();
			return *this;
		}

		///@brief Post-decrements iterator
		[[nodiscard]] constexpr ConstDereferenceIterator operator--(int) noexcept
		{
			auto iter = *this;
			--*this;
			return iter;
		}

		///@brief Pre-decrements iterator
		constexpr ConstDereferenceIterator &operator--() noexcept
		{
			Iterator::operator--();
			return *this;
		}


		///@brief Increments iterator by the given offset
		///@details Returns the result as a copy
		[[nodiscard]] constexpr ConstDereferenceIterator operator+(int off) const noexcept
		{
			auto iter = *this;
			iter.operator+=(off);
			return iter;
		}

		///@brief Decrements iterator by the given offset
		///@details Returns the result as a copy
		[[nodiscard]] constexpr ConstDereferenceIterator operator-(int off) const noexcept
		{
			auto iter = *this;
			iter.operator-=(off);
			return iter;
		}

		///@brief Increments iterator by the given offset
		constexpr ConstDereferenceIterator &operator+=(int off) noexcept
		{
			Iterator::operator+=(off);
			return *this;
		}

		///@brief Decrements iterator by the given offset
		constexpr ConstDereferenceIterator &operator-=(int off) noexcept
		{
			Iterator::operator-=(off);
			return *this;
		}


		///@brief Returns the difference (distance) between this and the given iterator
		[[nodiscard]] constexpr difference_type operator-(const ConstDereferenceIterator &rhs) const noexcept
		{
			return Iterator::operator-(rhs);
		}

		///@}

		/**
			@name Observers
			@{
		*/

		///@brief Returns the dereferenced underlying value
		[[nodiscard]] constexpr reference operator*() const noexcept
		{
			return *Iterator::operator*();
		}

		///@brief Returns a pointer to the dereferenced underlying value
		[[nodiscard]] constexpr pointer operator->() const noexcept
		{
			return &**this;
		}

		///@brief Returns the dereferenced underlying value at the given offset
		[[nodiscard]] constexpr reference operator[](difference_type off) const noexcept
		{
			return *Iterator::operator[](off);
		}

		///@}
	};


	///@brief An iterator adaptor class that automatically dereferences the iterator value
	template <typename Iterator>
	struct DereferenceIterator final : ConstDereferenceIterator<Iterator>
	{
		using iterator_category = typename ConstDereferenceIterator<Iterator>::iterator_category;

		using value_type = typename ConstDereferenceIterator<Iterator>::value_type;
		using difference_type = typename ConstDereferenceIterator<Iterator>::difference_type;
		using pointer = value_type*;
		using reference = value_type&;


		///@brief Default constructor
		constexpr DereferenceIterator() = default;

		///@brief Constructs a dereference iterator from underlying iterator type
		constexpr DereferenceIterator(const Iterator &iterator) noexcept :
			ConstDereferenceIterator<Iterator>{iterator}
		{
			//Empty
		}


		/**
			@name Operators
			@{
		*/

		///@brief Post-increments iterator
		[[nodiscard]] constexpr DereferenceIterator operator++(int) noexcept
		{
			auto iter = *this;
			++*this;
			return iter;
		}

		///@brief Pre-increments iterator
		constexpr DereferenceIterator &operator++() noexcept
		{
			Iterator::operator++();
			return *this;
		}

		///@brief Post-decrements iterator
		[[nodiscard]] constexpr DereferenceIterator operator--(int) noexcept
		{
			auto iter = *this;
			--*this;
			return iter;
		}

		///@brief Pre-decrements iterator
		constexpr DereferenceIterator &operator--() noexcept
		{
			Iterator::operator--();
			return *this;
		}


		///@brief Increments iterator by the given offset
		///@details Returns the result as a copy
		[[nodiscard]] constexpr DereferenceIterator operator+(int off) const noexcept
		{
			auto iter = *this;
			iter.operator+=(off);
			return iter;
		}

		///@brief Decrements iterator by the given offset
		///@details Returns the result as a copy
		[[nodiscard]] constexpr DereferenceIterator operator-(int off) const noexcept
		{
			auto iter = *this;
			iter.operator-=(off);
			return iter;
		}

		///@brief Increments iterator by the given offset
		constexpr DereferenceIterator &operator+=(int off) noexcept
		{
			Iterator::operator+=(off);
			return *this;
		}

		///@brief Decrements iterator by the given offset
		constexpr DereferenceIterator &operator-=(int off) noexcept
		{
			Iterator::operator-=(off);
			return *this;
		}


		///@brief Returns the difference (distance) between this and the given iterator
		[[nodiscard]] constexpr difference_type operator-(const DereferenceIterator &rhs) const noexcept
		{
			return Iterator::operator-(rhs);
		}

		///@}

		/**
			@name Observers
			@{
		*/

		///@brief Returns the dereferenced underlying value
		[[nodiscard]] constexpr reference operator*() const noexcept
		{
			return *Iterator::operator*();
		}

		///@brief Returns a pointer to the dereferenced underlying value
		[[nodiscard]] constexpr pointer operator->() const noexcept
		{
			return &**this;
		}

		///@brief Returns the dereferenced underlying value at the given offset
		[[nodiscard]] constexpr reference operator[](difference_type off) const noexcept
		{
			return *Iterator::operator[](off);
		}

		///@}
	};
} //ion::adaptors::iterators

#endif