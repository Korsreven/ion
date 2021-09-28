/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph
File:	IonSceneNode.h
-------------------------------------------
*/

#ifndef ION_SCENE_NODE_H
#define ION_SCENE_NODE_H

#include <algorithm>
#include <any>
#include <variant>
#include <vector>

#include "adaptors/ranges/IonDereferenceIterable.h"
#include "adaptors/ranges/IonIterable.h"
#include "animations/IonNodeAnimationManager.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declaration
namespace ion::graphics::scene
{
	class MovableObject;
	class Camera;
	class Light;
}

namespace ion::graphics::scene::graph
{
	using namespace graphics::utilities;
	using namespace types::type_literals;

	class SceneNode; //Forward declaration

	namespace scene_node
	{
		enum class NodeRotationOrigin : bool
		{
			Parent,
			Local
		};

		using SceneNodes = std::vector<OwningPtr<SceneNode>>;
		using AttachableObject = std::variant<MovableObject*, Camera*, Light*>;

		namespace detail
		{
			using node_container = std::vector<SceneNode*>;
			using object_container = std::vector<AttachableObject>;
			using camera_container = std::vector<Camera*>;
			using light_container = std::vector<Light*>;

			struct node_comparator
			{
				bool operator()(const SceneNode *x, const SceneNode *y) const noexcept;
			};


			template <typename Compare = node_comparator>
			inline void add_node(node_container &dest_nodes, SceneNode *node, Compare compare = Compare{})
			{
				//Search for first scene node with greater z-order
				auto iter = std::upper_bound(std::begin(dest_nodes), std::end(dest_nodes), node, compare);
				dest_nodes.insert(iter, node);
			}

			template <typename Compare = node_comparator>
			inline void move_nodes(node_container &dest_nodes, node_container &source_nodes, Compare compare = Compare{})
			{
				auto size = std::size(dest_nodes);

				//Insert source nodes to the back of dest nodes
				dest_nodes.insert(
					std::end(dest_nodes),
					std::begin(source_nodes), std::end(source_nodes));

				//One or more source node has been inserted
				if (auto first = std::begin(dest_nodes) + size; first != std::begin(dest_nodes))
					//Merge dest and source nodes
					std::inplace_merge(
						std::begin(dest_nodes), first,
						std::end(dest_nodes), compare);

				source_nodes.clear(); //Moved
				source_nodes.shrink_to_fit();
			}


			template <typename Compare = node_comparator>
			inline void remove_node(node_container &from_nodes, SceneNode *node, Compare compare = Compare{}) noexcept
			{
				//Search for first scene node with equal z-order
				if (auto first = std::lower_bound(std::begin(from_nodes), std::end(from_nodes), node, compare);
					first != std::end(from_nodes) && !compare(node, *first)) //Found z-order
				{
					//Search for first scene node with greater z-order
					auto last = std::upper_bound(first, std::end(from_nodes), node, compare);			

					//Search for exact scene node in range [first, last)
					if (first = std::find(first, last, node); first != last) //Found exact
						from_nodes.erase(first);
				}
			}

			template <typename Compare = node_comparator>
			inline void remove_nodes(node_container &from_nodes, node_container &nodes, Compare compare = Compare{}) noexcept
			{
				if (std::empty(nodes))
					return;

				if (std::size(nodes) == 1)
				{
					remove_node(from_nodes, nodes.front(), compare);
					return;
				}

				//Search for first scene node with equal z-order
				if (auto first = std::lower_bound(std::begin(from_nodes), std::end(from_nodes), nodes.front(), compare);
					first != std::end(from_nodes) && !compare(nodes.front(), *first)) //Found z-order
				{
					//Search for first scene node with greater z-order
					auto last = std::upper_bound(first, std::end(from_nodes), nodes.back(), compare);			

					for (auto iter = first; auto &node : nodes)
					{
						//Search for each exact scene node in range [first, last)
						//Range is narrowed for each node found
						if (iter = std::find(iter, last, node); iter != last) //Found exact
						{
							*iter = nullptr; //Tag
							++iter;
						}
					}

					//Erase all tagged in range [first, last)
					from_nodes.erase(
						std::remove_if(first, last,
							[](auto &node) noexcept
							{
								return !node;
							}),
						last);
				}
			}


			template <typename Container, typename Compare = std::less<>>
			inline void add_object(Container &dest_objects, typename Container::value_type object, Compare compare = Compare{})
			{
				auto iter = std::upper_bound(std::begin(dest_objects), std::end(dest_objects), object, compare);
				dest_objects.insert(iter, object);
			}

			template <typename Container, typename Compare = std::less<>>
			inline void move_objects(Container &dest_objects, Container &source_objects, Compare compare = Compare{})
			{
				auto size = std::size(dest_objects);

				//Insert source objects to the back of dest objects
				dest_objects.insert(
					std::end(dest_objects),
					std::begin(source_objects), std::end(source_objects));

				//One or more source object has been inserted
				if (auto first = std::begin(dest_objects) + size; first != std::begin(dest_objects))
					//Merge dest and source objects
					std::inplace_merge(
						std::begin(dest_objects), first,
						std::end(dest_objects), compare);

				source_objects.clear(); //Moved
				source_objects.shrink_to_fit();
			}


			template <typename Container, typename Compare = std::less<>>
			inline void remove_object(Container &from_objects, typename Container::value_type object, Compare compare = Compare{}) noexcept
			{
				if (auto first = std::lower_bound(std::begin(from_objects), std::end(from_objects), object, compare);
					first != std::end(from_objects) && !compare(object, *first)) //Found exact
					
					from_objects.erase(first);
			}

			template <typename Container, typename Compare = std::less<>>
			inline void remove_objects(Container &from_objects, Container &objects, Compare compare = Compare{}) noexcept
			{
				if (std::empty(objects))
					return;

				if (std::size(objects) == 1)
				{
					remove_object(from_objects, objects.front(), compare);
					return;
				}

				if (auto first = std::lower_bound(std::begin(from_objects), std::end(from_objects), objects.front(), compare);
					first != std::end(from_objects) && !compare(objects.front(), *first)) //Found first exact
				{
					//Search for first object with greater z-order
					auto last = std::upper_bound(first, std::end(from_objects), objects.back(), compare);			

					for (auto iter = first; auto &object : objects)
					{
						//Search for each exact object in range [first, last)
						//Range is narrowed for each object found
						if (iter = std::find(iter, last, object); iter != last) //Found
						{
							*iter = nullptr; //Tag
							++iter;
						}
					}

					//Erase all tagged in range [first, last)
					from_objects.erase(
						std::remove_if(first, last,
							[](auto &object) noexcept
							{
								return !object;
							}),
						last);
				}
			}


			inline auto to_scaling3(const Vector2 &scaling) noexcept
			{
				auto [x, y] = scaling.XY();
				return Vector3{x, y, 1.0_r};
			}

			Matrix4 make_transformation(const Vector3 &position, real rotation, const Vector2 &scaling) noexcept;
		} //detail
	} //scene_node


	class SceneNode final : public animations::NodeAnimationManager
	{
		private:

			Vector3 position_;
			Vector2 direction_ = vector2::UnitY;
			real rotation_ = 0.0_r;
			Vector2 scaling_ = vector2::UnitScale;

			Vector2 initial_direction_ = vector2::UnitY;
			scene_node::NodeRotationOrigin rotation_origin_ = scene_node::NodeRotationOrigin::Parent;
			bool inherit_rotation_ = true;
			bool inherit_scaling_ = true;
			bool visible_ = true;

			SceneNode *parent_node_ = nullptr;
			scene_node::SceneNodes child_nodes_;
			scene_node::detail::object_container attached_objects_;
			std::any user_data_;

			scene_node::detail::node_container ordered_nodes_; //Root node only
			scene_node::detail::camera_container attached_cameras_; //Root node only
			scene_node::detail::light_container attached_lights_; //Root node only
			bool removed_ = false; //For optimized destruction


			mutable Vector3 derived_position_;
			mutable Vector2 derived_direction_;
			mutable real derived_rotation_ = 0.0_r;
			mutable Vector2 derived_scaling_;
			mutable Matrix4 full_tranformation_;

			mutable Aabb aabb_;
			mutable Aabb world_aabb_;
			mutable Obb world_obb_;
			mutable Sphere world_sphere_;

			mutable bool need_update_ = true;
			mutable bool need_z_update_ = true;
			mutable bool transformation_out_of_date_ = true;


			/*
				Notifying
			*/

			void NotifyRemoved() noexcept;

			void NotifyUpdate() noexcept;
			void NotifyUpdateZ() noexcept;		


			/*
				Updating
			*/

			void Update() const noexcept;
			void UpdateZ() const noexcept;


			/*
				Helper functions
			*/

			void AddNode(scene_node::detail::node_container &dest_nodes, SceneNode *node);
			void MoveNodes(scene_node::detail::node_container &dest_nodes, scene_node::detail::node_container &source_nodes);
			void RemoveNode(scene_node::detail::node_container &from_nodes, SceneNode *node) noexcept;
			void RemoveNodes(scene_node::detail::node_container &from_nodes, scene_node::detail::node_container &nodes) noexcept;	
			void GatherNodes(scene_node::detail::node_container &nodes);

			void AddCamera(scene_node::detail::camera_container &dest_cameras, Camera *camera);
			void MoveCameras(scene_node::detail::camera_container &dest_cameras, scene_node::detail::camera_container &source_cameras);
			void RemoveCamera(scene_node::detail::camera_container &from_cameras, Camera *camera) noexcept;
			void RemoveCameras(scene_node::detail::camera_container &from_cameras, scene_node::detail::camera_container &cameras) noexcept;	
			void GatherCameras(scene_node::detail::camera_container &cameras);

			void AddLight(scene_node::detail::light_container &dest_lights, Light *light);
			void MoveLights(scene_node::detail::light_container &dest_lights, scene_node::detail::light_container &source_lights);
			void RemoveLight(scene_node::detail::light_container &from_lights, Light *light) noexcept;
			void RemoveLights(scene_node::detail::light_container &from_lights, scene_node::detail::light_container &lights) noexcept;	
			void GatherLights(scene_node::detail::light_container &lights);
			
			void AttachNode(SceneNode *node);
			void DetachNode(SceneNode *node);

			void AttachObjectToNode(scene_node::AttachableObject object);
			void DetachObjectFromNode(scene_node::AttachableObject object, bool tidy = true) noexcept;
			void DetachObjectsFromNode(scene_node::detail::object_container &objects, bool tidy = true) noexcept;

			bool AttachObject(scene_node::AttachableObject object);
			bool DetachObject(scene_node::AttachableObject object) noexcept;

			void Tidy();

		public:

			//Construct a scene node as the root with the given visibility
			explicit SceneNode(bool visible = true) noexcept;

			//Construct a scene node as the root with the given initial direction and visibility
			explicit SceneNode(const Vector2 &initial_direction, bool visible = true) noexcept;

			//Construct a scene node as the root with the given position, initial direction and visibility
			explicit SceneNode(const Vector3 &position, const Vector2 &initial_direction = vector2::UnitY, bool visible = true) noexcept;


			//Construct a scene node as a child with the given parent and visibility
			explicit SceneNode(SceneNode &parent_node, bool visible = true) noexcept;

			//Construct a scene node as a child with the given parent, initial direction and visibility
			SceneNode(SceneNode &parent_node, const Vector2 &initial_direction, bool visible = true) noexcept;

			//Construct a scene node as a child with the given parent, position, initial direction and visibility
			SceneNode(SceneNode &parent_node, const Vector3 &position, const Vector2 &initial_direction = vector2::UnitY, bool visible = true) noexcept;


			//Virtual destructor
			virtual ~SceneNode() noexcept;


			/*
				Operators
			*/

			//Checks if one node is less than another one (z-order wise)
			//Needed for sorting two nodes (strict weak ordering)
			[[nodiscard]] inline auto operator<(const SceneNode &rhs) const noexcept
			{
				if (need_z_update_)
					UpdateZ();

				if (rhs.need_z_update_)
					rhs.UpdateZ();

				return derived_position_.Z() < rhs.derived_position_.Z();
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all child nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ChildNodes() noexcept
			{
				return adaptors::ranges::DereferenceIterable<scene_node::SceneNodes&>{child_nodes_};
			}

			//Returns an immutable range of all child nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ChildNodes() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const scene_node::SceneNodes&>{child_nodes_};
			}


			//Returns a mutable range of all objects attached to this node
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedObjects() noexcept
			{
				return adaptors::ranges::Iterable<scene_node::detail::object_container&>{attached_objects_};
			}

			//Returns an immutable range of all objects attached to this node
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedObjects() const noexcept
			{
				return adaptors::ranges::Iterable<const scene_node::detail::object_container&>{attached_objects_};
			}


			/*
				Ranges
				Root node only
			*/

			//Returns a mutable range of this and all descendant nodes ordered for rendering
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto OrderedSceneNodes() noexcept
			{
				return adaptors::ranges::DereferenceIterable<scene_node::detail::node_container&>{ordered_nodes_};
			}

			//Returns an immutable range of this and all descendant nodes ordered for rendering
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto OrderedSceneNodes() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const scene_node::detail::node_container&>{ordered_nodes_};
			}


			//Returns a mutable range of all cameras attached to this and all descendant nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedCameras() noexcept
			{
				return adaptors::ranges::Iterable<scene_node::detail::camera_container&>{attached_cameras_};
			}

			//Returns an immutable range of all cameras attached to this and all descendant nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedCameras() const noexcept
			{
				return adaptors::ranges::Iterable<const scene_node::detail::camera_container&>{attached_cameras_};
			}


			//Returns a mutable range of all lights attached to this and all descendant nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedLights() noexcept
			{
				return adaptors::ranges::Iterable<scene_node::detail::light_container&>{attached_lights_};
			}

			//Returns an immutable range of all lights attached to this and all descendant nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedLights() const noexcept
			{
				return adaptors::ranges::Iterable<const scene_node::detail::light_container&>{attached_lights_};
			}


			/*
				Modifiers
			*/

			//Sets the local position of this node to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					auto z_changed = position_.Z() != position.Z();

					if (z_changed && parent_node_)
					{
						GatherNodes(ordered_nodes_);
						RemoveNodes(RootNode().ordered_nodes_, ordered_nodes_);
					}

					position_ = position;
					NotifyUpdate();

					if (z_changed)
					{
						NotifyUpdateZ();

						if (parent_node_)
							MoveNodes(RootNode().ordered_nodes_, ordered_nodes_);
					}
				}
			}

			//Sets the local direction of this node to the given direction
			inline void Direction(const Vector2 &direction) noexcept
			{
				if (direction_ != direction)
				{
					direction_ = direction;
					rotation_ = direction.SignedAngleBetween(initial_direction_); //Update rotation
					NotifyUpdate();
				}
			}

			//Sets the local rotation of this node to the given angle in radians
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					direction_ = initial_direction_.Deviant(angle); //Update direction
					NotifyUpdate();
				}
			}

			//Sets the local scaling of this node to the given scaling
			inline void Scaling(const Vector2 &scaling) noexcept
			{
				if (scaling_ != scaling)
				{
					scaling_ = scaling;
					NotifyUpdate();
				}
			}


			//Sets the rotation origin of this node to the given origin
			inline void RotationOrigin(scene_node::NodeRotationOrigin origin) noexcept
			{
				if (rotation_origin_ != origin)
				{
					rotation_origin_ = origin;
					NotifyUpdate();
				}
			}

			//Sets whether or not this node should inherit rotation
			inline void InheritRotation(bool inherit) noexcept
			{
				if (inherit_rotation_ != inherit)
				{
					inherit_rotation_ = inherit;
					NotifyUpdate();
				}
			}

			//Sets whether or not this node should inherit scaling
			inline void InheritScaling(bool inherit) noexcept
			{
				if (inherit_scaling_ != inherit)
				{
					inherit_scaling_ = inherit;
					NotifyUpdate();
				}
			}

			//Sets whether or not this and all descendant nodes should be visible
			//If cascade is set to false, only this node is set
			inline void Visible(bool visible, bool cascade = true) noexcept
			{
				visible_ = visible;

				if (cascade)
				{
					for (auto &child_node : child_nodes_)
						child_node->Visible(visible, cascade); //Recursive
				}
			}

			//Flips the visibility of this and all descendant nodes
			//If cascade is set to false, only this node is flipped
			inline void FlipVisibility(bool cascade = true) noexcept
			{
				visible_ = !visible_;

				if (cascade)
				{
					for (auto &child_node : child_nodes_)
						child_node->FlipVisibility(cascade); //Recursive
				}
			}


			//Sets the custom user data for this node to the given data
			inline void UserData(std::any data) noexcept
			{
				user_data_ = data;
			}


			/*
				Observers
			*/

			//Returns the local position of this node
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the local direction of this node
			[[nodiscard]] inline auto& Direction() const noexcept
			{
				return direction_;
			}

			//Returns the local rotation of this node in radians
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}

			//Returns the local scaling of this node
			[[nodiscard]] inline auto& Scaling() const noexcept
			{
				return scaling_;
			}


			//Returns the initial direction of this node
			[[nodiscard]] inline auto& InitialDirection() const noexcept
			{
				return initial_direction_;
			}

			//Returns the rotation origin of this node
			[[nodiscard]] inline auto RotationOrigin() const noexcept
			{
				return rotation_origin_;
			}

			//Returns whether or not this node inherit rotation
			[[nodiscard]] inline auto InheritRotation() const noexcept
			{
				return inherit_rotation_;
			}

			//Returns whether or not this node inherit scaling
			[[nodiscard]] inline auto InheritScaling() const noexcept
			{
				return inherit_scaling_;
			}

			//Returns whether or not this node is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}


			//Returns the parent node of this node
			//Returns nullptr if this node is the root
			[[nodiscard]] inline auto ParentNode() const noexcept
			{
				return parent_node_;
			}

			//Returns a mutable reference to the root node of this node
			[[nodiscard]] inline auto RootNode() noexcept -> SceneNode&
			{
				if (parent_node_)
					return parent_node_->RootNode(); //Recursive
				else
					return *this;
			}

			//Returns an immutable reference to the root node of this node
			[[nodiscard]] inline auto RootNode() const noexcept -> const SceneNode&
			{
				if (parent_node_)
					return parent_node_->RootNode(); //Recursive
				else
					return *this;
			}

			//Returns the custom user data for this node
			[[nodiscard]] inline auto& UserData() const noexcept
			{
				return user_data_;
			}


			//Returns the derived position of this node
			[[nodiscard]] inline auto& DerivedPosition() const noexcept
			{
				if (need_update_)
					Update();

				return derived_position_;
			}

			//Returns the derived direction of this node
			[[nodiscard]] inline auto& DerivedDirection() const noexcept
			{
				if (need_update_)
					Update();

				return derived_direction_;
			}

			//Returns the derived rotation of this node in radians
			[[nodiscard]] inline auto DerivedRotation() const noexcept
			{
				if (need_update_)
					Update();

				return derived_rotation_;
			}

			//Returns the derived scaling of this node
			[[nodiscard]] inline auto& DerivedScaling() const noexcept
			{
				if (need_update_)
					Update();

				return derived_scaling_;
			}

			//Returns the full transformation matrix for this node
			[[nodiscard]] inline auto& FullTransformation() const noexcept
			{
				if (need_update_)
					Update();

				if (transformation_out_of_date_)
				{
					full_tranformation_ =
						scene_node::detail::make_transformation(derived_position_, derived_rotation_, derived_scaling_);
					transformation_out_of_date_ = false;
				}

				return full_tranformation_;
			}


			//Returns true if this node is axis aligned
			[[nodiscard]] bool AxisAligned() const noexcept;


			//Returns the world axis-aligned bounding box (AABB) for objects attached to this and all descendant nodes
			[[nodiscard]] const Aabb& WorldAxisAlignedBoundingBox(bool derive = true) const noexcept;

			//Returns the world oriented bounding box (OBB) for objects attached to this and all descendant nodes
			[[nodiscard]] const Obb& WorldOrientedBoundingBox(bool derive = true) const noexcept;

			//Returns the world bounding sphere for objects attached to this and all descendant nodes
			[[nodiscard]] const Sphere& WorldBoundingSphere(bool derive = true) const noexcept;


			/*
				Transformations
				Relative
			*/

			//Translate this node by the given unit
			void Translate(const Vector3 &unit) noexcept;

			//Translate this node by the given unit
			void Translate(real unit) noexcept;

			//Rotate this node by the given angle in radians
			void Rotate(real angle) noexcept;

			//Scale this node by the given unit
			void Scale(const Vector2 &unit) noexcept;


			/*
				Transformations
				Absolute
			*/

			//Turn this node such that it faces the given position
			void LookAt(const Vector3 &position) noexcept;


			/*
				Elapse time
			*/

			//Elapse the total time for this node by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;


			/*
				Child nodes
				Creating
			*/

			//Create a new scene node as a child of this node with the given visibility
			NonOwningPtr<SceneNode> CreateChildNode(bool visible = true);

			//Create a new scene node as a child of this node with the given initial direction and visibility
			NonOwningPtr<SceneNode> CreateChildNode(const Vector2 &initial_direction, bool visible = true);

			//Create a new scene node as a child of this node with the given position, initial direction and visibility
			NonOwningPtr<SceneNode> CreateChildNode(const Vector3 &position, const Vector2 &initial_direction = vector2::UnitY, bool visible = true);


			/*
				Child nodes
				Take / release ownership
			*/

			//Adopt (take ownership of) the given scene node and returns a pointer to the adopted node
			//Returns nullptr if the scene node could not be adopted and scene node will remain untouched
			NonOwningPtr<SceneNode> Adopt(OwningPtr<SceneNode> &root_node);

			//Adopt (take ownership of) the given scene node and returns a pointer to the adopted node
			//Returns nullptr if the scene node could not be adopted and scene node will be released
			NonOwningPtr<SceneNode> Adopt(OwningPtr<SceneNode> &&root_node);


			//Adopt (take ownership of) all the given scene nodes
			//If one or more scene nodes could not be adopted, they will remain untouched in the given container
			void AdoptAll(scene_node::SceneNodes &nodes);

			//Adopt (take ownership of) all the given scene nodes
			//If one or more scene nodes could not be adopted, they will be released
			void AdoptAll(scene_node::SceneNodes &&nodes);


			//Orphan (release ownership of) the given child node
			//Returns a pointer to the scene node released
			[[nodiscard]] OwningPtr<SceneNode> Orphan(SceneNode &child_node) noexcept;

			//Orphan (release ownership of) all child nodes in this scene node
			//Returns pointers to the scene nodes released
			[[nodiscard]] scene_node::SceneNodes OrphanAll() noexcept;


			/*
				Child nodes
				Removing
			*/

			//Clear all child nodes from this scene node
			void ClearChildNodes() noexcept;

			//Remove the given child node from this scene node
			//Returns true if the given child node was removed
			bool RemoveChildNode(SceneNode &child_node) noexcept;


			/*
				Attachable objects
			*/

			//Attach the given object to this node if not already attached
			//Return true if the given object was attached
			bool AttachObject(MovableObject &object);

			//Attach the given camera to this node if not already attached
			//Return true if the given camera was attached
			bool AttachObject(Camera &camera);

			//Attach the given light to this node if not already attached
			//Return true if the given light was attached
			bool AttachObject(Light &light);


			//Detach the given object if attached to this node
			//Returns true if the given object was detached
			bool DetachObject(MovableObject &object) noexcept;

			//Detach the given camera if attached to this node
			//Returns true if the given camera was detached
			bool DetachObject(Camera &camera) noexcept;

			//Detach the given light if attached to this node
			//Returns true if the given light was detached
			bool DetachObject(Light &light) noexcept;


			//Detach all objects attached to this node
			void DetachAllObjects() noexcept;		
	};
} //ion::graphics::scene::graph

#endif