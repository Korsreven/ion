/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources
File:	IonResourceManager.h
-------------------------------------------
*/

#ifndef ION_RESOURCE_MANAGER_H
#define ION_RESOURCE_MANAGER_H

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include "IonResource.h"
#include "events/IonListenable.h"
#include "events/listeners/IonResourceListener.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "parallel/IonWorkerPool.h"
#include "types/IonProgress.h"
#include "types/IonTypes.h"

namespace ion::resources
{
	namespace resource_manager
	{
		enum class ExecutionModel : bool
		{
			Asynchronous,
			Synchronous
		};

		enum class EvaluationStrategy : bool
		{
			Eager,
			Lazy
		};
	} //resource_manager


	///@brief A class that manages, prepares, loads and unloads resources
	template <typename ResourceT, typename OwnerT>
	class ResourceManager : public managed::ObjectManager<ResourceT, OwnerT, events::listeners::ResourceListener<ResourceT, OwnerT>>
	{
		static_assert(std::is_base_of_v<Resource<OwnerT>, ResourceT>);

		private:
			
			parallel::WorkerPool<bool, ResourceT*> processes_;
			resource_manager::ExecutionModel process_execution_model_ = resource_manager::ExecutionModel::Asynchronous;
			std::optional<int> max_load_processes_;

		protected:

			/**
				@name Events
				@{
			*/

			///@brief See ObjectManager::Removed for more details
			void Removed(ResourceT &resource) noexcept override
			{
				//Wait for resource (could be in an async process)
				if (resource.LoadingState() == resource::LoadingState::Preparing)
					[[maybe_unused]] auto result = processes_.Get(&resource); //Blocking

				Unload(resource); //Eagerly
			}


			virtual bool PrepareResource(ResourceT&)
			{
				//Optional to override
				return true;
			}

			virtual ResourceT* DependentResource(ResourceT&) noexcept
			{
				//Optional to override
				return nullptr;
			}

			virtual bool LoadResource(ResourceT&) = 0;
			virtual bool UnloadResource(ResourceT&) noexcept = 0;


			///@brief See ResourceListener::ResourcePrepared for more details
			virtual void ResourcePrepared(ResourceT&) noexcept
			{
				//Optional to override
			}

			///@brief See ResourceListener::ResourceLoaded for more details
			virtual void ResourceLoaded(ResourceT&) noexcept
			{
				//Optional to override
			}

			///@brief See ResourceListener::ResourceUnloaded for more details
			virtual void ResourceUnloaded(ResourceT&) noexcept
			{
				//Optional to override
			}

			///@brief See ResourceListener::ResourceFailed for more details
			virtual void ResourceFailed(ResourceT&) noexcept
			{
				//Optional to override
			}

			///@}

			/**
				@name Notifying
				@{
			*/

			void NotifyResourcePrepared(ResourceT &resource) noexcept
			{
				ResourcePrepared(resource); //Notify derived first
				this->NotifyAll(this->Listeners(), &ResourceManager::listener_type::ResourcePrepared, resource);
			}

			void NotifyResourceLoaded(ResourceT &resource) noexcept
			{
				ResourceLoaded(resource); //Notify derived first
				this->NotifyAll(this->Listeners(), &ResourceManager::listener_type::ResourceLoaded, resource);
			}

			void NotifyResourceUnloaded(ResourceT &resource) noexcept
			{
				ResourceUnloaded(resource); //Notify derived first
				this->NotifyAll(this->Listeners(), &ResourceManager::listener_type::ResourceUnloaded, resource);
			}

			void NotifyResourceFailed(ResourceT &resource) noexcept
			{
				ResourceFailed(resource); //Notify derived first
				this->NotifyAll(this->Listeners(), &ResourceManager::listener_type::ResourceFailed, resource);
			}


			void NotifyResourceLoadingStateChanged(ResourceT &resource) noexcept
			{
				switch (resource.LoadingState())
				{
					case resource::LoadingState::Prepared:
					NotifyResourcePrepared(resource);
					break;

					case resource::LoadingState::Loaded:
					NotifyResourceLoaded(resource);
					break;

					case resource::LoadingState::Unloaded:
					NotifyResourceUnloaded(resource);
					break;

					case resource::LoadingState::Failed:
					NotifyResourceFailed(resource);
					break;
				}

				this->NotifyAll(this->Listeners(), &ResourceManager::listener_type::ResourceLoadingStateChanged, resource);
			}

			///@}

		private:

			void ChangeResourceLoadingState(ResourceT &resource, resource::LoadingState loading_state) noexcept
			{
				if (resource.LoadingState() != loading_state)
				{
					resource.LoadingState(loading_state);
					NotifyResourceLoadingStateChanged(resource);
				}
			}


			void ProcessPreparedResource(ResourceT &resource, bool prepared) noexcept
			{
				if (prepared)
				{
					ChangeResourceLoadingState(resource, resource::LoadingState::Prepared);

					//Check loading action
					switch (resource.LoadingAction())
					{
						case resource::LoadingAction::Load:
						ChangeResourceLoadingState(resource, resource::LoadingState::LoadPending);
						break;
					}
				}
				else
					ChangeResourceLoadingState(resource, resource::LoadingState::Failed);
			}

			void ProcessPreparedResources() noexcept
			{
				auto result = processes_.Get(parallel::worker_pool::Synchronization::NonBlocking);

				//Some resources are done preparing
				if (!std::empty(result))
				{
					for (auto &resource : Resources())
					{
						//Check if resource was prepared
						if (auto iter = result.find(&resource); iter != std::end(result))
							ProcessPreparedResource(resource, iter->second);
					}
				}
			}


			void JoinAsyncResourceProcess(ResourceT &resource) noexcept
			{
				if (auto prepared = processes_.Get(&resource); prepared) //Blocking
					ProcessPreparedResource(resource, *prepared);
			}

			void JoinAllAsyncResourceProcesses() noexcept
			{
				auto result = processes_.Get(); //Blocking

				for (auto [resource, prepared] : result)
					ProcessPreparedResource(*resource, prepared);
			}


			void ExecutePrepareResource(ResourceT &resource, resource_manager::ExecutionModel execution_model = resource_manager::ExecutionModel::Synchronous)
			{
				ChangeResourceLoadingState(resource, resource::LoadingState::Preparing);

				//Non-blocking
				if (execution_model == resource_manager::ExecutionModel::Asynchronous)
					processes_.RunTask(&resource, &ResourceManager::PrepareResource, std::ref(*this), std::ref(resource));
				//Blocking
				else
					ProcessPreparedResource(resource, PrepareResource(resource));
			}

			void ExecuteLoadResource(ResourceT &resource, resource_manager::EvaluationStrategy strategy)
			{
				//Check if there is a dependency
				if (auto dependent_resource = DependentResource(resource); dependent_resource)
				{
					if (strategy == resource_manager::EvaluationStrategy::Eager)
						Load(*dependent_resource);
					else //Lazy
						return; //Wait for the dependency to be resolved
				}

				ChangeResourceLoadingState(resource, resource::LoadingState::Loading);

				if (LoadResource(resource))
					ChangeResourceLoadingState(resource, resource::LoadingState::Loaded);
				else
					ChangeResourceLoadingState(resource, resource::LoadingState::Failed);
			}

			void ExecuteUnloadResource(ResourceT &resource) noexcept
			{
				ChangeResourceLoadingState(resource, resource::LoadingState::Unloading);
				
				if (UnloadResource(resource))
				{
					ChangeResourceLoadingState(resource, resource::LoadingState::Unloaded);

					//Check loading action
					switch (resource.LoadingAction())
					{
						case resource::LoadingAction::Prepare:
						case resource::LoadingAction::Load:
						ChangeResourceLoadingState(resource, resource::LoadingState::PreparePending);
						break;
					}
				}
				else
					ChangeResourceLoadingState(resource, resource::LoadingState::Failed);
			}


			void PreparePendingResources(resource_manager::ExecutionModel execution_model = resource_manager::ExecutionModel::Asynchronous)
			{
				//Check if any resources needs preparing
				for (auto &resource : Resources())
				{
					//Prepare
					if (resource.LoadingState() == resource::LoadingState::PreparePending)
					{
						NotifyResourceLoadingStateChanged(resource);
							//Make sure to notify the pending state (in case someone is listening)
						ExecutePrepareResource(resource, execution_model);

						if (execution_model == resource_manager::ExecutionModel::Synchronous)
							break; //One at a time
					}
				}

				ProcessPreparedResources();
					//Must be called even if the execution model is synchronous 
			}

			void LoadPendingResources()
			{
				//Check if any resources needs loading
				for (auto &resource : Resources())
				{
					//Load
					if (resource.LoadingState() == resource::LoadingState::LoadPending)
					{
						NotifyResourceLoadingStateChanged(resource);
							//Make sure to notify the pending state (in case someone is listening)
						ExecuteLoadResource(resource, resource_manager::EvaluationStrategy::Lazy);
						break; //One at a time
					}
				}
			}

			void UnloadPendingResources() noexcept
			{
				//Check if any resources needs unloading
				for (auto &resource : Resources())
				{
					//Unload
					if (resource.LoadingState() == resource::LoadingState::UnloadPending)
					{
						NotifyResourceLoadingStateChanged(resource);
							//Make sure to notify the pending state (in case someone is listening)
						ExecuteUnloadResource(resource);
						break; //One at a time
					}
				}
			}

			void UpdatePendingResources(resource_manager::ExecutionModel execution_model = resource_manager::ExecutionModel::Asynchronous) noexcept
			{
				PreparePendingResources(execution_model);
				LoadPendingResources();
				UnloadPendingResources();
			}

		public:

			///@brief Default constructor
			ResourceManager() = default;

			///@brief Deleted copy constructor
			ResourceManager(const ResourceManager&) = delete;

			///@brief Default move constructor
			ResourceManager(ResourceManager&&) = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			ResourceManager& operator=(const ResourceManager&) = delete;

			///@brief Default move assignment
			ResourceManager& operator=(ResourceManager&&) = default;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the process execution model the resource manager is allowed to use
			inline void ProcessExecutionModel(resource_manager::ExecutionModel execution_model) noexcept
			{
				process_execution_model_ = execution_model;
			}

			///@brief Sets the max number of load processes the resource manager is allowed to use
			///@details If nullopt is passed, a default number of load processes will be used (based on your system)
			inline void MaxLoadProcesses(std::optional<int> max_load_processes) noexcept
			{
				max_load_processes_ = max_load_processes;

				if (max_load_processes_)
					processes_.MaxWorkerThreads(*max_load_processes_);
				else
					processes_.MaxWorkerThreads(parallel::worker_pool::detail::default_number_of_threads());
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the process execution model the resource manager is allowed to use
			[[nodiscard]] inline auto ProcessExecutionModel() const noexcept
			{
				return process_execution_model_;
			}

			///@brief Returns the max number of load processes the resource manager is allowed to use
			///@details If nullopt is returned, a default number of load processes is being used (based on your system)
			[[nodiscard]] inline auto MaxLoadProcesses() const noexcept
			{
				return max_load_processes_;
			}

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all resources in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Resources() noexcept
			{
				return this->Objects();
			}

			///@brief Returns an immutable range of all resources in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Resources() const noexcept
			{
				return this->Objects();
			}

			///@}

			/**
				@name Updating
				@{
			*/

			///@brief Returns the number of resources that are waiting to be updated
			[[nodiscard]] auto ResourcesToUpdate() const noexcept
			{
				auto count = 0;

				for (auto &resource : Resources())
				{
					switch (resource.LoadingState())
					{
						case resource::LoadingState::PreparePending:	
						case resource::LoadingState::LoadPending:
						case resource::LoadingState::UnloadPending:
						case resource::LoadingState::Preparing:
						case resource::LoadingState::Loading:
						case resource::LoadingState::Unloading:
						++count;
						break;
					}
				}

				return count;
			}

			///@brief Updates all resources that is in a pending state (non-blocking)
			///@details Returns true when completed
			[[nodiscard]] auto Updated() noexcept
			{
				UpdatePendingResources(process_execution_model_);
				return ResourcesToUpdate() == 0;
			}

			///@brief Updates all resources that is in a pending state (non-blocking)
			///@details Updates the given progress and returns true when completed
			[[nodiscard]] auto Updated(types::Progress<int> &progress) noexcept
			{
				if (auto count = ResourcesToUpdate(); count > progress.Max())
					progress.Max(count);

				UpdatePendingResources(process_execution_model_);

				progress.Value(progress.Max() - ResourcesToUpdate());
				return progress.IsComplete();
			}

			///@}

			/**
				@name Preparing
				@{
			*/

			///@brief Prepares the given resource before returning (eager)
			///@details Marks the given resource ready to be prepared (lazy)
			auto Prepare(ResourceT &resource, resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager)
			{
				if (resource.Owner() != this)
					return false;

				//Resource already preparing
				if (resource.LoadingState() == resource::LoadingState::Preparing &&
					strategy == resource_manager::EvaluationStrategy::Eager)
				{
					JoinAsyncResourceProcess(resource);
					return resource.IsPrepared();
				}

				if (auto prepare = resource.Prepare();
					prepare && strategy == resource_manager::EvaluationStrategy::Eager)
				{
					NotifyResourceLoadingStateChanged(resource);
						//Make sure to notify the pending state (in case someone is listening)
					ExecutePrepareResource(resource);
					return resource.IsPrepared();
				}
				else
					return prepare;
			}

			///@brief Prepares all resources before returning (eager)
			///@details Marks all resources ready to be prepared (lazy)
			void PrepareAll(resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager)
			{
				if (strategy == resource_manager::EvaluationStrategy::Eager)
					JoinAllAsyncResourceProcesses();

				for (auto &resource : Resources())
					Prepare(resource, strategy);
			}


			///@brief Returns the number of resources that are waiting to be prepared
			[[nodiscard]] auto ResourcesToPrepare() const noexcept
			{
				auto count = 0;

				for (auto &resource : Resources())
				{
					switch (resource.LoadingState())
					{
						case resource::LoadingState::PreparePending:
						case resource::LoadingState::Preparing:
						++count;
						break;
					}
				}

				return count;
			}

			///@brief Prepares all resources that is in a pending state (non-blocking)
			///@details Returns true when completed
			[[nodiscard]] auto Prepared()
			{
				PreparePendingResources(process_execution_model_);
				return ResourcesToPrepare() == 0;
			}

			///@brief Prepares all resources that is in a pending state (non-blocking)
			///@details Updates the given progress and returns true when completed
			[[nodiscard]] auto Prepared(types::Progress<int> &progress)
			{
				if (auto count = ResourcesToPrepare(); count > progress.Max())
					progress.Max(count);

				PreparePendingResources(process_execution_model_);

				progress.Value(progress.Max() - ResourcesToPrepare());
				return progress.IsComplete();
			}

			///@}

			/**
				@name Loading
				@{
			*/

			///@brief Loads the given resource before returning (eager)
			///@details Marks the given resource ready to be loaded (lazy)
			auto Load(ResourceT &resource, resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager)
			{
				if (resource.Owner() != this)
					return false;

				//Resource already preparing
				if (resource.LoadingState() == resource::LoadingState::Preparing &&
					strategy == resource_manager::EvaluationStrategy::Eager)
					JoinAsyncResourceProcess(resource);

				if (auto load = resource.Load();
					load && strategy == resource_manager::EvaluationStrategy::Eager)
				{
					NotifyResourceLoadingStateChanged(resource);
						//Make sure to notify the pending state (in case someone is listening)

					if (resource.LoadingState() == resource::LoadingState::PreparePending)
						ExecutePrepareResource(resource);
					
					if (resource.LoadingState() == resource::LoadingState::LoadPending)
						ExecuteLoadResource(resource, strategy);

					return resource.IsLoaded();
				}
				else
					return load;				
			}

			///@brief Loads all resources before returning (eager)
			///@details Marks all resources ready to be loaded (lazy)
			void LoadAll(resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager)
			{
				if (strategy == resource_manager::EvaluationStrategy::Eager)
					JoinAllAsyncResourceProcesses();

				for (auto &resource : Resources())
					Load(resource, strategy);
			}


			///@brief Returns the number of resources that are waiting to be loaded
			[[nodiscard]] auto ResourcesToLoad() const noexcept
			{
				auto count = 0;

				for (auto &resource : Resources())
				{
					switch (resource.LoadingState())
					{
						case resource::LoadingState::PreparePending:
						case resource::LoadingState::LoadPending:
						case resource::LoadingState::Preparing:
						case resource::LoadingState::Loading:
						++count;
						break;
					}
				}

				return count;
			}

			///@brief Loads all resources that is in a pending state (non-blocking)
			///@details Returns true when completed
			[[nodiscard]] auto Loaded()
			{
				PreparePendingResources(process_execution_model_);
				LoadPendingResources();
				return ResourcesToLoad() == 0;
			}

			///@brief Loads all resources that is in a pending state (non-blocking)
			///@details Updates the given progress and returns true when completed
			[[nodiscard]] auto Loaded(types::Progress<int> &progress)
			{
				if (auto count = ResourcesToLoad(); count > progress.Max())
					progress.Max(count);

				PreparePendingResources(process_execution_model_);
				LoadPendingResources();

				progress.Value(progress.Max() - ResourcesToLoad());
				return progress.IsComplete();
			}

			///@}

			/**
				@name Unloading
				@{
			*/

			///@brief Unloads the given resource before returning (eager)
			///@details Marks the given resource ready to be unloaded (lazy)
			auto Unload(ResourceT &resource, resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager) noexcept
			{
				if (resource.Owner() != this)
					return false;

				if (auto unload = resource.Unload();
					unload && strategy == resource_manager::EvaluationStrategy::Eager)
				{
					NotifyResourceLoadingStateChanged(resource);
						//Make sure to notify the pending state (in case someone is listening)
					ExecuteUnloadResource(resource);
					return resource.IsUnloaded();
				}
				else
					return unload;
			}

			///@brief Unloads all resources before returning (eager)
			///@details Marks all resources ready to be unloaded (lazy)
			void UnloadAll(resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager) noexcept
			{
				for (auto &resource : Resources())
					Unload(resource, strategy);
			}


			///@brief Returns the number of resources that are waiting to be unloaded
			[[nodiscard]] auto ResourcesToUnload() const noexcept
			{
				auto count = 0;

				for (auto &resource : Resources())
				{
					switch (resource.LoadingState())
					{
						case resource::LoadingState::UnloadPending:
						case resource::LoadingState::Unloading:
						++count;
						break;
					}
				}

				return count;
			}

			///@brief Unloads all resources that is in a pending state (non-blocking)
			///@details Returns true when completed
			[[nodiscard]] auto Unloaded() noexcept
			{
				UnloadPendingResources();
				return ResourcesToUnload() == 0;
			}

			///@brief Unloads all resources that is in a pending state (non-blocking)
			///@details Updates the given progress and returns true when completed
			[[nodiscard]] auto Unloaded(types::Progress<int> &progress) noexcept
			{
				if (auto count = ResourcesToUnload(); count > progress.Max())
					progress.Max(count);

				UnloadPendingResources();

				progress.Value(progress.Max() - ResourcesToUnload());
				return progress.IsComplete();
			}

			///@}

			/**
				@name Reloading
				@{
			*/

			///@brief Reloads the given resource before returning (eager)
			///@details Marks the given resource ready to be reloaded (lazy)
			auto Reload(ResourceT &resource, resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager)
			{
				if (resource.Owner() != this)
					return false;

				if (auto reload = resource.Reload();
					reload && strategy == resource_manager::EvaluationStrategy::Eager)
				{
					NotifyResourceLoadingStateChanged(resource);
						//Make sure to notify the pending state (in case someone is listening)
					ExecuteUnloadResource(resource);

					if (resource.LoadingState() == resource::LoadingState::PreparePending)
						ExecutePrepareResource(resource);
					
					if (resource.LoadingState() == resource::LoadingState::LoadPending)
						ExecuteLoadResource(resource, strategy);

					return resource.IsLoaded();
				}
				else
					return reload;
			}

			///@brief Reloads all resources before returning (eager)
			///@details Marks all resources ready to be reloaded (lazy)
			void ReloadAll(resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager)
			{
				for (auto &resource : Resources())
					Reload(resource, strategy);
			}

			///@}

			/**
				@name Repairing
				@{
			*/

			///@brief Repairs the given resource before returning (eager)
			///@details Marks the given resource ready to be repaired (lazy)
			auto Repair(ResourceT &resource, resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager)
			{
				if (resource.Owner() != this)
					return false;

				if (auto repair = resource.Repair();
					repair && strategy == resource_manager::EvaluationStrategy::Eager)
				{
					NotifyResourceLoadingStateChanged(resource);
						//Make sure to notify the pending state (in case someone is listening)
					ExecuteUnloadResource(resource);

					if (resource.LoadingState() == resource::LoadingState::PreparePending)
						ExecutePrepareResource(resource);
					
					if (resource.LoadingState() == resource::LoadingState::LoadPending)
						ExecuteLoadResource(resource, strategy);

					return resource.IsLoaded();
				}
				else
					return repair;
			}

			///@brief Repairs all resources before returning (eager)
			///@details Marks all resources ready to be repaired (lazy)
			void RepairAll(resource_manager::EvaluationStrategy strategy = resource_manager::EvaluationStrategy::Eager)
			{
				for (auto &resource : Resources())
					Repair(resource, strategy);
			}


			///@brief Returns the number of resources that has failed
			[[nodiscard]] auto ResourcesToRepair() const noexcept
			{
				auto count = 0;

				for (auto &resource : Resources())
				{
					if (resource.HasFailed())
						++count;
				}

				return count;
			}

			///@}

			/**
				@name Resources - Creating
				@{
			*/

			///@brief Creates a resource with the given arguments
			template <typename... Args>
			auto CreateResource(Args &&...args)
			{
				return this->Create(std::forward<Args>(args)...);
			}

			///@brief Creates a resource of type T with the given arguments
			template <typename T, typename... Args>
			auto CreateResource(Args &&...args)
			{
				static_assert(std::is_base_of_v<ResourceT, T>);

				auto ptr = this->Create<T>(std::forward<Args>(args)...);
				return static_pointer_cast<T>(ptr);
			}

			///@}

			/**
				@name Resources - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable resource with the given name
			///@details Returns nullptr if resource could not be found
			[[nodiscard]] auto GetResource(std::string_view name) noexcept
			{
				return this->Get(name);
			}

			///@brief Gets a pointer to an immutable resource with the given name
			///@details Returns nullptr if resource could not be found
			[[nodiscard]] auto GetResource(std::string_view name) const noexcept
			{
				return this->Get(name);
			}

			///@}

			/**
				@name Resources - Removing
				@{
			*/

			///@brief Clears all removable resources from this manager
			void ClearResources() noexcept
			{
				this->Clear();
			}

			///@brief Removes a removable resource from this manager
			auto RemoveResource(ResourceT &resource) noexcept
			{
				return this->Remove(resource);
			}

			///@brief Removes a removable resource with the given name from this manager
			auto RemoveResource(std::string_view name) noexcept
			{
				return this->Remove(name);
			}

			///@}
	};
} //ion::resources

#endif