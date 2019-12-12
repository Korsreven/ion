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
#include "types/IonTypes.h"

namespace ion::graphics::particles
{
	class Particle;
}

namespace ion::graphics::particles::affectors
{
	namespace affector::detail
	{
		template <typename T>
		using container_type = std::vector<T>;

		using particle_range = adaptors::ranges::Iterable<container_type<Particle>&>;
	} //affector::detail


	//Affector class that can affect single particles
	//This base class must support inheritance (open set of affectors)
	class Affector
	{
		private:

			bool enable_ = true;

		protected:

			//Can only be instantiated by derived
			Affector() = default;


			/*
				Affect particles
			*/

			//Elapse affector by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			virtual void DoAffect(affector::detail::particle_range particles, duration time) noexcept = 0;

		public:

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