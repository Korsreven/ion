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

#ifndef ION_SCENE_NODE
#define ION_SCENE_NODE

#include <vector>

#include "adaptors/ranges/IonDereferenceIterable.h"
#include "adaptors/ranges/IonIterable.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonOwningPtr.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene
{
	class MovableObject;  //Forward declaration
}

namespace ion::graphics::scene::graph
{
	using namespace graphics::utilities;
	using namespace types::type_literals;

	class SceneNode; //Forward declaration

	namespace scene_node
	{
		enum class RotationOrigin : bool
		{
			Parent,
			Local
		};

		namespace detail
		{
			using scene_node_container = std::vector<OwningPtr<SceneNode>>;
			using movable_object_container = std::vector<NonOwningPtr<MovableObject>>;
		} //detail
	} //scene_node


	class SceneNode final
	{
		private:

			Vector3 position_;
			Vector2 direction_ = vector2::UnitY;
			real rotation_ = 0.0_r;
			Vector2 scaling_ = vector2::UnitScale;

			Vector2 initial_direction_ = vector2::UnitY;
			scene_node::RotationOrigin rotation_origin_ = scene_node::RotationOrigin::Parent;
			bool inherit_rotation_ = true;
			bool inherit_scaling_ = true;
			bool visible_ = true;

			NonOwningPtr<SceneNode> parent_node_;
			scene_node::detail::scene_node_container child_nodes_;
			scene_node::detail::movable_object_container movable_objects_;


			Vector3 world_position_;
			Vector2 world_direction_ = vector2::UnitY;
			real world_rotation_ = 0.0_r;
			Vector2 world_scaling_ = vector2::UnitScale;
			Matrix4 world_tranformation_;
			Matrix4 world_transformation_projection_;

			bool need_update_ = false;
			bool rearrange_node_ = false;

		public:

			//Default construct a scene node as the root
			SceneNode() = default;


			//Construct a scene node as the root with the given visibility
			explicit SceneNode(bool visible);

			//Construct a scene node as the root with the given initial direction and visibility
			explicit SceneNode(const Vector2 &initial_direction, bool visible = true);

			//Construct a scene node as the root with the given position, initial direction and visibility
			explicit SceneNode(const Vector3 &position, const Vector2 &initial_direction = vector2::UnitY, bool visible = true);


			//Construct a scene node as a child of the given parent node and visibility
			explicit SceneNode(NonOwningPtr<SceneNode> parent_node, bool visible = true);

			//Construct a scene node as a child of the given parent node, initial direction and visibility
			SceneNode(NonOwningPtr<SceneNode> parent_node, const Vector2 &initial_direction, bool visible = true);

			//Construct a scene node as a child of the given parent node, position, initial direction and visibility
			SceneNode(NonOwningPtr<SceneNode> parent_node, const Vector3 &position, const Vector2 &initial_direction = vector2::UnitY, bool visible = true);


			/*
				Ranges
			*/

			//Returns a mutable range of all child nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ChildNodes() noexcept
			{
				return adaptors::ranges::DereferenceIterable<scene_node::detail::scene_node_container&>{child_nodes_};
			}

			//Returns an immutable range of all child nodes
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ChildNodes() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const scene_node::detail::scene_node_container&>{child_nodes_};
			}


			//Returns a mutable range of all movable objects attached to this node
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto MovableObjects() noexcept
			{
				return adaptors::ranges::Iterable<scene_node::detail::movable_object_container&>{movable_objects_};
			}

			//Returns an immutable range of all movable objects attached to this node
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto MovableObjects() const noexcept
			{
				return adaptors::ranges::Iterable<const scene_node::detail::movable_object_container&>{movable_objects_};
			}


			/*
				Modifiers
			*/




			/*
				Observers
			*/




			//Returns true if this node is axis aligned
			[[nodiscard]] inline auto AxisAligned() noexcept
			{
				return true;
			}


			/*
				Child nodes
				Creating
			*/

			//Create a new scene node as a child of this node with the given visibility
			[[nodiscard]] NonOwningPtr<SceneNode> CreateChildNode(bool visible = true);

			//Create a new scene node as a child of this node with the given initial direction and visibility
			[[nodiscard]] NonOwningPtr<SceneNode> CreateChildNode(const Vector2 &initial_direction, bool visible = true);

			//Create a new scene node as a child of this node with the given position, initial direction and visibility
			[[nodiscard]] NonOwningPtr<SceneNode> CreateChildNode(const Vector3 &position, const Vector2 &initial_direction = vector2::UnitY, bool visible = true);


			/*
				Child nodes
				Take / release ownership
			*/

			//Adopt (take ownership of) the given scene node and returns a pointer to the adopted node
			//Returns nullptr if the scene node could not be adopted and scene node will be released
			NonOwningPtr<SceneNode> Adopt(OwningPtr<SceneNode> scene_node);

			//Adopt (take ownership of) all the given scene nodes
			//If one or more scene nodes could not be adopted, they will be released
			void Adopt(scene_node::detail::scene_node_container &scene_nodes);


			//Orphan (release ownership of) the given child node
			//Returns a pointer to the scene node released
			[[nodiscard]] OwningPtr<SceneNode> Orphan(SceneNode &child_node) noexcept;

			//Orphan (release ownership of) all child nodes in this scene node
			//Returns pointers to the scene nodes released
			[[nodiscard]] scene_node::detail::scene_node_container OrphanAll() noexcept;


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
				Movable objects
			*/

			//Attach the given movable object to this node if not already attached
			//Return true if the given movable object was attached
			bool Attach(NonOwningPtr<MovableObject> movable_object);

			//Detach the given movable objects if attached to this node
			//Returns true if the given movable object was detached
			bool Detach(MovableObject &movable_object) noexcept;

			//Detach all movable objects attached to this node
			void Detach() noexcept;
	};
} //ion::graphics::scene::graph

#endif