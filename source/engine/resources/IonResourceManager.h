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

#ifndef _ION_RESOURCE_MANAGER_
#define _ION_RESOURCE_MANAGER_

#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>

#include "adaptors/ranges/IonDereferenceIterable.h"
#include "events/listeners/IonListenerInterface.h"
#include "events/listeners/IonResourceListener.h"
#include "resources/IonResource.h"

namespace ion::resources
{
	namespace resource_manager::detail
	{
		template <typename T>
		using container_type = std::vector<std::unique_ptr<T>>; //Owning
	} //resource_manager::detail


	template <typename T, typename R>
	class ResourceManager : public events::listeners::ListenerInterface<events::listeners::ResourceListener<R>>
	{
		static_assert(std::is_base_of_v<Resource<T>, R>);

		private:

			resource_manager::detail::container_type<R> resources_;


			/*
				Notifying
			*/

			void NotifyCreated(R &resource) noexcept
			{
				Created(resource); //Notify derived first

				for (auto &listener : this->Listeners())
					this->Notify(&events::listeners::ResourceListener<R>::ResourceCreated, listener, resource);
			}


			auto NotifyRemovable(R &resource) noexcept
			{
				auto removable = Removable(resource); //Notify derived first
				
				for (auto &listener : this->Listeners())
					removable &= this->Notify(&events::listeners::ResourceListener<R>::ResourceRemovable, listener, resource).value_or(true);
						//If one listener disagrees (returns false), the resource can not be removed

				return removable;
			}

			void NotifyRemovableAll() noexcept
			{
				for (auto &resource : resources_)
					NotifyRemovable(*resource);
			}

			void NotifyRemoved(R &resource) noexcept
			{
				Removed(resource); //Notify derived first

				for (auto &listener : this->Listeners())
					this->Notify(&events::listeners::ResourceListener<R>::ResourceRemoved, listener, resource);
			}

			void NotifyRemovedAll(decltype(resources_) &resources) noexcept
			{
				for (auto &resource : resources)
					NotifyRemoved(*resource);
			}

		protected:

			/*
				Events
			*/

			//See ResourceListener::ResourceCreated for more details
			virtual void Created([[maybe_unused]] R &resource) noexcept
			{
				//Optional to override
			}


			//See ResourceListener::ResourceRemovable for more details
			virtual bool Removable([[maybe_unused]] R &resource) noexcept
			{
				//Optional to override
				return true;
			}

			//See ResourceListener::Removed for more details
			virtual void Removed([[maybe_unused]] R &resource) noexcept
			{
				//Optional to override
			}


			//Called right before starting to create or adopt resources
			virtual void AdditionStarted() noexcept
			{
				//Optional to override
			}

			//Called right after resources has been created or adopted
			virtual void AdditionEnded() noexcept
			{
				//Optional to override
			}


			//Called right before starting to remove or orphan resources
			virtual void RemovalStarted() noexcept
			{
				//Optional to override
			}

			//Called right after resources has been removed or orphaned
			virtual void RemovalEnded() noexcept
			{
				//Optional to override
			}


			/*
				Creating
			*/

			//Create a resource with the given arguments
			template <typename... Args>
			auto& Create(Args &&...args)
			{
				AdditionStarted();

				auto &resource = resources_.emplace_back(
					std::make_unique<R>(std::forward<Args>(args)...));
				resource->Owner(static_cast<T&>(*this));
				NotifyCreated(*resource);

				AdditionEnded();
				return *resource;
			}


			/*
				Removing
			*/

			auto Extract(R &resource) noexcept
			{
				auto iter =
					std::find_if(std::begin(resources_), std::end(resources_),
						[&](auto &x) noexcept
						{
							return x.get() == &resource;
						});

				//Resource found
				if (iter != std::end(resources_) &&
					NotifyRemovable(resource))
				{
					RemovalStarted();

					typename decltype(resources_)::value_type resource_ptr = std::move(*iter); //Extend lifetime
					resources_.erase(iter);
					NotifyRemoved(resource); //Still valid

					RemovalEnded();
					return std::move(resource_ptr);
				}
				else
					return typename decltype(resources_)::value_type{};
			}

			auto ExtractAll() noexcept
			{
				//[resources to keep, resources to remove]
				auto iter =
					std::stable_partition(std::begin(resources_), std::end(resources_),
						[&](auto &resource) noexcept
						{
							return !NotifyRemovable(*resource);
						});

				//Something to remove
				if (iter != std::end(resources_))
				{
					RemovalStarted();

					decltype(resources_) resources;
					std::move(iter, std::end(resources_), std::back_inserter(resources)); //Extend lifetime
					resources_.erase(iter, std::end(resources_)); //Erase the moved range
					resources_.shrink_to_fit();	
					NotifyRemovedAll(resources); //Still valid

					RemovalEnded();
					return std::move(resources);
				}
				else
					return decltype(resources_){};
			}

			void Tidy() noexcept
			{
				if (!std::empty(resources_))
				{
					NotifyRemovableAll();
					RemovalStarted();

					decltype(resources_) resources = std::move(resources_);
					resources_.shrink_to_fit();
					NotifyRemovedAll(resources);

					RemovalEnded();
				}
			}

		public:

			//Default constructor
			ResourceManager() = default;

			//Deleted copy constructor
			ResourceManager(const ResourceManager&) = delete;

			//Default move constructor
			ResourceManager(ResourceManager&&) = default;

			//Destructor
			~ResourceManager() noexcept
			{
				Tidy();
			}


			/*
				Operators
			*/

			//Deleted copy assignment
			ResourceManager& operator=(const ResourceManager&) = delete;

			//Move assignment
			inline auto& operator=(ResourceManager &&rhs) noexcept
			{
				Tidy();
				resources_ = std::move(rhs.resources_);
				return *this;
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all resources in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Resources() noexcept
			{
				return adaptors::ranges::DereferenceIterable<resource_manager::detail::container_type<R>&>{resources_};
			}

			//Returns an immutable range of all resources in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Resources() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const resource_manager::detail::container_type<R>&>{resources_};
			}


			/*
				Take / release ownership
			*/

			//Adopt (take ownership of) the given resource
			auto& Adopt(typename decltype(resources_)::value_type resource_ptr) noexcept
			{
				assert(resource_ptr);
				AdditionStarted();

				auto &resource = resources_.emplace_back(std::move(resource_ptr));
				resource->Owner(static_cast<T&>(*this));
				NotifyCreated(*resource);

				AdditionEnded();
				return *resource;
			}

			//Adopt (take ownership of) all the given resources
			void Adopt(typename decltype(resources_) resources) noexcept
			{
				resources.erase(
					std::remove_if(std::begin(resources), std::end(resources),
						[](const auto &resource) noexcept
						{
							return !resource;
						}),
					std::end(resources));

				if (!std::empty(resources))
				{
					AdditionStarted();

					std::move(std::begin(resources), std::end(resources), std::back_inserter(resources_));
					std::for_each(std::end(resources_) - std::size(resources), std::end(resources_),
						[](auto &resource) mutable noexcept
						{
							resource->Owner(static_cast<T&>(*this));
							NotifyCreated(*resource);
						});

					AdditionEnded();
				}
			}


			//Orphan (release ownership of) the given resource
			//Returns a pointer to the resource released
			[[nodiscard]] auto Orphan(R &resource) noexcept
			{
				auto resource_ptr = Extract(resource);

				//Release ownership
				if (resource_ptr)
					resource_ptr->Release();

				return resource_ptr;
			}

			//Orphan (release ownership of) all resources in this manager
			//Returns a pointer to the resource released
			[[nodiscard]] auto OrphanAll() noexcept
			{
				auto resources = ExtractAll();

				//Release ownership
				for (auto &resource : resources)
					resource->Release();

				return resources;
			}


			/*
				Removing
			*/

			//Clear all removable resources from this manager
			void Clear() noexcept
			{
				ExtractAll();
			}

			//Remove a removable resource from this manager
			auto Remove(R &resource) noexcept
			{
				auto ptr = Extract(resource);
				return !!ptr;
			}
	};
} //ion::resources

#endif