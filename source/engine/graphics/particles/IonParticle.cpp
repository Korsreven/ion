/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles
File:	IonParticle.cpp
-------------------------------------------
*/

#include "IonParticle.h"

namespace ion::graphics::particles
{

//ParticleRenderData

ParticleRenderData::ParticleRenderData(const Vector3 &position, real rotation, const Vector2 &size, const Color &color) noexcept :

	Position{position},
	Rotation{rotation},
	Size{size},
	FillColor{color}
{
	//Empty
}


//ParticleSimulationData

ParticleSimulationData::ParticleSimulationData(const Vector2 &direction, real mass, duration lifetime, const Vector2 &initial_direction) noexcept :

	Direction{direction},
	Mass{mass},
	Lifetime{lifetime},
	InitialDirection{initial_direction}
{
	//Empty
}


//Particles

int Particles::AddParticle() noexcept
{
	auto off = std::ssize(render_data_);
	render_data_.emplace_back();
	simulation_data_.emplace_back();
	return off;
}

int Particles::AddParticle(const Vector3 &position, const Vector2 &direction,
						   const Vector2 &size, real mass, const Color &color,
						   duration lifetime, const Vector2 &initial_direction) noexcept
{
	auto off = std::ssize(render_data_);
	render_data_.emplace_back(position, direction.SignedAngleBetween(initial_direction), size, color);
	simulation_data_.emplace_back(direction, mass, lifetime, initial_direction);
	return off;
}

} //ion::graphics::particles