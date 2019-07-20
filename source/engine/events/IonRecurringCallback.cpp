/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events
File:	IonRecurringCallback.cpp
-------------------------------------------
*/

#include "IonRecurringCallback.h"

namespace ion::events
{

//Private

void RecurringCallback::Execute() noexcept
{
	while (running_)
	{
		if (suspend_ == SuspendState::Suspended)
			continue;

		//Execute callback
		if (!(*this)())
		{
			running_ = false;
			Continue();
		}
		else if (suspend_ == SuspendState::Suspending)
			suspend_ = SuspendState::Suspended;
	}
}


//Public

RecurringCallback::~RecurringCallback() noexcept
{
	Stop();
}


/*
	Starting / stopping
*/

void RecurringCallback::Start() noexcept
{
	if (!IsRunning())
	{
		running_ = true;
		thread_ = std::thread{&RecurringCallback::Execute, this};
	}
}

void RecurringCallback::Stop() noexcept
{
	if (running_)
	{
		running_ = false;
		thread_.join();
		Continue();
	}
}


/*
	Suspending / continuing
*/

void RecurringCallback::Continue() noexcept
{
	if (suspend_ != SuspendState::None)
		suspend_ = SuspendState::None;
}

void RecurringCallback::Suspend() noexcept
{
	if (running_ && suspend_ == SuspendState::None)
		suspend_ = SuspendState::Suspending;
}

} //ion::events