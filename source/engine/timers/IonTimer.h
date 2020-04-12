/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	timers
File:	IonTimer.h
-------------------------------------------
*/

#ifndef ION_TIMER_H
#define ION_TIMER_H

#include <optional>

#include "types/IonTypes.h"
#include "events/IonCallback.h"
#include "managed/IonManagedObject.h"

namespace ion::timers
{
	struct TimerManager;

	namespace timer::detail
	{
		using namespace types::type_literals;

		constexpr auto valid_interval(duration interval) noexcept
		{
			return interval.count() < 0.0_r ?
				duration{} :
				interval;
		}
	} //timer::detail

	class Timer final : public managed::ManagedObject<TimerManager>
	{
		private:

			duration interval_{};
			duration elapsed_{};
			bool running_ = false;
			bool ticked_ = false;

			std::optional<events::Callback<void, Timer&>> on_tick_;

		public:

			//Default constructor
			Timer() = default;

			//Constructs a new timer from the given interval
			Timer(duration interval) noexcept;

			//Constructs a new timer from the given interval and callback
			Timer(duration interval, events::Callback<void, Timer&> on_tick) noexcept;


			/*
				Modifiers
			*/

			//Sets the interval in seconds (as real)
			inline void Interval(duration interval) noexcept
			{
				interval_ = timer::detail::valid_interval(interval);
			}

			//Sets the on tick callback
			inline void OnTick(std::optional<events::Callback<void, Timer&>> on_tick) noexcept
			{
				on_tick_.swap(on_tick);
			}


			/*
				Observers
			*/

			//Returns true if the timer is running
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_;
			}

			//Returns the interval in seconds (as real)
			[[nodiscard]] inline auto Interval() const noexcept
			{
				return interval_;
			}

			//Returns the on tick callback
			[[nodiscard]] inline auto OnTick() const noexcept
			{
				return on_tick_;
			}


			/*
				Common functions for controlling the timer
			*/

			//Starts or resumes timer
			void Start() noexcept;

			//Stops timer
			void Stop() noexcept;

			//Stops and reset time to zero
			void Reset() noexcept;

			//Stops, resets and starts timer
			void Restart() noexcept;


			/*
				Elapse time
			*/

			//Elapse timer by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;


			/*
				Ticked
			*/

			//Returns true if timer has ticked one or more time
			//If timer has ticked, ticked is set to false
			[[nodiscard]] bool HasTicked() noexcept;
	};
} //ion::timers

#endif