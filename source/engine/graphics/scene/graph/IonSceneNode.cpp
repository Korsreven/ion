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

#include <algorithm>
#include "graphics/scene/IonMovableObject.h"

namespace ion::graphics::scene::graph
{

using namespace scene_node;

namespace scene_node::detail
{
} //scene_node::detail


//Private

/*
	Removing
*/

OwningPtr<SceneNode> SceneNode::Extract(SceneNode &child_node) noexcept
{
	auto iter =
		std::find_if(std::begin(child_nodes_), std::end(child_nodes_),
			[&](auto &x) noexcept
			{
				return x.get() == &child_node;
			});

	//Child node found
	if (iter != std::end(child_nodes_))
	{
		auto node_ptr = std::move(*iter); //Extend lifetime
		child_nodes_.erase(iter);
		return std::move(node_ptr);
	}
	else
		return nullptr;
}

scene_node::detail::scene_node_container SceneNode::ExtractAll() noexcept
{
	//Something to remove
	if (!std::empty(child_nodes_))
	{
		auto scene_nodes = std::move(child_nodes_); //Extend lifetime
		child_nodes_.shrink_to_fit();	
		return std::move(scene_nodes);
	}
	else
		return {};
}


//Public

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


SceneNode::~SceneNode() noexcept
{
	DetachAll();
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
	return Extract(child_node);
}

detail::scene_node_container SceneNode::OrphanAll() noexcept
{
	return ExtractAll();
}


/*
	Child nodes
	Removing
*/

void SceneNode::ClearChildNodes() noexcept
{
	ExtractAll();
}

bool SceneNode::RemoveChildNode(SceneNode &child_node) noexcept
{
	auto ptr = Extract(child_node);
	return !!ptr;
}


/*
	Movable objects
*/

bool SceneNode::Attach(NonOwningPtr<MovableObject> movable_object)
{
	if (movable_object && !movable_object->ParentNode())
	{
		movable_object->ParentNode(this);
		movable_objects_.push_back(movable_object);
		return true;
	}
	else
		return false;
}

bool SceneNode::Detach(MovableObject &movable_object) noexcept
{
	auto iter =
		std::find_if(std::begin(movable_objects_), std::end(movable_objects_),
			[&](auto &x) noexcept
			{
				return x.get() == &movable_object;
			});

	//Movable object found
	if (iter != std::end(movable_objects_))
	{
		(*iter)->ParentNode(nullptr);
		movable_objects_.erase(iter);
		return true;
	}
	else
		return false;
}

void SceneNode::DetachAll() noexcept
{
	for (auto &mov_object : movable_objects_)
	{
		if (mov_object)
			mov_object->ParentNode(nullptr);
	}

	movable_objects_.clear();
	movable_objects_.shrink_to_fit();
}

} //ion::graphics::scene::graph