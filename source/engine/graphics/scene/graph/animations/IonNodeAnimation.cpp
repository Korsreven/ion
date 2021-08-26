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

/*
	Actions
*/

bool execute_action(action &a, duration time, duration current_time, duration start_time) noexcept
{
	auto local_time = current_time - (start_time + a.time);

	//Execute action?
	if (auto reverse = time < 0.0_sec; reverse ?
		local_time <= 0.0_sec && local_time - time > 0.0_sec :
		local_time >= 0.0_sec && local_time - time < 0.0_sec)
	
		return true;

	else
		return false;
}


void execute_action(node_action &a, duration time, duration current_time, duration start_time, SceneNode &node) noexcept
{
	if (execute_action(static_cast<action&>(a), time, current_time, start_time))
	{
		auto reverse = time < 0.0_sec;
			//Execute the opposite action if in reverse

		switch (a.type)
		{
			//Visibility

			case NodeActionType::FlipVisibility:
			node.FlipVisibility(false);
			break;

			case NodeActionType::FlipVisibilityCascading:
			node.FlipVisibility();
			break;

			case NodeActionType::Show:
			node.Visible(reverse ? false : true, false);
			break;

			case NodeActionType::ShowCascading:
			node.Visible(reverse ? false : true);
			break;

			case NodeActionType::Hide:
			node.Visible(reverse ? true : false, false);
			break;

			case NodeActionType::HideCascading:
			node.Visible(reverse ? true : false);
			break;


			//Transformation

			case NodeActionType::InheritRotation:
			node.InheritRotation(reverse ? false : true);
			break;

			case NodeActionType::InheritScaling:
			node.InheritScaling(reverse ? false : true);
			break;

			case NodeActionType::DisinheritRotation:
			node.InheritRotation(reverse ? true : false);
			break;

			case NodeActionType::DisinheritScaling:
			node.InheritScaling(reverse ? true : false);
			break;
		}
	}
}

void execute_action(user_action &a, duration time, duration current_time, duration start_time, SceneNode &node) noexcept
{
	if (execute_action(static_cast<action&>(a), time, current_time, start_time))
	{
		node; //Todo
	}
}


/*
	Motions
*/

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
		local_time <= m.total_duration && local_time - time > 0.0_sec :
		local_time >= 0.0_sec && local_time - time < m.total_duration)
	{
		auto percent = local_time / m.total_duration;
		return std::clamp(percent, 0.0_r, 1.0_r);
	}
	else
		return local_time < 0.0_sec ? 0.0_r : 1.0_r;
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


//Private

duration NodeAnimation::RetrieveTotalDuration() const noexcept
{
	auto total_duration = 0.0_sec;

	for (auto &a : actions_)
		total_duration = std::visit(
			[&](auto &&a) noexcept
			{
				return std::max(total_duration_, a.time);
			}, a);

	for (auto &m : motions_)
		total_duration = std::visit(
			[&](auto &&m) noexcept
			{
				return std::max(total_duration_, m.start_time + m.total_duration);
			}, m);

	return total_duration;
}


//Public

NodeAnimation::NodeAnimation(std::string name) noexcept :
	ManagedObject{std::move(name)}
{
	//Empty
}


/*
	Modifiers
*/

void NodeAnimation::Reset() noexcept
{
	for (auto &m : motions_)
		std::visit([&](auto &&m) noexcept { m.Reset(); }, m);
}


/*
	Elapse time
*/

void NodeAnimation::Elapse(duration time, duration current_time, duration start_time, SceneNode &node) noexcept
{
	auto local_time = current_time - start_time;

	if (auto reverse = time < 0.0_sec; reverse ?
		local_time <= total_duration_ && local_time - time > 0.0_sec :
		local_time >= 0.0_sec && local_time - time < total_duration_)
	{
		auto percent = local_time / total_duration_;
		percent = std::clamp(percent, 0.0_r, 1.0_r);
		
		for (auto &a : actions_)
			std::visit([&](auto &&a) noexcept { execute_action(a, time, current_time, start_time, node); }, a);

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
	Actions
*/

void NodeAnimation::AddAction(node_animation::NodeActionType type, duration time)
{
	assert(time >= 0.0_sec);

	actions_.push_back(detail::node_action{{time},{type}});
	total_duration_ = std::max(total_duration_, time);
}

void NodeAnimation::ClearActions() noexcept
{
	actions_.clear();
	actions_.shrink_to_fit();
	total_duration_ = RetrieveTotalDuration();
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


void NodeAnimation::ClearMotions() noexcept
{
	motions_.clear();
	motions_.shrink_to_fit();
	total_duration_ = RetrieveTotalDuration();
}


/*
	Actions / motions
*/

void NodeAnimation::Clear() noexcept
{
	actions_.clear();
	actions_.shrink_to_fit();

	motions_.clear();
	motions_.shrink_to_fit();

	total_duration_ = 0.0_sec;
}

} //ion::graphics::scene::graph::animations