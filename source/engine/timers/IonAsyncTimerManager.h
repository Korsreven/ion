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


	///@brief A manager class that contains async timers (not in sync with the engine)
	///@details All timers are updated using a recurring callback with a minimum time resolution
	class AsyncTimerManager final : public TimerManager
	{
		private:

			Stopwatch stopwatch_;
			events::RecurringCallback callback_{&AsyncTimerManager::Elapse, this};


			/**
				@name Events
				@{
			*/

			///@brief See ResourceManager<T, R>::AdditionStarted for more details
			void AdditionStarted() noexcept override final;

			///@brief See ResourceManager<T, R>::AdditionEnded for more details
			void AdditionEnded() noexcept override final;


			///@brief See ResourceManager<T, R>::RemovalStarted for more details
			void RemovalStarted() noexcept override final;

			///@brief See ResourceManager<T, R>::RemovalEnded for more details
			void RemovalEnded() noexcept override final;

			///@}

			bool Elapse() noexcept;
			void SuspendAndWait() noexcept;

		public:

			///@brief Default constructor
			AsyncTimerManager() = default;

			///@brief Deleted copy constructor
			AsyncTimerManager(const AsyncTimerManager&) = delete;

			///@brief Default move constructor
			AsyncTimerManager(AsyncTimerManager&&) = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			AsyncTimerManager& operator=(const AsyncTimerManager&) = delete;

			///@brief Default move assignment
			AsyncTimerManager& operator=(AsyncTimerManager&&) = default;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns true if time is elapsing
			[[nodiscard]] bool IsTimeElapsing() const noexcept;

			///@}

			/**
				@name Suspending/continuing
				@{
			*/

			///@brief Continues the elapsing of time
			void ContinueTimeElapse() noexcept;

			///@brief Suspends the elapsing of time
			void SuspendTimeElapse() noexcept;

			///@}
	};
} //ion::timers

#endif