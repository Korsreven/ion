/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimation.cpp
-------------------------------------------
*/

#include "IonNodeAnimation.h"

namespace ion::graphics::scene::graph::animations
{

using namespace node_animation;

namespace node_animation::detail
{

} //node_animation::detail


NodeAnimation::NodeAnimation(std::string name) noexcept :
	ManagedObject{std::move(name)}
{
	//Empty
}


/*
	Motions
*/

void NodeAnimation::AddRotation(real angle, duration total_duration, duration start_time,
	MotionTechniqueType technique) noexcept
{
	motions_.push_back(detail::rotating_motion{
		{start_time, 0.0_sec, total_duration},
		{technique, types::Cumulative<real>{angle}}});
}


void NodeAnimation::AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechniqueType technique) noexcept
{
	motions_.push_back(detail::scaling_motion{
		{start_time, 0.0_sec, total_duration},
		{technique, types::Cumulative<real>{unit.X()}},
		{technique, types::Cumulative<real>{unit.Y()}}});
}

void NodeAnimation::AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechniqueType technique_x, MotionTechniqueType technique_y) noexcept
{
	motions_.push_back(detail::scaling_motion{
		{start_time, 0.0_sec, total_duration},
		{technique_x, types::Cumulative<real>{unit.X()}},
		{technique_y, types::Cumulative<real>{unit.Y()}}});
}


void NodeAnimation::AddTranslation(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechniqueType technique) noexcept
{
	motions_.push_back(detail::translating_motion{
		{start_time, 0.0_sec, total_duration},
		{technique, types::Cumulative<real>{unit.X()}},
		{technique, types::Cumulative<real>{unit.Y()}}});
}

void NodeAnimation::AddTranslation(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechniqueType technique_x, MotionTechniqueType technique_y) noexcept
{
	motions_.push_back(detail::translating_motion{
		{start_time, 0.0_sec, total_duration},
		{technique_x, types::Cumulative<real>{unit.X()}},
		{technique_y, types::Cumulative<real>{unit.Y()}}});
}

} //ion::graphics::scene::graph::animations