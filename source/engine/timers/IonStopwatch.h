/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	timers
File:	IonStopwatch.h
-------------------------------------------
*/

#ifndef ION_STOPWATCH_H
#define ION_STOPWATCH_H

#include <chrono>
#include "types/IonTypes.h"

namespace ion::timers
{
	class Stopwatch final
	{
		private:

			using clock = std::chrono::high_resolution_clock;

			clock::time_point start_;
			clock::time_point stop_;
			bool running_ = false;


			static clock::time_point Now() noexcept;
			clock::duration Sync() noexcept;

		public:

			Stopwatch() = default;


			/*
				Static functions
			*/

			//Returns a new stopwatch and starts measuring elapsed time
			[[nodiscard]] static Stopwatch StartNew() noexcept;

			//Get a timestamp in the most precision unit available from the underlying system
			[[nodiscard]] static long long GetTimestamp() noexcept;


			/*
				Common functions for controlling the stopwatch
			*/

			//Starts or resumes, measuring elapsed time
			void Start() noexcept;

			//Stops measuring elapsed time
			void Stop() noexcept;

			//Stops measuring and reset elapsed time to zero
			void Reset() noexcept;

			//Stops, resets and starts measuring elapsed time
			void Restart() noexcept;

			//Returns true if the stopwatch is running
			[[nodiscard]] bool IsRunning() const noexcept;


			/*
				Get total elapsed time in different units
			*/

			//Returns elapsed time in seconds (as real)
			[[nodiscard]] std::chrono::duration<real> Elapsed() noexcept;

			//Returns elapsed time in hours (1 hour = 60 min)
			[[nodiscard]] std::chrono::hours ElapsedHours() noexcept;

			//Returns elapsed time in minutes (1 min = 60 sec)
			[[nodiscard]] std::chrono::minutes ElapsedMinutes() noexcept;

			//Returns elapsed time in seconds (1 sec = 1000 ms)
			[[nodiscard]] std::chrono::seconds ElapsedSeconds() noexcept;

			//Returns elapsed time in milliseconds (1 ms = 1000 mu)
			[[nodiscard]] std::chrono::milliseconds ElapsedMilliseconds() noexcept;

			//Returns elapsed time in microseconds (1 mu = 1000 ns)
			[[nodiscard]] std::chrono::microseconds ElapsedMicroseconds() noexcept;

			//Returns elapsed time in nanoseconds (highest precision supported)
			[[nodiscard]] std::chrono::nanoseconds ElapsedNanoseconds() noexcept;
	};
} //ion::timers

#endif