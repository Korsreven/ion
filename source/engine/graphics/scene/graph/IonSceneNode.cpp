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


SceneNode::SceneNode(bool visible)
{
	//Empty
}

SceneNode::SceneNode(const Vector2 &initial_direction, bool visible)
{
	//Empty
}

SceneNode::SceneNode(const Vector3 &position, const Vector2 &initial_direction, bool visible)
{
	//Empty
}


SceneNode::SceneNode(NonOwningPtr<SceneNode> parent_node, bool visible)
{
	//Empty
}

SceneNode::SceneNode(NonOwningPtr<SceneNode> parent_node, const Vector2 &initial_direction, bool visible)
{
	//Empty
}

SceneNode::SceneNode(NonOwningPtr<SceneNode> parent_node, const Vector3 &position, const Vector2 &initial_direction, bool visible)
{
	//Empty
}


/*
	Child nodes
	Creating
*/




/*
	Child nodes
	Take / release ownership
*/




/*
	Child nodes
	Removing
*/




/*
	Movable objects
*/



} //ion::graphics::scene::graph