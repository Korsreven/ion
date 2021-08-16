/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimationTimeline.cpp
-------------------------------------------
*/

#include "IonNodeAnimationTimeline.h"
#include <type_traits>

namespace ion::graphics::scene::graph::animations
{

using namespace node_animation_timeline;

namespace node_animation_timeline::detail
{

} //node_animation_timeline::detail


NodeAnimationTimeline::NodeAnimationTimeline(real playback_rate, bool running) noexcept :
	
	playback_rate_{playback_rate},
	running_{running}
{
	//Empty
}

NodeAnimationTimeline::NodeAnimationTimeline(std::string name, real playback_rate, bool running) noexcept :

	managed::ManagedObject<NodeAnimationManager>{std::move(name)},
	playback_rate_{playback_rate},
	running_{running}
{
	//Empty
}


/*
	Modifiers
*/

void NodeAnimationTimeline::RepeatCount(std::optional<int> repeat_count) noexcept
{
	if (repeat_count)
	{
		auto current_cycle = repeat_count_ ? repeat_count_->first : 0;
		repeat_count_.emplace(current_cycle, *repeat_count < current_cycle ? current_cycle : *repeat_count);
	}
	else
		repeat_count_.reset();
}


/*
	Playback
*/

void NodeAnimationTimeline::Start() noexcept
{
	running_ = true;
}

void NodeAnimationTimeline::Stop() noexcept
{
	running_ = false;
}

void NodeAnimationTimeline::Reset() noexcept
{
	running_ = false;
	Revert();
}

void NodeAnimationTimeline::Restart() noexcept
{
	Reset();
	Start();
}


void NodeAnimationTimeline::Revert(duration total_duration)
{
	total_duration;
	reverse_ = true;
}


/*
	Node animations
	Attaching
*/

NonOwningPtr<AttachableNodeAnimation> NodeAnimationTimeline::Attach(NonOwningPtr<NodeAnimation> node_animation,
	duration start_time, bool enable)
{
	return NodeAnimationBase::Create(node_animation, start_time, enable);
}


/*
	Node animations
	Detaching
*/

void NodeAnimationTimeline::DetachAllAnimations() noexcept
{
	NodeAnimationBase::Clear();
}

bool NodeAnimationTimeline::DetachAnimation(AttachableNodeAnimation &node_animation) noexcept
{
	return NodeAnimationBase::Remove(node_animation);
}


/*
	Animation groups
	Attaching
*/

NonOwningPtr<AttachableNodeAnimationGroup> NodeAnimationTimeline::Attach(NonOwningPtr<NodeAnimationGroup> node_animation_group,
	duration start_time, bool enable)
{
	return NodeAnimationGroupBase::Create(node_animation_group, start_time, enable);
}


/*
	Node animation groups
	Detaching
*/

void NodeAnimationTimeline::DetachAllAnimationGroups() noexcept
{
	NodeAnimationGroupBase::Clear();
}

bool NodeAnimationTimeline::DetachAnimationGroup(AttachableNodeAnimationGroup &node_animation_group) noexcept
{
	return NodeAnimationGroupBase::Remove(node_animation_group);
}


/*
	Elapse time
*/

void NodeAnimationTimeline::Elapse(duration time) noexcept
{
	time;
}

} //ion::graphics::scene::graph::animations