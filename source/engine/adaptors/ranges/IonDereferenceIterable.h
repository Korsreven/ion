/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors/ranges
File:	IonDereferenceIterable.h
-------------------------------------------
*/

#ifndef ION_DEREFERENCE_ITERABLE_H
#define ION_DEREFERENCE_ITERABLE_H

#include "IonBasicIterable.h"
#include "IonBasicReverseIterable.h"
#include "adaptors/iterators/IonDereferenceIterator.h"

namespace ion::adaptors::ranges
{
	//A class that provides functionality for a forward dereference iterable range
	template <typename Container>
	struct DereferenceIterable :
		BasicIterable<Container, adaptors::iterators::dereference_iterator::detail::get_dereference_iterator>
	{
		using BasicIterable<Container, adaptors::iterators::dereference_iterator::detail::get_dereference_iterator>::BasicIterable;
	};

	//A class that provides functionality for an reverse dereference iterable range
	template <typename Container>
	struct ReverseDereferenceIterable :
		BasicReverseIterable<Container, adaptors::iterators::dereference_iterator::detail::get_dereference_iterator>
	{
		using BasicReverseIterable<Container, adaptors::iterators::dereference_iterator::detail::get_dereference_iterator>::BasicReverseIterable;
	};
} //ion::adaptors::ranges

#endif