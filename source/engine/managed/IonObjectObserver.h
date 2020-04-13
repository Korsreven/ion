/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	managed
File:	IonObjectObserver.h
-------------------------------------------
*/

#ifndef ION_OBJECT_OBSERVER_H
#define ION_OBJECT_OBSERVER_H

#include <type_traits>
#include <vector>

#include "IonManagedObject.h"
#include "IonObservedObject.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "events/IonEventChannel.h"

namespace ion::managed
{
	namespace object_observer::detail
	{
		template <typename T>
		using container_type = std::vector<T*>; //Non-owning
	} //object_observer::detail


	template <typename T,
		typename = std::enable_if_t<observed_object::detail::has_owner_type_v<T>>>
	class ObjectObserver final : protected events::EventChannel<typename T::owner_type>
	{
		static_assert(std::is_base_of_v<ManagedObject<typename T::owner_type>, T>);

		private:

			object_observer::detail::container_type<T> managed_objects_;


			void Tidy() noexcept
			{
				managed_objects_.clear();
				managed_objects_.shrink_to_fit();
			}

		protected:
			
			/*
				Events
			*/

			//See ManagedObjectListener::ObjectRemoved for more details
			void ObjectRemoved(T &object) noexcept override
			{
				auto iter =
					std::find_if(std::begin(managed_objects_), std::end(managed_objects_),
						[&](auto &x) noexcept
						{
							return x == &object;
						});

				//Object found
				if (iter != std::end(managed_objects_))
				{
					managed_objects_.erase(iter);

					//Empty, unsubscribe from publisher
					if (std::empty(managed_objects_))
					{
						this->DoUnsubscribe(true);
						Tidy();
					}
				}
			}

			//See ManagedObjectListener::ObjectRemovable for more details
			bool ObjectRemovable(T&) noexcept override
			{
				return this->Contract() == events::event_channel::SubscriptionContract::Cancelable;
			}


			//See EventChannel::Unsubscribed for more details
			void Unsubscribed() noexcept override
			{
				Tidy();
			}

		public:

			//Default constructor
			ObjectObserver() = default;

			//Construct a new empty observed object with the given requirement
			ObjectObserver(observed_object::ObjectRequirement requirement) noexcept :
				events::EventChannel<typename T::owner_type>{observed_object::detail::as_subscription_contract(requirement)}
			{
				//Empty
			}

			//Copy constructor
			ObjectObserver(const ObjectObserver &rhs) :

				events::EventChannel<typename T::owner_type>{rhs},
				managed_objects_{this->Active() ? rhs.managed_objects_ : decltype(managed_objects_){}}
			{
				//Empty
			}

			//Move constructor
			ObjectObserver(ObjectObserver &&rhs) :

				events::EventChannel<typename T::owner_type>{std::move(rhs)},
				managed_objects_{this->Active() ? std::exchange(rhs.managed_objects_, decltype(managed_objects_){}) : decltype(managed_objects_){}}
			{
				//Empty
			}


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const ObjectObserver &rhs)
			{
				events::EventChannel<typename T::owner_type>::operator=(rhs);

				if (this != &rhs)
					managed_objects_ = this->Active() ? rhs.managed_object_ : decltype(managed_objects_){};

				return *this;
			}

			//Move assignment
			inline auto& operator=(ObjectObserver &&rhs)
			{
				events::EventChannel<typename T::owner_type>::operator=(std::move(rhs));

				if (this != &rhs)
					managed_objects_ = this->Active() ? std::exchange(rhs.managed_objects_, decltype(managed_objects_){}) : decltype(managed_objects_){};

				return *this;
			}


			//Returns true if this object observer is observing one or more object
			inline operator bool() const noexcept
			{
				return !std::empty(managed_objects_);
			}


			/*
				Modifiers
			*/

			//Sets the object requirement for this observed object
			inline void Requirement(observed_object::ObjectRequirement requirement)
			{
				this->Contract(observed_object::detail::as_subscription_contract(requirement));
			}


			/*
				Observers
			*/

			//Returns the object requirement for this observed object
			[[nodiscard]] inline auto Requirement() const noexcept
			{
				return as_object_requirement(this->Contract());
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all objects this object observer are observing
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() noexcept
			{
				return adaptors::ranges::DereferenceIterable<object_observer::detail::container_type<T>&>{managed_objects_};
			}

			//Returns an immutable range of all objects this object observer are observing
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Objects() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const object_observer::detail::container_type<T>&>{managed_objects_};
			}


			/*
				Observing
			*/

			//Observe the given object
			//Returns true if the object has successfully been observed, or change requirements
			inline auto Observe(T &object)
			{
				if (auto owner = object.Owner(); owner)
				{
					auto iter =
						std::find_if(std::begin(managed_objects_), std::end(managed_objects_),
							[&](auto &x) noexcept
							{
								return x == &object;
							});

					//Object not already observed
					if (iter == std::end(managed_objects_) &&
						((std::empty(managed_objects_) && this->Subscribe(*owner)) ||
						(!std::empty(managed_objects_) && managed_objects_.front()->Owner() == owner)))
					{
						managed_objects_.push_back(&object);
						return true;
					}
				}

				return false;
			}


			/*
				Releasing
			*/

			//Release all objects being observed
			//Returns true if all object has successfully been released, or change requirements
			inline auto ReleaseAll() noexcept
			{
				if (this->Unsubscribe())
					Tidy();

				return std::empty(managed_objects_);
			}

			//Release the given object being observed
			//Returns true if the object has successfully been released, or change requirements
			inline auto Release(T &object) noexcept
			{
				auto iter =
					std::find_if(std::begin(managed_objects_), std::end(managed_objects_),
						[&](auto &x) noexcept
						{
							return x == &object;
						});

				//Object found
				if (iter != std::end(managed_objects_) &&
					ObjectRemovable(**iter))
				{
					managed_objects_.erase(iter);
					
					//Empty, unsubscribe from publisher
					if (std::empty(managed_objects_))
						ReleaseAll();

					return true;
				}
				else
					return false;
			}
	};
} //ion::managed

#endif