/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimationTimeline.h
-------------------------------------------
*/

#ifndef ION_NODE_ANIMATION_TIMELINE_H
#define ION_NODE_ANIMATION_TIMELINE_H

#include "IonAttachableNodeAnimation.h"
#include "IonAttachableNodeAnimationGroup.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph::animations
{
	using namespace types::type_literals;

	namespace node_animation_timeline::detail
	{
	} //node_animation_timeline::detail


	class NodeAnimationTimeline final :
		public managed::ManagedObject<NodeAnimationManager>,
		public managed::ObjectManager<AttachableNodeAnimation, NodeAnimationTimeline>,
		public managed::ObjectManager<AttachableNodeAnimationGroup, NodeAnimationTimeline>
	{
		private:

			

		public:

			using NodeAnimationBase = managed::ObjectManager<AttachableNodeAnimation, NodeAnimationTimeline>;
			using NodeAnimationGroupBase = managed::ObjectManager<AttachableNodeAnimationGroup, NodeAnimationTimeline>;


			using managed::ManagedObject<NodeAnimationManager>::ManagedObject;

			//Deleted copy constructor
			NodeAnimationTimeline(const NodeAnimationTimeline&) = delete;

			//Default move constructor
			NodeAnimationTimeline(NodeAnimationTimeline&&) = default;

			//Destructor
			~NodeAnimationTimeline() = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			NodeAnimationTimeline& operator=(const NodeAnimationTimeline&) = delete;

			//Move assignment
			NodeAnimationTimeline& operator=(NodeAnimationTimeline&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all attached node animations in this timeline
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimations() noexcept
			{
				return NodeAnimationBase::Objects();
			}

			//Returns an immutable range of all attached node animations in this timeline
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimations() const noexcept
			{
				return NodeAnimationBase::Objects();
			}


			//Returns a mutable range of all attached node animation groups in this timeline
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimationGroups() noexcept
			{
				return NodeAnimationGroupBase::Objects();
			}

			//Returns an immutable range of all attached node animation groups in this timeline
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimationGroups() const noexcept
			{
				return NodeAnimationGroupBase::Objects();
			}


			/*
				Animations
				Attaching
			*/

			//Attaches the given node animation with a given start time, to this node animation timeline
			NonOwningPtr<AttachableNodeAnimation> Attach(NonOwningPtr<NodeAnimation> node_animation, duration start_time = 0.0_sec);


			/*
				Node animations
				Detaching
			*/

			//Detaches all removable node animations from this timeline
			void DetachAllAnimations() noexcept;

			//Detaches a removable node animation from this timeline
			bool DetachAnimation(AttachableNodeAnimation &node_animation) noexcept;


			/*
				Animation groups
				Attaching
			*/

			NonOwningPtr<AttachableNodeAnimationGroup> Attach(NonOwningPtr<NodeAnimationGroup> node_animation_group, duration start_time = 0.0_sec);


			/*
				Node animation groups
				Detaching
			*/

			//Detaches all removable node animation groups from this timeline
			void DetachAllAnimationGroups() noexcept;

			//Detaches a removable node animation group from this timeline
			bool DetachAnimationGroup(AttachableNodeAnimationGroup &node_animation_group) noexcept;


			/*
				Elapse time
			*/

			//Elapse the total time for this node animation timeline by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;
	};
} //ion::graphics::scene::graph::animations

#endif