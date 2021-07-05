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
#include "graphics/utilities/IonMatrix3.h"
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
		
		derived_position_.Z(position_.Z() + parent_node_->derived_position_.Z());
	}
	else
		derived_position_.Z(position_.Z());
	
	need_z_update_ = false;
}


/*
	Helper functions
*/

void SceneNode::AddNode(detail::node_container &dest_nodes, SceneNode *node)
{
	detail::add_node(dest_nodes, node);
}

void SceneNode::MoveNodes(detail::node_container &dest_nodes, detail::node_container &source_nodes)
{
	detail::move_nodes(dest_nodes, source_nodes);
}

void SceneNode::RemoveNode(detail::node_container &from_nodes, SceneNode *node) noexcept
{
	detail::remove_node(from_nodes, node);
}

void SceneNode::RemoveNodes(detail::node_container &from_nodes, detail::node_container &nodes) noexcept
{
	detail::remove_nodes(from_nodes, nodes);
}

void SceneNode::GatherNodes(detail::node_container &nodes)
{
	detail::add_node(nodes, this);

	for (auto &child_node : child_nodes_)
		child_node->GatherNodes(nodes); //Recursive
}


void SceneNode::AddCamera(detail::camera_container &dest_cameras, Camera *camera)
{
	detail::add_object(dest_cameras, camera);
}

void SceneNode::MoveCameras(detail::camera_container &dest_cameras, detail::camera_container &source_cameras)
{
	detail::move_objects(dest_cameras, source_cameras);
}

void SceneNode::RemoveCamera(detail::camera_container &from_cameras, Camera *camera) noexcept
{
	detail::remove_object(from_cameras, camera);
}

void SceneNode::RemoveCameras(detail::camera_container &from_cameras, detail::camera_container &cameras) noexcept
{
	detail::remove_objects(from_cameras, cameras);
}

void SceneNode::GatherCameras(detail::camera_container &cameras)
{
	for (auto &object : attached_objects_)
	{
		if (auto camera = std::get_if<Camera*>(&object))
			detail::add_object(cameras, *camera);
	}

	for (auto &child_node : child_nodes_)
		child_node->GatherCameras(cameras); //Recursive
}


void SceneNode::AddLight(detail::light_container &dest_lights, Light *light)
{
	detail::add_object(dest_lights, light);
}

void SceneNode::MoveLights(detail::light_container &dest_lights, detail::light_container &source_lights)
{
	detail::move_objects(dest_lights, source_lights);
}

void SceneNode::RemoveLight(detail::light_container &from_lights, Light *light) noexcept
{
	detail::remove_object(from_lights, light);
}

void SceneNode::RemoveLights(detail::light_container &from_lights, detail::light_container &lights) noexcept
{
	detail::remove_objects(from_lights, lights);
}

void SceneNode::GatherLights(detail::light_container &lights)
{
	for (auto &object : attached_objects_)
	{
		if (auto light = std::get_if<Light*>(&object))
			detail::add_object(lights, *light);
	}

	for (auto &child_node : child_nodes_)
		child_node->GatherLights(lights); //Recursive
}


void SceneNode::AttachNode(SceneNode *node)
{
	node->parent_node_ = this;

	node->MoveNodes(node->RootNode().ordered_nodes_, node->ordered_nodes_);
	node->MoveCameras(node->RootNode().attached_cameras_, node->attached_cameras_);
	node->MoveLights(node->RootNode().attached_lights_, node->attached_lights_);
	
	node->NotifyUpdate();
}

void SceneNode::DetachNode(SceneNode *node)
{
	node->Tidy();
	node->parent_node_ = nullptr;
	node->NotifyUpdate();
}


void SceneNode::AttachObjectToNode(AttachableObject object)
{
	std::visit([&](auto &&object) noexcept { object->ParentNode(this); }, object);

	//More likely
	if (auto light = std::get_if<Light*>(&object))
		AddLight(RootNode().attached_lights_, *light);

	//Less likely
	else if (auto camera = std::get_if<Camera*>(&object))
		AddCamera(RootNode().attached_cameras_, *camera);
}

void SceneNode::DetachObjectFromNode(AttachableObject object, bool tidy) noexcept
{
	if (tidy)
	{
		//More likely
		if (auto light = std::get_if<Light*>(&object))
			RemoveLight(RootNode().attached_lights_, *light);

		//Less likely
		else if (auto camera = std::get_if<Camera*>(&object))
			RemoveCamera(RootNode().attached_cameras_, *camera);
	}

	std::visit([](auto &&object) noexcept { object->ParentNode(nullptr); }, object);
}

void SceneNode::DetachObjectsFromNode(scene_node::detail::object_container &objects, bool tidy) noexcept
{
	for (auto &object : objects)
		DetachObjectFromNode(object, tidy);
}


bool SceneNode::AttachObject(AttachableObject object)
{
	if (!std::visit([](auto &&object) noexcept { return object->ParentNode(); }, object))
	{
		attached_objects_.push_back(object);
		AttachObjectToNode(object);
		return true;
	}
	else
		return false;
}

bool SceneNode::DetachObject(AttachableObject object) noexcept
{
	auto iter =
		std::find_if(std::begin(attached_objects_), std::end(attached_objects_),
			[&](auto &x) noexcept
			{
				return
					std::visit([](auto &&x) noexcept -> MovableObject* { return x; }, x)
					==
					std::visit([](auto &&object) noexcept -> MovableObject* { return object; }, object);
			});

	//Object found
	if (iter != std::end(attached_objects_))
	{
		DetachObjectFromNode(*iter);
		attached_objects_.erase(iter);
		return true;
	}
	else
		return false;
}


void SceneNode::Tidy()
{
	//Gather and remove this and all descendant nodes in one batch
	GatherNodes(ordered_nodes_);
	RemoveNodes(RootNode().ordered_nodes_, ordered_nodes_);

	//Gather and remove all cameras attached to this and all descendant nodes
	GatherCameras(attached_cameras_);
	RemoveCameras(RootNode().attached_cameras_, attached_cameras_);

	//Gather and remove all lights attached to this and all descendant nodes
	GatherLights(attached_lights_);
	RemoveLights(RootNode().attached_lights_, attached_lights_);
}


//Public

SceneNode::SceneNode(bool visible) noexcept :
	visible_{visible}
{
	AddNode(ordered_nodes_, this);
}

SceneNode::SceneNode(const Vector2 &initial_direction, bool visible) noexcept :

	direction_{initial_direction},

	initial_direction_{initial_direction},
	visible_{visible}
{
	AddNode(ordered_nodes_, this);
}

SceneNode::SceneNode(const Vector3 &position, const Vector2 &initial_direction, bool visible) noexcept :

	position_{position},
	direction_{initial_direction},

	initial_direction_{initial_direction},
	visible_{visible}
{
	AddNode(ordered_nodes_, this);
}


SceneNode::SceneNode(SceneNode &parent_node, bool visible) noexcept :

	visible_{visible},
	parent_node_{&parent_node}
{
	AddNode(RootNode().ordered_nodes_, this);
}

SceneNode::SceneNode(SceneNode &parent_node, const Vector2 &initial_direction, bool visible) noexcept :

	direction_{initial_direction},

	initial_direction_{initial_direction},
	visible_{visible},
	parent_node_{&parent_node}
{
	AddNode(RootNode().ordered_nodes_, this);
}

SceneNode::SceneNode(SceneNode &parent_node, const Vector3 &position, const Vector2 &initial_direction, bool visible) noexcept :
	
	position_{position},
	direction_{initial_direction},

	initial_direction_{initial_direction},
	visible_{visible},
	parent_node_{&parent_node}
{
	AddNode(RootNode().ordered_nodes_, this);
}


SceneNode::~SceneNode() noexcept
{
	//Root node (fast)
	if (!parent_node_)
		NotifyRemoved();

	//Child node (slower)
	else if (!removed_)
	{
		Tidy();
		NotifyRemoved();
	}

	DetachObjectsFromNode(attached_objects_, false);
}


/*
	Observers
*/

bool SceneNode::AxisAligned() const noexcept
{
	//Axis aligned when 0, +-90, +-180, +-270 and +-360 (half degree tolerance)
	return std::fmod(math::Round(math::ToDegrees(DerivedRotation())), 90.0_r) == 0.0_r;
}


const Aabb& SceneNode::WorldAxisAlignedBoundingBox(bool derive) const noexcept
{
	if (derive)
	{
		world_aabb_ = {};

		//Merge world AABBs
		for (auto &object : attached_objects_)
			world_aabb_.Merge(std::visit([&](auto &&object) noexcept { return object->WorldAxisAlignedBoundingBox(derive); }, object));

		for (auto &child_node : child_nodes_)
			world_aabb_.Merge(child_node->WorldAxisAlignedBoundingBox(derive)); //Recursive
	}

	return world_aabb_;
}

const Obb& SceneNode::WorldOrientedBoundingBox(bool derive) const noexcept
{
	if (derive)
	{
		aabb_ = {};

		//Merge AABBs
		for (auto &object : attached_objects_)
			aabb_.Merge(std::visit([](auto &&object) noexcept { return object->AxisAlignedBoundingBox(); }, object));

		for (auto &child_node : child_nodes_)
		{
			static_cast<void>(child_node->WorldOrientedBoundingBox(derive)); //Recursive
			aabb_.Merge(child_node->aabb_);
		}

		world_obb_ = aabb_;
		world_obb_.Transform(FullTransformation());
	}

	return world_obb_;
}

const Sphere& SceneNode::WorldBoundingSphere(bool derive) const noexcept
{
	if (derive)
	{
		world_sphere_ = {};

		//Merge world spheres
		for (auto &object : attached_objects_)
			world_sphere_.Merge(std::visit([&](auto &&object) noexcept { return object->WorldBoundingSphere(derive); }, object));

		for (auto &child_node : child_nodes_)
			world_sphere_.Merge(child_node->WorldBoundingSphere(derive)); //Recursive
	}

	return world_sphere_;
}


/*
	Transformations
	Relative
*/

void SceneNode::Translate(const Vector3 &unit) noexcept
{
	if (unit != vector3::Zero)
		Position(position_ + unit.Deviant(vector2::UnitY.SignedAngleBetween(direction_)));
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

NonOwningPtr<SceneNode> SceneNode::Adopt(OwningPtr<SceneNode> &root_node)
{
	assert(root_node && &RootNode() != root_node.get());
		//nullptr and cyclic check

	auto &node = child_nodes_.emplace_back(std::move(root_node));
	AttachNode(node.get());
	return node;
}

NonOwningPtr<SceneNode> SceneNode::Adopt(OwningPtr<SceneNode> &&root_node)
{
	return Adopt(root_node);
}


void SceneNode::AdoptAll(SceneNodes &nodes)
{
	for (auto iter = std::begin(nodes); iter != std::end(nodes);)
	{
		if (*iter)
		{
			Adopt(*iter);
			iter = nodes.erase(iter);
		}
		else
			++iter;
	}
}

void SceneNode::AdoptAll(SceneNodes &&nodes)
{
	return AdoptAll(nodes);
}


OwningPtr<SceneNode> SceneNode::Orphan(SceneNode &child_node) noexcept
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
		DetachNode(iter->get());

		auto node = std::move(*iter); //Extend lifetime
		child_nodes_.erase(iter);	
		return node;
	}
	else
		return nullptr;
}

SceneNodes SceneNode::OrphanAll() noexcept
{
	//Something to remove
	if (!std::empty(child_nodes_))
	{
		for (auto &node : child_nodes_)
			DetachNode(node.get());

		auto nodes = std::move(child_nodes_); //Extend lifetime
		child_nodes_.shrink_to_fit();	
		return nodes;
	}
	else
		return {};
}


/*
	Child nodes
	Removing
*/

void SceneNode::ClearChildNodes() noexcept
{
	child_nodes_.clear();
	child_nodes_.shrink_to_fit();
}

bool SceneNode::RemoveChildNode(SceneNode &child_node) noexcept
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
		child_nodes_.erase(iter);	
		return true;
	}
	else
		return false;
}


/*
	Attachable objects
*/

bool SceneNode::AttachObject(MovableObject &object)
{
	return AttachObject(&object);
}

bool SceneNode::AttachObject(Camera &camera)
{
	return AttachObject(&camera);
}

bool SceneNode::AttachObject(Light &light)
{
	return AttachObject(&light);
}


bool SceneNode::DetachObject(MovableObject &object) noexcept
{
	return DetachObject(&object);
}

bool SceneNode::DetachObject(Camera &camera) noexcept
{
	return DetachObject(&camera);
}

bool SceneNode::DetachObject(Light &light) noexcept
{
	return DetachObject(&light);
}


void SceneNode::DetachAllObjects() noexcept
{
	DetachObjectsFromNode(attached_objects_);
	attached_objects_.clear();
	attached_objects_.shrink_to_fit();
}

} //ion::graphics::scene::graph