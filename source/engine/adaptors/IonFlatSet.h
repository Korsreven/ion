/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors
File:	IonFlatSet.h
-------------------------------------------
*/

#ifndef _ION_FLAT_SET_
#define _ION_FLAT_SET_

#include <functional>
#include <iterator>
#include <utility>
#include <vector>

#include "IonFlatAssociativeAdaptor.h"
#include "iterators/IonFlatSetIterator.h"

namespace ion::adaptors
{
	template <typename Key, typename Compare = std::less<>, //Transparent as default
		template <typename...> typename Container = std::vector, typename... Types>
	struct FlatSet final :
		FlatAssociativeAdaptor<Container<Key, Types...>, Key, Compare,
			iterators::flat_set_iterator::detail::get_flat_set_iterator>
	{
		using my_base = FlatAssociativeAdaptor<Container<Key, Types...>, Key, Compare,
			iterators::flat_set_iterator::detail::get_flat_set_iterator>;
		using my_base::my_base;
	};
} //ion::adaptors

#endif