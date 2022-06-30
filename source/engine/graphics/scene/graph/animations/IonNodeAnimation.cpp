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
#include "adaptors/ranges/IonIterable.h"
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


void elapse_action(NodeAnimation &animation, node_action &a, duration time, duration current_time, duration start_time) noexcept
{
	if (execute_action(a, time, current_time, start_time))
	{
		auto reverse = time < 0.0_sec;
			//Execute the opposite action if in reverse

		if (auto owner = animation.Owner(); owner)
		{
			auto &node = owner->ParentNode();

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
}

void elapse_action(NodeAnimation &animation, user_action &a, duration time, duration current_time, duration start_time) noexcept
{
	if (execute_action(a, time, current_time, start_time))
	{
		if (auto reverse = time < 0.0_sec; reverse)
			//Execute the opposite action if in reverse
		{
			if (a.on_execute_opposite)
				(*a.on_execute_opposite)(animation, a.user_data);
		}
		else
			a.on_execute(animation, a.user_data);
	}
}


/*
	Motions
*/

real move_amount(moving_amount &amount, real percent) noexcept
{
	using namespace ion::utilities;

	auto current =
		[&]() noexcept
		{
			if (amount.user_technique)
				return (*amount.user_technique)(amount.target, percent);

			else
			{
				switch (amount.technique)
				{
					case MotionTechniqueType::Cubic:
					return amount.target * cubic(percent, 0.0_r, 10.0_r);

					case MotionTechniqueType::Exponential:
					return amount.target * exp(percent, 0.0_r, 10.0_r);

					case MotionTechniqueType::Logarithmic:
					return amount.target * log(percent, 1.0_r, 10.0_r);

					case MotionTechniqueType::Sigmoid:
					return amount.target * sigmoid(percent, -5.0_r, 5.0_r);

					case MotionTechniqueType::Sinh:
					return amount.target * sinh(percent, -2.5_r, 2.5_r);

					case MotionTechniqueType::Tanh:
					return amount.target * tanh(percent, -2.5_r, 2.5_r);

					case MotionTechniqueType::Linear:
					default:
					return amount.target * percent;
				}
			}
		}();

	auto delta = current - amount.current;
	amount.current = current;
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

void elapse_motion(NodeAnimation &animation, rotating_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);
		
	if (auto angle = move_amount(m.angle, percent);
		angle != 0.0_r)
	{
		if (auto owner = animation.Owner(); owner)
			owner->ParentNode().Rotate(angle);
	}
}

void elapse_motion(NodeAnimation &animation, scaling_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);

	if (auto unit = Vector2{move_amount(m.x, percent), move_amount(m.y, percent)};
		unit != vector2::Zero)
	{
		if (auto owner = animation.Owner(); owner)
			owner->ParentNode().Scale(unit);
	}
}

void elapse_motion(NodeAnimation &animation, translating_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);

	if (auto unit = Vector3{move_amount(m.x, percent), move_amount(m.y, percent), move_amount(m.z, percent)};
		unit != vector3::Zero)
	{
		if (auto owner = animation.Owner(); owner)
			owner->ParentNode().Translate(unit);
	}
}

void elapse_motion(NodeAnimation &animation, user_motion &m, duration time, duration current_time, duration start_time) noexcept
{
	auto percent = elapse_motion(m, time, current_time, start_time);
	
	if (auto amount = move_amount(m.amount, percent);
		amount != 0.0_r)

		m.on_elapse(animation, amount);
}

} //node_animation::detail


//Private

duration NodeAnimation::RetrieveTotalDuration() const noexcept
{
	auto total_duration = 0.0_sec;

	if (!std::empty(actions_))
		total_duration = std::visit(
			[&](auto &&a) noexcept
			{
				return std::max(total_duration_, a.time);
			}, actions_.back());

	if (!std::empty(motions_))
		total_duration = std::visit(
			[&](auto &&m) noexcept
			{
				return std::max(total_duration_, m.start_time + m.total_duration);
			}, motions_.back());

	return total_duration;
}


//Public

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

void NodeAnimation::Elapse(duration time, duration current_time, duration start_time) noexcept
{
	Elapse(*this, time, current_time, start_time);
}

void NodeAnimation::Elapse(NodeAnimation &animation, duration time, duration current_time, duration start_time) noexcept
{
	auto local_time = current_time - start_time;

	if (auto reverse = time < 0.0_sec; reverse ?
		local_time <= total_duration_ && local_time - time > 0.0_sec :
		local_time >= 0.0_sec && local_time - time < total_duration_)
	{
		auto percent = local_time / total_duration_;
		percent = std::clamp(percent, 0.0_r, 1.0_r);
		
		//Start
		if (on_start_ &&
			local_time >= 0.0_sec && local_time - time < 0.0_sec)
			(*on_start_)(animation);


		//Reverse
		if (reverse)
		{
			for (auto &a : adaptors::ranges::ReverseIterable<decltype(actions_)&>(actions_))
				std::visit([&](auto &&a) noexcept { elapse_action(animation, a, time, current_time, start_time); }, a);

			for (auto &m : adaptors::ranges::ReverseIterable<decltype(motions_)&>(motions_))
				std::visit([&](auto &&m) noexcept { elapse_motion(animation, m, time, current_time, start_time); }, m);
		}
		else //Forward
		{
			for (auto &a : actions_)
				std::visit([&](auto &&a) noexcept { elapse_action(animation, a, time, current_time, start_time); }, a);

			for (auto &m : motions_)
				std::visit([&](auto &&m) noexcept { elapse_motion(animation, m, time, current_time, start_time); }, m);
		}


		//Finish
		if (on_finish_ &&
			local_time >= total_duration_ && local_time - time < total_duration_)
			(*on_finish_)(animation);

		//Finish revert
		if (on_finish_revert_ &&
			local_time <= 0.0_sec && local_time - time > 0.0_sec)
			(*on_finish_revert_)(animation);
	}
}


/*
	Playback
*/

NonOwningPtr<NodeAnimationTimeline> NodeAnimation::Start(real playback_rate, bool running)
{
	if (auto owner = Owner(); owner)
	{
		auto timeline = owner->CreateTimeline({}, playback_rate, running);
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

	auto a = detail::node_action{
		{time}, type};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);		

	total_duration_ = std::max(total_duration_, time);
}


void NodeAnimation::AddAction(events::Callback<void, NodeAnimation&, std::any&> on_execute,
	duration time, std::any user_data)
{
	assert(time >= 0.0_sec);

	auto a = detail::user_action{
		{time},
		std::move(user_data),
		on_execute};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

	total_duration_ = std::max(total_duration_, time);
}

void NodeAnimation::AddAction(events::Callback<void, NodeAnimation&, std::any&> on_execute,
	events::Callback<void, NodeAnimation&, std::any&> on_execute_opposite,
	duration time, std::any user_data)
{
	assert(time >= 0.0_sec);

	auto a = detail::user_action{
		{time},
		std::move(user_data),
		on_execute, on_execute_opposite};

	//Insert sorted
	actions_.insert(
		std::upper_bound(std::begin(actions_), std::end(actions_), a,
			detail::action_types_comparator{}),
		a);

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

void NodeAnimation::AddMotion(real target_amount, duration total_duration,
	events::Callback<void, NodeAnimation&, real> on_elapse, duration start_time,
	node_animation::MotionTechnique technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::user_motion{
		{start_time, total_duration},
		{0.0_r, target_amount, technique.type, technique.method},
		on_elapse};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddRotation(real angle, duration total_duration, duration start_time,
	MotionTechnique technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::rotating_motion{
		{start_time, total_duration},
		{0.0_r, angle, technique.type, technique.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechnique technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::scaling_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique.type, technique.method},
		{0.0_r, unit.Y(), technique.type, technique.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}

void NodeAnimation::AddScaling(const Vector2 &unit, duration total_duration, duration start_time,
	MotionTechnique technique_x, MotionTechnique technique_y)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::scaling_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique_x.type, technique_x.method},
		{0.0_r, unit.Y(), technique_y.type, technique_y.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}


void NodeAnimation::AddTranslation(const Vector3 &unit, duration total_duration, duration start_time,
	MotionTechnique technique)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::translating_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique.type, technique.method},
		{0.0_r, unit.Y(), technique.type, technique.method},
		{0.0_r, unit.Z(), technique.type, technique.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

	total_duration_ = std::max(total_duration_, start_time + total_duration);
}

void NodeAnimation::AddTranslation(const Vector3 &unit, duration total_duration, duration start_time,
	MotionTechnique technique_x, MotionTechnique technique_y, MotionTechnique technique_z)
{
	assert(total_duration > 0.0_sec);
	assert(start_time >= 0.0_sec);

	auto m = detail::translating_motion{
		{start_time, total_duration},
		{0.0_r, unit.X(), technique_x.type, technique_x.method},
		{0.0_r, unit.Y(), technique_y.type, technique_y.method},
		{0.0_r, unit.Z(), technique_z.type, technique_z.method}};

	//Insert sorted
	motions_.insert(
		std::upper_bound(std::begin(motions_), std::end(motions_), m,
			detail::motion_types_comparator{}),
		m);

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