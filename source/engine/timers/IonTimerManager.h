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


	//A manager class that contains timers (usually in sync with the engine)
	//All timers are updated when elapse is called, with the time provided
	class TimerManager :
		public managed::ObjectManager<Timer, TimerManager>
	{
		public:

			//Default constructor
			TimerManager() = default;

			//Deleted copy constructor
			TimerManager(const TimerManager&) = delete;

			//Default move constructor
			TimerManager(TimerManager&&) = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			TimerManager& operator=(const TimerManager&) = delete;

			//Default move assignment
			TimerManager& operator=(TimerManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all timers in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Timers() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all timers in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Timers() const noexcept
			{
				return Objects();
			}


			/*
				Timers
				Creating
			*/

			//Create a timer with the given name and interval
			NonOwningPtr<Timer> CreateTimer(std::string name, duration interval);

			//Create a timer with the given name, interval and callback
			NonOwningPtr<Timer> CreateTimer(std::string name, duration interval, events::Callback<void, Timer&> on_tick);


			//Create a timer as a copy of the given timer
			NonOwningPtr<Timer> CreateTimer(const Timer &timer);

			//Create a timer by moving the given timer
			NonOwningPtr<Timer> CreateTimer(Timer &&timer);


			/*
				Timers
				Retrieving
			*/

			//Gets a pointer to a mutable timer with the given name
			//Returns nullptr if timer could not be found
			[[nodiscard]] NonOwningPtr<Timer> GetTimer(std::string_view name) noexcept;

			//Gets a pointer to an immutable timer with the given name
			//Returns nullptr if timer could not be found
			[[nodiscard]] NonOwningPtr<const Timer> GetTimer(std::string_view name) const noexcept;


			/*
				Timers
				Removing
			*/

			//Clear all removable timers from this manager
			void ClearTimers() noexcept;

			//Remove a removable timer from this manager
			bool RemoveTimer(Timer &timer) noexcept;

			//Remove a removable timer with the given name from this manager
			bool RemoveTimer(std::string_view name) noexcept;


			/*
				Elapse time
			*/

			//Elapse all timers by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;
	};
} //ion::timers

#endif