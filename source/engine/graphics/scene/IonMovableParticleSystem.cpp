/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableParticleSystem.cpp
-------------------------------------------
*/

#include "IonMovableParticleSystem.h"

#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::scene
{

using namespace movable_particle_system;

namespace movable_particle_system::detail
{
} //movable_particle_system::detail


//Private

void MovableParticleSystem::PrepareEmitterVertexData()
{
	for (auto off = 0; auto &emitter : particle_system_->Emitters())
	{
		//New emitter
		if (off == std::ssize(emitters_))
		{
			emitters_.emplace_back(
				emitter.ParticleQuota(),
				render::vertex::VertexBatch{
					render::vertex::vertex_batch::VertexDrawMode::Points,
					detail::get_vertex_declaration(),
					{std::data(emitter.Particles()), std::ssize(emitter.Particles())},
					emitter.ParticleMaterial()
				}
			);

			reload_vertex_buffer_ = true;
		}

		//Update existing emitter
		else
		{
			if (emitters_[off].particle_quota != emitter.ParticleQuota())
			{
				emitters_[off].particle_quota = emitter.ParticleQuota();
				reload_vertex_buffer_ = true;
			}

			emitters_[off].vertex_batch.VertexData({std::data(emitter.Particles()), std::ssize(emitter.Particles())});
			emitters_[off].vertex_batch.BatchMaterial(emitter.ParticleMaterial());
		}

		++off;
	}
}


//Public

MovableParticleSystem::MovableParticleSystem(NonOwningPtr<particles::ParticleSystem> particle_system) :
	
	particle_system_{particle_system ? std::make_optional(particle_system->Clone()) : std::nullopt},
	initial_particle_system_{particle_system}
{
	//Empty
}


/*
	Modifiers
*/

void MovableParticleSystem::Revert()
{
	if (initial_particle_system_)
		particle_system_ = initial_particle_system_->Clone();
}


/*
	Preparing / drawing
*/

void MovableParticleSystem::Prepare() noexcept
{
	if (!particle_system_)
		return;

	PrepareEmitterVertexData();

	if (reload_vertex_buffer_)
	{
		if (!vbo_)
			vbo_.emplace(render::vertex::vertex_buffer_object::VertexBufferUsage::Stream);

		if (vbo_ && *vbo_)
		{
			if (!std::empty(emitters_))
			{
				auto size = 0;
				for (auto &emitter : emitters_)
					size += emitter.particle_quota;

				vbo_->Reserve(size * sizeof(particles::Particle));

				for (auto offset = 0; auto &emitter : emitters_)
				{
					size = emitter.particle_quota;
					emitter.vertex_batch.VertexBuffer(vbo_->SubBuffer(offset * sizeof(particles::Particle), size * sizeof(particles::Particle)));
					offset += size;
				}
			}
		}

		reload_vertex_buffer_ = false;
	}

	for (auto &emitter : emitters_)
		emitter.vertex_batch.Prepare();
}


void MovableParticleSystem::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (particle_system_)
	{
		for (auto &emitter : emitters_)
		{
			//auto &[min_size, max_size] = emitter.ParticleSize();
			//glPointSize((min_size.X() + max_size.X()) * 0.5f);
			emitter.vertex_batch.Draw(shader_program);
		}
	}
}


/*
	Elapse time
*/

void MovableParticleSystem::Elapse(duration time) noexcept
{
	if (particle_system_)
		particle_system_->Elapse(time);
}

} //ion::graphics::scene