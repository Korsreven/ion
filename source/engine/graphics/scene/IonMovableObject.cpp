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
#include "graphics/utilities/IonMatrix3.h"

namespace ion::graphics::scene
{

using namespace movable_object;

namespace movable_object::detail
{
} //movable_object::detail


//Private

/*
	Updating
*/

void MovableObject::UpdateBoundingVolumes() const noexcept
{
	world_aabb_ = aabb_;
	world_obb_ = obb_;
	world_sphere_ = sphere_;

	if (parent_node_)
	{
		auto mat = graphics::utilities::Matrix3{parent_node_->FullTransformation()};
		world_aabb_.Transform(mat);
		world_obb_.Transform(mat);
		world_sphere_.Transform(mat);
	}

	need_bounding_update_ = false;
}


/*
	Helper functions
*/

void MovableObject::Detach() noexcept
{
	if (parent_node_)
	{
		parent_node_->DetachObject(*this);
		parent_node_ = nullptr;
	}
}


//Public

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
	Detach();
}


/*
	Rendering
*/

void MovableObject::Render() noexcept
{
	//Optional to override
}


/*
	Elapse time
*/

void MovableObject::Elapse([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}

} //ion::graphics::scene