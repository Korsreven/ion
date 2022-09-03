/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events
File:	IonRecurringCallback.h
-------------------------------------------
*/

#ifndef ION_RECURRING_CALLBACK_H
#define ION_RECURRING_CALLBACK_H

#include <atomic>
#include <thread>

#include "IonCallback.h"

namespace ion::events
{
	namespace recurring_callback::detail
	{

	} //recurring_callback::detail


	//A class representing a recurring callback, meaning it's called repeatedly when running
	//A callback is anything invocable, like a free function, member function, lambda or an object with a call operator
	class RecurringCallback : public Callback<bool>
		//Restrict recurring callbacks to no parameters and bool return type
	{
		private:
		
			enum class SuspendState : int
			{
				None,
				Suspending,
				Suspended
			};

			std::thread thread_;

			std::atomic<bool> running_ = false;
			std::atomic<SuspendState> suspend_ = SuspendState::None;


			void Execute() noexcept;

		public:

			using Callback<bool>::Callback;

			//Deleted copy constructor
			RecurringCallback(const RecurringCallback&) = delete;

			//Default move constructor
			RecurringCallback(RecurringCallback&&) = default;

			//Destructor
			~RecurringCallback() noexcept;


			/*
				Operators
			*/

			//Deleted copy assignment
			RecurringCallback& operator=(const RecurringCallback&) = delete;

			//Move assignment
			RecurringCallback& operator=(RecurringCallback&&) = default;


			/*
				Observers
			*/

			//Returns true if this recurring callback is running
			//The recurring callback could be running even if suspended
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_ || thread_.joinable();
			}

			//Returns true if this recurring callback is suspended
			[[nodiscard]] inline auto IsSuspended() const noexcept
			{
				return suspend_ == SuspendState::Suspended;
			}


			/*
				Starting / stopping
			*/

			//Starts the recurring callback
			void Start() noexcept;

			//Stops the recurring callback
			void Stop() noexcept;


			/*
				Suspending / continuing
			*/

			//Continues the recurring callback
			void Continue() noexcept;

			//Suspends the recurring callback
			void Suspend() noexcept;
	};
} //ion::events

#endif