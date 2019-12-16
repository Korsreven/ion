/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonAffectorInterface.h
-------------------------------------------
*/

#ifndef ION_AFFECTOR_INTERFACE_H
#define ION_AFFECTOR_INTERFACE_H

#include <memory>
#include <type_traits>
#include <vector>

#include "IonAffector.h"
#include "adaptors/ranges/IonDereferenceIterable.h"

namespace ion::graphics::particles::affectors
{
	namespace affector_interface::detail
	{
		template <typename T>
		using container_type = std::vector<std::unique_ptr<T>>; //Owning
	} //affector_interface::detail


	class AffectorInterface
	{
		private:

			affector_interface::detail::container_type<Affector> affectors_;

		protected:

			//Can only be instantiated by derived
			AffectorInterface() = default;

		public:

			/*
				Ranges
			*/

			//Returns a mutable range of all affectors in this interface
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Affectors() noexcept
			{
				return adaptors::ranges::DereferenceIterable<affector_interface::detail::container_type<Affector>&>{affectors_};
			}

			//Returns an immutable range of all affectors in this interface
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Affectors() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const affector_interface::detail::container_type<Affector>&>{affectors_};
			}


			/*
				Creating
			*/

			//Create an affector of type T
			template <typename T>
			auto& CreateAffector()
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto &affector = affectors_.emplace_back(
					std::make_unique<T>());
				return *affector;
			}

			//Create an affector of type T as a copy of the given affector
			template <typename T>
			auto& CreateAffector(T &&affector)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto &ref = affectors_.emplace_back(
					std::make_unique<T>(std::forward<T>(affector)));
				return *ref;
			}

			//Create an affector of type T with the given arguments
			template <typename T, typename... Args>
			auto& CreateAffector(Args &&...args)
			{
				static_assert(std::is_base_of_v<Affector, T>);

				auto &affector = affectors_.emplace_back(
					std::make_unique<T>(std::forward<Args>(args)...));
				return *affector;
			}


			/*
				Removing
			*/

			//Clear all affectors from this interface
			void ClearAffectors() noexcept;

			//Remove an affector from this interface
			bool RemoveAffector(Affector &affector) noexcept;
	};
} //ion::graphics::particles::affectors

#endif