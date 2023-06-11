/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonTrace.h
-------------------------------------------
*/

#ifndef ION_TRACE_H
#define ION_TRACE_H

#include <algorithm>
#include <cassert>
#include <vector>

#include "adaptors/ranges/IonDereferenceIterable.h"

namespace ion::types
{
	namespace trace::detail
	{
		template <typename T>
		using container_type = std::vector<T*>;


		template <typename T>
		void add_to(container_type<T> &instances, T &instance)
		{
			instances.push_back(&instance); //Assumes unique
		}

		template <typename T>
		void remove_from(container_type<T> &instances, T &instance)
		{
			if (auto iter = std::find(std::begin(instances), std::end(instances), &instance);
				iter != std::end(instances))
				instances.erase(iter);
		}
	} //trace::detail


	///@brief A class providing the ability to trace instances of the derived type
	///@details Allowed: struct Foo : Trace&lt;Foo&gt; {};
	///Not allowed: struct Foo : Trace&lt;Bar&gt; {};
	///Not allowed:	Trace&lt;int&gt; foobar;
	template <typename T>
	class Trace
	{
		friend T;

		private:

			static inline trace::detail::container_type<T> instances_;


			///@brief Can only be instantiated if inherited from by T
			Trace()
			{
				trace::detail::add_to(instances_, static_cast<T&>(*this));
			}

			///@brief Can only be destroyed if inherited from by T
			~Trace()
			{
				trace::detail::remove_from(instances_, static_cast<T&>(*this));
			}


			///@brief Copy constructor
			Trace(const Trace&)
			{
				trace::detail::add_to(instances_, static_cast<T&>(*this));
			}

			///@brief Move constructor
			Trace(Trace&&)
			{
				trace::detail::add_to(instances_, static_cast<T&>(*this));
			}


			///@brief Default copy assignment
			Trace& operator=(const Trace&) = default;

			///@brief Default move assignment
			Trace& operator=(Trace&&) = default;

		public:

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all instances of T in this trace
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] static inline auto Instances() noexcept
			{
				return adaptors::ranges::DereferenceIterable<trace::detail::container_type<T>&>{instances_};
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns an instance of T with the given index
			[[nodiscard]] static auto& Instance(int index) noexcept
			{
				assert(index >= 0 && index < std::ssize(instances_));
				return *instances_[index];
			}

			///@brief Returns the first instance of T
			[[nodiscard]] static auto& FirstInstance() noexcept
			{
				assert(!std::empty(instances_));
				return *instances_.front();
			}

			///@brief Returns the last instance of T
			[[nodiscard]] static auto& LastInstance() noexcept
			{
				assert(!std::empty(instances_));
				return *instances_.back();
			}

			///@}
	};
} //ion::types

#endif