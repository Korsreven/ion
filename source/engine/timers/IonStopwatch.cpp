/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	timers
File:	IonStopwatch.cpp
-------------------------------------------
*/

#include "IonStopwatch.h"

namespace ion::timers
{

//Private

Stopwatch::clock::time_point Stopwatch::Now() noexcept
{
	return clock::now();
}

Stopwatch::clock::duration Stopwatch::Sync() noexcept
{
	if (running_)
		stop_ = Now();

	return stop_ - start_;
}

//Public

/*
	Static functions
*/

Stopwatch Stopwatch::StartNew() noexcept
{
	Stopwatch stopwatch;
	stopwatch.Start();
	return stopwatch;
}

long long Stopwatch::GetTimestamp() noexcept
{
	return Now().time_since_epoch().count();
}


/*
	Common functions for controlling the stopwatch
*/

void Stopwatch::Start() noexcept
{
	if (!running_)
	{
		start_ = Now();
		running_ = true;
	}
}

void Stopwatch::Stop() noexcept
{
	if (running_)
	{
		Sync();
		running_ = false;
	}
}

void Stopwatch::Reset() noexcept
{
	start_ = stop_ = {};
	running_ = false;
}

void Stopwatch::Restart() noexcept
{
	Reset();
	Start();
}

bool Stopwatch::IsRunning() const noexcept
{
	return running_;
}


/*
	Get total elapsed time in different units
*/

duration Stopwatch::Elapsed() noexcept
{
	return duration(Sync());
}

std::chrono::hours Stopwatch::ElapsedHours() noexcept
{
	return std::chrono::duration_cast<std::chrono::hours>(Sync());
}

std::chrono::minutes Stopwatch::ElapsedMinutes() noexcept
{
	return std::chrono::duration_cast<std::chrono::minutes>(Sync());
}

std::chrono::seconds Stopwatch::ElapsedSeconds() noexcept
{
	return std::chrono::duration_cast<std::chrono::seconds>(Sync());
}

std::chrono::milliseconds Stopwatch::ElapsedMilliseconds() noexcept
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(Sync());
}

std::chrono::microseconds Stopwatch::ElapsedMicroseconds() noexcept
{
	return std::chrono::duration_cast<std::chrono::microseconds>(Sync());
}

Stopwatch::clock::duration Stopwatch::ElapsedNanoseconds() noexcept
{
	return Sync();
}

} //ion::timers