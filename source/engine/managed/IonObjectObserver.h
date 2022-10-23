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

#include <optional>
#include <type_traits>

#include "IonManagedObject.h"
#include "IonObservedObject.h"
#include "adaptors/IonFlatSet.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "events/IonCallback.h"
#include "events/IonEventChannel.h"
#include "events/IonListenable.h"
#include "events/listeners/IonListenerTraits.h"

namespace ion::managed
{
	namespace object_observer::detail
	{
		template <typename T>
		using container_type = adaptors::FlatSet<T*>; //Non-owning (unique)
	} //object_observer::detail


	//A class representing multiple observed (managed) objects
	//An object observer can be optional or mandatory, meaning if the observed objects are removable or not
	//When an observed object is removed, or all are removed at once, it will notify the user by the given callback
	template <typename T,
		typename = std::enable_if_t<observed_object::detail::has_owner_type_v<T>>>
	class ObjectObserver final :
		protected events::EventChannel<events::Listenable<events::listeners::listener_of_t<T, typename T::owner_type>>, typename T::owner_type>
	{
		static_assert(std::is_base_of_v<ManagedObject<typename T::owner_type>, T>);
		using my_base = events::EventChannel<events::Listenable<events::listeners::listener_of_t<T, typename T::owner_type>>, typename T::owner_type>;

		private:

			object_observer::detail::container_type<T> managed_objects_;
			std::optional<events::Callback<void, T&>> on_removed_;
			std::optional<events::Callback<>> on_removed_all_;

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
				if (auto iter = managed_objects_.find(&object); iter != std::end(managed_objects_))
				{
					//Execute callback before object is erased
					//The callback owner can then know in advance which object is going to be removed
					if (on_removed_)
						(*on_removed_)(object);

					managed_objects_.erase(iter);

					//Object erased
					//Unsubscribe from publisher if empty
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
				//Execute callback before all objects are erased
				//The callback owner can then know in advance which object is going to be removed
				if (on_removed_all_ && !std::empty(managed_objects_))
					(*on_removed_all_)();

				Tidy();
			}

		public:

			//Default constructor
			ObjectObserver() = default;

			//Constructs a new empty observed object with the given requirement
			ObjectObserver(observed_object::ObjectRequirement requirement) noexcept :
				my_base{observed_object::detail::as_subscription_contract(requirement)}
			{
				//Empty
			}

			//Constructs a new empty observed object with the given requirement and callback
			ObjectObserver(events::Callback<void, T&> on_removed, events::Callback<> on_removed_all,
				observed_object::ObjectRequirement requirement = observed_object::ObjectRequirement::Optional) noexcept :

				my_base{observed_object::detail::as_subscription_contract(requirement)},
				on_removed_{on_removed},
				on_removed_all_{on_removed_all}
			{
				//Empty
			}

			//Copy constructor
			ObjectObserver(const ObjectObserver &rhs) :

				my_base{rhs},
				managed_objects_{this->Active() ? rhs.managed_objects_ : decltype(managed_objects_){}},
				on_removed_{rhs.on_removed_},
				on_removed_all_{rhs.on_removed_all_}
			{
				//Empty
			}

			//Move constructor
			ObjectObserver(ObjectObserver &&rhs) :

				my_base{std::move(rhs)},
				managed_objects_{this->Active() ? std::exchange(rhs.managed_objects_, decltype(managed_objects_){}) : decltype(managed_objects_){}},
				on_removed_{std::move(rhs.on_removed_)},
				on_removed_all_{std::move(rhs.on_removed_all_)}
			{
				//Empty
			}


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const ObjectObserver &rhs)
			{
				my_base::operator=(rhs);

				if (this != &rhs)
				{
					managed_objects_ = this->Active() ? rhs.managed_object_ : decltype(managed_objects_){};
					on_removed_ = rhs.on_removed_;
					on_removed_all_ = rhs.on_removed_all_;
				}

				return *this;
			}

			//Move assignment
			inline auto& operator=(ObjectObserver &&rhs)
			{
				my_base::operator=(std::move(rhs));

				if (this != &rhs)
				{
					managed_objects_ = this->Active() ? std::exchange(rhs.managed_objects_, decltype(managed_objects_){}) : decltype(managed_objects_){};
					on_removed_ = std::move(rhs.on_removed_);
					on_removed_all_ = std::move(rhs.on_removed_all_);
				}

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

			//Sets the on removed callback
			inline void OnRemoved(events::Callback<void, T&> on_removed) noexcept
			{
				on_removed_ = on_removed;
			}

			//Sets the on removed callback
			inline void OnRemoved(std::nullopt_t) noexcept
			{
				on_removed_ = {};
			}

			//Sets the on removed all callback
			inline void OnRemovedAll(events::Callback<> on_removed_all) noexcept
			{
				on_removed_all_ = on_removed_all;
			}

			//Sets the on removed all callback
			inline void OnRemovedAll(std::nullopt_t) noexcept
			{
				on_removed_all_ = {};
			}


			/*
				Observers
			*/

			//Returns the object requirement for this observed object
			[[nodiscard]] inline auto Requirement() const noexcept
			{
				return as_object_requirement(this->Contract());
			}

			//Returns the on removed callback
			[[nodiscard]] inline auto OnRemoved() const noexcept
			{
				return on_removed_;
			}

			//Returns the on removed all callback
			[[nodiscard]] inline auto OnRemovedAll() const noexcept
			{
				return on_removed_all_;
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
			[[nodiscard]] inline auto Objects() const noexcept
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
				//Is object observable
				if (auto owner = object.Owner(); owner &&
					((std::empty(managed_objects_) && this->Subscribe(*owner)) ||
					(!std::empty(managed_objects_) && (*std::begin(managed_objects_))->Owner() == owner)))
				{
					managed_objects_.insert(&object);
					return true;
				}
				else
					return false;
			}


			/*
				Releasing
			*/

			//Releases all objects being observed
			//Returns true if all object has successfully been released, or change requirements
			inline auto ReleaseAll() noexcept
			{
				if (this->Unsubscribe())
					Unsubscribed();

				return std::empty(managed_objects_);
			}

			//Releases the given object being observed
			//Returns true if the object has successfully been released, or change requirements
			inline auto Release(T &object) noexcept
			{
				if (ObjectRemovable(object) && ObjectRemoved(object))
				{
					//Object released
					//Unsubscribe from publisher if empty
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