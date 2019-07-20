/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	timers
File:	IonTimerManager.cpp
-------------------------------------------
*/

#include "IonTimerManager.h"

namespace ion::timers
{

/*
	Creating
*/

Timer& TimerManager::CreateTimer(std::chrono::duration<real> interval)
{
	return Create(interval);
}

Timer& TimerManager::CreateTimer(std::chrono::duration<real> interval, events::Callback<void, Timer&> on_tick)
{
	return Create(interval, on_tick);
}

Timer& TimerManager::CreateTimer(const Timer &timer)
{
	return Create(timer);
}


/*
	Elapse time
*/

void TimerManager::Elapse(std::chrono::duration<real> time) noexcept
{
	for (auto &timer : Resources())
		timer.Elapse(time);
}

} //ion::timers