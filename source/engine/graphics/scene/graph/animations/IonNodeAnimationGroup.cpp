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
	Node animations
*/

void NodeAnimationGroup::Attach(NonOwningPtr<NodeAnimation> node_animation, duration start_time)
{
	attached_animations_.emplace_back(node_animation, start_time);
}

void NodeAnimationGroup::DetachAll() noexcept
{
	attached_animations_.clear();
	attached_animations_.shrink_to_fit();
}

} //ion::graphics::scene::graph::animations