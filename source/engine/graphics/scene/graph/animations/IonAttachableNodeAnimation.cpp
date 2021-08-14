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

namespace ion::graphics::scene::graph::animations
{

using namespace attachable_node_animation;

namespace attachable_node_animation::detail
{

} //attachable_node_animation::detail


AttachableNodeAnimation::AttachableNodeAnimation(NonOwningPtr<NodeAnimation> node_animation, duration start_time) noexcept :

	node_animation_{node_animation ? std::make_optional(*node_animation) : std::nullopt},
	initial_node_animation_{node_animation},

	start_time_{start_time}
{
	//Empty
}


/*
	Modifiers
*/

void AttachableNodeAnimation::Revert()
{
	if (initial_node_animation_)
		node_animation_ = *initial_node_animation_;
}


/*
	Elapse time
*/

void AttachableNodeAnimation::Elapse(duration time) noexcept
{
	time;
}

} //ion::graphics::scene::graph::animations