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
#include <tuple>
#include <vector>

#include "IonDrawableObject.h"
#include "adaptors/IonFlatMap.h"
#include "graphics/particles/IonParticleSystem.h"
#include "graphics/render/IonRenderPrimitive.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexDeclaration.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/utilities/IonAabb.h"
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
	class DrawableParticleSystem; //Forward declaration

	using utilities::Aabb;
	using utilities::Vector2;

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

				/**
					@name Events
					@{
				*/

				void RenderPassesChanged() noexcept override;
				Aabb GetAabb() const noexcept override;

				///@}

			public:

				DrawableParticleSystem *owner = nullptr;

				particle_emitter_primitive(NonOwningPtr<materials::Material> particle_material);
		};

		using particle_emitter_primitives = std::vector<OwningPtr<particle_emitter_primitive>>;

		using particle_emitter_pointers = std::vector<particles::Emitter*>;
		using particle_emitter_batches = adaptors::FlatMap<std::pair<real, materials::Material*>, particle_emitter_pointers>;


		inline auto get_vertex_declaration() noexcept
		{
			return
				render::vertex::VertexDeclaration
				{
					{
						{shaders::shader_layout::AttributeName::Vertex_Position,
							render::vertex::vertex_declaration::VertexElementType::Float3,
							offsetof(particles::ParticleRenderData, Position),
							sizeof(particles::ParticleRenderData)},

						{shaders::shader_layout::AttributeName::Vertex_Rotation,
							render::vertex::vertex_declaration::VertexElementType::Float1,
							offsetof(particles::ParticleRenderData, Rotation),
							sizeof(particles::ParticleRenderData)},

						{shaders::shader_layout::AttributeName::Vertex_PointSize,
							render::vertex::vertex_declaration::VertexElementType::Float1,
							offsetof(particles::ParticleRenderData, Size),
							sizeof(particles::ParticleRenderData)},

						{shaders::shader_layout::AttributeName::Vertex_Color,
							render::vertex::vertex_declaration::VertexElementType::Float4,
							offsetof(particles::ParticleRenderData, FillColor),
							sizeof(particles::ParticleRenderData)}
					},
					sizeof(particles::ParticleRenderData)
				};
		}

		vertex_metrics get_vertex_metrics(const render::vertex::VertexDeclaration &vertex_declaration) noexcept;
		std::tuple<Aabb, Obb, Sphere> generate_bounding_volumes(const particles::ParticleSystem &particle_system,
			const Vector2 &position, real rotation) noexcept;


		/**
			@name Rendering
			@{
		*/

		void get_emitter_primitives(const particle_emitter_batches &emitter_batches, particle_emitter_primitives &emitter_primitives);

		///@}
	} //drawable_particle_system::detail


	///@brief A class representing a drawable particle system that can be attached to a scene node
	class DrawableParticleSystem final : public DrawableObject
	{
		private:

			std::optional<particles::ParticleSystem> particle_system_;
			NonOwningPtr<particles::ParticleSystem> initial_particle_system_;

			drawable_particle_system::detail::vertex_metrics vertex_metrics_;
			drawable_particle_system::detail::particle_emitter_primitives emitter_primitives_;
			drawable_particle_system::detail::particle_emitter_batches emitter_batches_;

			bool reload_primitives_ = false;
			bool update_bounding_volumes_ = false;


			void ReloadPrimitives();
			void UpdateBatches();

		public:

			///@brief Constructs a new drawable particle system with the given name, particle system and visibility
			DrawableParticleSystem(std::optional<std::string> name,
				NonOwningPtr<particles::ParticleSystem> particle_system, bool visible = true);


			/**
				@name Modifiers
				@{
			*/

			///@brief Reverts to the initial particle system
			void Revert();

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns a mutable reference to the particle system
			[[nodiscard]] auto& Get() noexcept
			{
				reload_primitives_ = true; //Particle system could be changed
				return particle_system_;
			}

			///@brief Returns an immutable reference to the particle system
			[[nodiscard]] auto& Get() const noexcept
			{
				return particle_system_;
			}

			///@brief Returns an immutable reference to the particle system
			[[nodiscard]] auto& GetImmutable() const noexcept
			{
				return particle_system_;
			}

			///@}

			/**
				@name Preparing/drawing
				@{
			*/

			///@brief Prepares this particle system such that it is ready to be drawn
			///@details This function is typically called each frame
			void Prepare() override;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this particle system by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;

			///@}
	};
} //ion::graphics::scene

#endif