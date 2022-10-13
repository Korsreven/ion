/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonDrawableParticleSystem.h
-------------------------------------------
*/

#ifndef ION_MOVABLE_PARTICLE_SYSTEM_H
#define ION_MOVABLE_PARTICLE_SYSTEM_H

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "IonDrawableObject.h"
#include "graphics/particles/IonParticleSystem.h"
#include "graphics/render/IonRenderPrimitive.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexDeclaration.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics
{
	namespace materials
	{
		class Material; //Forward declaration
	}
}

namespace ion::graphics::scene
{
	class DrawableParticleSystem;

	namespace drawable_particle_system::detail
	{
		struct vertex_metrics final
		{
			int rotation_offset = 0;
			int point_size_offset = 0;
		};


		struct particle_emitter_primitive final : render::RenderPrimitive
		{
			protected:

				/*
					Events
				*/

				void RenderPassesChanged() noexcept override;

			public:

				DrawableParticleSystem *owner = nullptr;
				render::render_primitive::VertexContainer vertex_data;

				particle_emitter_primitive(NonOwningPtr<materials::Material> particle_material);
		};

		using particle_emitter_primitives = std::vector<OwningPtr<particle_emitter_primitive>>;


		inline auto get_vertex_declaration() noexcept
		{
			static const auto particle = particles::Particle{};
			return
				render::vertex::VertexDeclaration
				{
					{
						{shaders::shader_layout::AttributeName::Vertex_Position,
							render::vertex::vertex_declaration::VertexElementType::Float3,
							reinterpret_cast<const std::byte*>(particle.Position().Components()) - reinterpret_cast<const std::byte*>(&particle),
							sizeof(particles::Particle)},

						{shaders::shader_layout::AttributeName::Vertex_Rotation,
							render::vertex::vertex_declaration::VertexElementType::Float1,
							reinterpret_cast<const std::byte*>(&particle.Rotation()) - reinterpret_cast<const std::byte*>(&particle),
							sizeof(particles::Particle)},

						{shaders::shader_layout::AttributeName::Vertex_PointSize,
							render::vertex::vertex_declaration::VertexElementType::Float1,
							reinterpret_cast<const std::byte*>(particle.Size().Components()) - reinterpret_cast<const std::byte*>(&particle),
							sizeof(particles::Particle)},

						{shaders::shader_layout::AttributeName::Vertex_Color,
							render::vertex::vertex_declaration::VertexElementType::Float4,
							reinterpret_cast<const std::byte*>(particle.FillColor().Channels()) - reinterpret_cast<const std::byte*>(&particle),
							sizeof(particles::Particle)}
					},
					sizeof(particles::Particle)
				};
		}

		vertex_metrics get_vertex_metrics(const render::vertex::VertexDeclaration &vertex_declaration) noexcept;


		/*
			Rendering
		*/

		void apply_node_rotation(const vertex_metrics &metrics, real node_rotation, render::render_primitive::VertexContainer &data) noexcept;
		void apply_node_scaling(const vertex_metrics &metrics, const Vector2 &node_scaling, render::render_primitive::VertexContainer &data) noexcept;

		void get_emitter_primitives(const particles::ParticleSystem &particle_system, const vertex_metrics &metrics,
			real node_rotation, const Vector2 &node_scaling, particle_emitter_primitives &emitter_primitives);
	} //drawable_particle_system::detail


	//A class representing a drawable particle system that can be attached to a scene node
	class DrawableParticleSystem final : public DrawableObject
	{
		private:

			std::optional<particles::ParticleSystem> particle_system_;
			NonOwningPtr<particles::ParticleSystem> initial_particle_system_;

			drawable_particle_system::detail::vertex_metrics vertex_metrics_;
			drawable_particle_system::detail::particle_emitter_primitives emitter_primitives_;

			bool reload_primitives_ = false;
			bool update_bounding_volumes_ = false;


			void ReloadPrimitives();

		public:

			//Construct a new drawable particle system with the given name, particle system and visibility
			DrawableParticleSystem(std::optional<std::string> name,
				NonOwningPtr<particles::ParticleSystem> particle_system, bool visible = true);


			/*
				Modifiers
			*/

			//Revert to the initial particle system
			void Revert();


			/*
				Observers
			*/

			//Returns a mutable reference to the particle system
			[[nodiscard]] auto& Get() noexcept
			{
				reload_primitives_ = true; //Particle system could be changed
				return particle_system_;
			}

			//Returns an immutable reference to the particle system
			[[nodiscard]] auto& Get() const noexcept
			{
				return particle_system_;
			}

			//Returns an immutable reference to the particle system
			[[nodiscard]] auto& GetImmutable() const noexcept
			{
				return particle_system_;
			}


			/*
				Preparing / drawing
			*/

			//Prepare this particle system such that it is ready to be drawn
			//This function is typically called each frame
			void Prepare() override;


			/*
				Elapse time
			*/

			//Elapse the total time for this particle system by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;
	};
} //ion::graphics::scene

#endif