/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	unmanaged
File:	IonObjectFactory.h
-------------------------------------------
*/

#ifndef ION_OBJECT_FACTORY_H
#define ION_OBJECT_FACTORY_H

#include <utility>
#include <vector>

#include "adaptors/ranges/IonDereferenceIterable.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"

namespace ion::unmanaged
{
	namespace object_factory::detail
	{
		template <typename T>
		using container_type = std::vector<OwningPtr<T>>;
	} //object_factory::detail


	template <typename T>
	class ObjectFactory
	{
		public:

			using object_type = T;

		private:

			object_factory::detail::container_type<T> objects_;

		protected:

			/*
				Creating
			*/

			//Create an object with the given arguments
			template <typename... Args>
			auto Create(Args &&...args)
			{
				auto &ptr = objects_.emplace_back(
					make_owning<T>(std::forward<Args>(args)...));
				return NonOwningPtr<T>{ptr};
			}

		public:

			//Default constructor
			ObjectFactory() = default;

			//Deleted copy constructor
			ObjectFactory(const ObjectFactory&) = delete;


			/*
				Operators
			*/

			//Deleted copy assignment
			ObjectFactory& operator=(const ObjectFactory&) = delete;


			/*
				Ranges
			*/

			//Returns a mutable range of all objects in this factory
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() noexcept
			{
				return adaptors::ranges::DereferenceIterable<object_factory::detail::container_type<T>&>{objects_};
			}

			//Returns an immutable range of all objects in this factory
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const object_factory::detail::container_type<T>&>{objects_};
			}


			/*
				Removing
			*/

			//Clear all objects from this factory
			void Clear() noexcept
			{
				objects_.clear();
				objects_.shrink_to_fit();
			}

			//Remove an object from this factory
			auto Remove(T &object) noexcept
			{
				auto iter =
					std::find_if(std::begin(objects_), std::end(objects_),
						[&](auto &x) noexcept
						{
							return x.get() == &object;
						});

				//Object found
				if (iter != std::end(objects_))
				{
					objects_.erase(iter);
					return true;
				}
				else
					return false;
			}
	};
} //ion::unmanaged

#endif