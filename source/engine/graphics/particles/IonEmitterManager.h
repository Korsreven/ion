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
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::particles
{
	namespace emitter_manager::detail
	{
	} //emitter_manager::detail


	///@brief A class that manages and stores emitters
	class EmitterManager : public managed::ObjectManager<Emitter, EmitterManager>
	{
		protected:

			///@brief Can only be instantiated by derived
			EmitterManager() = default;

			///@brief Deleted copy constructor
			EmitterManager(const EmitterManager&) = delete;

			///@brief Default move constructor
			EmitterManager(EmitterManager&&) = default;

		public:

			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			EmitterManager& operator=(const EmitterManager&) = delete;

			///@brief Default move assignment
			EmitterManager& operator=(EmitterManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all emitters in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Emitters() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all emitters in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Emitters() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Emitters
				Creating
				@{
			*/

			///@brief Creates an emitter with the given name
			NonOwningPtr<Emitter> CreateEmitter(std::string name);

			///@brief Creates an emitter with the given name and initial values
			NonOwningPtr<Emitter> CreateEmitter(std::string name, emitter::EmitterType type, const Vector2 &position, const Vector2 &direction,
				const Vector2 &size, const Vector2 &inner_size, real emission_rate, real emission_angle,
				std::optional<duration> emission_duration, int particle_quota = 100);


			///@brief Creates an emitter by moving the given emitter
			NonOwningPtr<Emitter> CreateEmitter(Emitter &&emitter);

			///@}

			/**
				@name Emitters
				Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable emitter with the given name
			///@details Returns nullptr if emitter could not be found
			[[nodiscard]] NonOwningPtr<Emitter> GetEmitter(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable emitter with the given name
			///@details Returns nullptr if emitter could not be found
			[[nodiscard]] NonOwningPtr<const Emitter> GetEmitter(std::string_view name) const noexcept;

			///@}

			/**
				@name Emitters
				Removing
				@{
			*/

			///@brief Clears all emitters from this manager
			void ClearEmitters() noexcept;

			///@brief Removes an emitter from this manager
			bool RemoveEmitter(Emitter &Emitter) noexcept;

			///@}
	};
} //ion::graphics::particles

#endif