/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonMovableObject.cpp
-------------------------------------------
*/

#include "IonMovableObject.h"

#include "graph/IonSceneNode.h"

namespace ion::graphics::scene
{

using namespace movable_object;

namespace movable_object::detail
{
} //movable_object::detail


MovableObject::MovableObject(bool visible) :
	visible_{visible}
{
	//Empty
}

MovableObject::MovableObject(std::string name, bool visible) :

	managed::ManagedObject<SceneManager>{std::move(name)},
	visible_{visible}
{
	//Empty
}

MovableObject::MovableObject(const MovableObject &rhs) noexcept :

	managed::ManagedObject<SceneManager>{rhs},
	parent_node_{nullptr} //A copy of a movable object has no parent node
{
	//Empty
}

MovableObject::~MovableObject() noexcept
{
	if (parent_node_)
		parent_node_->DetachObject(*this);
}

} //ion::graphics::scene