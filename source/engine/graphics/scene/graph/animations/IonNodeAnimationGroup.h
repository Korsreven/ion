/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimationGroup.h
-------------------------------------------
*/

#ifndef ION_NODE_ANIMATION_GROUP
#define ION_NODE_ANIMATION_GROUP

#include <string>
#include <vector>

#include "IonAttachableNodeAnimation.h"
#include "adaptors/ranges/IonIterable.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::graph::animations
{
	class NodeAnimationManager; //Forward declaration
	class NodeAnimationTimeline; //Forward declaration

	using namespace types::type_literals;

	namespace node_animation_group
	{
		namespace detail
		{
			using attached_animations = std::vector<AttachableNodeAnimation>;
		} //detail
	} //node_animation_group


	class NodeAnimationGroup final : public managed::ManagedObject<NodeAnimationManager>
	{
		private:

			duration current_time_ = 0.0_sec;
			duration total_duration_ = 0.0_sec;

			node_animation_group::detail::attached_animations attached_animations_;

		public:

			using managed::ManagedObject<NodeAnimationManager>::ManagedObject;

			//Construct a new node animation group with the given name
			NodeAnimationGroup(std::string name) noexcept;


			/*
				Ranges
			*/

			//Returns a mutable range of all attached node animations in this node animation group
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimations() noexcept
			{
				return adaptors::ranges::Iterable<node_animation_group::detail::attached_animations&>{attached_animations_};
			}

			//Returns an immutable range of all attached node animations in this node animation group
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttachedAnimations() const noexcept
			{
				return adaptors::ranges::Iterable<const node_animation_group::detail::attached_animations&>{attached_animations_};
			}


			/*
				Observers
			*/

			//Returns the current time of this node animation group
			[[nodiscard]] inline auto CurrentTime() const noexcept
			{
				return current_time_;
			}

			//Returns the total duration of this node animation group
			[[nodiscard]] inline auto TotalDuration() const noexcept
			{
				return total_duration_;
			}

			//Returns the total percent of this node animation group
			[[nodiscard]] inline auto TotalPercent() const noexcept
			{
				return current_time_ / total_duration_;
			}


			/*
				Playback
			*/

			//Returns a newly created timeline with this animation group attached to it
			NonOwningPtr<NodeAnimationTimeline> Start(real playback_rate = 1.0_r, bool running = true) noexcept;


			/*
				Node animations
			*/

			//Attaches the given node animation with a given start time, to this node animation group
			void Attach(NonOwningPtr<NodeAnimation> node_animation, duration start_time = 0.0_sec);

			//Detaches all node animations from this node animation group
			void DetachAll() noexcept;
	};
} //ion::graphics::scene::graph::animations

#endif