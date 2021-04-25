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
#include <cassert>

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
		auto node = std::move(*iter); //Extend lifetime
		node->parent_node_ = nullptr;
		child_nodes_.erase(iter);	
		return std::move(node);
	}
	else
		return nullptr;
}

scene_node::detail::scene_node_container SceneNode::ExtractAll() noexcept
{
	//Something to remove
	if (!std::empty(child_nodes_))
	{
		auto nodes = std::move(child_nodes_); //Extend lifetime
		
		for (auto &node : nodes)
			node->parent_node_ = nullptr;

		child_nodes_.shrink_to_fit();	
		return std::move(nodes);
	}
	else
		return {};
}


//Public

SceneNode::SceneNode(bool visible) noexcept :
	visible_{visible}
{
	//Empty
}

SceneNode::SceneNode(const Vector2 &initial_direction, bool visible) noexcept :

	initial_direction_{initial_direction},
	visible_{visible}
{
	//Empty
}

SceneNode::SceneNode(const Vector3 &position, const Vector2 &initial_direction, bool visible) noexcept :

	position_{position},
	initial_direction_{initial_direction},
	visible_{visible}
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
	auto &node = child_nodes_.emplace_back(make_owning<SceneNode>(visible));
	node->parent_node_ = this;
	return node;
}

NonOwningPtr<SceneNode> SceneNode::CreateChildNode(const Vector2 &initial_direction, bool visible)
{
	auto &node = child_nodes_.emplace_back(make_owning<SceneNode>(initial_direction, visible));
	node->parent_node_ = this;
	return node;
}

NonOwningPtr<SceneNode> SceneNode::CreateChildNode(const Vector3 &position, const Vector2 &initial_direction, bool visible)
{
	auto &node = child_nodes_.emplace_back(make_owning<SceneNode>(position, initial_direction, visible));
	node->parent_node_ = this;
	return node;
}


/*
	Child nodes
	Take / release ownership
*/

NonOwningPtr<SceneNode> SceneNode::Adopt(OwningPtr<SceneNode> &scene_node)
{
	assert(scene_node);

	auto &node = child_nodes_.emplace_back(std::move(scene_node));
	node->parent_node_ = this;
	node->NotifyUpdate();
	return node;
}

NonOwningPtr<SceneNode> SceneNode::Adopt(OwningPtr<SceneNode> &&scene_node)
{
	return Adopt(scene_node);
}


void SceneNode::Adopt(detail::scene_node_container &scene_nodes)
{
	for (auto iter = std::begin(scene_nodes); iter != std::end(scene_nodes);)
	{
		if (*iter)
		{
			Adopt(*iter);
			iter = scene_nodes.erase(iter);
		}
		else
			++iter;
	}
}

void SceneNode::Adopt(detail::scene_node_container &&scene_nodes)
{
	return Adopt(scene_nodes);
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
	auto node = Extract(child_node);
	return !!node;
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