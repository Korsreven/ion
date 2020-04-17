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

Stopwatch::clock::duration Stopwatch::Duration() const noexcept
{
	return (running_ ? Now() : stop_) - start_;
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
		stop_ = Now();
		running_ = false;
	}
}

void Stopwatch::Reset() noexcept
{
	start_ = stop_ = {};
	running_ = false;
}

duration Stopwatch::Restart() noexcept
{
	auto elapsed = Duration();
	Reset();
	Start();
	return elapsed;
}

bool Stopwatch::IsRunning() const noexcept
{
	return running_;
}


/*
	Get total elapsed time in different units
*/

duration Stopwatch::Elapsed() const noexcept
{
	return duration(Duration());
}

std::chrono::hours Stopwatch::ElapsedHours() const noexcept
{
	return std::chrono::duration_cast<std::chrono::hours>(Duration());
}

std::chrono::minutes Stopwatch::ElapsedMinutes() const noexcept
{
	return std::chrono::duration_cast<std::chrono::minutes>(Duration());
}

std::chrono::seconds Stopwatch::ElapsedSeconds() const noexcept
{
	return std::chrono::duration_cast<std::chrono::seconds>(Duration());
}

std::chrono::milliseconds Stopwatch::ElapsedMilliseconds() const noexcept
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(Duration());
}

std::chrono::microseconds Stopwatch::ElapsedMicroseconds() const noexcept
{
	return std::chrono::duration_cast<std::chrono::microseconds>(Duration());
}

Stopwatch::clock::duration Stopwatch::ElapsedNanoseconds() const noexcept
{
	return Duration();
}

} //ion::timers