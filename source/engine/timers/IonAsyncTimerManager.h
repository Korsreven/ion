/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	timers
File:	IonAsyncTimerManager.h
-------------------------------------------
*/

#ifndef ION_ASYNC_TIMER_MANAGER_H
#define ION_ASYNC_TIMER_MANAGER_H

#include <chrono>

#include "IonStopwatch.h"
#include "IonTimerManager.h"
#include "events/IonRecurringCallback.h"

namespace ion::timers
{
	namespace async_timer_manager::detail
	{
		using namespace std::chrono_literals;
		constexpr auto minimum_time_elapse_resolution = 100us;
	} //async_timer_manager::detail


	class AsyncTimerManager final : public TimerManager
	{
		private:

			Stopwatch stopwatch_;
			events::RecurringCallback callback_{&AsyncTimerManager::Elapse, this};


			/*
				Events
			*/

			//See ResourceManager<T, R>::AdditionStarted for more details
			void AdditionStarted() noexcept override final;

			//See ResourceManager<T, R>::AdditionEnded for more details
			void AdditionEnded() noexcept override final;


			//See ResourceManager<T, R>::RemovalStarted for more details
			void RemovalStarted() noexcept override final;

			//See ResourceManager<T, R>::RemovalEnded for more details
			void RemovalEnded() noexcept override final;


			bool Elapse() noexcept;
			void SuspendAndWait() noexcept;

		public:

			//Default constructor
			AsyncTimerManager() = default;

			//Deleted copy constructor
			AsyncTimerManager(const AsyncTimerManager&) = delete;

			//Default move constructor
			AsyncTimerManager(AsyncTimerManager&&) = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			AsyncTimerManager& operator=(const AsyncTimerManager&) = delete;

			//Move assignment
			AsyncTimerManager& operator=(AsyncTimerManager&&) = default;


			/*
				Observers
			*/

			//Returns true if time is elapsing
			[[nodiscard]] bool IsTimeElapsing() const noexcept;


			/*
				Suspending / continuing
			*/

			//Continue the elapsing of time
			void ContinueTimeElapse() noexcept;

			//Suspend the elapsing of time
			void SuspendTimeElapse() noexcept;
	};
} //ion::timers

#endif