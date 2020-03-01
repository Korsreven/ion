/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources
File:	IonResourceHolder.h
-------------------------------------------
*/

#ifndef ION_RESOURCE_HOLDER_H
#define ION_RESOURCE_HOLDER_H

#include <type_traits>

#include "IonResource.h"
#include "events/listeners/IonResourceListener.h"

namespace ion::resources
{
	namespace resource_holder
	{
		enum class ResourceRequirement : bool
		{
			Optional,
			Mandatory
		};

		namespace detail
		{
			template <typename T>
			inline auto subscribe_resource(T &resource, events::listeners::ResourceListener<T> &listener) -> T*
			{
				//Resource need owner to be subscribable
				if (auto owner = resource.Owner(); owner)
				{
					//Try to subscribe to resource owner
					if (owner->Subscribe(listener))
						return &resource; //Subscribed
				}

				return nullptr;
			}

			template <typename T>
			inline auto unsubscribe_resource(T &resource, events::listeners::ResourceListener<T> &listener) noexcept -> T*
			{
				//Resource need owner to be unsubscribable
				if (auto owner = resource.Owner(); owner)
				{
					//Try to unsubscribe from resource owner
					if (owner->Unsubscribe(listener))
						return nullptr; //Unsubscribed
				}

				return &resource;
			}
		} //detail
	} //resource_holder


	template <typename T>
	class ResourceHolder final : protected events::listeners::ResourceListener<T>
	{
		static_assert(std::is_base_of_v<Resource<std::remove_cvref_t<decltype(*std::declval<T>().Owner())>>, T>);

		private:

			T *resource_ = nullptr;
			resource_holder::ResourceRequirement requirement_ = resource_holder::ResourceRequirement::Optional;


			inline void DoSubscribe(T &resource)
			{
				if (!resource_)
					resource_ = resource_holder::detail::subscribe_resource(resource, *this);
			}

			inline void DoUnsubscribe() noexcept
			{
				if (resource_)
					resource_ = resource_holder::detail::unsubscribe_resource(*resource_, *this);
			}

		protected:

			/*
				Events
			*/

			//See ResourceListener::ResourceCreated for more details
			void ResourceCreated(T&) noexcept override
			{
				//Ignore
			}

			//See ResourceListener::ResourceRemoved for more details
			void ResourceRemoved(T &resource) noexcept override
			{
				if (resource_ == &resource)
					DoUnsubscribe();
			}

			//See ResourceListener::ResourceRemovable for more details
			bool ResourceRemovable(T&) noexcept override
			{
				return requirement_ == resource_holder::ResourceRequirement::Optional;
			}


			//See Listener::Unsubscribed for more details
			void Unsubscribed(events::listeners::ListenerInterface<events::listeners::ResourceListener<T>>&) noexcept override
			{
				resource_ = nullptr;
			}

			//See Listener::Unsubscribable for more details
			bool Unsubscribable(events::listeners::ListenerInterface<events::listeners::ResourceListener<T>>&) noexcept override
			{
				return requirement_ == resource_holder::ResourceRequirement::Optional;
			}

		public:

			//Default constructor
			ResourceHolder() = default;

			//Construct a new empty resource holder with the given requirement
			ResourceHolder(resource_holder::ResourceRequirement requirement) noexcept :
				requirement_{requirement}
			{
				//Empty
			}

			//Construct a new resource holder with the given resource and requirement
			ResourceHolder(T &resource, resource_holder::ResourceRequirement requirement = resource_holder::ResourceRequirement::Optional) :
				resource_{resource_holder::detail::subscribe_resource(resource, *this)},
				requirement_{requirement}
			{
				//Empty
			}

			//Copy constructor
			ResourceHolder(const ResourceHolder &rhs) :
				resource_(rhs.resource_ ? resource_holder::detail::subscribe_resource(*rhs.resource_, *this) : nullptr),
				requirement_{rhs.requirement_}
			{
				//Empty
			}

			//Move constructor
			ResourceHolder(ResourceHolder &&rhs) :
				resource_(rhs.resource_ ? resource_holder::detail::subscribe_resource(*rhs.resource_, *this) : nullptr),
				requirement_{rhs.requirement_}
			{
				rhs.DoUnsubscribe();
			}

			//Destructor
			~ResourceHolder() noexcept
			{
				this->Listening(false);
				DoUnsubscribe(); //Force
			}


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const ResourceHolder &rhs)
			{
				if (this != &rhs)
				{
					DoUnsubscribe();

					if (rhs.resource_)
						DoSubscribe(*rhs.resource_);

					requirement_ = rhs.requirement_;
				}

				return *this;
			}

			//Move assignment
			inline auto& operator=(ResourceHolder &&rhs)
			{
				if (this != &rhs)
				{
					DoUnsubscribe();

					if (rhs.resource_)
					{
						DoSubscribe(*rhs.resource_);
						rhs.DoUnsubscribe();
					}

					requirement_ = rhs.requirement_;
				}

				return *this;
			}


			//Returns true if this resource holder holds a resource
			inline operator bool() const noexcept
			{
				return !!resource_;
			}

			//Returns a pointer to a mutable resource
			//Returns nullptr if this resource holder does not hold a resource
			inline auto operator->() noexcept
			{
				return resource_;
			}

			//Returns a pointer to an immutable resource
			//Returns nullptr if this resource holder does not hold a resource
			inline const auto operator->() const noexcept
			{
				return resource_;
			}


			/*
				Modifiers
			*/

			//Hold the given resource
			//If another resource is already being held, that resource is unsubscribed automatically (if possible by the requirements)
			//Returns true if the resource has successfully been held, or change requirements
			inline auto Hold(T &resource)
			{
				if (resource_ != &resource)
				{
					DoUnsubscribe();
					DoSubscribe(resource);
				}

				return !!resource_;
			}

			//Release the holded resource
			//If another resource is already being held, that resource is unsubscribed automatically (if possible by the requirements)
			//Returns true if the resource has successfully been released, or change requirements
			inline auto Release() noexcept
			{
				DoUnsubscribe();
				return !resource_;
			}

			//Sets the resource requirement for this resource holder
			inline void Requirement(resource_holder::ResourceRequirement requirement)
			{
				requirement_ = requirement;
			}


			/*
				Observers
			*/

			//Returns a pointer to a mutable resource
			//Returns nullptr if this resource holder does not hold a resource
			[[nodiscard]] inline auto Resource() noexcept	
			{
				return resource_;
			}

			//Returns a pointer to an immutable resource
			//Returns nullptr if this resource holder does not hold a resource
			[[nodiscard]] inline const auto Resource() const noexcept	
			{
				return resource_;
			}

			//Returns the resource requirement for this resource holder
			[[nodiscard]] inline auto Requirement() const noexcept
			{
				return requirement_;
			}


			/*
				Empty
			*/

			//Returns true if this resource holder is empty
			[[nodiscard]] inline auto Empty() noexcept	
			{
				return !resource_;
			}
	};
} //ion::resources

#endif