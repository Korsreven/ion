/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonEmitterFactory.h
-------------------------------------------
*/

#ifndef ION_EMITTER_FACTORY_H
#define ION_EMITTER_FACTORY_H

#include "IonEmitter.h"
#include "unmanaged/IonObjectFactory.h"

namespace ion::graphics::particles
{
	namespace emitter_factory::detail
	{
	} //emitter_factory::detail


	class EmitterFactory : public unmanaged::ObjectFactory<Emitter>
	{
		protected:

			//Can only be instantiated by derived
			EmitterFactory() = default;

			//Deleted copy constructor
			EmitterFactory(const EmitterFactory&) = delete;

			//Default move constructor
			EmitterFactory(EmitterFactory&&) = default;

		public:

			/*
				Operators
			*/

			//Deleted copy assignment
			EmitterFactory& operator=(const EmitterFactory&) = delete;

			//Move assignment
			EmitterFactory& operator=(EmitterFactory&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all emitters in this factory
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Emitters() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all emitters in this factory
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Emitters() const noexcept
			{
				return Objects();
			}


			/*
				Creating
			*/

			//Create an emitter with the given arguments
			template <typename... Args>
			auto& CreateEmitter(Args &&...args)
			{
				return Create(std::forward<Args>(args)...);
			}

			//Create an emitter by moving the given emitter
			auto& CreateEmitter(Emitter &&emitter)
			{
				return Create(std::move(emitter));
			}


			/*
				Removing
			*/

			//Clear all emitters from this factory
			void ClearEmitters() noexcept
			{
				Clear();
			}

			//Remove an emitter from this factory
			auto RemoveEmitter(Emitter &Emitter) noexcept
			{
				return Remove(Emitter);
			}
	};
} //ion::graphics::particles

#endif