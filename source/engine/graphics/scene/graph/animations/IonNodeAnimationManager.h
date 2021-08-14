/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimationManager.h
-------------------------------------------
*/

#ifndef ION_NODE_ANIMATION_MANAGER_H
#define ION_NODE_ANIMATION_MANAGER_H

#include <string>
#include <string_view>

#include "IonNodeAnimation.h"
#include "IonNodeAnimationGroup.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::scene::graph
{
	class SceneNode; //Forward declaration
}

namespace ion::graphics::scene::graph::animations
{
	namespace node_animation_manager::detail
	{
	} //node_animation_manager::detail


	class NodeAnimationManager final :
		public managed::ObjectManager<NodeAnimation, NodeAnimationManager>,
		public managed::ObjectManager<NodeAnimationGroup, NodeAnimationManager>
	{
		private:

			SceneNode &parent_node_;

		public:

			using NodeAnimationBase = managed::ObjectManager<NodeAnimation, NodeAnimationManager>;
			using NodeAnimationGroupBase = managed::ObjectManager<NodeAnimationGroup, NodeAnimationManager>;


			//Construct a new node animation manager with the given scene node
			NodeAnimationManager(SceneNode &scene_node) noexcept;

			//Deleted copy constructor
			NodeAnimationManager(const NodeAnimationManager&) = delete;

			//Default move constructor
			NodeAnimationManager(NodeAnimationManager&&) = default;

			//Destructor
			~NodeAnimationManager() = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			NodeAnimationManager& operator=(const NodeAnimationManager&) = delete;

			//Move assignment
			NodeAnimationManager& operator=(NodeAnimationManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all node animations in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto NodeAnimations() noexcept
			{
				return NodeAnimationBase::Objects();
			}

			//Returns an immutable range of all node animations in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto NodeAnimations() const noexcept
			{
				return NodeAnimationBase::Objects();
			}


			//Returns a mutable range of all node animation groups in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto NodeAnimationGroups() noexcept
			{
				return NodeAnimationGroupBase::Objects();
			}

			//Returns an immutable range of all node animation groups in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto NodeAnimationGroups() const noexcept
			{
				return NodeAnimationGroupBase::Objects();
			}


			/*
				Observers
			*/

			//Returns a mutable reference to the parent node for this manager
			[[nodiscard]] inline auto& ParentNode() noexcept
			{
				return parent_node_;
			}

			//Returns an immutable reference to the parent node for this manager
			[[nodiscard]] inline auto& ParentNode() const noexcept
			{
				return parent_node_;
			}


			/*
				Animations
				Creating
			*/

			//Create a node animation with the given name
			NonOwningPtr<NodeAnimation> CreateAnimation(std::string name);


			//Create a node animation as a copy of the given node animation
			NonOwningPtr<NodeAnimation> CreateAnimation(const NodeAnimation &node_animation);

			//Create a node animation by moving the given node animation
			NonOwningPtr<NodeAnimation> CreateAnimation(NodeAnimation &&node_animation);


			/*
				Node animations
				Retrieving
			*/

			//Gets a pointer to a mutable node animation with the given name
			//Returns nullptr if node animation could not be found
			[[nodiscard]] NonOwningPtr<NodeAnimation> GetAnimation(std::string_view name) noexcept;

			//Gets a pointer to an immutable node animation with the given name
			//Returns nullptr if node animation could not be found
			[[nodiscard]] NonOwningPtr<const NodeAnimation> GetAnimation(std::string_view name) const noexcept;


			/*
				Node animations
				Removing
			*/

			//Clear all removable node animations from this manager
			void ClearAnimations() noexcept;

			//Remove a removable node animation from this manager
			bool RemoveAnimation(NodeAnimation &node_animation) noexcept;

			//Remove a removable node animation with the given name from this manager
			bool RemoveAnimation(std::string_view name) noexcept;


			/*
				Animation groups
				Creating
			*/

			//Create a node animation group with the given name
			NonOwningPtr<NodeAnimationGroup> CreateAnimationGroup(std::string name);


			//Create a node animation group as a copy of the given node animation group
			NonOwningPtr<NodeAnimationGroup> CreateAnimationGroup(const NodeAnimationGroup &node_animation_group);

			//Create a node animation group by moving the given node animation group
			NonOwningPtr<NodeAnimationGroup> CreateAnimationGroup(NodeAnimationGroup &&node_animation_group);


			/*
				Node animation groups
				Retrieving
			*/

			//Gets a pointer to a mutable node animation group with the given name
			//Returns nullptr if node animation group could not be found
			[[nodiscard]] NonOwningPtr<NodeAnimationGroup> GetAnimationGroup(std::string_view name) noexcept;

			//Gets a pointer to an immutable node animation group with the given name
			//Returns nullptr if node animation group could not be found
			[[nodiscard]] NonOwningPtr<const NodeAnimationGroup> GetAnimationGroup(std::string_view name) const noexcept;


			/*
				Node animation groups
				Removing
			*/

			//Clear all removable node animation groups from this manager
			void ClearAnimationGroups() noexcept;

			//Remove a removable node animation group from this manager
			bool RemoveAnimationGroup(NodeAnimationGroup &node_animation_group) noexcept;

			//Remove a removable node animation group with the given name from this manager
			bool RemoveAnimationGroup(std::string_view name) noexcept;


			/*
				Elapse time
			*/

			//Elapse the total time for this node animation manager by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;
	};
} //ion::graphics::scene::graph::animations

#endif