/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonAnimation.cpp
-------------------------------------------
*/

#include "IonAnimation.h"

namespace ion::graphics::textures
{

using namespace animation;

namespace animation::detail
{
} //animation::detail


//Private

bool Animation::AddFrame(Texture &frame)
{
	if (textures_.Observe(frame))
	{
		frames_.push_back(&frame);
		return true;
	}
	else
		return false;
}

bool Animation::AddFrames(const detail::container_type &frames)
{
	for (auto &frame : frames)
	{
		if (!frame || !AddFrame(*frame))
		{
			ClearFrames(); //Missing a frame
			return false;
		}
	}

	total_observed_textures_ = std::ssize(textures_.Objects());
	return !std::empty(frames_);
}

void Animation::ClearFrames() noexcept
{
	frames_.clear();
	frames_.shrink_to_fit();
	textures_.ReleaseAll();	
}


void Animation::StepForward(bool rewind) noexcept
{
	//Forward
	if (current_frame_ < std::ssize(frames_) - 1)
		++current_frame_;

	//Loop (next cycle)
	else if (!repeat_count_ || //Indefinitely
			(rewind ?
				repeat_count_->first > 0 :
				repeat_count_->first < repeat_count_->second))
	{
		if (repeat_count_)
		{
			if (rewind)
				--repeat_count_->first;
			else
				++repeat_count_->first;
		}

		switch (direction_)
		{
			//Flip direction
			case PlaybackDirection::Alternate:
			case PlaybackDirection::AlternateReverse:
			{
				reverse_ = !reverse_;
				--current_frame_;
				break;
			}

			default:
			{
				current_frame_ = 0;
				break;
			}
		}
	}
	//Animation is done
	else
	{
		Stop();
		frame_duration_.Clamp();
			//Make sure animation stays at 100% when stopped
	}
}

void Animation::StepBackward(bool rewind) noexcept
{
	//Backward
	if (current_frame_ > 0)
		--current_frame_;

	//Loop (next cycle)
	else if (!repeat_count_ || //Indefinitely
			(rewind ?
				repeat_count_->first > 0 :
				repeat_count_->first < repeat_count_->second))
	{
		if (repeat_count_)
		{
			if (rewind)
				--repeat_count_->first;
			else
				++repeat_count_->first;
		}

		switch (direction_)
		{
			//Flip direction
			case PlaybackDirection::Alternate:
			case PlaybackDirection::AlternateReverse:
			{
				reverse_ = !reverse_;
				++current_frame_;
				break;
			}

			default:
			{
				current_frame_ = std::ssize(frames_) - 1;
				break;
			}
		}
	}
	//Animation is done
	else
	{
		Stop();
		frame_duration_.Clamp();
			//Make sure animation stays at 100% when stopped
	}
}

void Animation::Next() noexcept
{
	if (reverse_)
		StepBackward();
	else
		StepForward();
}

void Animation::Previous() noexcept
{
	if (reverse_)
		StepForward(true);
	else
		StepBackward(true);
}

//Public

Animation::Animation(const detail::container_type &frames,
	duration cycle_duration, std::optional<int> repeat_count,
	PlaybackDirection direction, real playback_rate) :
	
	frame_duration_{!std::empty(frames) ? cycle_duration / std::size(frames) : 0.0_sec},
	repeat_count_{repeat_count ? std::make_optional(std::pair{0, *repeat_count}) : std::nullopt},
	direction_{direction},
	playback_rate_{playback_rate > 0.0_r ? playback_rate : 1.0_r},
	reverse_{detail::is_direction_in_reverse(direction)},
	current_frame_{!std::empty(frames) && reverse_ ? std::ssize(frames) - 1 : 0}
{
	AddFrames(frames);
}

Animation::Animation(const detail::container_type &frames,
	duration cycle_duration, std::optional<int> repeat_count, real playback_rate) :

	Animation{frames, cycle_duration, repeat_count, PlaybackDirection::Normal, playback_rate}
{
	//Empty
}


/*
	Static animation conversions
*/

Animation Animation::Looping(const detail::container_type &frames,
	duration cycle_duration, PlaybackDirection direction, real playback_rate) noexcept
{
	return {frames, cycle_duration, std::nullopt, direction, playback_rate};
}

Animation Animation::Looping(const detail::container_type &frames,
	duration cycle_duration, real playback_rate) noexcept
{
	return {frames, cycle_duration, std::nullopt, playback_rate};
}


Animation Animation::NonLooping(const detail::container_type &frames,
	duration cycle_duration, PlaybackDirection direction, real playback_rate) noexcept
{
	return {frames, cycle_duration, 0, direction, playback_rate};
}

Animation Animation::NonLooping(const detail::container_type &frames,
	duration cycle_duration, real playback_rate) noexcept
{
	return {frames, cycle_duration, 0, playback_rate};
}


void Animation::CycleTime(duration time) noexcept
{
	if (auto cycle_duration = CycleDuration(); cycle_duration > 0.0_sec)
	{
		auto [current_duration, current_frame] =
			detail::cycle_duration_to_frame_duration(
				std::clamp(time, 0.0_sec, cycle_duration), cycle_duration,
				std::ssize(frames_), reverse_);

		frame_duration_.Total(current_duration);
		current_frame_ = current_frame;
	}
}


void Animation::CycleDuration(duration time) noexcept
{
	if (HasAllFrames() && time > 0.0_sec)
		frame_duration_.Limit(time / std::size(frames_));
}

void Animation::CyclePercent(real percent) noexcept
{
	CycleTime(CycleDuration() * percent);
}


void Animation::TotalTime(duration time) noexcept
{
	if (auto cycle_duration = CycleDuration(); cycle_duration > 0.0_sec)
	{
		if (repeat_count_)
			time = std::clamp(time, 0.0_sec, *TotalDuration());

		auto position = time / cycle_duration;
		auto current_cycle = static_cast<int>(position);

		if (repeat_count_)
			repeat_count_->first = current_cycle;

		if (current_cycle % 2 == 1)
		{
			switch (direction_)
			{
				//Flip direction
				case PlaybackDirection::Alternate:
				case PlaybackDirection::AlternateReverse:
				{
					reverse_ = !reverse_;
					break;
				}
			}
		}
		
		CyclePercent(position - current_cycle);
	}
}

void Animation::TotalDuration(duration time) noexcept
{
	CycleDuration(time / (repeat_count_ ? repeat_count_->second + 1 : 1));
}

void Animation::TotalPercent(real percent) noexcept
{
	if (repeat_count_) 
		TotalTime(*TotalDuration() * percent);
	else
		CyclePercent(percent);
}


void Animation::FrameRate(real rate) noexcept
{
	if (rate > 0.0_r)
		CycleDuration(duration{std::size(frames_) / rate});
}

void Animation::RepeatCount(std::optional<int> repeat_count) noexcept
{
	if (repeat_count)
	{
		auto current_cycle = repeat_count_ ? repeat_count_->first : 0;
		repeat_count_.emplace(current_cycle, *repeat_count < current_cycle ? current_cycle : *repeat_count);
		
		//Flip direction
		if (current_cycle == 0 &&
			reverse_ != detail::is_direction_in_reverse(direction_))
		{
			frame_duration_.Total(frame_duration_.Remaining());
			reverse_ = !reverse_;
		}
	}
	else
		repeat_count_.reset();
}

void Animation::Direction(PlaybackDirection direction) noexcept
{
	if (direction_ != direction)
	{
		//Flip direction
		if (detail::is_direction_in_reverse(direction_) !=
			detail::is_direction_in_reverse(direction))
		{
			frame_duration_.Total(frame_duration_.Remaining());
			reverse_ = !reverse_;
		}

		direction_ = direction;
	}
}

/*
	Observers
*/

duration Animation::CycleTime() const noexcept
{
	if (HasAllFrames())
		return detail::frame_duration_to_cycle_duration(
			frame_duration_.Total(), frame_duration_.Limit(),
			current_frame_, std::ssize(frames_), reverse_);
	else
		return 0.0_sec;
}

duration Animation::CycleDuration() const noexcept
{
	if (HasAllFrames())
		return frame_duration_.Limit() * std::size(frames_);
	else
		return 0.0_sec;
}

real Animation::CyclePercent() const noexcept
{
	if (auto cycle_duration = CycleDuration(); cycle_duration > 0.0_sec)
		return CycleTime() / cycle_duration;
	else
		return 1.0_r;
}


std::optional<duration> Animation::TotalTime() const noexcept
{
	return repeat_count_ ?
		std::make_optional(CycleDuration() * repeat_count_->first + CycleTime()) :
		std::nullopt;
}

std::optional<duration> Animation::TotalDuration() const noexcept
{
	return repeat_count_ ?
		std::make_optional(CycleDuration() * (repeat_count_->second + 1)) :
		std::nullopt;
}

std::optional<real> Animation::TotalPercent() const noexcept
{
	return repeat_count_ ?
		std::make_optional(CycleDuration() > 0.0_sec ? *TotalTime() / *TotalDuration() : 1.0_r) :
		std::nullopt;
}


real Animation::FrameRate() const noexcept
{
	if (auto cycle_duration = CycleDuration(); cycle_duration > 0.0_sec)
		return std::size(frames_) / cycle_duration.count();
	else
		return 0.0_r;
}


/*
	Elapse time
*/

void Animation::Elapse(duration time) noexcept
{
	if (HasAllFrames())
	{
		if (IsRunning() &&
		   (frame_duration_ += time / playback_rate_))
			Next();
	}
	else if (!std::empty(textures_.Objects()))
		ClearFrames();
}


/*
	Frames
*/

void Animation::NextFrame() noexcept
{
	if (HasAllFrames())
	{
		Next();
		frame_duration_.Reset();
			//Make sure animation stays at start of frame
	}
}

void Animation::PreviousFrame() noexcept
{
	if (HasAllFrames())
	{
		Previous();
		frame_duration_.Reset();
			//Make sure animation stays at start of frame
	}
}

void Animation::FirstFrame() noexcept
{
	if (HasAllFrames())
	{
		reverse_ = detail::is_direction_in_reverse(direction_);

		if (reverse_)
			current_frame_ = std::ssize(frames_) - 1;
		else
			current_frame_ = 0;

		frame_duration_.Reset();
			//Make sure animation stays at start of frame

		if (repeat_count_)
			repeat_count_->first = 0;
	}
}

void Animation::LastFrame() noexcept
{
	if (HasAllFrames())
	{
		reverse_ = detail::is_direction_in_reverse(direction_);

		//Flip direction
		if (repeat_count_ &&
			repeat_count_->second % 2 == 1)
			reverse_ = !reverse_;

		if (reverse_)
			current_frame_ = 0;
		else
			current_frame_ = std::ssize(frames_) - 1;

		frame_duration_.Reset();
			//Make sure animation stays at start of frame

		if (repeat_count_)
			repeat_count_->first = repeat_count_->second;
	}
}


Texture* Animation::CurrentFrame() const noexcept
{
	if (HasAllFrames())
		return frames_[current_frame_];
	else
		return nullptr;
}

bool Animation::HasAllFrames() const noexcept
{
	return !std::empty(frames_) &&
		total_observed_textures_ == std::ssize(textures_.Objects());
			//All textures are still being observed
}


/*
	Playback
*/

void Animation::Start() noexcept
{
	running_ = true;
}

void Animation::Stop() noexcept
{
	running_ = false;
}

void Animation::Reset() noexcept
{
	running_ = false;
	FirstFrame();
}

void Animation::Restart() noexcept
{
	Reset();
	Start();
}


void Animation::JumpForward(duration time) noexcept
{
	if (repeat_count_)
		TotalTime(*TotalTime() + time);
	else
		CycleTime(CycleTime() + time);
}

void Animation::JumpBackward(duration time) noexcept
{
	if (repeat_count_)
		TotalTime(*TotalTime() - time);
	else
		CycleTime(CycleTime() - time);
}


duration Animation::CyclePlaybackTime() const noexcept
{
	return CycleTime() / playback_rate_;
}

duration Animation::CyclePlaybackDuration() const noexcept
{
	return CycleDuration() / playback_rate_;
}


std::optional<duration> Animation::TotalPlaybackTime() const noexcept
{
	return repeat_count_ ?
		std::make_optional(CyclePlaybackDuration() * repeat_count_->first + CyclePlaybackTime()) :
		std::nullopt;
}

std::optional<duration> Animation::TotalPlaybackDuration() const noexcept
{
	return repeat_count_ ?
		std::make_optional(CyclePlaybackDuration() * (repeat_count_->second + 1)) :
		std::nullopt;
}


real Animation::PlaybackFrameRate() const noexcept
{
	return FrameRate() / playback_rate_;
}

} //ion::graphics::textures