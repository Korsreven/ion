/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph
File:	IonSceneNode.cpp
-------------------------------------------
*/

#include "IonSceneNode.h"

namespace ion::graphics::scene::graph
{

using namespace scene_node;

namespace scene_node::detail
{
} //scene_node::detail


SceneNode::SceneNode(bool visible) noexcept
{
	//Empty
}

SceneNode::SceneNode(const Vector2 &initial_direction, bool visible) noexcept
{
	//Empty
}

SceneNode::SceneNode(const Vector3 &position, const Vector2 &initial_direction, bool visible) noexcept
{
	//Empty
}


SceneNode::SceneNode(SceneNode &parent_node, bool visible)
{
	//Empty
}

SceneNode::SceneNode(SceneNode &parent_node, const Vector2 &initial_direction, bool visible)
{
	//Empty
}

SceneNode::SceneNode(SceneNode &parent_node, const Vector3 &position, const Vector2 &initial_direction, bool visible)
{
	//Empty
}


/*
	Transformations
	Relative
*/

void SceneNode::Translate(const Vector3 &unit) noexcept
{

}

void SceneNode::Translate(real unit) noexcept
{

}

void SceneNode::Rotate(real angle) noexcept
{

}

void SceneNode::Scale(const Vector2 &unit) noexcept
{

}


/*
	Transformations
	Absolute
*/

void SceneNode::LookAt(const Vector3 &position) noexcept
{

}


/*
	Child nodes
	Creating
*/

NonOwningPtr<SceneNode> SceneNode::CreateChildNode(bool visible)
{
	return child_nodes_.emplace_back(make_owning<SceneNode>(std::ref(*this), visible));
}

NonOwningPtr<SceneNode> SceneNode::CreateChildNode(const Vector2 &initial_direction, bool visible)
{
	return child_nodes_.emplace_back(make_owning<SceneNode>(std::ref(*this), initial_direction, visible));
}

NonOwningPtr<SceneNode> SceneNode::CreateChildNode(const Vector3 &position, const Vector2 &initial_direction, bool visible)
{
	return child_nodes_.emplace_back(make_owning<SceneNode>(std::ref(*this), position, initial_direction, visible));
}


/*
	Child nodes
	Take / release ownership
*/

NonOwningPtr<SceneNode> SceneNode::Adopt(OwningPtr<SceneNode> scene_node)
{
	return nullptr;
}

void SceneNode::Adopt(detail::scene_node_container &scene_nodes)
{

}


OwningPtr<SceneNode> SceneNode::Orphan(SceneNode &child_node) noexcept
{
	return nullptr;
}

detail::scene_node_container SceneNode::OrphanAll() noexcept
{
	return {};
}


/*
	Child nodes
	Removing
*/

void SceneNode::ClearChildNodes() noexcept
{

}

bool SceneNode::RemoveChildNode(SceneNode &child_node) noexcept
{
	return false;
}


/*
	Movable objects
*/

bool SceneNode::Attach(NonOwningPtr<MovableObject> movable_object)
{
	return false;
}

bool SceneNode::Detach(MovableObject &movable_object) noexcept
{
	return false;
}

void SceneNode::Detach() noexcept
{

}

} //ion::graphics::scene::graph