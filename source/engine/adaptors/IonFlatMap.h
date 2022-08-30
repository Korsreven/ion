/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors
File:	IonFlatMap.h
-------------------------------------------
*/

#ifndef ION_FLAT_MAP_H
#define ION_FLAT_MAP_H

#include <functional>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "IonFlatAssociativeAdaptor.h"
#include "iterators/IonFlatMapIterator.h"
#include "types/IonTypeTraits.h"

namespace ion::adaptors
{
	namespace flat_map::detail
	{
		constexpr auto out_of_range_message = "invalid FlatMap<Key, T> key";
	} //flat_map::detail


	//An adaptor class that provides flat map functionality to any flat container
	template <typename Key, typename T, typename Compare = std::less<>, //Transparent as default
		template <typename...> typename Container = std::vector, typename... Types>
	struct FlatMap final :
		FlatAssociativeAdaptor<Container<std::pair<Key, T>, Types...>, Key, Compare,
			iterators::flat_map_iterator::detail::get_flat_map_iterator>
	{
		using my_base = FlatAssociativeAdaptor<Container<std::pair<Key, T>, Types...>, Key, Compare,
			iterators::flat_map_iterator::detail::get_flat_map_iterator>;
		using my_base::my_base;

		using mapped_type = T;


		/*
			Element access
		*/

		//Return a mutable reference to the mapped type for the given key
		//If key does not exist insert it to the map with a default constructed mapped type
		[[nodiscard]] inline auto& operator[](const typename my_base::key_type &key)
		{
			auto [iter, found] = this->insert({key, mapped_type{}});
			return iter->second;
		}

		//Return a mutable reference to the mapped type for the given key (by move)
		//If key does not exist insert it to the map with a default constructed mapped type
		[[nodiscard]] inline auto& operator[](typename my_base::key_type &&key)
		{
			auto [iter, found] = this->insert({std::move(key), mapped_type{}});
			return iter->second;
		}


		//Return a mutable reference to the mapped type for the given key
		//Does bounds checking by throwing out of range if key does not exist
		[[nodiscard]] inline auto& at(const typename my_base::key_type &key)
		{
			return at<typename my_base::key_type>(key);
		}

		//Return a mutable reference to the mapped type for the given key
		//Does bounds checking by throwing out of range if key does not exist
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, typename my_base::key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto& at(const T &key)
		{
			auto iter = this->find(key);

			if (iter == std::end(this->container_))
				throw std::out_of_range{flat_map::detail::out_of_range_message};

			return iter->second;
		}

		//Return an immutable reference to the mapped type for the given key
		//Does bounds checking by throwing out of range if key does not exist
		[[nodiscard]] inline auto& at(const typename my_base::key_type &key) const
		{
			return at<typename my_base::key_type>(key);
		}

		//Return an immutable reference to the mapped type for the given key
		//Does bounds checking by throwing out of range if key does not exist
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, typename my_base::key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto& at(const T &key) const
		{
			auto iter = this->find(key);

			if (iter == std::end(this->container_))
				throw std::out_of_range{flat_map::detail::out_of_range_message};

			return iter->second;
		}
	};
} //ion::adaptors

#endif