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

#include <cassert>
#include <cmath>
#include <type_traits>

#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonMovableObject.h"
#include "utilities/IonMath.h"

namespace ion::graphics::scene::graph
{

using namespace scene_node;
using namespace ion::utilities;

namespace scene_node::detail
{

bool scene_node_comparator::operator()(const SceneNode *x, const SceneNode *y) const noexcept
{
	return *x < *y;
}


Matrix4 make_transformation(const Vector3 &position, real rotation, const Vector2 &scaling) noexcept
{
	return Matrix4::Transformation(rotation, to_scaling3(scaling), position);
}

} //scene_node::detail


//Private

/*
	Updating
*/

void SceneNode::Update() const noexcept
{
	if (parent_node_)
	{
		if (parent_node_->need_update_)
			parent_node_->Update(); //Recursive
		else
		{
			derived_rotation_ = inherit_rotation_ ? rotation_ + parent_node_->derived_rotation_ : rotation_;
			derived_direction_ = initial_direction_.Deviant(derived_rotation_);
			derived_scaling_ = inherit_scaling_ ? scaling_ * parent_node_->derived_scaling_ : scaling_;

			switch (rotation_origin_)
			{
				case NodeRotationOrigin::Local:
				derived_position_ =
					position_ * detail::to_scaling3(parent_node_->derived_scaling_) + parent_node_->derived_position_;
				break;

				case NodeRotationOrigin::Parent:
				default:
				derived_position_ =
					(position_ * detail::to_scaling3(parent_node_->derived_scaling_)).Deviant(parent_node_->derived_rotation_) + parent_node_->derived_position_;
				break;
			}
		}
	}
	else
	{
		derived_position_ = position_;
		derived_direction_ = direction_;
		derived_rotation_ = rotation_;
		derived_scaling_ = scaling_;
	}

	need_update_ = false;
	need_z_update_ = false;
	transformation_out_of_date_ = true;
}

void SceneNode::UpdateZ() const noexcept
{
	if (parent_node_)
	{
		if (parent_node_->need_z_update_)
			parent_node_->UpdateZ(); //Recursive
		else
			derived_position_.Z(position_.Z() + parent_node_->derived_position_.Z());
	}
	else
		derived_position_.Z(position_.Z());
	
	need_z_update_ = false;
}


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

SceneNodes SceneNode::ExtractAll() noexcept
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
	detail::add_node(ordered_nodes_, this);
}

SceneNode::SceneNode(const Vector2 &initial_direction, bool visible) noexcept :

	initial_direction_{initial_direction},
	visible_{visible}
{
	detail::add_node(ordered_nodes_, this);
}

SceneNode::SceneNode(const Vector3 &position, const Vector2 &initial_direction, bool visible) noexcept :

	position_{position},
	initial_direction_{initial_direction},
	visible_{visible}
{
	detail::add_node(ordered_nodes_, this);
}


SceneNode::SceneNode(SceneNode &parent_node, bool visible) noexcept :

	visible_{visible},
	parent_node_{&parent_node}
{
	detail::add_node(RootNode().ordered_nodes_, this);
}

SceneNode::SceneNode(SceneNode &parent_node, const Vector2 &initial_direction, bool visible) noexcept :

	initial_direction_{initial_direction},
	visible_{visible},
	parent_node_{&parent_node}
{
	detail::add_node(RootNode().ordered_nodes_, this);
}

SceneNode::SceneNode(SceneNode &parent_node, const Vector3 &position, const Vector2 &initial_direction, bool visible) noexcept :
	
	position_{position},
	initial_direction_{initial_direction},
	visible_{visible},
	parent_node_{&parent_node}
{
	detail::add_node(RootNode().ordered_nodes_, this);
}


SceneNode::~SceneNode() noexcept
{
	DetachAllObjects();
	detail::remove_node(RootNode().ordered_nodes_, this);
}


/*
	Observers
*/

bool SceneNode::AxisAligned() const noexcept
{
	//Axis aligned when 0, +-90, +-180, +-270 and +-360 (half degree tolerance)
	return std::fmod(math::Round(math::ToDegrees(DerivedRotation())), 90.0_r) == 0.0_r;
}


/*
	Transformations
	Relative
*/

void SceneNode::Translate(const Vector3 &unit) noexcept
{
	if (unit != vector3::Zero)
		Position(position_ + unit.Deviant(unit.SignedAngleBetween(vector3::UnitY)));
}

void SceneNode::Translate(real unit) noexcept
{
	if (unit != 0.0_r)
		Position(position_ + direction_ * unit);
}

void SceneNode::Rotate(real angle) noexcept
{
	if (angle != 0.0_r)
		Rotation(rotation_ + angle);
}

void SceneNode::Scale(const Vector2 &unit) noexcept
{
	if (unit != vector2::Zero)
		Scaling(scaling_ + unit);
}


/*
	Transformations
	Absolute
*/

void SceneNode::LookAt(const Vector3 &position) noexcept
{
	Rotate((position - DerivedPosition()).SignedAngleBetween(DerivedDirection()));
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
	return child_nodes_.emplace_back(make_owning<SceneNode>(std::ref(*this), initial_direction, visible));;
}

NonOwningPtr<SceneNode> SceneNode::CreateChildNode(const Vector3 &position, const Vector2 &initial_direction, bool visible)
{
	return child_nodes_.emplace_back(make_owning<SceneNode>(std::ref(*this), position, initial_direction, visible));;
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


void SceneNode::Adopt(SceneNodes &scene_nodes)
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

void SceneNode::Adopt(SceneNodes &&scene_nodes)
{
	return Adopt(scene_nodes);
}


OwningPtr<SceneNode> SceneNode::Orphan(SceneNode &child_node) noexcept
{
	return Extract(child_node);
}

SceneNodes SceneNode::OrphanAll() noexcept
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

bool SceneNode::AttachObject(MovableObject &movable_object)
{
	if (!movable_object.ParentNode())
	{
		movable_object.ParentNode(this);
		attached_objects_.push_back(&movable_object);
		return true;
	}
	else
		return false;
}

bool SceneNode::DetachObject(MovableObject &movable_object) noexcept
{
	auto iter =
		std::find_if(std::begin(attached_objects_), std::end(attached_objects_),
			[&](auto &x) noexcept
			{
				return x == &movable_object;
			});

	//Movable object found
	if (iter != std::end(attached_objects_))
	{
		auto &root_node = RootNode();
		root_node.attached_cameras_.erase(*iter);
		root_node.attached_lights_.erase(*iter);

		(*iter)->ParentNode(nullptr);
		attached_objects_.erase(iter);
		return true;
	}
	else
		return false;
}

void SceneNode::DetachAllObjects() noexcept
{
	for (auto &mov_object : attached_objects_)
	{
		if (mov_object)
		{
			auto &root_node = RootNode();
			root_node.attached_cameras_.erase(mov_object);
			root_node.attached_lights_.erase(mov_object);

			mov_object->ParentNode(nullptr);
		}
	}

	attached_objects_.clear();
	attached_objects_.shrink_to_fit();
}


/*
	Attached objects
	Camera / light
*/

bool SceneNode::AttachObject(Camera &camera)
{
	if (AttachObject(static_cast<MovableObject&>(camera)))
	{
		RootNode().attached_cameras_.insert(&camera);
		return true;
	}
	else
		return false;
}

bool SceneNode::AttachObject(Light &light)
{
	if (AttachObject(static_cast<MovableObject&>(light)))
	{
		RootNode().attached_lights_.insert(&light);
		return true;
	}
	else
		return false;
}

} //ion::graphics::scene::graph