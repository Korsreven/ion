/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimationManager.cpp
-------------------------------------------
*/

#include "IonNodeAnimationManager.h"
#include <type_traits>

namespace ion::graphics::scene::graph::animations
{

using namespace node_animation_manager;

namespace node_animation_manager::detail
{

} //node_animation_manager::detail


NodeAnimationManager::NodeAnimationManager(SceneNode &scene_node) noexcept :
	parent_node_{scene_node}
{
	//Empty
}


/*
	Node animations
	Creating
*/

NonOwningPtr<NodeAnimation> NodeAnimationManager::CreateAnimation(std::string name)
{
	return NodeAnimationBase::Create(std::move(name));
}


NonOwningPtr<NodeAnimation> NodeAnimationManager::CreateAnimation(const NodeAnimation &node_animation)
{
	return NodeAnimationBase::Create(node_animation);
}

NonOwningPtr<NodeAnimation> NodeAnimationManager::CreateAnimation(NodeAnimation &&node_animation)
{
	return NodeAnimationBase::Create(std::move(node_animation));
}


/*
	Node animations
	Retrieving
*/

NonOwningPtr<NodeAnimation> NodeAnimationManager::GetAnimation(std::string_view name) noexcept
{
	return NodeAnimationBase::Get(name);
}

NonOwningPtr<const NodeAnimation> NodeAnimationManager::GetAnimation(std::string_view name) const noexcept
{
	return NodeAnimationBase::Get(name);
}


/*
	Node animations
	Removing
*/

void NodeAnimationManager::ClearAnimations() noexcept
{
	NodeAnimationBase::Clear();
}

bool NodeAnimationManager::RemoveAnimation(NodeAnimation &node_animation) noexcept
{
	return NodeAnimationBase::Remove(node_animation);
}

bool NodeAnimationManager::RemoveAnimation(std::string_view name) noexcept
{
	return NodeAnimationBase::Remove(name);
}


/*
	Animation groups
	Creating
*/

NonOwningPtr<NodeAnimationGroup> NodeAnimationManager::CreateAnimationGroup(std::string name)
{
	return NodeAnimationGroupBase::Create(std::move(name));
}


NonOwningPtr<NodeAnimationGroup> NodeAnimationManager::CreateAnimationGroup(const NodeAnimationGroup &node_animation_group)
{
	return NodeAnimationGroupBase::Create(node_animation_group);
}

NonOwningPtr<NodeAnimationGroup> NodeAnimationManager::CreateAnimationGroup(NodeAnimationGroup &&node_animation_group)
{
	return NodeAnimationGroupBase::Create(std::move(node_animation_group));
}


/*
	Node animation groups
	Retrieving
*/

NonOwningPtr<NodeAnimationGroup> NodeAnimationManager::GetAnimationGroup(std::string_view name) noexcept
{
	return NodeAnimationGroupBase::Get(name);
}

 NonOwningPtr<const NodeAnimationGroup> NodeAnimationManager::GetAnimationGroup(std::string_view name) const noexcept
 {
	return NodeAnimationGroupBase::Get(name);
 }


/*
	Node animation groups
	Removing
*/

void NodeAnimationManager::ClearAnimationGroups() noexcept
{
	NodeAnimationGroupBase::Clear();
}

bool NodeAnimationManager::RemoveAnimationGroup(NodeAnimationGroup &node_animation_group) noexcept
{
	return NodeAnimationGroupBase::Remove(node_animation_group);
}

bool NodeAnimationManager::RemoveAnimationGroup(std::string_view name) noexcept
{
	return NodeAnimationGroupBase::Remove(name);
}


/*
	Elapse time
*/

void NodeAnimationManager::Elapse(duration time) noexcept
{
	time;
}

} //ion::graphics::scene::graph::animations