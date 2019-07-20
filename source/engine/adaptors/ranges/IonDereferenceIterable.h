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

#ifndef _ION_DEREFERENCE_ITERABLE_
#define _ION_DEREFERENCE_ITERABLE_

#include "IonBasicIterable.h"
#include "IonBasicReverseIterable.h"
#include "adaptors/iterators/IonDereferenceIterator.h"

namespace ion::adaptors::ranges
{
	template <typename Container>
	struct DereferenceIterable :
		BasicIterable<Container, adaptors::iterators::dereference_iterator::detail::get_dereference_iterator>
	{
		using BasicIterable<Container, adaptors::iterators::dereference_iterator::detail::get_dereference_iterator>::BasicIterable;
	};

	template <typename Container>
	struct ReverseDereferenceIterable :
		BasicReverseIterable<Container, adaptors::iterators::dereference_iterator::detail::get_dereference_iterator>
	{
		using BasicReverseIterable<Container, adaptors::iterators::dereference_iterator::detail::get_dereference_iterator>::BasicReverseIterable;
	};
} //ion::adaptors::ranges

#endif