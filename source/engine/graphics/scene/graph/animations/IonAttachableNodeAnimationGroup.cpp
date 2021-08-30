/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonAttachableNodeAnimationGroup.cpp
-------------------------------------------
*/

#include "IonAttachableNodeAnimationGroup.h"

#include "IonNodeAnimationTimeline.h"

namespace ion::graphics::scene::graph::animations
{

using namespace attachable_node_animation_group;

namespace attachable_node_animation_group::detail
{

} //attachable_node_animation_group::detail


//Private

void AttachableNodeAnimationGroup::NotifyUpdate() noexcept
{
	if (auto owner = Owner(); owner)
		owner->Refresh();
}


//Public

AttachableNodeAnimationGroup::AttachableNodeAnimationGroup(NonOwningPtr<NodeAnimationGroup> node_animation_group,
	duration start_time, bool enable) noexcept :

	node_animation_group_{node_animation_group ? std::make_optional(*node_animation_group) : std::nullopt},
	initial_node_animation_group_{node_animation_group},

	start_time_{start_time},
	enable_{enable}
{
	//Empty
}


/*
	Modifiers
*/

void AttachableNodeAnimationGroup::Reset() noexcept
{
	if (node_animation_group_)
		node_animation_group_->Reset();
}

void AttachableNodeAnimationGroup::Revert()
{
	if (initial_node_animation_group_)
	{
		node_animation_group_ = *initial_node_animation_group_;
		NotifyUpdate();
	}
}

} //ion::graphics::scene::graph::animations