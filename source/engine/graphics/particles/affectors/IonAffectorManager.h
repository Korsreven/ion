/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonAffectorManager.h
-------------------------------------------
*/

#ifndef ION_AFFECTOR_MANAGER_H
#define ION_AFFECTOR_MANAGER_H

#include "IonAffector.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::particles::affectors
{
	namespace affector_manager::detail
	{
	} //affector_manager::detail


	class AffectorManager : public managed::ObjectManager<Affector, AffectorManager>
	{
		protected:

			//Can only be instantiated by derived
			AffectorManager() = default;

			//Deleted copy constructor
			AffectorManager(const AffectorManager&) = delete;

			//Default move constructor
			AffectorManager(AffectorManager&&) = default;

		public:

			/*
				Operators
			*/

			//Deleted copy assignment
			AffectorManager& operator=(const AffectorManager&) = delete;

			//Move assignment
			AffectorManager& operator=(AffectorManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all affectors in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Affectors() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all affectors in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Affectors() const noexcept
			{
				return Objects();
			}


			/*
				Affectors
				Creating
			*/

			//Create an affector of type T with the given name and arguments
			template <typename T, typename... Args>
			auto CreateAffector(std::string name, Args &&...args)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto ptr = Create(std::move(name), std::forward<Args>(args)...);
				return static_pointer_cast<T>(ptr);
			}


			//Create an affector of type T as a copy of the given affector
			template <typename T>
			auto CreateAffector(const T &affector_t)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto ptr = Create(affector_t);
				return static_pointer_cast<T>(ptr);
			}

			//Create an affector of type T by moving the given affector
			template <typename T>
			auto CreateAffector(T &&affector_t)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto ptr = Create(std::move(affector_t));
				return static_pointer_cast<T>(ptr);
			}


			/*
				Affectors
				Retrieving
			*/

			//Gets a pointer to a mutable affector with the given name
			//Returns nullptr if affector could not be found
			[[nodiscard]] auto GetAffector(std::string_view name) noexcept
			{
				return Get(name);
			}

			//Gets a pointer to an immutable affector with the given name
			//Returns nullptr if affector could not be found
			[[nodiscard]] auto GetAffector(std::string_view name) const noexcept
			{
				return Get(name);
			}


			/*
				Affectors
				Removing
			*/

			//Clear all affectors from this manager
			void ClearAffectors() noexcept
			{
				Clear();
			}

			//Remove an affector from this manager
			auto RemoveAffector(Affector &affector) noexcept
			{
				return Remove(affector);
			}
	};
} //ion::graphics::particles::affectors

#endif