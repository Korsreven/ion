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


	//Affector class that can affect single particles
	//This base class must support inheritance (open set of affectors)
	class Affector : public managed::ManagedObject<AffectorManager>
	{
		private:

			bool enable_ = true;

		protected:

			//Construct a new affector with the given name
			//Can only be instantiated by derived
			explicit Affector(std::string name);


			/*
				Affect particles
			*/

			//Elapse affector by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			virtual void DoAffect(affector::detail::particle_range particles, duration time) noexcept = 0;

		public:

			//Default copy constructor
			Affector(const Affector&) = default;

			//Default move constructor
			Affector(Affector&&) = default;

			//Default virtual destructor
			virtual ~Affector() = default;


			/*
				Operators
			*/

			//Default copy assignment
			Affector& operator=(const Affector&) = default;

			//Default move assignment
			Affector& operator=(Affector&&) = default;


			/*
				Cloning
			*/

			//Returns an owning ptr to a clone of this affector
			[[nodiscard]] virtual OwningPtr<Affector> Clone() const = 0;


			/*
				Modifiers
			*/

			//Enable the affector
			inline void Enable() noexcept
			{
				enable_ = true;
			}

			//Disable the affector
			inline void Disable() noexcept
			{
				enable_ = false;
			}


			/*
				Observers
			*/

			//Returns true if the affector is enabled
			[[nodiscard]] inline auto IsEnabled() const noexcept
			{
				return enable_;
			}


			/*
				Affect particles
			*/

			//Calls virtual function DoAffect if this affector is enabled
			//This function is typically called each frame, with the time in seconds since last frame
			void Affect(affector::detail::particle_range particles, duration time) noexcept;
	};
} //ion::graphics::particles::affectors

#endif