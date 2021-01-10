/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonEmitterManager.h
-------------------------------------------
*/

#ifndef ION_EMITTER_MANAGER_H
#define ION_EMITTER_MANAGER_H

#include "IonEmitter.h"
#include "managed/IonObjectManager.h"

namespace ion::graphics::particles
{
	namespace emitter_manager::detail
	{
	} //emitter_manager::detail


	class EmitterManager : public managed::ObjectManager<Emitter, EmitterManager>
	{
		protected:

			//Can only be instantiated by derived
			EmitterManager() = default;

			//Deleted copy constructor
			EmitterManager(const EmitterManager&) = delete;

			//Default move constructor
			EmitterManager(EmitterManager&&) = default;

		public:

			/*
				Operators
			*/

			//Deleted copy assignment
			EmitterManager& operator=(const EmitterManager&) = delete;

			//Move assignment
			EmitterManager& operator=(EmitterManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all emitters in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Emitters() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all emitters in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Emitters() const noexcept
			{
				return Objects();
			}


			/*
				Creating
			*/

			//Create an emitter with the given name and arguments
			template <typename... Args>
			auto& CreateEmitter(std::string name, Args &&...args)
			{
				return Create(std::move(name), std::forward<Args>(args)...);
			}

			//Create an emitter by moving the given emitter
			auto& CreateEmitter(Emitter &&emitter)
			{
				return Create(std::move(emitter));
			}


			/*
				Removing
			*/

			//Clear all emitters from this manager
			void ClearEmitters() noexcept
			{
				Clear();
			}

			//Remove an emitter from this manager
			auto RemoveEmitter(Emitter &Emitter) noexcept
			{
				return Remove(Emitter);
			}
	};
} //ion::graphics::particles

#endif