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

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
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

		enum class Synchronization : bool
		{
			NonBlocking,
			Blocking
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

			inline auto default_number_of_threads() noexcept
			{
				return number_of_cores() * threads_per_core;
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


	//A class representing a pool of workers (with an optional worker id)
	//Each worker runs in parallel and returns the results when done
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
				max_worker_threads_{worker_pool::detail::default_number_of_threads()}
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
				Ranges
			*/

			//Returns a mutable range of all workers in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Workers() noexcept
			{
				return adaptors::ranges::Iterable<worker_pool::detail::container_type<Ret, Id>&>{workers_};
			}

			//Returns an immutable range of all workers in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Workers() const noexcept
			{
				return adaptors::ranges::Iterable<const worker_pool::detail::container_type<Ret, Id>&>{workers_};
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
			//Returns the results that are available, then return to caller (non-blocking)
			[[nodiscard]] auto Get(worker_pool::Synchronization synchronization = worker_pool::Synchronization::Blocking)
			{
				if (synchronization == worker_pool::Synchronization::Blocking)
					Wait();

				worker_pool::detail::result_type<Ret, Id> result;

				if (synchronization == worker_pool::Synchronization::NonBlocking)
				{
					std::lock_guard lock{m_};

					if constexpr (std::is_same_v<Id, void>)
					{
						//[tasks not ready, tasks ready]
						auto iter =
							std::stable_partition(std::begin(workers_), std::end(workers_),
								[](auto &worker) noexcept
								{
									return !worker.IsReady();
								});

						//One or more tasks are ready
						if (iter != std::end(workers_))
						{
							result.reserve(std::end(workers_) - iter);

							for (auto it = iter; it != std::end(workers_); ++it)
								result.push_back(it->Get());

							workers_.erase(iter, std::end(workers_)); //Non-blocking
						}
					}
					else
					{
						for (auto &[id, worker] : workers_)
						{
							if (worker.IsReady())
								result.emplace(id, worker.Get());
						}

						workers_.erase_if(
							[](auto &value) noexcept
							{
								auto &[id, worker] = value;
								return worker.IsEmpty();
							}
						); //Non-blocking
					}
				}
				else
				{				
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
				}

				return result;
			}

			//Returns the result of the worker with the given id once it is available (blocking)
			//Returns the result if available, or return to caller with nullopt (non-blocking)
			template <typename = std::enable_if_t<!std::is_same_v<Id, void>>>
			[[nodiscard]] auto Get(const Id &id, worker_pool::Synchronization synchronization = worker_pool::Synchronization::Blocking)
				-> std::optional<Ret>
			{
				if (synchronization == worker_pool::Synchronization::Blocking)
					Wait(id);

				std::lock_guard lock{m_};
				if (auto iter = workers_.find(id); iter != std::end(workers_) && iter->second.IsReady())
				{
					auto result = iter->second.Get();
					workers_.erase(iter, std::end(workers_)); //Non-blocking
					return result;
				}
				else
					return {};			
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

			//Wait for the worker with the given id to finish its task (blocking)
			template <typename = std::enable_if_t<!std::is_same_v<Id, void>>>
			void Wait(const Id &id) noexcept
			{
				std::lock_guard lock{m_};
				if (auto iter = workers_.find(id); iter != std::end(workers_))
					iter->second.Wait();
			}


			/*
				Observers
			*/

			//Returns true if this is running
			[[nodiscard]] inline auto IsRunning() const noexcept
			{
				return running_state_ == worker_pool::RunningState::NonSuspended;
			}

			//Returns true if one or more workers are active
			[[nodiscard]] inline auto HasActiveWorkers() const noexcept
			{
				return worker_threads_ > 0;
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