/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph/animations
File:	IonNodeAnimationTimeline.cpp
-------------------------------------------
*/

#include "IonNodeAnimationTimeline.h"

#include <algorithm>
#include <type_traits>

#include "adaptors/ranges/IonIterable.h"

namespace ion::graphics::scene::graph::animations
{

using namespace node_animation_timeline;

namespace node_animation_timeline::detail
{

} //node_animation_timeline::detail


//Private

void NodeAnimationTimeline::ResetCycle() noexcept
{
	current_time_ = 0.0_sec;
	reverse_ = false;
	
	for (auto &animation : AttachedAnimations())
		animation.Reset();

	for (auto &animation_group : AttachedAnimationGroups())
		animation_group.Reset();
}

duration NodeAnimationTimeline::RetrieveTotalDuration() const noexcept
{
	auto total_duration = 0.0_sec;

	if (!std::empty(attached_animations_))
	{
		if (auto animation_group = attached_animations_.back().group_ptr; animation_group)
			total_duration = std::max(total_duration_, animation_group->StartTime() + animation_group->TotalDuration());

		else if (auto animation = attached_animations_.back().ptr; animation)
			total_duration = std::max(total_duration_, animation->StartTime() + animation->TotalDuration());
	}

	return total_duration;
}


/*
	Events
*/

void NodeAnimationTimeline::Created(AttachableNodeAnimation &animation) noexcept
{
	detail::attached_animation a{&animation, nullptr};
	attached_animations_.insert(
		std::upper_bound(std::begin(attached_animations_), std::end(attached_animations_), a),
		a);
}

void NodeAnimationTimeline::Created(AttachableNodeAnimationGroup &animation_group) noexcept
{
	if (auto &group = animation_group.Get(); group)
	{
		for (auto &animation : group->Animations())
		{
			detail::attached_animation a{&const_cast<AttachableNodeAnimation&>(animation), nullptr};
			attached_animations_.insert(
				std::upper_bound(std::begin(attached_animations_), std::end(attached_animations_), a),
				a);
		}
	}
}


void NodeAnimationTimeline::Removed(AttachableNodeAnimation &animation) noexcept
{
	if (auto iter = std::find_if(std::begin(attached_animations_), std::end(attached_animations_),
		[&](auto &a) noexcept
		{
			return a.ptr == &animation;
		}); iter != std::end(attached_animations_))

		attached_animations_.erase(iter);
}

void NodeAnimationTimeline::Removed(AttachableNodeAnimationGroup &animation_group) noexcept
{
	attached_animations_.erase(
		std::remove_if(std::begin(attached_animations_), std::end(attached_animations_),
			[&](auto &a) noexcept
			{
				return a.group_ptr == &animation_group;
			}), std::end(attached_animations_));
}


//Public

NodeAnimationTimeline::NodeAnimationTimeline(std::optional<std::string> name, real playback_rate, bool running) noexcept :
	
	managed::ManagedObject<NodeAnimationManager>{std::move(name)},

	playback_rate_{playback_rate},
	running_{running}
{
	//Empty
}


/*
	Modifiers
*/

void NodeAnimationTimeline::RepeatCount(std::optional<int> repeat_count) noexcept
{
	if (repeat_count)
	{
		auto current_cycle = repeat_count_ ? repeat_count_->first : 0;
		repeat_count_.emplace(current_cycle, *repeat_count < current_cycle ? current_cycle : *repeat_count);
	}
	else
		repeat_count_.reset();
}


void NodeAnimationTimeline::Refresh() noexcept
{
	std::stable_sort(std::begin(attached_animations_), std::end(attached_animations_));

	total_duration_ = RetrieveTotalDuration();
	current_time_ = std::clamp(current_time_, 0.0_sec, total_duration_);
}


/*
	Playback
*/

void NodeAnimationTimeline::Start() noexcept
{
	running_ = true;
}

void NodeAnimationTimeline::Stop() noexcept
{
	running_ = false;
	reverse_ = false;
}

void NodeAnimationTimeline::Reset() noexcept
{
	running_ = false;
	ResetCycle();

	if (repeat_count_)
		repeat_count_->first = 0;
}

void NodeAnimationTimeline::Restart() noexcept
{
	Reset();
	Start();
}


void NodeAnimationTimeline::Revert(duration total_duration)
{
	//Something to revert
	if (current_time_ > 0.0_sec)
	{
		reverse_ = true;

		//Over time
		if (total_duration > 0.0_sec)
			reverse_playback_rate_ = current_time_ / total_duration;
		else //Instantaneously
		{
			reverse_playback_rate_ = 1.0_r;
			Elapse(current_time_);
		}
	}
}


/*
	Node animations
	Attaching
*/

NonOwningPtr<AttachableNodeAnimation> NodeAnimationTimeline::Attach(NonOwningPtr<NodeAnimation> node_animation,
	duration start_time, bool enabled)
{
	auto ptr = NodeAnimationBase::Create(node_animation, start_time, enabled);
	total_duration_ = std::max(total_duration_, ptr->StartTime() +
		(node_animation ? node_animation->TotalDuration() : 0.0_sec));
	return ptr;
}


/*
	Node animations
	Detaching
*/

void NodeAnimationTimeline::DetachAllAnimations() noexcept
{
	attached_animations_.erase(
		std::remove_if(std::begin(attached_animations_), std::end(attached_animations_),
			[](auto &a) noexcept
			{
				return !a.group_ptr; //Not in animation group
			}), std::end(attached_animations_));

	NodeAnimationBase::Clear();
	total_duration_ = RetrieveTotalDuration();
	current_time_ = std::clamp(current_time_, 0.0_sec, total_duration_);
}

bool NodeAnimationTimeline::DetachAnimation(AttachableNodeAnimation &node_animation) noexcept
{
	auto total_duration = node_animation.TotalDuration();
	auto removed = NodeAnimationBase::Remove(node_animation);

	if (removed && total_duration == total_duration_)
	{
		total_duration_ = RetrieveTotalDuration();
		current_time_ = std::clamp(current_time_, 0.0_sec, total_duration_);
	}

	return removed;
}


/*
	Animation groups
	Attaching
*/

NonOwningPtr<AttachableNodeAnimationGroup> NodeAnimationTimeline::Attach(NonOwningPtr<NodeAnimationGroup> node_animation_group,
	duration start_time, bool enabled)
{
	auto ptr = NodeAnimationGroupBase::Create(node_animation_group, start_time, enabled);
	total_duration_ = std::max(total_duration_, ptr->StartTime() +
		(node_animation_group ? node_animation_group->TotalDuration() : 0.0_sec));
	return ptr;
}


/*
	Node animation groups
	Detaching
*/

void NodeAnimationTimeline::DetachAllAnimationGroups() noexcept
{
	attached_animations_.erase(
		std::remove_if(std::begin(attached_animations_), std::end(attached_animations_),
			[](auto &a) noexcept
			{
				return !!a.group_ptr; //In animation group
			}), std::end(attached_animations_));

	NodeAnimationGroupBase::Clear();
	total_duration_ = RetrieveTotalDuration();
	current_time_ = std::clamp(current_time_, 0.0_sec, total_duration_);
}

bool NodeAnimationTimeline::DetachAnimationGroup(AttachableNodeAnimationGroup &node_animation_group) noexcept
{
	auto total_duration = node_animation_group.TotalDuration();
	auto removed = NodeAnimationGroupBase::Remove(node_animation_group);

	if (removed && total_duration == total_duration_)
	{
		total_duration_ = RetrieveTotalDuration();
		current_time_ = std::clamp(current_time_, 0.0_sec, total_duration_);
	}

	return removed;
}


/*
	Detaching
*/

void NodeAnimationTimeline::DetachAll() noexcept
{
	attached_animations_.clear();
	attached_animations_.shrink_to_fit();

	NodeAnimationBase::Clear();
	NodeAnimationGroupBase::Clear();

	current_time_ = total_duration_ = 0.0_sec;
}


/*
	Elapse time
*/

void NodeAnimationTimeline::Elapse(duration time) noexcept
{
	if (running_)
	{
		if (reverse_)
			time = -time;

		time *= (reverse_ ? reverse_playback_rate_ : playback_rate_);
		current_time_ += time;

		//Reverse
		if (reverse_)
		{
			for (auto &a : adaptors::ranges::ReverseIterable<detail::attached_animations&>(attached_animations_))
				a.ptr->Elapse(time, current_time_, a.group_ptr ? a.group_ptr->StartTime() : 0.0_sec);
		}
		else //Forward
		{
			for (auto &a : attached_animations_)
				a.ptr->Elapse(time, current_time_, a.group_ptr ? a.group_ptr->StartTime() : 0.0_sec);
		}


		//A timeline cycle has been completed
		if (reverse_ ?
			current_time_ <= 0.0_sec :
			current_time_ >= total_duration_)
		{
			//Loop (next cycle)
			if (!reverse_ && //Forwarding
				(!repeat_count_ || //Indefinitely
				  repeat_count_->first < repeat_count_->second))
			{
				if (repeat_count_)
					++repeat_count_->first;

				//Finish cycle
				if (on_finish_cycle_ &&
					current_time_ >= total_duration_ && current_time_ - time < total_duration_)
					(*on_finish_cycle_)(*this);

				ResetCycle();
			}
			//Timeline is done
			else
			{
				if (reverse_)
				{
					//Finish revert
					if (on_finish_revert_ &&
						current_time_ <= 0.0_sec && current_time_ - time > 0.0_sec)
						(*on_finish_revert_)(*this);
				}
				else
				{
					//Finish
					if (on_finish_ &&
						current_time_ >= total_duration_ && current_time_ - time < total_duration_)
						(*on_finish_)(*this);
				}

				Reset();
			}
		}
	}
}

} //ion::graphics::scene::graph::animations