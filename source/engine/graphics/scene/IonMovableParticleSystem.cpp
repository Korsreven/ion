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
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"

namespace ion::graphics::scene
{

using namespace movable_particle_system;
using namespace types::type_literals;

namespace movable_particle_system::detail
{

/*
	Graphics API
*/

void set_point_size(real size) noexcept
{
	glPointSize(size); //Set fixed point size
}


void enable_point_sprites() noexcept
{
	switch (gl::PointSprite_Support())
	{
		case gl::Extension::Core:
		glEnable(GL_POINT_SPRITE); //Enable point sprite
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE); //Enable varying point size
		glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE); //Enable sprite tex coords
		glPointParameterf(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT); //Set 0,0 to lower left
		break;

		case gl::Extension::ARB:
		glEnable(GL_POINT_SPRITE_ARB); //Enable point sprite
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB); //Enable varying point size
		glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE); //Enable sprite tex coords
		glPointParameterf(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT); //Set 0,0 to lower left
		break;
	}
}

void disable_point_sprites() noexcept
{
	switch (gl::PointSprite_Support())
	{
		case gl::Extension::Core:
		glPointParameterf(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT); //Set 0,0 back to upper left
		glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE); //Disable sprite tex coords
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE); //Disable varying point size
		glDisable(GL_POINT_SPRITE); //Disable point sprite
		break;

		case gl::Extension::ARB:
		glPointParameterf(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT); //Set 0,0 back to upper left
		glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE); //Disable sprite tex coords
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB); //Disable varying point size
		glDisable(GL_POINT_SPRITE_ARB); //Disable point sprite
		break;
	}
}

} //movable_particle_system::detail


//Private

void MovableParticleSystem::PrepareVertexStreams()
{
	if (std::size(particle_system_->Emitters()) > vertex_streams_.capacity())
		vertex_streams_.reserve(std::size(particle_system_->Emitters()));

	for (auto off = 0; auto &emitter : particle_system_->Emitters())
	{
		//Update existing stream
		if (off < std::ssize(vertex_streams_))
		{
			if (vertex_streams_[off].particle_quota != emitter.ParticleQuota())
			{
				vertex_streams_[off].particle_quota = emitter.ParticleQuota();
				reload_vertex_buffer_ = true;
			}

			if (vertex_streams_[off].emitter.get() != &emitter)
				vertex_streams_[off].emitter = particle_system_->GetEmitter(*emitter.Name());

			vertex_streams_[off].vertex_batch.VertexData({std::data(emitter.Particles()), std::ssize(emitter.Particles())});
			vertex_streams_[off].vertex_batch.ReloadData(); //Must reload data even if vertex data view (range) is unchanged
			vertex_streams_[off].vertex_batch.BatchMaterial(emitter.ParticleMaterial());
		}

		//New stream
		else
		{
			vertex_streams_.emplace_back(
				emitter.ParticleQuota(),
				particle_system_->GetEmitter(*emitter.Name()),
				render::vertex::VertexBatch{
					render::vertex::vertex_batch::VertexDrawMode::Points,
					detail::get_vertex_declaration(),
					{std::data(emitter.Particles()), std::ssize(emitter.Particles())},
					emitter.ParticleMaterial()
				}
			);

			reload_vertex_buffer_ = true;
		}

		++off;
	}

	//Erase unused vertex streams
	if (std::size(vertex_streams_) > std::size(particle_system_->Emitters()))
		vertex_streams_.erase(std::begin(vertex_streams_) + std::size(particle_system_->Emitters()), std::end(vertex_streams_));
}

//Public

MovableParticleSystem::MovableParticleSystem(NonOwningPtr<particles::ParticleSystem> particle_system, bool visible) :
	
	DrawableObject{visible},
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

	PrepareVertexStreams();

	if (reload_vertex_buffer_)
	{
		if (!vbo_)
			vbo_.emplace(render::vertex::vertex_buffer_object::VertexBufferUsage::Stream);

		if (vbo_ && *vbo_)
		{
			if (!std::empty(vertex_streams_))
			{
				auto size = 0;
				for (auto &stream : vertex_streams_)
					size += stream.particle_quota;

				vbo_->Reserve(size * sizeof(particles::Particle));

				for (auto offset = 0; auto &stream : vertex_streams_)
				{
					size = stream.particle_quota;
					stream.vertex_batch.VertexBuffer(vbo_->SubBuffer(offset * sizeof(particles::Particle), size * sizeof(particles::Particle)));
					offset += size;
				}
			}
		}

		reload_vertex_buffer_ = false;
	}

	for (auto &stream : vertex_streams_)
		stream.vertex_batch.Prepare();
}


void MovableParticleSystem::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (visible_ && particle_system_ && !std::empty(vertex_streams_))
	{
		auto use_shader = shader_program && shader_program->Owner() && shader_program->Handle();

		if (use_shader)
			shader_program->Owner()->ActivateShaderProgram(*shader_program);

		for (auto &stream : vertex_streams_)
		{
			auto &[min_size, max_size] = stream.emitter->ParticleSize();
			detail::set_point_size((min_size.X() + max_size.X()) * 0.5_r);

			detail::enable_point_sprites();
			stream.vertex_batch.Draw(shader_program);
			detail::disable_point_sprites();
		}

		if (use_shader)
			shader_program->Owner()->ActivateShaderProgram(*shader_program);
	}
}


/*
	Elapse time
*/

void MovableParticleSystem::Elapse(duration time) noexcept
{
	if (particle_system_)
	{
		particle_system_->Elapse(time);

		for (auto &stream : vertex_streams_)
			stream.vertex_batch.Elapse(time);
	}
}

} //ion::graphics::scene