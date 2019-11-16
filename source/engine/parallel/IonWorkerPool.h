/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	parallel
File:	IonWorkerPool.h
-------------------------------------------
*/

#ifndef ION_WORKER_POOL_H
#define ION_WORKER_POOL_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

#include "IonWorker.h"
#include "adaptors/IonFlatMap.h"

namespace ion::parallel
{
	namespace worker_pool
	{
		enum class RunningState : bool
		{
			NonSuspended,
			Suspended
		};


		namespace detail
		{
			template <typename Ret, typename Id>
			using container_type = std::conditional_t<std::is_same_v<Id, void>,
				std::vector<Worker<Ret>>,
				adaptors::FlatMap<Id, Worker<Ret>>>;

			template <typename Ret, typename Id>
			using queue_type = std::conditional_t<std::is_same_v<Id, void>,
				std::vector<std::function<Ret()>>,
				adaptors::FlatMap<Id, std::function<Ret()>>>;

			template <typename Ret, typename Id>
			using result_type = std::conditional_t<std::is_same_v<Id, void>,
				std::vector<Ret>,
				adaptors::FlatMap<Id, Ret>>;


			constexpr auto threads_per_core = 2;

			inline auto number_of_cores() noexcept
			{
				if (auto count = static_cast<int>(std::thread::hardware_concurrency()); count > 0)
					return count;
				else
					return 1;
			}

			template <typename Function, typename... Args>
			inline auto deferred_call(Function &&function, Args &&...args)
			{
				return
					[&function, &args...]()
					{
						return std::invoke(std::forward<Function>(function), std::forward<Args>(args)...);
					};
			}
		} //detail
	} //worker_pool


	//This is the master class defined by the master/slave model
	template <typename Ret, typename Id = void>
	class WorkerPool final
	{
		private:
		
			worker_pool::detail::container_type<Ret, Id> workers_;
			worker_pool::detail::queue_type<Ret, Id> queue_;
			worker_pool::RunningState running_state_ = worker_pool::RunningState::NonSuspended;
			int worker_threads_ = 0;
			int max_worker_threads_ = 0;

			std::mutex m_;
			std::condition_variable cv_;


			struct worker_entry_point
			{
				WorkerPool &worker_pool;

				template <typename Function, typename... Args>
				auto operator()(Function &&function, Args &&...args) const
				{
					//Do work
					auto ret = std::invoke(std::forward<Function>(function), std::forward<Args>(args)...);
                    
					//Notify if all workers are done
					if (std::unique_lock lock{worker_pool.m_};
						--worker_pool.worker_threads_ == 0 && std::empty(worker_pool.queue_))
					{
						lock.unlock();
						worker_pool.cv_.notify_one();
					}
					else
						worker_pool.RunQueuedTasks(); //Mutex is still locked
					
					return ret;
				}
			};

			//Precondition: Caller needs to lock mutex
			void RunQueuedTasks()
			{
				//Start next task (dequeue from the back)
				while (!std::empty(queue_) &&
					IsRunning() && worker_threads_ < max_worker_threads_)
				{
					++worker_threads_;

					if constexpr (std::is_same_v<Id, void>)
					{
						auto task = std::move(queue_.back());
						queue_.pop_back(); //Dequeue
						workers_.emplace_back(worker_entry_point{*this}, std::move(task)); //Run queued task
					}
					else
					{
						auto [id, task] = std::move(*queue_.rbegin());
						queue_.erase(id); //Dequeue
						workers_.emplace(id, Worker<Ret>{worker_entry_point{*this}, std::move(task)}); //Run queued task
					}
				}
			}

		public:

			//Create a worker pool, either running (default) or not
			WorkerPool(worker_pool::RunningState running_state = worker_pool::RunningState::NonSuspended) noexcept :
				running_state_{running_state},
				max_worker_threads_{worker_pool::detail::number_of_cores() * worker_pool::detail::threads_per_core}
			{
				//Empty
			}

			//Create a worker pool with the given max number of worker threads, and either running (default) or not
			WorkerPool(int max_worker_threads, worker_pool::RunningState running_state = worker_pool::RunningState::NonSuspended) noexcept :
				running_state_{running_state},
				max_worker_threads_{max_worker_threads < 1 ? 1 : max_worker_threads}
			{
				//Empty
			}

			//Destructor
			~WorkerPool()
			{
				Wait();
			}


			/*
				Modifiers
			*/

			//Sets the max number of worker threads allowed simultaneously, in range [1, ...)
			void MaxWorkerThreads(int max_worker_threads) noexcept
			{
				std::lock_guard lock{m_};
				max_worker_threads_ = max_worker_threads < 1 ? 1 : max_worker_threads;
			}

			//Returns all of the result once they are available (blocking)
			[[nodiscard]] auto Get()
			{
				Wait();

				worker_pool::detail::result_type<Ret, Id> result;
				result.reserve(std::size(workers_));

				if constexpr (std::is_same_v<Id, void>)
				{
					for (auto &worker : workers_)
						result.push_back(worker.Get());
				}
				else
				{
					for (auto &[id, worker] : workers_)
						result.emplace(id, worker.Get());
				}

				workers_.clear(); //Non-blocking
				return result;
			}

			//Wait for all of the workers to finish their tasks (blocking)
			void Wait() noexcept
			{
				std::unique_lock lock{m_};
				cv_.wait(lock,
					[&]() noexcept
					{
						return worker_threads_ == 0;
					});
			}


			/*
				Observers
			*/

			//Returns true if this is running
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_state_ == worker_pool::RunningState::NonSuspended;
			}

			//Returns the max number of worker threads allowed simultaneously
			[[nodiscard]] inline auto MaxWorkerThreads() const noexcept
			{
				return max_worker_threads_;
			}


			/*
				Removing
			*/

			//Clear all workers and all queued work
			void Clear() noexcept
			{
				std::lock_guard lock{m_};
				workers_.clear(); //Blocking
				queue_.clear(); //Non-blocking
			}

			//Clear all queued work
			void ClearQueue() noexcept
			{
				std::lock_guard lock{m_};
				queue_.clear(); //Non-blocking
			}


			/*
				Tasks
			*/

			//Run or enqueue the given function with the given arguments
			template <typename Function, typename... Args,
				typename = std::enable_if_t<std::is_same_v<Id, void>>>
			auto RunTask(Function &&function, Args &&...args)
			{
				static_assert(std::is_invocable_r_v<Ret, Function, Args...>);

				std::lock_guard lock{m_};

				//Run now
				if (IsRunning() && worker_threads_ < max_worker_threads_)
				{
					++worker_threads_;
					workers_.emplace_back(worker_entry_point{*this}, std::forward<Function>(function), std::forward<Args>(args)...);
				}
				//Enqueue
				else
					queue_.emplace_back(worker_pool::detail::deferred_call(std::forward<Function>(function), std::forward<Args>(args)...));

				return true;
			}

			//Run or enqueue the given function with the given arguments
			//The task is stored in a map under the given unique id
			//If the given id is not unique the task is discarded
			template <typename Key, typename Function, typename... Args,
				typename = std::enable_if_t<!std::is_same_v<Id, void>>>
			auto RunTask(Key id, Function &&function, Args &&...args)
			{
				static_assert(std::is_same_v<Key, Id> || std::is_constructible_v<Key, Id> || std::is_convertible_v<Key, Id>);
				static_assert(std::is_invocable_r_v<Ret, Function, Args...>);

				std::lock_guard lock{m_};
                
				if (workers_.find(id) == std::end(workers_) &&
					queue_.find(id) == std::end(queue_))
				{
					//Run now
					if (IsRunning() && worker_threads_ < max_worker_threads_)
					{
						++worker_threads_;
						workers_.emplace(std::move(id), Worker<Ret>{worker_entry_point{*this}, std::forward<Function>(function), std::forward<Args>(args)...});
					}
					//Enqueue
					else
						queue_.emplace(std::move(id), worker_pool::detail::deferred_call(std::forward<Function>(function), std::forward<Args>(args)...));

					return true;
				}
				else
					return false;
			}

			//Resume all queued work
			void Resume() noexcept
			{
				if (!IsRunning())
				{
					std::lock_guard lock{m_};
					running_state_ = worker_pool::RunningState::NonSuspended;
					RunQueuedTasks();
				}
			}

			//Suspend all queued work
			void Suspend() noexcept
			{
				if (IsRunning())
				{
					std::lock_guard lock{m_};
					running_state_ = worker_pool::RunningState::Suspended;
				}
			}
	};
} //ion::parallel

#endif