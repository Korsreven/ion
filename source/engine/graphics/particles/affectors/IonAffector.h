/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonAffector.h
-------------------------------------------
*/

#ifndef ION_AFFECTOR_H
#define ION_AFFECTOR_H

#include "adaptors/ranges/IonIterable.h"
#include "managed/IonManagedObject.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::particles
{
	class Particle;
}

namespace ion::graphics::particles::affectors
{
	class AffectorManager; //Forward declaration

	namespace affector::detail
	{
		template <typename T>
		using container_type = std::vector<T>;

		using particle_range = adaptors::ranges::Iterable<container_type<Particle>&>;
	} //affector::detail


	///@brief A class representing a generic affector that can affect single particles
	///@details This base class must support inheritance (open set of affectors)
	class Affector : public managed::ManagedObject<AffectorManager>
	{
		private:

			bool enabled_ = true;

		protected:

			///@brief Constructs a new affector with the given name
			///@details Can only be instantiated by derived
			explicit Affector(std::string name) noexcept;


			/**
				@name Affect particles
				@{
			*/

			///@brief Elapses affector by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			virtual void DoAffect(affector::detail::particle_range particles, duration time) noexcept = 0;

			///@}

		public:

			///@brief Default copy constructor
			Affector(const Affector&) = default;

			///@brief Default move constructor
			Affector(Affector&&) = default;

			///@brief Default virtual destructor
			virtual ~Affector() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Default copy assignment
			Affector& operator=(const Affector&) = default;

			///@brief Default move assignment
			Affector& operator=(Affector&&) = default;

			///@}

			/**
				@name Cloning
				@{
			*/

			///@brief Returns an owning ptr to a clone of this affector
			[[nodiscard]] virtual OwningPtr<Affector> Clone() const = 0;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Enables the affector
			inline void Enable() noexcept
			{
				enabled_ = true;
			}

			///@brief Disables the affector
			inline void Disable() noexcept
			{
				enabled_ = false;
			}

			///@brief Sets whether or not the affector is enabled
			inline void Enabled(bool enabled) noexcept
			{
				if (enabled)
					Enable();
				else
					Disable();
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns true if the affector is enabled
			[[nodiscard]] inline auto IsEnabled() const noexcept
			{
				return enabled_;
			}

			///@}

			/**
				@name Affect particles
				@{
			*/

			///@brief Calls virtual function DoAffect if this affector is enabled
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Affect(affector::detail::particle_range particles, duration time) noexcept;

			///@}
	};
} //ion::graphics::particles::affectors

#endif