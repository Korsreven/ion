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

#include <optional>
#include <string>
#include <string_view>

#include "IonNodeAnimation.h"
#include "IonNodeAnimationGroup.h"
#include "IonNodeAnimationTimeline.h"
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


	///@brief A class that manages and stores node animations, animations groups and timelines
	class NodeAnimationManager :
		public managed::ObjectManager<NodeAnimation, NodeAnimationManager>,
		public managed::ObjectManager<NodeAnimationGroup, NodeAnimationManager>,
		public managed::ObjectManager<NodeAnimationTimeline, NodeAnimationManager>
	{
		private:

			SceneNode &parent_node_;

		public:

			using NodeAnimationBase = managed::ObjectManager<NodeAnimation, NodeAnimationManager>;
			using NodeAnimationGroupBase = managed::ObjectManager<NodeAnimationGroup, NodeAnimationManager>;
			using NodeAnimationTimelineBase = managed::ObjectManager<NodeAnimationTimeline, NodeAnimationManager>;


			///@brief Constructs a new node animation manager with the given scene node
			NodeAnimationManager(SceneNode &scene_node) noexcept;

			///@brief Deleted copy constructor
			NodeAnimationManager(const NodeAnimationManager&) = delete;

			///@brief Default move constructor
			NodeAnimationManager(NodeAnimationManager&&) = default;

			///@brief Virtual destructor
			virtual ~NodeAnimationManager() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			NodeAnimationManager& operator=(const NodeAnimationManager&) = delete;

			///@brief Default move assignment
			NodeAnimationManager& operator=(NodeAnimationManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all node animations in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Animations() noexcept
			{
				return NodeAnimationBase::Objects();
			}

			///@brief Returns an immutable range of all node animations in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Animations() const noexcept
			{
				return NodeAnimationBase::Objects();
			}


			///@brief Returns a mutable range of all node animation groups in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AnimationGroups() noexcept
			{
				return NodeAnimationGroupBase::Objects();
			}

			///@brief Returns an immutable range of all node animation groups in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AnimationGroups() const noexcept
			{
				return NodeAnimationGroupBase::Objects();
			}


			///@brief Returns a mutable range of all node animation timelines in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Timelines() noexcept
			{
				return NodeAnimationTimelineBase::Objects();
			}

			///@brief Returns an immutable range of all node animation timelines in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Timelines() const noexcept
			{
				return NodeAnimationTimelineBase::Objects();
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns a mutable reference to the parent node for this manager
			[[nodiscard]] inline auto& ParentNode() noexcept
			{
				return parent_node_;
			}

			///@brief Returns an immutable reference to the parent node for this manager
			[[nodiscard]] inline auto& ParentNode() const noexcept
			{
				return parent_node_;
			}

			///@}

			/**
				@name Node animations - Creating
				@{
			*/

			///@brief Creates a node animation with the given name
			NonOwningPtr<NodeAnimation> CreateAnimation(std::optional<std::string> name = {});


			///@brief Creates a node animation as a copy of the given node animation
			NonOwningPtr<NodeAnimation> CreateAnimation(const NodeAnimation &node_animation);

			///@brief Creates a node animation by moving the given node animation
			NonOwningPtr<NodeAnimation> CreateAnimation(NodeAnimation &&node_animation);

			///@}

			/**
				@name Node animations - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable node animation with the given name
			///@details Returns nullptr if node animation could not be found
			[[nodiscard]] NonOwningPtr<NodeAnimation> GetAnimation(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable node animation with the given name
			///@details Returns nullptr if node animation could not be found
			[[nodiscard]] NonOwningPtr<const NodeAnimation> GetAnimation(std::string_view name) const noexcept;

			///@}

			/**
				@name Node animations - Removing
				@{
			*/

			///@brief Clears all removable node animations from this manager
			void ClearAnimations() noexcept;

			///@brief Removes a removable node animation from this manager
			bool RemoveAnimation(NodeAnimation &node_animation) noexcept;

			///@brief Removes a removable node animation with the given name from this manager
			bool RemoveAnimation(std::string_view name) noexcept;

			///@}

			/**
				@name Node animation groups - Creating
				@{
			*/

			///@brief Creates a node animation group with the given name
			NonOwningPtr<NodeAnimationGroup> CreateAnimationGroup(std::optional<std::string> name = {});


			///@brief Creates a node animation group as a copy of the given node animation group
			NonOwningPtr<NodeAnimationGroup> CreateAnimationGroup(const NodeAnimationGroup &node_animation_group);

			///@brief Creates a node animation group by moving the given node animation group
			NonOwningPtr<NodeAnimationGroup> CreateAnimationGroup(NodeAnimationGroup &&node_animation_group);

			///@}

			/**
				@name Node animation groups - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable node animation group with the given name
			///@details Returns nullptr if node animation group could not be found
			[[nodiscard]] NonOwningPtr<NodeAnimationGroup> GetAnimationGroup(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable node animation group with the given name
			///@details Returns nullptr if node animation group could not be found
			[[nodiscard]] NonOwningPtr<const NodeAnimationGroup> GetAnimationGroup(std::string_view name) const noexcept;

			///@}

			/**
				@name Node animation groups - Removing
				@{
			*/

			///@brief Clears all removable node animation groups from this manager
			void ClearAnimationGroups() noexcept;

			///@brief Removes a removable node animation group from this manager
			bool RemoveAnimationGroup(NodeAnimationGroup &node_animation_group) noexcept;

			///@brief Removes a removable node animation group with the given name from this manager
			bool RemoveAnimationGroup(std::string_view name) noexcept;

			///@}

			/**
				@name Node animation timelines - Creating
				@{
			*/

			///@brief Creates a node animation timeline with the given name, playback rate and whether it is running or not
			NonOwningPtr<NodeAnimationTimeline> CreateTimeline(std::optional<std::string> name = {}, real playback_rate = 1.0_r, bool running = true);

			///@}

			/**
				@name Node animation timelines - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable node animation timeline with the given name
			///@details Returns nullptr if node animation timeline could not be found
			[[nodiscard]] NonOwningPtr<NodeAnimationTimeline> GetTimeline(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable node animation timeline with the given name
			///@details Returns nullptr if node animation timeline could not be found
			[[nodiscard]] NonOwningPtr<const NodeAnimationTimeline> GetTimeline(std::string_view name) const noexcept;

			///@}

			/**
				@name Node animation timelines - Removing
				@{
			*/

			///@brief Clears all removable node animation timelines from this manager
			void ClearTimelines() noexcept;

			///@brief Removes a removable node animation timeline from this manager
			bool RemoveTimeline(NodeAnimationTimeline &node_animation_timeline) noexcept;

			///@brief Removes a removable node animation timeline with the given name from this manager
			bool RemoveTimeline(std::string_view name) noexcept;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this node animation manager by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;

			///@}
	};
} //ion::graphics::scene::graph::animations

#endif