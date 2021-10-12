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

#include <optional>
#include <utility>
#include <vector>

#include "IonAttachableNodeAnimation.h"
#include "IonAttachableNodeAnimationGroup.h"
#include "events/IonCallback.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph::animations
{
	using namespace types::type_literals;

	namespace node_animation_timeline::detail
	{
		struct attached_animation
		{
			AttachableNodeAnimation *ptr = nullptr;
			AttachableNodeAnimationGroup *group_ptr = nullptr;

			inline auto operator<(const attached_animation &rhs) const noexcept
			{
				return (group_ptr ? group_ptr->StartTime() : 0.0_sec) + ptr->StartTime() + ptr->TotalDuration() <
					(rhs.group_ptr ? rhs.group_ptr->StartTime() : 0.0_sec) + rhs.ptr->StartTime() + rhs.ptr->TotalDuration();
			}
		};

		using attached_animations = std::vector<attached_animation>; //Non-owning
	} //node_animation_timeline::detail


	class NodeAnimationTimeline final :
		public managed::ManagedObject<NodeAnimationManager>,
		public managed::ObjectManager<AttachableNodeAnimation, NodeAnimationTimeline>,
		public managed::ObjectManager<AttachableNodeAnimationGroup, NodeAnimationTimeline>
	{
		private:

			duration current_time_ = 0.0_sec;
			duration total_duration_ = 0.0_sec;
			std::optional<std::pair<int, int>> repeat_count_;
			real playback_rate_ = 1.0_r;
			real reverse_playback_rate_ = 1.0_r;
			bool running_ = true;
			bool reverse_ = false;

			std::optional<events::Callback<void, NodeAnimationTimeline&>> on_finish_;
			std::optional<events::Callback<void, NodeAnimationTimeline&>> on_finish_cycle_;
			std::optional<events::Callback<void, NodeAnimationTimeline&>> on_finish_revert_;

			node_animation_timeline::detail::attached_animations attached_animations_;
				//Sorted for internal use only


			void ResetCycle() noexcept;
			duration RetrieveTotalDuration() const noexcept;


			/*
				Events
			*/

			//See ObjectManager::Created for more details
			void Created(AttachableNodeAnimation &animation) noexcept override final;

			//See ObjectManager::Created for more details
			void Created(AttachableNodeAnimationGroup &animation_group) noexcept override final;


			//See ObjectManager::Removed for more details
			void Removed(AttachableNodeAnimation &animation) noexcept override final;

			//See ObjectManager::Removed for more details
			void Removed(AttachableNodeAnimationGroup &animation_group) noexcept override final;

		public:

			using NodeAnimationBase = managed::ObjectManager<AttachableNodeAnimation, NodeAnimationTimeline>;
			using NodeAnimationGroupBase = managed::ObjectManager<AttachableNodeAnimationGroup, NodeAnimationTimeline>;


			using managed::ManagedObject<NodeAnimationManager>::ManagedObject;

			//Construct a new timeline with the given playback rate and whether it is running or not
			explicit NodeAnimationTimeline(real playback_rate, bool running = true) noexcept;

			//Construct a new timeline with the given name, playback rate and whether it is running or not
			NodeAnimationTimeline(std::string name, real playback_rate, bool running = true) noexcept;

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
				Modifiers
			*/

			//Sets the given repeat count to the given value
			//A repeat count of 0 means that the timeline will not loop
			//If repeat count is nullopt, the timeline will loop indefinitely
			void RepeatCount(std::optional<int> repeat_count) noexcept;

			//Sets the given playback rate to the given rate in range (0.0, oo)
			inline void PlaybackRate(real rate) noexcept
			{
				if (rate > 0.0_r)
					playback_rate_ = rate;
			}


			//Sets the on finish callback
			inline void OnFinish(events::Callback<void, NodeAnimationTimeline&> on_finish) noexcept
			{
				on_finish_ = on_finish;
			}

			//Sets the on finish callback
			inline void OnFinish(std::nullopt_t) noexcept
			{
				on_finish_ = {};
			}


			//Sets the on finish cycle callback
			inline void OnFinishCycle(events::Callback<void, NodeAnimationTimeline&> on_finish_cycle) noexcept
			{
				on_finish_cycle_ = on_finish_cycle;
			}

			//Sets the on finish cycle callback
			inline void OnFinishCycle(std::nullopt_t) noexcept
			{
				on_finish_cycle_ = {};
			}


			//Sets the on finish revert callback
			inline void OnFinishRevert(events::Callback<void, NodeAnimationTimeline&> on_finish_revert) noexcept
			{
				on_finish_revert_ = on_finish_revert;
			}

			//Sets the on finish revert callback
			inline void OnFinishRevert(std::nullopt_t) noexcept
			{
				on_finish_revert_ = {};
			}


			//Refreshes the current time and total duration of this timeline
			//This function is typically called by an attached animation or animation group
			void Refresh() noexcept;


			/*
				Observers
			*/

			//Returns the current time of this timeline
			[[nodiscard]] inline auto CurrentTime() const noexcept
			{
				return current_time_;
			}

			//Returns the total duration of this timeline
			[[nodiscard]] inline auto TotalDuration() const noexcept
			{
				return total_duration_;
			}

			//Returns the total percent of this timeline
			[[nodiscard]] inline auto TotalPercent() const noexcept
			{
				return current_time_ / total_duration_;
			}

			//Returns the playback rate of this timeline
			[[nodiscard]] inline auto PlaybackRate() const noexcept
			{
				return playback_rate_;
			}


			//Returns true if this timeline is running
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_;
			}

			//Returns true if this timeline is currently in reverse
			[[nodiscard]] inline auto InReverse() const noexcept
			{
				return reverse_;
			}


			//Returns the on finish callback
			[[nodiscard]] inline auto OnFinish() const noexcept
			{
				return on_finish_;
			}

			//Returns the on finish cycle callback
			[[nodiscard]] inline auto OnFinishCycle() const noexcept
			{
				return on_finish_cycle_;
			}

			//Returns the on finish revert callback
			[[nodiscard]] inline auto OnFinishRevert() const noexcept
			{
				return on_finish_revert_;
			}


			/*
				Playback
			*/

			//Starts or resumes, timeline playback
			void Start() noexcept;

			//Stops timeline playback
			void Stop() noexcept;

			//Stops timeline playback and reset elapsed time to zero
			void Reset() noexcept;

			//Stops, resets and starts timeline playback
			void Restart() noexcept;


			//Reverts this timeline back to start by the given duration
			//The timeline is reverted by reversing the timeline playback
			//A total duration of 0 seconds is instantaneous
			void Revert(duration total_duration = 0.0_sec);


			/*
				Animations
				Attaching
			*/

			//Attaches the given node animation, to this timeline
			NonOwningPtr<AttachableNodeAnimation> Attach(NonOwningPtr<NodeAnimation> node_animation,
				duration start_time = 0.0_sec, bool enable = true);


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

			//Attaches the given node animation group, to this timeline
			NonOwningPtr<AttachableNodeAnimationGroup> Attach(NonOwningPtr<NodeAnimationGroup> node_animation_group,
				duration start_time = 0.0_sec, bool enable = true);


			/*
				Node animation groups
				Detaching
			*/

			//Detaches all removable node animation groups from this timeline
			void DetachAllAnimationGroups() noexcept;

			//Detaches a removable node animation group from this timeline
			bool DetachAnimationGroup(AttachableNodeAnimationGroup &node_animation_group) noexcept;


			/*
				Detaching
			*/

			//Detaches all removable node animations and animation groups from this timeline
			void DetachAll() noexcept;


			/*
				Elapse time
			*/

			//Elapse the total time for this timeline by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;
	};
} //ion::graphics::scene::graph::animations

#endif