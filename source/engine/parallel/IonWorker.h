/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	parallel
File:	IonWorker.h
-------------------------------------------
*/

#ifndef ION_WORKER_H
#define ION_WORKER_H

#include <chrono>
#include <future>

namespace ion::parallel
{
	namespace worker::detail
	{
		template <typename Ret>
		using future_type = std::future<Ret>;
	} //worker::detail


	///@brief A class representing an async waitable worker
	///@details The worker starts on a given task and returns the result when queried.
	///This is the slave class defined by the master/slave model
	template <typename Ret>
	class Worker final
	{
		private:

			worker::detail::future_type<Ret> task_;

		public:

			///@brief Constructs a worker by passing it a function (or something that is invocable) and its arguments
			template <typename Function, typename... Args>
			Worker(Function &&function, Args &&...args) :
				task_{std::async(std::launch::async, std::forward<Function>(function), std::forward<Args>(args)...)}
			{
				//Empty
			}

			///@brief Default move constructor
			Worker(Worker&&) = default;

			///@brief Destructor
			~Worker()
			{
				Wait();
			}


			/**
				Operators
				@{
			*/

			///@brief Default move assignment
			Worker& operator=(Worker&&) = default;

			///@}

			/**
				Modifiers
				@{
			*/

			///@brief Returns the result once it's available (blocking)
			[[nodiscard]] inline auto Get() noexcept
			{
				return task_.get();
			}

			///@brief Returns the future handle, by moving it out of the worker (non-blocking)
			[[nodiscard]] inline auto GetFuture() noexcept
			{
				return std::move(task_);
			}

			///@brief Wait for the worker to finish its task (blocking)
			void Wait() noexcept
			{
				if (task_.valid())
					task_.wait();
			}

			///@}

			/**
				Observers
				@{
			*/

			///@brief Returns true if this worker is empty
			[[nodiscard]] inline auto IsEmpty() const noexcept
			{
				return !task_.valid();
			}

			///@brief Returns true if this worker is ready
			[[nodiscard]] inline auto IsReady() const noexcept
			{
				return task_.valid() && task_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
			}

			///@}
	};
} //ion::parallel

#endif