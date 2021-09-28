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

#ifndef ION_NODE_ANIMATION_GROUP_H
#define ION_NODE_ANIMATION_GROUP_H

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
			using animation_container = std::vector<AttachableNodeAnimation>;
		} //detail
	} //node_animation_group


	class NodeAnimationGroup final : public managed::ManagedObject<NodeAnimationManager>
	{
		private:

			duration total_duration_ = 0.0_sec;
			node_animation_group::detail::animation_container animations_;

		public:

			using managed::ManagedObject<NodeAnimationManager>::ManagedObject;

			//Construct a new node animation group with the given name
			NodeAnimationGroup(std::string name) noexcept;


			/*
				Ranges
			*/

			//Returns a mutable range of all node animations in this node animation group
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Animations() noexcept
			{
				return adaptors::ranges::Iterable<node_animation_group::detail::animation_container&>{animations_};
			}

			//Returns an immutable range of all node animations in this node animation group
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Animations() const noexcept
			{
				return adaptors::ranges::Iterable<const node_animation_group::detail::animation_container&>{animations_};
			}


			/*
				Modifiers
			*/

			//Reset this node animation group
			void Reset() noexcept;


			/*
				Observers
			*/

			//Returns the total duration of this node animation group
			[[nodiscard]] inline auto TotalDuration() const noexcept
			{
				return total_duration_;
			}


			/*
				Playback
			*/

			//Returns a newly created timeline with this animation group attached to it
			NonOwningPtr<NodeAnimationTimeline> Start(real playback_rate = 1.0_r, bool running = true) noexcept;


			/*
				Node animations
			*/

			//Add the given node animation to this node animation group
			void Add(NonOwningPtr<NodeAnimation> node_animation, duration start_time = 0.0_sec, bool enable = true);

			//Clear all node animations from this node animation group
			void Clear() noexcept;
	};
} //ion::graphics::scene::graph::animations

#endif