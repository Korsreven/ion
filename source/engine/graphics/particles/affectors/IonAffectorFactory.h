/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonAffectorFactory.h
-------------------------------------------
*/

#ifndef ION_AFFECTOR_FACTORY_H
#define ION_AFFECTOR_FACTORY_H

#include "IonAffector.h"
#include "unmanaged/IonObjectFactory.h"

namespace ion::graphics::particles::affectors
{
	namespace affector_factory::detail
	{
	} //affector_factory::detail


	class AffectorFactory : public unmanaged::ObjectFactory<Affector>
	{
		protected:

			//Can only be instantiated by derived
			AffectorFactory() = default;

			//Deleted copy constructor
			AffectorFactory(const AffectorFactory&) = delete;

			//Default move constructor
			AffectorFactory(AffectorFactory&&) = default;

		public:

			/*
				Operators
			*/

			//Deleted copy assignment
			AffectorFactory& operator=(const AffectorFactory&) = delete;

			//Move assignment
			AffectorFactory& operator=(AffectorFactory&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all affectors in this factory
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Affectors() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all affectors in this factory
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Affectors() const noexcept
			{
				return Objects();
			}


			/*
				Creating
			*/

			//Create an affector of type T with the given arguments
			template <typename T, typename... Args>
			auto& CreateAffector(Args &&...args)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto &affector = Create(std::forward<Args>(args)...);
				return static_cast<T&>(affector);
			}


			//Create an affector of type T as a copy of the given affector
			template <typename T>
			auto& CreateAffector(const T &affector_t)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto &affector = Create(affector_t);
				return static_cast<T&>(affector);
			}

			//Create an affector of type T by moving the given affector
			template <typename T>
			auto& CreateAffector(T &&affector_t)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto &affector = Create(std::move(affector_t));
				return static_cast<T&>(affector);
			}


			/*
				Removing
			*/

			//Clear all affectors from this factory
			void ClearAffectors() noexcept
			{
				Clear();
			}

			//Remove an affector from this factory
			auto RemoveAffector(Affector &affector) noexcept
			{
				return Remove(affector);
			}
	};
} //ion::graphics::particles::affectors

#endif