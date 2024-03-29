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

#ifndef ION_ITERABLE_H
#define ION_ITERABLE_H

#include "IonBasicIterable.h"
#include "IonBasicReverseIterable.h"

namespace ion::adaptors::ranges
{
	///@brief A class that provides functionality for a forward iterable range
	template <typename Container>
	struct Iterable : BasicIterable<Container>
	{
		using BasicIterable<Container>::BasicIterable;
	};

	///@brief A class that provides functionality for an reverse iterable range
	template <typename Container>
	struct ReverseIterable : BasicReverseIterable<Container>
	{
		using BasicReverseIterable<Container>::BasicReverseIterable;
	};
} //ion::adaptors::ranges

#endif