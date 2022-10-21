/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	timers
File:	IonTimer.cpp
-------------------------------------------
*/

#include "IonTimer.h"

namespace ion::timers
{

using namespace timer;

//Public

Timer::Timer(std::string name) noexcept :
	managed::ManagedObject<TimerManager>{std::move(name)}
{
	//Empty
}

Timer::Timer(std::string name, duration interval) noexcept :

	managed::ManagedObject<TimerManager>{std::move(name)},
	interval_{detail::valid_interval(interval)}
{
	//Empty
}

Timer::Timer(std::string name, duration interval, events::Callback<void, Timer&> on_tick) noexcept :

	managed::ManagedObject<TimerManager>{std::move(name)},
	interval_{detail::valid_interval(interval)},
	on_tick_{std::in_place, std::move(on_tick)}
{
	//Empty
}


/*
	Common functions for controlling the timer
*/

void Timer::Start() noexcept
{
	running_ = true;
}

void Timer::Stop() noexcept
{
	running_ = false;
}

void Timer::Reset() noexcept
{
	elapsed_ = {};
	running_ = false;
}

void Timer::Restart() noexcept
{
	Reset();
	Start();
}


/*
	Elapse time
*/

void Timer::Elapse(duration time) noexcept
{
	if (running_ &&
		(elapsed_ += time) >= interval_)
	{
		elapsed_ = {};
		ticked_ = true;

		if (on_tick_)
			(*on_tick_)(*this);
	}
}


/*
	Ticked
*/

bool Timer::HasTicked() noexcept
{
	if (ticked_)
	{
		ticked_ = false;
		return true;
	}
	else
		return false;
}

} //ion::timers