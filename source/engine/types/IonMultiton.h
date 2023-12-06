/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonMultiton.h
-------------------------------------------
*/

#ifndef ION_MULTITON_H
#define ION_MULTITON_H

#include <string>
#include <string_view>

#include "adaptors/IonFlatMap.h"
#include "memory/IonOwningPtr.h"

namespace ion::types
{
	namespace multiton::detail
	{
		template <typename T>
		using container_type = adaptors::FlatMap<std::string, OwningPtr<T>>;
	} //multiton::detail


	///@brief A class providing multiple instances of the derived type
	///@details Allowed: struct Foo : Multiton&lt;Foo&gt; {};
	///Not allowed: struct Foo : Multiton&lt;Bar&gt; {};
	///Not allowed:	Multiton&lt;int&gt; foobar;
	template <typename T>
	class Multiton
	{
		friend T;

		private:

			static inline multiton::detail::container_type<T> instances_;


			///@brief Can only be instantiated if inherited from by T
			Multiton() = default;

		public:

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all instances of T
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] static inline auto Instances() noexcept
			{
				return instances_.Elements();
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns a static instance of T with the given name
			///@details T is lazily constructed, meaning the first time this is called
			[[nodiscard]] static auto& Instance(std::string_view name)
			{
				if (auto iter = instances_.lower_bound(name);
					iter != std::end(instances_) && !instances_.key_comp()(name, iter->first))
					return *iter->second;
				else
					return *instances_.emplace_hint(iter, std::string{name}, ion::make_owning<T>())->second;
			}

			///@brief Returns true if an instance of T with the given name exists
			[[nodiscard]] static auto HasInstance(std::string_view name) noexcept
			{
				return instances_.contains(name);
			}

			///@brief Clears all instances of T
			static void ClearInstances() noexcept
			{
				instances_.clear();
				instances_.shrink_to_fit();
			}

			///@brief Removes an instance of T with the given name
			static auto RemoveInstance(std::string_view name) noexcept
			{
				if (auto iter = instances_.find(name); iter != std::end(instances_))
				{
					instances_.erase(iter);
					return true;
				}
				else
					return false;
			}

			///@}
	};
} //ion::types

#endif