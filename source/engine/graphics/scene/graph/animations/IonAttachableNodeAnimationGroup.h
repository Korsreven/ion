/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonAttachableNodeAnimationGroup.h
-------------------------------------------
*/

#ifndef ION_ATTACHABLE_NODE_ANIMATION_GROUP
#define ION_ATTACHABLE_NODE_ANIMATION_GROUP

#include <optional>

#include "IonNodeAnimationGroup.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph::animations
{
	class NodeAnimationTimeline; //Forward declaration
	using namespace types::type_literals;

	namespace attachable_node_animation_group
	{
		namespace detail
		{
		} //detail
	} //attachable_node_animation_group


	class AttachableNodeAnimationGroup final : public managed::ManagedObject<NodeAnimationTimeline>
	{
		private:

			duration start_time_ = 0.0_sec;
			bool enable_ = true;

			std::optional<NodeAnimationGroup> node_animation_group_;
			NonOwningPtr<NodeAnimationGroup> initial_node_animation_group_;


			void NotifyUpdate() noexcept;

		public:

			//Construct a new attachable node animation group with the given node animation group, start time and whether it is enabled or not
			explicit AttachableNodeAnimationGroup(NonOwningPtr<NodeAnimationGroup> node_animation_group,
				duration start_time = 0.0_sec, bool enable = true) noexcept;


			/*
				Modifiers
			*/

			//Sets the start time of this node animation group to the given time
			inline void StartTime(duration time) noexcept
			{
				start_time_ = time;
				NotifyUpdate();
			}

			//Enable the node animation group
			inline void Enable() noexcept
			{
				enable_ = true;
			}

			//Disable the node animation group
			inline void Disable() noexcept
			{
				enable_ = false;
			}


			//Reset this node animation group
			void Reset() noexcept;

			//Revert to the initial node animation group
			void Revert();


			/*
				Observers
			*/

			//Returns the start time of this node animation group
			[[nodiscard]] inline auto StartTime() const noexcept
			{
				return start_time_;
			}

			//Returns the total duration of this node animation group
			[[nodiscard]] inline auto TotalDuration() const noexcept
			{
				return node_animation_group_ ?
					node_animation_group_->TotalDuration() :
					0.0_sec;
			}

			//Returns true if this node animation group is enabled
			[[nodiscard]] inline auto IsEnabled() const noexcept
			{
				return enable_;
			}


			//Returns an immutable reference to the node animation group
			[[nodiscard]] auto& Get() const noexcept
			{
				return node_animation_group_;
			}


			/*
				Elapse time
			*/

			//Elapse the total time for this node animation group by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time, duration current_time) noexcept;
	};
} //ion::graphics::scene::graph::animations

#endif