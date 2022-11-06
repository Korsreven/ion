/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	timers
File:	IonTimerManager.h
-------------------------------------------
*/

#ifndef ION_TIMER_MANAGER_H
#define ION_TIMER_MANAGER_H

#include <string>
#include <string_view>

#include "IonTimer.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::timers
{
	namespace timer_manager::detail
	{
	} //timer_manager::detail


	///@brief A manager class that contains timers (usually in sync with the engine)
	///@details All timers are updated when elapse is called, with the time provided
	class TimerManager :
		public managed::ObjectManager<Timer, TimerManager>
	{
		public:

			///@brief Default constructor
			TimerManager() = default;

			///@brief Deleted copy constructor
			TimerManager(const TimerManager&) = delete;

			///@brief Default move constructor
			TimerManager(TimerManager&&) = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			TimerManager& operator=(const TimerManager&) = delete;

			///@brief Default move assignment
			TimerManager& operator=(TimerManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all timers in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Timers() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all timers in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Timers() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Timers
				Creating
				@{
			*/

			///@brief Creates a timer with the given name and interval
			NonOwningPtr<Timer> CreateTimer(std::string name, duration interval);

			///@brief Creates a timer with the given name, interval and callback
			NonOwningPtr<Timer> CreateTimer(std::string name, duration interval, events::Callback<void, Timer&> on_tick);


			///@brief Creates a timer as a copy of the given timer
			NonOwningPtr<Timer> CreateTimer(const Timer &timer);

			///@brief Creates a timer by moving the given timer
			NonOwningPtr<Timer> CreateTimer(Timer &&timer);

			///@}

			/**
				@name Timers
				Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable timer with the given name
			///@details Returns nullptr if timer could not be found
			[[nodiscard]] NonOwningPtr<Timer> GetTimer(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable timer with the given name
			///@details Returns nullptr if timer could not be found
			[[nodiscard]] NonOwningPtr<const Timer> GetTimer(std::string_view name) const noexcept;

			///@}

			/**
				@name Timers
				Removing
				@{
			*/

			///@brief Clears all removable timers from this manager
			void ClearTimers() noexcept;

			///@brief Removes a removable timer from this manager
			bool RemoveTimer(Timer &timer) noexcept;

			///@brief Removes a removable timer with the given name from this manager
			bool RemoveTimer(std::string_view name) noexcept;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses all timers by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;

			///@}
	};
} //ion::timers

#endif