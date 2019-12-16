/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonParticleSystem.h
-------------------------------------------
*/

#ifndef ION_PARTICLE_SYSTEM_H
#define ION_PARTICLE_SYSTEM_H

#include <memory>
#include <vector>

#include "IonEmitter.h"
#include "affectors/IonAffectorInterface.h"
#include "events/listeners/IonResourceListener.h"


//TEMP begin, remove when texture has been implemented
#include "resources/IonResource.h"
#include "resources/IonResourceManager.h"

namespace ion::graphics::resources
{
	struct TextureManger;

	struct Texture : ion::resources::Resource<TextureManger>
	{	
	};

	struct TextureManger : ion::resources::ResourceManager<TextureManger, Texture>
	{
	};
}
//TEMP end

namespace ion::graphics::particles
{
	namespace particle_system
	{
		enum class ParticlePrimitive : bool
		{
			Point,
			Rectangle
		};

		namespace detail
		{
			template <typename T>
			using container_type = std::vector<std::unique_ptr<T>>; //Owning
		} //detail
	} //particle_system


	//A particle system class that can contain one ore more emitters and affectors
	class ParticleSystem final :
		public affectors::AffectorInterface,
		protected events::listeners::ResourceListener<resources::Texture>
	{
		private:

			particle_system::ParticlePrimitive particle_primitive_ = particle_system::ParticlePrimitive::Point;
			particle_system::detail::container_type<Emitter> emitters_;

		protected:

			/*
				Events
			*/

			//See ResourceListener::ResourceCreated for more details
			void ResourceCreated(resources::Texture&) noexcept override;

			//See ResourceListener::ResourceRemoved for more details
			void ResourceRemoved(resources::Texture &texture) noexcept override;

			//See Listener::Unsubscribed for more details
			void Unsubscribed(events::listeners::ListenerInterface<events::listeners::ResourceListener<resources::Texture>>&) noexcept override;

		public:

			//Default constructor
			ParticleSystem() = default;

			//Construct a new particle system with the given particle primitive
			ParticleSystem(particle_system::ParticlePrimitive particle_primitive) noexcept;


			/*
				Modifiers
			*/

			//Sets the render primitive used by this particle system to the given value
			inline void RenderPrimitive(particle_system::ParticlePrimitive particle_primitive) noexcept
			{
				particle_primitive_ = particle_primitive;
			}


			/*
				Observers
			*/

			//Returns the render primitive used by this particle system
			[[nodiscard]] inline auto RenderPrimitive() const noexcept
			{
				return particle_primitive_;
			}


			/*
				Common functions for controlling the particle system
			*/

			//Starts or resumes all emitters
			void StartAll() noexcept;

			//Stops all emitters
			void StopAll() noexcept;

			//Stops and reset all emitters
			void ResetAll() noexcept;

			//Stops, resets and starts all emitters
			void RestartAll() noexcept;


			/*
				Elapse time
			*/

			//Elapse particle system by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;


			/*
				Creating
			*/

			//Create an emitter
			Emitter& CreateEmitter();

			//Create an emitter as a copy of the given emitter
			Emitter& CreateEmitter(Emitter emitter);

			//Create an emitter with the given arguments
			template <typename... Args>
			auto& CreateEmitter(Args &&...args)
			{
				auto &emitter = emitters_.emplace_back(
					std::make_unique<Emitter>(std::forward<Args>(args)...));
				return *emitter;
			}


			/*
				Removing
			*/

			//Clear all emitters from this particle system
			void ClearEmitters() noexcept;

			//Remove an emitters from this particle system
			bool RemoveEmitter(Emitter &emitter) noexcept;


			/*
				Particles
			*/

			//Returns true if this emitter has any active particles
			[[nodiscard]] bool HasActiveParticles() const noexcept;


			/*
				Ranges
			*/

			//Returns a mutable range of all emitters in this particle system
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Emitters() noexcept
			{
				return adaptors::ranges::DereferenceIterable<particle_system::detail::container_type<Emitter>&>{emitters_};
			}

			//Returns an immutable range of all emitters in this particle system
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Emitters() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const particle_system::detail::container_type<Emitter>&>{emitters_};
			}
	};
} //ion::graphics::particles

#endif