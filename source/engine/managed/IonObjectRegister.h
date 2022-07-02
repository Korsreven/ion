/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	managed
File:	IonObjectRegister.h
-------------------------------------------
*/

#ifndef ION_OBJECT_REGISTER_H
#define ION_OBJECT_REGISTER_H

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "adaptors/ranges/IonDereferenceIterable.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::managed
{
	namespace object_register::detail
	{
		template <typename T>
		struct registered_object
		{
			std::optional<std::string> name;
			NonOwningPtr<T> ptr;


			inline auto& operator*() const noexcept
			{
				return ptr;
			}

			inline auto operator->() const noexcept
			{
				return ptr.get();
			}
		};

		template <typename T>
		using registered_objects = std::vector<registered_object<T>>;

		template <typename T>
		using object_pointers = std::vector<NonOwningPtr<T>>;


		template <typename T>
		inline auto get_object_by_name(std::string_view name, const object_register::detail::registered_objects<T> &objects) noexcept
		{
			auto iter =
				std::find_if(std::begin(objects), std::end(objects),
					[&](auto &object) noexcept
					{
						if (auto registered_name = object.name; registered_name)
							return *registered_name == name;
						else
							return false;
					});

			return iter != std::end(objects) ? iter->ptr : NonOwningPtr<T>{};
		}


		template <typename T, typename U>
		inline auto get_object_as(const object_register::detail::registered_objects<T> &objects)
		{
			for (auto &object : objects)
			{
				if (auto ptr = dynamic_pointer_cast<U>(object.ptr); ptr)
					return ptr;
			}

			return NonOwningPtr<U>{};
		}

		template <typename T, typename U>
		inline auto get_objects_as(const object_register::detail::registered_objects<T> &objects)
		{
			object_pointers<U> pointers;

			for (auto &object : objects)
			{
				if (auto ptr = dynamic_pointer_cast<U>(object.ptr); ptr)
					pointers.push_back(ptr);
			}

			return pointers;
		}
	} //object_register::detail


	template <typename T>
	class ObjectRegister
	{
		private:

			object_register::detail::registered_objects<T> objects_;

		public:

			//Default constructor
			ObjectRegister() = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all registered objects in this register
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() noexcept
			{
				return adaptors::ranges::DereferenceIterable<object_register::detail::registered_objects<T>&>{objects_};
			}

			//Returns an immutable range of all registered objects in this register
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const object_register::detail::registered_objects<T>&>{objects_};
			}


			//Returns a mutable range of all registered objects of type U in this register
			//This can be used directly with a range-based for loop
			template <typename U>
			[[nodiscard]] inline auto ObjectsOf() noexcept
			{
				static_assert(std::is_base_of_v<T, U>);
				return adaptors::ranges::Iterable<object_register::detail::object_pointers<U>>{
					object_register::detail::get_objects_as<T, U>(objects_)};
			}

			//Returns an immutable range of all registered objects of type U in this register
			//This can be used directly with a range-based for loop
			template <typename U>
			[[nodiscard]] inline auto ObjectsOf() const noexcept
			{
				static_assert(std::is_base_of_v<T, U>);
				return adaptors::ranges::Iterable<const object_register::detail::object_pointers<U>>{
					object_register::detail::get_objects_as<T, U>(objects_)};
			}


			/*
				Registering
			*/

			//Register an object to this register
			auto Register(NonOwningPtr<T> object)
			{
				auto iter =
					std::find_if(std::begin(objects_), std::end(objects_),
						[&](auto &x) noexcept
						{
							return x.ptr == object;
						});

				//Object not already registered
				if (iter == std::end(objects_))
				{
					objects_.emplace_back(std::nullopt, object);
					return true;
				}
				else
					return false;
			}

			//Register an object with the given name to this register
			auto Register(std::string name, NonOwningPtr<T> object)
			{
				if (!object_register::detail::get_object_by_name(name, objects_))
				{
					if (Register(object))
					{
						objects_.back().name = std::move(name);
						return true;
					}
				}

				return false;
			}


			/*
				Unregistering
			*/

			//Unregister an object from this register
			auto Unregister(T &object) noexcept
			{
				auto iter =
					std::find_if(std::begin(objects_), std::end(objects_),
						[&](auto &x) noexcept
						{
							return x.ptr.get() == &object;
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

			//Unregister an object with the given name from this register
			auto Unregister(std::string_view name) noexcept
			{
				auto iter =
					std::find_if(std::begin(objects_), std::end(objects_),
						[&](auto &x) noexcept
						{
							return x.name == name;
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

			//Unregister all objects from this register
			void UnregisterAll() noexcept
			{
				objects_.clear();
				objects_.shrink_to_fit();
			}


			/*
				Retrieving
			*/

			//Gets a pointer to an object with the given name
			//Returns nullptr if object could not be found
			[[nodiscard]] auto Get(std::string_view name) const noexcept
			{
				return object_register::detail::get_object_by_name(name, objects_);
			}

			//Gets a pointer to an object of type U with the given name
			//Returns nullptr if object of type U could not be found
			template <typename U>
			[[nodiscard]] auto GetAs(std::string_view name) const noexcept
			{
				static_assert(std::is_base_of_v<T, U>);
				return dynamic_pointer_cast<U>(Get(name));
			}

			//Gets a pointer to the first object of type U
			//Returns nullptr if object of type U could not be found
			template <typename U>
			[[nodiscard]] auto GetFirstOf() const noexcept
			{
				static_assert(std::is_base_of_v<T, U>);
				return object_register::detail::get_object_as<T, U>(objects_);
			}
	};
} //ion::managed

#endif