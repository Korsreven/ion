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


	///@brief A class representing a recurring callback, meaning it's called repeatedly when running
	///@details A callback is anything invocable, like a free function, member function, lambda or an object with a call operator
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

			///@brief Deleted copy constructor
			RecurringCallback(const RecurringCallback&) = delete;

			///@brief Default move constructor
			RecurringCallback(RecurringCallback&&) = default;

			///@brief Destructor
			~RecurringCallback() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			RecurringCallback& operator=(const RecurringCallback&) = delete;

			///@brief Move assignment
			RecurringCallback& operator=(RecurringCallback&&) = default;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns true if this recurring callback is running
			///@details The recurring callback could be running even if suspended
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_ || thread_.joinable();
			}

			///@brief Returns true if this recurring callback is suspended
			[[nodiscard]] inline auto IsSuspended() const noexcept
			{
				return suspend_ == SuspendState::Suspended;
			}

			///@}

			/**
				@name Starting/stopping
				@{
			*/

			///@brief Starts the recurring callback
			void Start() noexcept;

			///@brief Stops the recurring callback
			void Stop() noexcept;

			///@}

			/**
				@name Suspending/continuing
				@{
			*/

			///@brief Continues the recurring callback
			void Continue() noexcept;

			///@brief Suspends the recurring callback
			void Suspend() noexcept;

			///@}
	};
} //ion::events

#endif