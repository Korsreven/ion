/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonLinearForce.h
-------------------------------------------
*/

#ifndef ION_LINEAR_FORCE_H
#define ION_LINEAR_FORCE_H

#include <algorithm>

#include "IonAffector.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::particles::affectors
{
	using graphics::utilities::Vector2;

	namespace linear_force
	{
		enum class ForceType : bool
		{
			Add,
			Average
		};


		namespace detail
		{
			void affect_particles(affector::detail::particle_range particles, duration time,
								  ForceType type, const Vector2 &force) noexcept;
		} //detail
	} //linear_force


	//A class representing an affector that can add a linear force to single particles
	class LinearForce final : public Affector
	{
		private:

			linear_force::ForceType type_ = linear_force::ForceType::Add;
			Vector2 force_;

		protected:

			/*
				Affect particles
			*/

			//Elapse linear force by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void DoAffect(affector::detail::particle_range particles, duration time) noexcept override;

		public:

			//Construct a new linear force with the given name
			explicit LinearForce(std::string name);

			//Construct a new linear force with the given name, type and force
			LinearForce(std::string name, linear_force::ForceType type, const Vector2 &force);


			/*
				Cloning
			*/

			//Returns an owning ptr to a clone of this affector
			[[nodiscard]] OwningPtr<Affector> Clone() const override;


			/*
				Modifiers
			*/

			//Sets the force type of the linear force
			inline void Type(linear_force::ForceType type) noexcept
			{
				type_ = type;
			}

			//Sets the force of the linear force
			inline void Force(const Vector2 &force) noexcept
			{
				force_ = force;
			}


			/*
				Observers
			*/

			//Returns the force type of the linear force
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			//Returns the force of the linear force
			[[nodiscard]] inline auto Force() const noexcept
			{
				return force_;
			}
	};
} //ion::graphics::particles::affectors

#endif