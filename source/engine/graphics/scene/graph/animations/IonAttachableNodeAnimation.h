/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonAttachableNodeAnimation.h
-------------------------------------------
*/

#ifndef ION_ATTACHABLE_NODE_ANIMATION
#define ION_ATTACHABLE_NODE_ANIMATION

#include <optional>

#include "IonNodeAnimation.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph::animations
{
	class NodeAnimationTimeline; //Forward declaration
	using namespace types::type_literals;

	namespace attachable_node_animation
	{
		namespace detail
		{
		} //detail
	} //attachable_node_animation


	class AttachableNodeAnimation final : public managed::ManagedObject<NodeAnimationTimeline>
	{
		private:

			duration start_time_ = 0.0_sec;
			bool enable_ = true;

			std::optional<NodeAnimation> node_animation_;
			NonOwningPtr<NodeAnimation> initial_node_animation_;

		public:

			//Construct a new attachable node animation with the given node animation, start time and whether it is enabled or not
			explicit AttachableNodeAnimation(NonOwningPtr<NodeAnimation> node_animation,
				duration start_time = 0.0_sec, bool enable = true) noexcept;


			/*
				Modifiers
			*/

			//Returns the start time of this node animation
			inline void StartTime(duration time) noexcept
			{
				start_time_ = time;
			}

			//Enable the node animation
			inline void Enable() noexcept
			{
				enable_ = true;
			}

			//Disable the node animation
			inline void Disable() noexcept
			{
				enable_ = false;
			}


			//Revert to the initial node animation
			void Revert();


			/*
				Observers
			*/

			//Returns the start time of this node animation
			[[nodiscard]] inline auto StartTime() const noexcept
			{
				return start_time_;
			}

			//Returns true if this node animation is enabled
			[[nodiscard]] inline auto IsEnabled() const noexcept
			{
				return enable_;
			}


			//Returns a mutable reference to the node animation
			[[nodiscard]] auto& Get() noexcept
			{
				return node_animation_;
			}

			//Returns an immutable reference to the node animation
			[[nodiscard]] auto& Get() const noexcept
			{
				return node_animation_;
			}


			/*
				Elapse time
			*/

			//Elapse the total time for this node animation by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time, duration start_time = 0.0_sec) noexcept;
	};
} //ion::graphics::scene::graph::animations

#endif