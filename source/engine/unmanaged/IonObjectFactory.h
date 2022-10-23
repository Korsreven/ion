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


	//A class representing an owning factory that can create and store multiple objects of a single type
	//The created objects can be of any type, and does not need to have any knowledge about the factory
	//This class must be derived from to be able to create objects
	template <typename ObjectT>
	class ObjectFactory
	{
		public:

			using object_type = ObjectT;

		private:

			object_factory::detail::container_type<ObjectT> objects_;


			/*
				Creating
			*/

			template <typename T, typename... Args>
			auto Emplace(Args &&...args)
			{
				auto &ptr = objects_.emplace_back(
					make_owning<T>(std::forward<Args>(args)...));
				return NonOwningPtr<ObjectT>{ptr};
			}

		protected:

			/*
				Creating
			*/

			//Creates an object with the given arguments
			template <typename T = ObjectT, typename... Args,
				typename = std::enable_if_t<std::is_base_of_v<ObjectT, std::remove_cvref_t<T>>>>
			auto Create(Args &&...args)
			{
				return Emplace<std::remove_cvref_t<T>>(std::forward<Args>(args)...);
			}

			//Creates an object by copying/moving the given object
			template <typename T,
				typename = std::enable_if_t<std::is_base_of_v<ObjectT, std::remove_cvref_t<T>>>>
			auto Create(T &&object)
			{
				return Emplace<std::remove_cvref_t<T>>(std::forward<T>(object));
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
				return adaptors::ranges::DereferenceIterable<object_factory::detail::container_type<ObjectT>&>{objects_};
			}

			//Returns an immutable range of all objects in this factory
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const object_factory::detail::container_type<ObjectT>&>{objects_};
			}


			/*
				Removing
			*/

			//Clears all objects from this factory
			void Clear() noexcept
			{
				objects_.clear();
				objects_.shrink_to_fit();
			}

			//Removes an object from this factory
			auto Remove(ObjectT &object) noexcept
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