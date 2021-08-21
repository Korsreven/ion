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

#include <algorithm>
#include <cassert>
#include <utility>

#include "IonNodeAnimationManager.h"
#include "IonNodeAnimationTimeline.h"
#include "graphics/scene/graph/IonSceneNode.h"

namespace ion::graphics::scene::graph::animations
{

using namespace node_animation;

namespace node_animation::detail
{

real move_amount(moving_amount &amount, real percent) noexcept
{
	auto delta = 0.0_r;

	switch (amount.technique)
	{
		case MotionTechniqueType::Linear:
		default:
		{
			auto current = amount.target * percent;
			delta = current - amount.current;
			amount.current = current;
			break;
		}
	}

	return delta;
}


real elapse_motion(motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto local_time = current_time - (start_time + m.start_time);

	if (auto reverse = time < 0.0_sec; reverse ?
		local_time <= m.total_duration :
		local_time >= 0.0_sec)
	{
		auto percent = local_time / m.total_duration;
		return std::clamp(percent, 0.0_r, 1.0_r);
	}
	else
		return reverse ? 1.0_r : 0.0_r;
}

void elapse_motion(rotating_motion &m, duration time, duration current_time, duration start_time, SceneNode &node) noexcept
{
	auto percent = elapse_motion(static_cast<motion&>(m), time, current_time, start_time);
		
	if (auto angle = move_amount(m.angle, percent);
		angle != 0.0_r)

		node.Rotate(angle);
}

void elapse_motion(scaling_motion &m, duration time, duration current_time, duration start_time, SceneNode &node) noexcept
{
	auto percent = elapse_motion(static_cast<motion&>(m), time, current_time, start_time);

	if (auto unit = Vector2{move_amount(m.x, percent), move_amount(m.y, percent)};
		unit != vector2::Zero)

		node.Scale(unit);
}

void elapse_motion(translating_motion &m, duration time, duration current_time, duration start_time, SceneNode &node) noexcept
{
	auto percent = elapse_motion(static_cast<motion&>(m), time, current_time, start_time);

	if (auto unit = Vector3{move_amount(m.x, percent), move_amount(m.y, percent), move_amount(m.z, percent)};
		unit != vector3::Zero)

		node.Translate(unit);
}

} //node_animation::detail


NodeAnimation::NodeAnimation(std::string name) noexcept :
	ManagedObject{std::move(name)}
{
	//Empty
}


/*
	Elapse time
*/

void NodeAnimation::Elapse(duration time, duration current_time, duration start_time, SceneNode &node) noexcept
{
	auto local_time = current_time - start_time;

	if (auto reverse = time < 0.0_sec; reverse ?
		local_time <= total_duration_ :
		local_time >= 0.0_sec)
	{
		auto percent = local_time / total_duration_;
		percent = std::clamp(percent, 0.0_r, 1.0_r);
		
		for (auto &m : motions_)
			std::visit([&](auto &&m) noexcept { elapse_motion(m, time, current_time, start_time, node); }, m);
	}
}


/*
	Playback
*/

NonOwningPtr<NodeAnimationTimeline> NodeAnimation::Start(real playback_rate, bool running)
{
	if (auto owner = Owner(); owner)
	{
		auto timeline = owner->CreateTimeline(playback_rate, running);
		timeline->Attach(owner->GetAnimation(*Name()));
		return timeline;
	}
	else
		return nullptr;
}


/*
	Motions
*/

void NodeAnimation::AddRotation(real angle, duration total_duration, duration start_time,
	MotionTechniqueType technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	motions_.push_back(detail::rotating_motion{
		{start_time, total_duration},
		{0.0_r, angle, technique}});

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechniqueType technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	motions_.push_back(detail::scaling_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique},
		{0.0_r, unit.Y(), technique}});

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}

void NodeAnimation::AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechniqueType technique_x, MotionTechniqueType technique_y)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	motions_.push_back(detail::scaling_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique_x},
		{0.0_r, unit.Y(), technique_y}});

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddTranslation(const Vector3 &unit, duration total_duration, duration start_time,
	MotionTechniqueType technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	motions_.push_back(detail::translating_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique},
		{0.0_r, unit.Y(), technique},
		{0.0_r, unit.Z(), technique}});

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}

void NodeAnimation::AddTranslation(const Vector3 &unit, duration total_duration, duration start_time,
	MotionTechniqueType technique_x, MotionTechniqueType technique_y, MotionTechniqueType technique_z)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	motions_.push_back(detail::translating_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique_x},
		{0.0_r, unit.Y(), technique_y},
		{0.0_r, unit.Z(), technique_z}});

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


/*
	Motions
	Removing
*/

void NodeAnimation::ClearMotions() noexcept
{
	motions_.clear();
	motions_.shrink_to_fit();
	total_duration_ = 0.0_sec;
}

} //ion::graphics::scene::graph::animations