/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonSceneNodeAnimationGroup.cpp
-------------------------------------------
*/

#include "IonNodeAnimationGroup.h"

#include <utility>

#include "IonNodeAnimationManager.h"
#include "IonNodeAnimationTimeline.h"

namespace ion::graphics::scene::graph::animations
{

using namespace node_animation_group;

namespace node_animation_group::detail
{

} //node_animation_group::detail


NodeAnimationGroup::NodeAnimationGroup(std::string name) noexcept :
	ManagedObject{std::move(name)}
{
	//Empty
}


/*
	Modifiers
*/

void NodeAnimationGroup::Reset() noexcept
{
	for (auto &animation : AttachedAnimations())
		animation.Reset();
}


/*
	Elapse time
*/

void NodeAnimationGroup::Elapse(duration time, duration current_time, duration start_time) noexcept
{
	for (auto &animation : AttachedAnimations())
		animation.Elapse(time, current_time, start_time);
}


/*
	Playback
*/

NonOwningPtr<NodeAnimationTimeline> NodeAnimationGroup::Start(real playback_rate, bool running) noexcept
{
	if (auto owner = Owner(); owner)
	{
		auto timeline = owner->CreateTimeline(playback_rate, running);
		timeline->Attach(owner->GetAnimationGroup(*Name()));
		return timeline;
	}
	else
		return nullptr;
}


/*
	Node animations
*/

void NodeAnimationGroup::Attach(NonOwningPtr<NodeAnimation> node_animation, duration start_time, bool enable)
{
	if (node_animation)
	{
		attached_animations_.emplace_back(node_animation, start_time, enable);
		total_duration_ = std::max(total_duration_, start_time + node_animation->TotalDuration());
	}
}

void NodeAnimationGroup::DetachAll() noexcept
{
	attached_animations_.clear();
	attached_animations_.shrink_to_fit();
	total_duration_ = 0.0_sec;
}

} //ion::graphics::scene::graph::animations