/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonAnimation.h
-------------------------------------------
*/

#ifndef ION_ANIMATION_H
#define ION_ANIMATION_H

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

#include "IonFrameSequence.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonCumulative.h"
#include "types/IonTypes.h"

namespace ion::graphics::textures
{
	class AnimationManager; //Forward declaration

	using namespace types::type_literals;

	namespace animation
	{
		enum class PlaybackDirection
		{
			Normal,				//Forward
			Reverse,			//Backward
			Alternate,			//Forward then backward
			AlternateReverse	//Backward then forward
		};

		namespace detail
		{
			inline auto is_direction_in_reverse(PlaybackDirection direction) noexcept
			{
				return direction == animation::PlaybackDirection::Reverse ||
					   direction == animation::PlaybackDirection::AlternateReverse;
			}

			inline auto cycle_duration_to_frame_duration(duration current_cycle_duration, duration total_cycle_duration, int frame_count, bool reverse) noexcept
			{
				auto frame_position = current_cycle_duration / total_cycle_duration * frame_count;
				auto current_frame = static_cast<int>(frame_position);
				auto frame_duration = total_cycle_duration / frame_count * (frame_position - current_frame);

				if (reverse)
					current_frame = frame_count - current_frame - 1;

				return std::pair{frame_duration, current_frame};
			}			

			inline auto frame_duration_to_cycle_duration(duration current_frame_duration, duration total_frame_duration, int current_frame, int frame_count, bool reverse) noexcept
			{
				if (reverse)
					current_frame = frame_count - current_frame - 1;

				return total_frame_duration * current_frame + current_frame_duration;
			}

			int frame_at(duration time, duration cycle_duration, std::optional<int> repeat_count, PlaybackDirection direction, int frame_count);
		} //detail
	} //animation


	///@brief A class representing an animation that contains a sequence of frames
	class Animation final : public managed::ManagedObject<AnimationManager>
	{
		private:

			types::Cumulative<duration> frame_duration_;
			std::optional<std::pair<int, int>> repeat_count_;
			animation::PlaybackDirection direction_ = animation::PlaybackDirection::Normal;
			real playback_rate_ = 1.0_r;
			bool running_ = false;
			bool reverse_ = false;	
			int current_frame_ = 0;
			
			NonOwningPtr<FrameSequence> frame_sequence_;


			inline auto FrameCount() const noexcept
			{
				return frame_sequence_ ? frame_sequence_->FrameCount() : 0;
			}

			inline auto HasFrames() const noexcept
			{
				return frame_sequence_ && !frame_sequence_->IsEmpty();
			}


			void StepForward(bool rewind = false) noexcept;
			void StepBackward(bool rewind = false) noexcept;

			void Next() noexcept;
			void Previous() noexcept;

		public:

			///@brief Constructs a new animation with the given name, frames, cycle duration, repeat count, playback direction and rate
			Animation(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
				duration cycle_duration, std::optional<int> repeat_count = std::nullopt,
				animation::PlaybackDirection direction = animation::PlaybackDirection::Normal, real playback_rate = 1.0_r) noexcept;

			///@brief Constructs a new animation (in normal direction) with the given name, frames, cycle duration, repeat count and playback rate
			Animation(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
				duration cycle_duration, std::optional<int> repeat_count, real playback_rate) noexcept;


			/**
				@name Static animation conversions
				@{
			*/

			///@brief Returns a new looping animation with the given name, frames, cycle duration, playback direction and rate
			[[nodiscard]] static Animation Looping(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
				duration cycle_duration, animation::PlaybackDirection direction, real playback_rate = 1.0_r) noexcept;

			///@brief Returns a new looping animation (in normal direction) with the given name, frames, cycle duration and playback rate
			[[nodiscard]] static Animation Looping(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
				duration cycle_duration, real playback_rate = 1.0_r) noexcept;


			///@brief Returns a new non-looping animation with the given name, frames, cycle duration, playback direction and rate
			[[nodiscard]] static Animation NonLooping(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
				duration cycle_duration, animation::PlaybackDirection direction, real playback_rate = 1.0_r) noexcept;

			///@brief Returns a new non-looping animation (in normal direction) with the given name, frames, cycle duration and playback rate
			[[nodiscard]] static Animation NonLooping(std::string name, NonOwningPtr<FrameSequence> frame_sequence,
				duration cycle_duration, real playback_rate = 1.0_r) noexcept;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the current cycle time to the given value in range [0.0, cycle duration]
			void CycleTime(duration time) noexcept;

			///@brief Sets the current cycle duration to the given value
			void CycleDuration(duration time) noexcept;

			///@brief Sets the current cycle percent to the given value in range [0.0, 1.0]
			void CyclePercent(real percent) noexcept;


			///@brief Sets the current total time to the given value in range [0.0, total duration]
			///@details Is equal to cycle time if animation is non-looping
			void TotalTime(duration time) noexcept;

			///@brief Sets the current total duration to the given value
			///@details Is equal to cycle duration if animation is non-looping
			void TotalDuration(duration time) noexcept;

			///@brief Sets the current total percent to the given value in range [0.0, 1.0]
			///@details Is equal to cycle percent if animation is non-looping
			void TotalPercent(real percent) noexcept;


			///@brief Sets the current frame rate to the given rate in range (0.0, oo)
			void FrameRate(real rate) noexcept;

			///@brief Sets the given repeat count to the given value
			///@details A repeat count of 0 means that the animation will not loop.
			///If repeat count is nullopt, the animation will loop indefinitely
			void RepeatCount(std::optional<int> repeat_count) noexcept;

			///@brief Sets the given playback direction to the given direction
			void Direction(animation::PlaybackDirection direction) noexcept;

			///@brief Sets the given playback rate to the given rate in range (0.0, oo)
			inline void PlaybackRate(real rate) noexcept
			{
				if (rate > 0.0_r)
					playback_rate_ = rate;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the current cycle time of the animation
			[[nodiscard]] duration CycleTime() const noexcept;

			///@brief Returns the current cycle duration of the animation
			[[nodiscard]] duration CycleDuration() const noexcept;

			///@brief Returns the current cycle percent of the animation
			[[nodiscard]] real CyclePercent() const noexcept;


			///@brief Returns the total time of the animation (total time * repeat count)
			///@details Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<duration> TotalTime() const noexcept;

			///@brief Returns the total duration of the animation (total duration * repeat count)
			///@details Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<duration> TotalDuration() const noexcept;

			///@brief Returns the total percent of the animation
			///@details Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<real> TotalPercent() const noexcept;


			///@brief Returns the current frame rate of the animation
			[[nodiscard]] real FrameRate() const noexcept;

			///@brief Returns the repeat count of the animation
			///@details A repeat count of 0 means that the animation will not loop.
			///If repeat count is nullopt, the animation will loop indefinitely
			[[nodiscard]] inline auto RepeatCount() const noexcept
			{
				return repeat_count_;
			}

			///@brief Returns the playback direction of the animation
			[[nodiscard]] inline auto Direction() const noexcept
			{
				return direction_;
			}

			///@brief Returns the playback rate of the animation
			[[nodiscard]] inline auto PlaybackRate() const noexcept
			{
				return playback_rate_;
			}


			///@brief Returns true if the animation is running
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_;
			}

			///@brief Returns true if the animation is currently in reverse
			[[nodiscard]] inline auto InReverse() const noexcept
			{
				return reverse_;
			}
			
			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses animation by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;

			///@}

			/**
				@name Frames
				@{
			*/

			///@brief Go to the next frame of the animation
			void NextFrame() noexcept;

			///@brief Go to the previous frame of the animation
			void PreviousFrame() noexcept;

			///@brief Rewind to the first frame of the animation
			void FirstFrame() noexcept;

			///@brief Fast forward to the last frame of the animation
			void LastFrame() noexcept;


			///@brief Returns a pointer to the current frame in this animation
			///@details Returns nullptr if there is no current frame
			[[nodiscard]] NonOwningPtr<Texture> CurrentFrame() const noexcept;

			///@brief Returns a pointer to the frame in this animation at the given time
			///@details Returns nullptr if there is no frame at the given time
			[[nodiscard]] NonOwningPtr<Texture> FrameAt(duration time) const noexcept;


			///@brief Returns a pointer to the underlying frame sequence in this animation
			///@details Returns nullptr if this animation does not have an underlying frame sequence
			[[nodiscard]] inline auto UnderlyingFrameSequence() const noexcept
			{
				return frame_sequence_;
			}

			///@}

			/**
				@name Playback
				@{
			*/

			///@brief Starts or resumes, animation playback
			void Start() noexcept;

			///@brief Stops animation playback
			void Stop() noexcept;

			///@brief Stops animation playback and reset elapsed time to zero
			void Reset() noexcept;

			///@brief Stops, resets and starts animation playback
			void Restart() noexcept;


			///@brief Jumps forward by the given amount of time
			void JumpForward(duration time) noexcept;

			///@brief Jumps backward by the given amount of time
			void JumpBackward(duration time) noexcept;


			///@brief Returns the actual cycle playback time of the animation
			[[nodiscard]] duration CyclePlaybackTime() const noexcept;

			///@brief Returns the actual cycle playback duration of the animation
			[[nodiscard]] duration CyclePlaybackDuration() const noexcept;


			///@brief Returns the actual total playback time of the animation
			///@details Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<duration> TotalPlaybackTime() const noexcept;

			///@brief Returns the actual total playback duration of the animation
			///@details Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<duration> TotalPlaybackDuration() const noexcept;


			///@brief Returns the actual playback frame rate of the animation
			[[nodiscard]] real PlaybackFrameRate() const noexcept;

			///@}
	};
} //ion::graphics::textures

#endif