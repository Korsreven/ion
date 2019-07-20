/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors/ranges
File:	IonIterable.h
-------------------------------------------
*/

#ifndef _ION_ITERABLE_
#define _ION_ITERABLE_

#include "IonBasicIterable.h"
#include "IonBasicReverseIterable.h"

namespace ion::adaptors::ranges
{
	template <typename Container>
	struct Iterable : BasicIterable<Container>
	{
		using BasicIterable<Container>::BasicIterable;
	};

	template <typename Container>
	struct ReverseIterable : BasicReverseIterable<Container>
	{
		using BasicReverseIterable<Container>::BasicReverseIterable;
	};
} //ion::adaptors::ranges

#endif