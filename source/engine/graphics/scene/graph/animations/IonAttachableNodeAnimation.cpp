/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonAttachableNodeAnimation.cpp
-------------------------------------------
*/

#include "IonAttachableNodeAnimation.h"

#include "IonNodeAnimationTimeline.h"

namespace ion::graphics::scene::graph::animations
{

using namespace attachable_node_animation;

namespace attachable_node_animation::detail
{

} //attachable_node_animation::detail


//Private

void AttachableNodeAnimation::NotifyUpdate() noexcept
{
	if (auto owner = Owner(); owner)
		owner->Refresh();
}


//Public

AttachableNodeAnimation::AttachableNodeAnimation(NonOwningPtr<NodeAnimation> node_animation,
	duration start_time, bool enabled) noexcept :

	node_animation_{node_animation ? std::make_optional(*node_animation) : std::nullopt},
	initial_node_animation_{node_animation},

	start_time_{start_time},
	enabled_{enabled}
{
	//Empty
}


/*
	Modifiers
*/

void AttachableNodeAnimation::Reset() noexcept
{
	if (node_animation_)
		node_animation_->Reset();
}

void AttachableNodeAnimation::Revert()
{
	if (initial_node_animation_)
	{
		node_animation_ = *initial_node_animation_;
		NotifyUpdate();
	}
}


/*
	Elapse time
*/

void AttachableNodeAnimation::Elapse(duration time, duration current_time, duration start_time) noexcept
{
	if (enabled_ && node_animation_ && initial_node_animation_)
		node_animation_->Elapse(*initial_node_animation_, time, current_time, start_time + start_time_);
}

} //ion::graphics::scene::graph::animations