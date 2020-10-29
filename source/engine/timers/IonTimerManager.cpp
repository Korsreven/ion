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
	Timers
	Creating
*/

Timer& TimerManager::CreateTimer(std::string name, duration interval)
{
	return Create(std::move(name), interval);
}

Timer& TimerManager::CreateTimer(std::string name, duration interval, events::Callback<void, Timer&> on_tick)
{
	return Create(std::move(name), interval, on_tick);
}

Timer& TimerManager::CreateTimer(const Timer &timer)
{
	return Create(timer);
}

Timer& TimerManager::CreateTimer(Timer &&timer)
{
	return Create(std::move(timer));
}


/*
	Timers
	Retrieving
*/

Timer* TimerManager::GetTimer(std::string_view name) noexcept
{
	return Get(name);
}

const Timer* TimerManager::GetTimer(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Timers
	Removing
*/

void TimerManager::ClearTimers() noexcept
{
	Clear();
}

bool TimerManager::RemoveTimer(Timer &timer) noexcept
{
	return Remove(timer);
}

bool TimerManager::RemoveTimer(std::string_view name) noexcept
{
	return Remove(name);
}


/*
	Elapse time
*/

void TimerManager::Elapse(duration time) noexcept
{
	for (auto &timer : Objects())
		timer.Elapse(time);
}

} //ion::timers