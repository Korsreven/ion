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


/*
	Node animations
	Attaching
*/

NonOwningPtr<AttachableNodeAnimation> NodeAnimationTimeline::Attach(NonOwningPtr<NodeAnimation> node_animation, duration start_time)
{
	return NodeAnimationBase::Create(node_animation, start_time);
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

NonOwningPtr<AttachableNodeAnimationGroup> NodeAnimationTimeline::Attach(NonOwningPtr<NodeAnimationGroup> node_animation_group, duration start_time)
{
	return NodeAnimationGroupBase::Create(node_animation_group, start_time);
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