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

#include "IonFrameSequenceManager.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObservedObject.h"
#include "types/IonCumulative.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene
{
	class SceneManager; //Forward declaration
} //ion::graphics::scene

namespace ion::graphics::textures
{
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
		} //detail
	} //animation


	//An animation class that contains one or more frames (textures)
	class Animation final : public managed::ManagedObject<scene::SceneManager>
	{
		private:

			types::Cumulative<duration> frame_duration_;
			std::optional<std::pair<int, int>> repeat_count_;
			animation::PlaybackDirection direction_ = animation::PlaybackDirection::Normal;
			real playback_rate_ = 1.0_r;
			bool running_ = false;
			bool reverse_ = false;	
			int current_frame_ = 0;
			
			managed::ObservedObject<FrameSequence> frame_sequence_;


			inline auto FrameCount() const noexcept
			{
				return frame_sequence_ ? frame_sequence_->FrameCount() : 0;
			}

			inline auto HasFrames() const noexcept
			{
				return frame_sequence_ && frame_sequence_->HasAllInitialFrames();
			}


			void StepForward(bool rewind = false) noexcept;
			void StepBackward(bool rewind = false) noexcept;

			void Next() noexcept;
			void Previous() noexcept;

		public:

			//Constructs a new animation with the given frames, cycle duration, repeat count, playback direction and rate
			//Duplicate textures are allowed within an animation
			Animation(FrameSequence &frame_sequence,
				duration cycle_duration, std::optional<int> repeat_count = std::nullopt,
				animation::PlaybackDirection direction = animation::PlaybackDirection::Normal, real playback_rate = 1.0_r);

			//Constructs a new animation (in normal direction) with the given frames, cycle duration, repeat count and playback rate
			//Duplicate textures are allowed within an animation
			Animation(FrameSequence &frame_sequence,
				duration cycle_duration, std::optional<int> repeat_count, real playback_rate);


			/*
				Static animation conversions
			*/

			//Returns a new looping animation from the given frames, cycle duration, playback direction and rate
			//Duplicate textures are allowed within an animation
			[[nodiscard]] static Animation Looping(FrameSequence &frame_sequence,
				duration cycle_duration, animation::PlaybackDirection direction, real playback_rate = 1.0_r) noexcept;

			//Returns a new looping animation (in normal direction) from the given frames, cycle duration and playback rate
			//Duplicate textures are allowed within an animation
			[[nodiscard]] static Animation Looping(FrameSequence &frame_sequence,
				duration cycle_duration, real playback_rate = 1.0_r) noexcept;


			//Returns a new non-looping animation from the given frames, cycle duration, playback direction and rate
			//Duplicate textures are allowed within an animation
			[[nodiscard]] static Animation NonLooping(FrameSequence &frame_sequence,
				duration cycle_duration, animation::PlaybackDirection direction, real playback_rate = 1.0_r) noexcept;

			//Returns a new non-looping animation (in normal direction) from the given frames, cycle duration and playback rate
			//Duplicate textures are allowed within an animation
			[[nodiscard]] static Animation NonLooping(FrameSequence &frame_sequence,
				duration cycle_duration, real playback_rate = 1.0_r) noexcept;


			/*
				Modifiers
			*/

			//Sets the current cycle time to the given value in range [0.0, cycle duration]
			void CycleTime(duration time) noexcept;

			//Sets the current cycle duration to the given value
			void CycleDuration(duration time) noexcept;

			//Sets the current cycle percent to the given value in range [0.0, 1.0]
			void CyclePercent(real percent) noexcept;


			//Sets the current total time to the given value in range [0.0, total duration]
			//Is equal to cycle time if animation is non-looping
			void TotalTime(duration time) noexcept;

			//Sets the current total duration to the given value
			//Is equal to cycle duration if animation is non-looping
			void TotalDuration(duration time) noexcept;

			//Sets the current total percent to the given value in range [0.0, 1.0]
			//Is equal to cycle percent if animation is non-looping
			void TotalPercent(real percent) noexcept;


			//Sets the current frame rate to the given rate in range (0.0, oo)
			void FrameRate(real rate) noexcept;

			//Sets the given repeat count to the given value
			//A repeat count of 0 means that the animation will not loop
			//If repeat count is nullopt, the animation will loop indefinitely
			void RepeatCount(std::optional<int> repeat_count) noexcept;

			//Sets the given playback direction to the given direction
			void Direction(animation::PlaybackDirection direction) noexcept;

			//Sets the given playback rate to the given rate range (0.0, oo)
			inline void PlaybackRate(real rate) noexcept
			{
				if (rate > 0.0_r)
					playback_rate_ = rate;
			}


			/*
				Observers
			*/

			//Returns the current cycle time of the animation
			[[nodiscard]] duration CycleTime() const noexcept;

			//Returns the current cycle duration of the animation
			[[nodiscard]] duration CycleDuration() const noexcept;

			//Returns the current cycle percent of the animation
			[[nodiscard]] real CyclePercent() const noexcept;


			//Returns the total time of the animation (total time * repeat count)
			//Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<duration> TotalTime() const noexcept;

			//Returns the total duration of the animation (total duration * repeat count)
			//Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<duration> TotalDuration() const noexcept;

			//Returns the total percent of the animation
			//Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<real> TotalPercent() const noexcept;


			//Returns the current frame rate of the animation
			[[nodiscard]] real FrameRate() const noexcept;

			//Returns the repeat count of the animation
			//A repeat count of 0 means that the animation will not loop
			//If repeat count is nullopt, the animation will loop indefinitely
			[[nodiscard]] inline auto RepeatCount() const noexcept
			{
				return repeat_count_;
			}

			//Returns the playback direction of the animation
			[[nodiscard]] inline auto Direction() const noexcept
			{
				return direction_;
			}

			//Returns the playback rate of the animation
			[[nodiscard]] inline auto PlaybackRate() const noexcept
			{
				return playback_rate_;
			}


			//Returns true if the animation is running
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_;
			}

			//Returns true if the animation is currently in reverse
			[[nodiscard]] inline auto InReverse() const noexcept
			{
				return reverse_;
			}
			

			/*
				Elapse time
			*/

			//Elapse animation by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;


			/*
				Frames
			*/

			//Go to the next frame of the animation
			void NextFrame() noexcept;

			//Go to the previous frame of the animation
			void PreviousFrame() noexcept;

			//Rewind to the first frame of the animation
			void FirstFrame() noexcept;

			//Fast forward to the last frame of the animation
			void LastFrame() noexcept;


			//Returns a pointer to the mutable current frame in this animation
			//Returns nullptr if there is no current frame
			[[nodiscard]] Texture* CurrentFrame() noexcept;

			//Returns a pointer to the immutable current frame in this animation
			//Returns nullptr if there is no current frame
			[[nodiscard]] const Texture* CurrentFrame() const noexcept;


			//Returns a pointer to the mutable frame sequence in this animation
			//Returns nullptr if this animation does not have an underlying frame sequence
			[[nodiscard]] FrameSequence* UnderlyingFrameSequence() noexcept;

			//Returns a pointer to the immutable frame sequence in this animation
			//Returns nullptr if this animation does not have an underlying frame sequence
			[[nodiscard]] const FrameSequence* UnderlyingFrameSequence() const noexcept;


			/*
				Playback
			*/

			//Starts or resumes, animation playback
			void Start() noexcept;

			//Stops animation playback
			void Stop() noexcept;

			//Stops animation playback and reset elapsed time to zero
			void Reset() noexcept;

			//Stops, resets and starts animation playback
			void Restart() noexcept;


			//Jump forward by the given amount of time
			void JumpForward(duration time) noexcept;

			//Jump backward by the given amount of time
			void JumpBackward(duration time) noexcept;


			//Returns the actual cycle playback time of the animation
			[[nodiscard]] duration CyclePlaybackTime() const noexcept;

			//Returns the actual cycle playback duration of the animation
			[[nodiscard]] duration CyclePlaybackDuration() const noexcept;


			//Returns the actual total playback time of the animation
			//Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<duration> TotalPlaybackTime() const noexcept;

			//Returns the actual total playback duration of the animation
			//Returns nullopt if the animation will run indefinitely
			[[nodiscard]] std::optional<duration> TotalPlaybackDuration() const noexcept;


			//Returns the actual playback frame rate of the animation
			[[nodiscard]] real PlaybackFrameRate() const noexcept;
	};
} //ion::graphics::textures

#endif