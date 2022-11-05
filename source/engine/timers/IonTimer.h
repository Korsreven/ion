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
	class TimerManager; //Forward declaration

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


	///@brief A class representing a controllable timer with an interval
	///@details The timer can be queried manually or by providing a callback
	class Timer final : public managed::ManagedObject<TimerManager>
	{
		private:

			duration interval_{};
			duration elapsed_{};
			bool running_ = false;
			bool ticked_ = false;

			std::optional<events::Callback<void, Timer&>> on_tick_;

		public:

			///@brief Constructs a new timer with the given name
			explicit Timer(std::string name) noexcept;

			///@brief Constructs a new timer with the given name and interval
			Timer(std::string name, duration interval) noexcept;

			///@brief Constructs a new timer with the given name, interval and callback
			Timer(std::string name, duration interval, events::Callback<void, Timer&> on_tick) noexcept;


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the interval in seconds (as real)
			inline void Interval(duration interval) noexcept
			{
				interval_ = timer::detail::valid_interval(interval);
			}

			///@brief Sets the on tick callback
			inline void OnTick(events::Callback<void, Timer&> on_tick) noexcept
			{
				on_tick_ = on_tick;
			}

			///@brief Sets the on tick callback
			inline void OnTick(std::nullopt_t) noexcept
			{
				on_tick_ = {};
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns true if the timer is running
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_;
			}

			///@brief Returns the interval in seconds (as real)
			[[nodiscard]] inline auto Interval() const noexcept
			{
				return interval_;
			}

			///@brief Returns the on tick callback
			[[nodiscard]] inline auto OnTick() const noexcept
			{
				return on_tick_;
			}

			///@}

			/**
				@name Common functions for controlling the timer
				@{
			*/

			///@brief Starts or resumes timer
			void Start() noexcept;

			///@brief Stops timer
			void Stop() noexcept;

			///@brief Stops and reset time to zero
			void Reset() noexcept;

			///@brief Stops, resets and starts timer
			void Restart() noexcept;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses timer by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;

			///@}

			/**
				@name Ticked
				@{
			*/

			///@brief Returns true if timer has ticked one or more time
			///@details If timer has ticked, ticked is set to false
			[[nodiscard]] bool HasTicked() noexcept;

			///@}
	};
} //ion::timers

#endif