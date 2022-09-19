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
#include "graphics/particles/IonParticleSystem.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexBufferObject.h"
#include "graphics/render/vertex/IonVertexDeclaration.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics
{
	namespace shaders
	{
		class ShaderProgram; //Forward declaration
	}
}

namespace ion::graphics::scene
{
	namespace drawable_particle_system::detail
	{
		struct emitter_vertex_stream
		{
			int particle_quota = 0;
			NonOwningPtr<particles::Emitter> emitter;
			render::vertex::VertexBatch vertex_batch;
		};

		using emitter_vertex_streams = std::vector<emitter_vertex_stream>;


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

		std::tuple<Aabb, Obb, Sphere> generate_bounding_volumes(const particles::ParticleSystem &particle_system) noexcept;


		/*
			Graphics API
		*/

		void set_point_size(real size) noexcept;

		void enable_point_sprites() noexcept;
		void disable_point_sprites() noexcept;
	} //drawable_particle_system::detail


	//A class representing a drawable particle system that can be attached to a scene node
	class DrawableParticleSystem final : public DrawableObject
	{
		private:

			std::optional<particles::ParticleSystem> particle_system_;
			NonOwningPtr<particles::ParticleSystem> initial_particle_system_;

			drawable_particle_system::detail::emitter_vertex_streams vertex_streams_;
			std::optional<render::vertex::VertexBufferObject> vbo_;

			bool reload_vertex_buffer_ = false;
			bool update_bounding_volumes_ = false;


			void ReloadVertexStreams();

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


			//Return the vertex buffer this particle system uses
			[[nodiscard]] inline auto& VertexBuffer() const noexcept
			{
				return vbo_;
			}


			/*
				Preparing / drawing
			*/

			//Prepare this particle system such that it is ready to be drawn
			//This is called once regardless of passes
			void Prepare() noexcept override;

			//Draw this particle system with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept override;


			/*
				Elapse time
			*/

			//Elapse the total time for this particle system by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;
	};
} //ion::graphics::scene

#endif