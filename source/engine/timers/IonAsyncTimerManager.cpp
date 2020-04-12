/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	timers
File:	IonAsyncTimerManager.cpp
-------------------------------------------
*/

#include "IonAsyncTimerManager.h"

namespace ion::timers
{

using namespace async_timer_manager;

//Private

void AsyncTimerManager::AdditionStarted() noexcept
{
	if (callback_.IsRunning() && !callback_.IsSuspended())
		SuspendAndWait();
}

void AsyncTimerManager::AdditionEnded() noexcept
{
	//Timer(s) added for the first time
	if (!callback_.IsRunning())
	{
		stopwatch_.Start();
		callback_.Start();
	}
	else if (stopwatch_.IsRunning())
		callback_.Continue();
}


void AsyncTimerManager::RemovalStarted() noexcept
{
	if (callback_.IsRunning() && !callback_.IsSuspended())
		SuspendAndWait();
}

void AsyncTimerManager::RemovalEnded() noexcept
{
	//All timers where removed
	if (std::empty(Objects()))
	{
		callback_.Stop();
		stopwatch_.Stop();
	}
	else if (stopwatch_.IsRunning())
		callback_.Continue();
}


bool AsyncTimerManager::Elapse() noexcept
{
	auto time = stopwatch_.Elapsed();

	if (time >= detail::minimum_time_elapse_resolution)
	{
		for (auto &timer : Objects())
			timer.Elapse(time);

		stopwatch_.Restart();
	}

	return true;
}

void AsyncTimerManager::SuspendAndWait() noexcept
{
	callback_.Suspend();
	while (!callback_.IsSuspended()); //wait
}


//Public


/*
	Observers
*/

bool AsyncTimerManager::IsTimeElapsing() const noexcept
{
	return stopwatch_.IsRunning();
}


/*
	Suspending / continuing
*/

void AsyncTimerManager::ContinueTimeElapse() noexcept
{
	if (!IsTimeElapsing())
	{
		stopwatch_.Start();
		callback_.Continue();	
	}
}

void AsyncTimerManager::SuspendTimeElapse() noexcept
{
	if (IsTimeElapsing())
	{
		SuspendAndWait();
		stopwatch_.Stop();
	}
}

} //ion::timers