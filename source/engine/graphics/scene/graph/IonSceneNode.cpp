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

bool node_comparator::operator()(const SceneNode *x, const SceneNode *y) const noexcept
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
	Notifying
*/

void SceneNode::NotifyRemoved() noexcept
{
	removed_ = true;

	for (auto &child_node : child_nodes_)
		child_node->NotifyRemoved(); //Recursive
}


void SceneNode::NotifyUpdate() noexcept
{
	need_update_ = true;

	for (auto &child_node : child_nodes_)
		child_node->NotifyUpdate(); //Recursive
}

void SceneNode::NotifyUpdateZ() noexcept
{
	need_z_update_ = true;

	for (auto &child_node : child_nodes_)
		child_node->NotifyUpdateZ(); //Recursive
}


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


/*
	Helper functions
*/

void SceneNode::AddNode(detail::node_container &dest_nodes)
{
	detail::add_node(dest_nodes, this);
}

void SceneNode::MoveNodes(detail::node_container &dest_nodes, detail::node_container &source_nodes)
{
	detail::move_nodes(dest_nodes, source_nodes);
}

void SceneNode::RemoveNode(detail::node_container &from_nodes) noexcept
{
	detail::remove_node(from_nodes, this);
}

void SceneNode::RemoveNodes(detail::node_container &from_nodes, detail::node_container &nodes) noexcept
{
	detail::remove_nodes(from_nodes, nodes);
}

void SceneNode::GatherNodes(detail::node_container &nodes)
{
	nodes.push_back(this);

	for (auto &child_node : child_nodes_)
		child_node->GatherNodes(nodes); //Recursive
}


void SceneNode::AddCamera(detail::camera_container &dest_cameras, Camera &camera)
{
	detail::add_object(dest_cameras, &camera);
}

void SceneNode::MoveCameras(detail::camera_container &dest_cameras, detail::camera_container &source_cameras)
{
	detail::move_objects(dest_cameras, source_cameras);
}

void SceneNode::RemoveCamera(detail::camera_container &from_cameras, Camera &camera) noexcept
{
	detail::remove_object(from_cameras, &camera);
}

void SceneNode::RemoveCameras(detail::camera_container &from_cameras, detail::camera_container &cameras) noexcept
{
	detail::remove_objects(from_cameras, cameras);
}

void SceneNode::GatherCameras(detail::camera_container &cameras)
{
	for (auto &object : attached_objects_)
	{
		if (auto camera = dynamic_cast<Camera*>(object))
			cameras.push_back(camera);
	}

	for (auto &child_node : child_nodes_)
		child_node->GatherCameras(cameras); //Recursive
}


void SceneNode::AddLight(detail::light_container &dest_lights, Light &light)
{
	detail::add_object(dest_lights, &light);
}

void SceneNode::MoveLights(detail::light_container &dest_lights, detail::light_container &source_lights)
{
	detail::move_objects(dest_lights, source_lights);
}

void SceneNode::RemoveLight(detail::light_container &from_lights, Light &light) noexcept
{
	detail::remove_object(from_lights, &light);
}

void SceneNode::RemoveLights(detail::light_container &from_lights, detail::light_container &lights) noexcept
{
	detail::remove_objects(from_lights, lights);
}

void SceneNode::GatherLights(detail::light_container &lights)
{
	for (auto &object : attached_objects_)
	{
		if (auto light = dynamic_cast<Light*>(object))
			lights.push_back(light);
	}

	for (auto &child_node : child_nodes_)
		child_node->GatherLights(lights); //Recursive
}


void SceneNode::AttachObjectToNode(MovableObject &object)
{
	//More likely
	if (auto light = dynamic_cast<Light*>(&object))
		AddLight(RootNode().attached_lights_, *light);

	//Less likely
	else if (auto camera = dynamic_cast<Camera*>(&object))
		AddCamera(RootNode().attached_cameras_, *camera);

	object.ParentNode(this);
}

void SceneNode::DetachObjectFromNode(MovableObject &object) noexcept
{
	//More likely
	if (auto light = dynamic_cast<Light*>(&object))
		RemoveLight(RootNode().attached_lights_, *light);

	//Less likely
	else if (auto camera = dynamic_cast<Camera*>(&object))
		RemoveCamera(RootNode().attached_cameras_, *camera);

	object.ParentNode(nullptr);
}


//Public

SceneNode::SceneNode(bool visible) noexcept :
	visible_{visible}
{
	AddNode(ordered_nodes_);
}

SceneNode::SceneNode(const Vector2 &initial_direction, bool visible) noexcept :

	initial_direction_{initial_direction},
	visible_{visible}
{
	AddNode(ordered_nodes_);
}

SceneNode::SceneNode(const Vector3 &position, const Vector2 &initial_direction, bool visible) noexcept :

	position_{position},
	initial_direction_{initial_direction},
	visible_{visible}
{
	AddNode(ordered_nodes_);
}


SceneNode::SceneNode(SceneNode &parent_node, bool visible) noexcept :

	visible_{visible},
	parent_node_{&parent_node}
{
	AddNode(RootNode().ordered_nodes_);
}

SceneNode::SceneNode(SceneNode &parent_node, const Vector2 &initial_direction, bool visible) noexcept :

	initial_direction_{initial_direction},
	visible_{visible},
	parent_node_{&parent_node}
{
	AddNode(RootNode().ordered_nodes_);
}

SceneNode::SceneNode(SceneNode &parent_node, const Vector3 &position, const Vector2 &initial_direction, bool visible) noexcept :
	
	position_{position},
	initial_direction_{initial_direction},
	visible_{visible},
	parent_node_{&parent_node}
{
	AddNode(RootNode().ordered_nodes_);
}


SceneNode::~SceneNode() noexcept
{
	//Root node (fast)
	if (!parent_node_)
	{
		ordered_nodes_.clear();
		attached_cameras_.clear();
		attached_lights_.clear();
		NotifyRemoved();
	}
	//Child node (slower)
	else if (!removed_)
	{
		//All descendant nodes will have their destructor called once
		//Gather and remove this and all descendant nodes in one batch
		GatherNodes(ordered_nodes_);
		RemoveNodes(RootNode().ordered_nodes_, ordered_nodes_);	

		//Gather and remove all cameras attached to this and all descendant nodes
		GatherCameras(attached_cameras_);
		RemoveCameras(RootNode().attached_cameras_, attached_cameras_);

		//Gather and remove all lights attached to this and all descendant nodes
		GatherLights(attached_lights_);
		RemoveLights(RootNode().attached_lights_, attached_lights_);

		NotifyRemoved();
	}

	DetachAllObjects();
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

bool SceneNode::AttachObject(MovableObject &object)
{
	if (!object.ParentNode())
	{
		AttachObjectToNode(object);
		attached_objects_.push_back(&object);
		return true;
	}
	else
		return false;
}

bool SceneNode::DetachObject(MovableObject &object) noexcept
{
	auto iter =
		std::find_if(std::begin(attached_objects_), std::end(attached_objects_),
			[&](auto &x) noexcept
			{
				return x == &object;
			});

	//Movable object found
	if (iter != std::end(attached_objects_))
	{
		DetachObjectFromNode(**iter);
		attached_objects_.erase(iter);
		return true;
	}
	else
		return false;
}

void SceneNode::DetachAllObjects() noexcept
{
	for (auto &object : attached_objects_)
	{
		if (object)
			DetachObjectFromNode(*object);
	}

	attached_objects_.clear();
	attached_objects_.shrink_to_fit();
}

} //ion::graphics::scene::graph