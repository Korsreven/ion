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
#include <string>
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


	///@brief A class representing a node animation timeline that can contain both node animations and animation groups
	///@details The total duration of a timeline is calculated from all of the added animations and animation groups
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


			/**
				@name Events
				@{
			*/

			///@brief See ObjectManager::Created for more details
			void Created(AttachableNodeAnimation &animation) noexcept override final;

			///@brief See ObjectManager::Created for more details
			void Created(AttachableNodeAnimationGroup &animation_group) noexcept override final;


			///@brief See ObjectManager::Removed for more details
			void Removed(AttachableNodeAnimation &animation) noexcept override final;

			///@brief See ObjectManager::Removed for more details
			void Removed(AttachableNodeAnimationGroup &animation_group) noexcept override final;

			///@}

		public:

			using NodeAnimationBase = managed::ObjectManager<AttachableNodeAnimation, NodeAnimationTimeline>;
			using NodeAnimationGroupBase = managed::ObjectManager<AttachableNodeAnimationGroup, NodeAnimationTimeline>;


			using managed::ManagedObject<NodeAnimationManager>::ManagedObject;

			///@brief Constructs a new timeline with the given name, playback rate and whether it is running or not
			explicit NodeAnimationTimeline(std::optional<std::string> name = {}, real playback_rate = 1.0_r, bool running = true) noexcept;

			///@brief Deleted copy constructor
			NodeAnimationTimeline(const NodeAnimationTimeline&) = delete;

			///@brief Default move constructor
			NodeAnimationTimeline(NodeAnimationTimeline&&) = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			NodeAnimationTimeline& operator=(const NodeAnimationTimeline&) = delete;

			///@brief Default move assignment
			NodeAnimationTimeline& operator=(NodeAnimationTimeline&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all attached node animations in this timeline
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimations() noexcept
			{
				return NodeAnimationBase::Objects();
			}

			///@brief Returns an immutable range of all attached node animations in this timeline
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimations() const noexcept
			{
				return NodeAnimationBase::Objects();
			}


			///@brief Returns a mutable range of all attached node animation groups in this timeline
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimationGroups() noexcept
			{
				return NodeAnimationGroupBase::Objects();
			}

			///@brief Returns an immutable range of all attached node animation groups in this timeline
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimationGroups() const noexcept
			{
				return NodeAnimationGroupBase::Objects();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the given repeat count to the given value
			///@details A repeat count of 0 means that the timeline will not loop.
			///If repeat count is nullopt, the timeline will loop indefinitely
			void RepeatCount(std::optional<int> repeat_count) noexcept;

			///@brief Sets the given playback rate to the given rate in range (0.0, oo)
			inline void PlaybackRate(real rate) noexcept
			{
				if (rate > 0.0_r)
					playback_rate_ = rate;
			}


			///@brief Sets the on finish callback
			inline void OnFinish(events::Callback<void, NodeAnimationTimeline&> on_finish) noexcept
			{
				on_finish_ = on_finish;
			}

			///@brief Sets the on finish callback
			inline void OnFinish(std::nullopt_t) noexcept
			{
				on_finish_ = {};
			}


			///@brief Sets the on finish cycle callback
			inline void OnFinishCycle(events::Callback<void, NodeAnimationTimeline&> on_finish_cycle) noexcept
			{
				on_finish_cycle_ = on_finish_cycle;
			}

			///@brief Sets the on finish cycle callback
			inline void OnFinishCycle(std::nullopt_t) noexcept
			{
				on_finish_cycle_ = {};
			}


			///@brief Sets the on finish revert callback
			inline void OnFinishRevert(events::Callback<void, NodeAnimationTimeline&> on_finish_revert) noexcept
			{
				on_finish_revert_ = on_finish_revert;
			}

			///@brief Sets the on finish revert callback
			inline void OnFinishRevert(std::nullopt_t) noexcept
			{
				on_finish_revert_ = {};
			}


			///@brief Refreshes the current time and total duration of this timeline
			///@details This function is typically called by an attached animation or animation group
			void Refresh() noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the current time of this timeline
			[[nodiscard]] inline auto CurrentTime() const noexcept
			{
				return current_time_;
			}

			///@brief Returns the total duration of this timeline
			[[nodiscard]] inline auto TotalDuration() const noexcept
			{
				return total_duration_;
			}

			///@brief Returns the total percent of this timeline
			[[nodiscard]] inline auto TotalPercent() const noexcept
			{
				return current_time_ / total_duration_;
			}

			///@brief Returns the playback rate of this timeline
			[[nodiscard]] inline auto PlaybackRate() const noexcept
			{
				return playback_rate_;
			}


			///@brief Returns true if this timeline is running
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_;
			}

			///@brief Returns true if this timeline is currently in reverse
			[[nodiscard]] inline auto InReverse() const noexcept
			{
				return reverse_;
			}


			///@brief Returns the on finish callback
			[[nodiscard]] inline auto OnFinish() const noexcept
			{
				return on_finish_;
			}

			///@brief Returns the on finish cycle callback
			[[nodiscard]] inline auto OnFinishCycle() const noexcept
			{
				return on_finish_cycle_;
			}

			///@brief Returns the on finish revert callback
			[[nodiscard]] inline auto OnFinishRevert() const noexcept
			{
				return on_finish_revert_;
			}

			///@}

			/**
				@name Playback
				@{
			*/

			///@brief Starts or resumes, timeline playback
			void Start() noexcept;

			///@brief Stops timeline playback
			void Stop() noexcept;

			///@brief Stops timeline playback and reset elapsed time to zero
			void Reset() noexcept;

			///@brief Stops, resets and starts timeline playback
			void Restart() noexcept;


			///@brief Reverts this timeline back to start by the given duration
			///@details The timeline is reverted by reversing the timeline playback.
			///A total duration of 0 seconds is instantaneous
			void Revert(duration total_duration = 0.0_sec);

			///@}

			/**
				@name Animations - Attaching
				@{
			*/

			///@brief Attaches the given node animation, to this timeline
			NonOwningPtr<AttachableNodeAnimation> Attach(NonOwningPtr<NodeAnimation> node_animation,
				duration start_time = 0.0_sec, bool enabled = true);

			///@}

			/**
				@name Node animations - Detaching
				@{
			*/

			///@brief Detaches all removable node animations from this timeline
			void DetachAllAnimations() noexcept;

			///@brief Detaches a removable node animation from this timeline
			bool DetachAnimation(AttachableNodeAnimation &node_animation) noexcept;

			///@}

			/**
				@name Animation groups - Attaching
				@{
			*/

			///@brief Attaches the given node animation group, to this timeline
			NonOwningPtr<AttachableNodeAnimationGroup> Attach(NonOwningPtr<NodeAnimationGroup> node_animation_group,
				duration start_time = 0.0_sec, bool enabled = true);

			///@}

			/**
				@name Node animation groups - Detaching
				@{
			*/

			///@brief Detaches all removable node animation groups from this timeline
			void DetachAllAnimationGroups() noexcept;

			///@brief Detaches a removable node animation group from this timeline
			bool DetachAnimationGroup(AttachableNodeAnimationGroup &node_animation_group) noexcept;

			///@}

			/**
				@name Detaching
				@{
			*/

			///@brief Detaches all removable node animations and animation groups from this timeline
			void DetachAll() noexcept;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this timeline by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;

			///@}
	};
} //ion::graphics::scene::graph::animations

#endif