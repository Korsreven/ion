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

#include "IonTimer.h"
#include "resources/IonResourceManager.h"

namespace ion::timers
{
	namespace timer_manager::detail
	{
	} //timer_manager::detail


	struct TimerManager : resources::ResourceManager<TimerManager, Timer>
	{
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

		//Move assignment
		TimerManager& operator=(TimerManager&&) = default;


		/*
			Creating
		*/

		//Creates a timer with the given interval
		//Returns a reference to the newly created timer
		Timer& CreateTimer(duration interval);

		//Creates a timer with the given interval and callback
		//Returns a reference to the newly created timer
		Timer& CreateTimer(duration interval, events::Callback<void, Timer&> on_tick);

		//Creates a copy of the given timer
		//Returns a reference to the newly created timer
		Timer& CreateTimer(const Timer &timer);


		/*
			Elapse time
		*/

		//Elapse all timers by the given time in seconds
		//This function is typically called each frame, with the time in seconds since last frame
		void Elapse(duration time) noexcept;
	};
} //ion::timers

#endif