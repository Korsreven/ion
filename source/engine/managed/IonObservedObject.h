/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	managed
File:	IonObservedObject.h
-------------------------------------------
*/

#ifndef ION_OBSERVED_OBJECT_H
#define ION_OBSERVED_OBJECT_H

#include <optional>
#include <type_traits>

#include "IonManagedObject.h"
#include "events/IonCallback.h"
#include "events/IonEventChannel.h"
#include "events/IonListenable.h"
#include "events/listeners/IonListenerTraits.h"

namespace ion::managed
{
	namespace observed_object
	{
		enum class ObjectRequirement : bool
		{	
			Optional,
			Mandatory
		};

		namespace detail
		{
			template <typename T, typename = void>
			struct has_owner_type : std::false_type {};

			template <typename T>
			struct has_owner_type<T, std::void_t<typename T::owner_type>> : std::true_type {};

			template <typename T>
			constexpr auto has_owner_type_v = has_owner_type<T>::value;


			inline auto as_object_requirement(events::event_channel::SubscriptionContract contract) noexcept
			{
				switch (contract)
				{
					case events::event_channel::SubscriptionContract::NonCancelable:
					return ObjectRequirement::Mandatory;

					case events::event_channel::SubscriptionContract::Cancelable:
					default:
					return ObjectRequirement::Optional;
				}
			}

			inline auto as_subscription_contract(ObjectRequirement requirement) noexcept
			{
				switch (requirement)
				{
					case ObjectRequirement::Mandatory:
					return events::event_channel::SubscriptionContract::NonCancelable;

					case ObjectRequirement::Optional:
					default:
					return events::event_channel::SubscriptionContract::Cancelable;
				}
			}
		} //detail
	} //observed_object


	//A class representing an observed (managed) object
	//An observed object can be optional or mandatory, meaning if it's removable or not
	//When an observed object is removed, it will notify the user by the given callback
	template <typename T,
		typename = std::enable_if_t<observed_object::detail::has_owner_type_v<T>>>
	class ObservedObject final :
		protected events::EventChannel<events::Listenable<events::listeners::listener_of_t<T, typename T::owner_type>>, typename T::owner_type>
	{
		static_assert(std::is_base_of_v<ManagedObject<typename T::owner_type>, T>);
		using my_base = events::EventChannel<events::Listenable<events::listeners::listener_of_t<T, typename T::owner_type>>, typename T::owner_type>;

		private:

			T *managed_object_ = nullptr;
			std::optional<events::Callback<void, T&>> on_removed_;

		protected:

			/*
				Events
			*/

			//See ManagedObjectListener::ObjectRemoved for more details
			void ObjectRemoved(T &object) noexcept override
			{
				if (managed_object_ == &object)
				{
					//Execute callback before object is set to nullptr
					//The callback owner can then know in advance which object is going to be removed
					if (on_removed_)
						(*on_removed_)(object);

					managed_object_ = nullptr;
					this->DoUnsubscribe(true);
				}
			}

			//See ManagedObjectListener::ObjectRemovable for more details
			bool ObjectRemovable(T&) noexcept override
			{
				return this->Contract() == events::event_channel::SubscriptionContract::Cancelable;
			}


			void Unsubscribed() noexcept override
			{
				//Execute callback before object is set to nullptr
				//The callback owner can then know in advance which object is going to be removed
				if (on_removed_ && managed_object_)
					(*on_removed_)(*managed_object_);

				managed_object_ = nullptr;
			}

		public:

			//Default constructor
			ObservedObject() = default;

			//Constructs a new empty observed object with the given requirement
			ObservedObject(observed_object::ObjectRequirement requirement) noexcept :
				my_base{observed_object::detail::as_subscription_contract(requirement)}
			{
				//Empty
			}

			//Constructs a new empty observed object with the given requirement and callback
			ObservedObject(events::Callback<void, T&> on_removed, observed_object::ObjectRequirement requirement = observed_object::ObjectRequirement::Optional) noexcept :
				my_base{observed_object::detail::as_subscription_contract(requirement)},
				on_removed_{on_removed}
			{
				//Empty
			}

			//Constructs a new observed object with the given object and requirement
			ObservedObject(T &object, observed_object::ObjectRequirement requirement = observed_object::ObjectRequirement::Optional) :

				my_base{object.Owner(), observed_object::detail::as_subscription_contract(requirement)},
				managed_object_{this->Active() ? &object : nullptr}
			{
				//Empty
			}

			//Constructs a new observed object with the given object, callback and requirement
			ObservedObject(T &object, events::Callback<void, T&> on_removed, observed_object::ObjectRequirement requirement = observed_object::ObjectRequirement::Optional) :

				my_base{object.Owner(), observed_object::detail::as_subscription_contract(requirement)},
				managed_object_{this->Active() ? &object : nullptr},
				on_removed_{on_removed}
			{
				//Empty
			}

			//Copy constructor
			ObservedObject(const ObservedObject &rhs) :

				my_base{rhs},
				managed_object_{this->Active() ? rhs.managed_object_ : nullptr},
				on_removed_{rhs.on_removed_}
			{
				//Empty
			}

			//Move constructor
			ObservedObject(ObservedObject &&rhs) :

				my_base{std::move(rhs)},
				managed_object_{this->Active() ? std::exchange(rhs.managed_object_, nullptr) : nullptr},
				on_removed_{std::move(rhs.on_removed_)}
			{
				//Empty
			}


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const ObservedObject &rhs)
			{
				my_base::operator=(rhs);

				if (this != &rhs)
				{
					managed_object_ = this->Active() ? rhs.managed_object_ : nullptr;
					on_removed_ = rhs.on_removed_;
				}

				return *this;
			}

			//Move assignment
			inline auto& operator=(ObservedObject &&rhs)
			{
				my_base::operator=(std::move(rhs));

				if (this != &rhs)
				{
					managed_object_ = this->Active() ? std::exchange(rhs.managed_object_, nullptr) : nullptr;
					on_removed_ = std::move(rhs.on_removed_);
				}

				return *this;
			}


			//Returns true if this observed object is observing an object
			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!managed_object_;
			}

			//Returns a pointer to the managed object
			//Returns nullptr if this observed object is not observing an object
			[[nodiscard]] inline auto operator->() const noexcept
			{
				return managed_object_;
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


			/*
				Observers
			*/

			//Returns a pointer to a mutable object
			//Returns nullptr if this observed object does not observe an object
			[[nodiscard]] inline auto Object() noexcept	
			{
				return managed_object_;
			}

			//Returns a pointer to an immutable object
			//Returns nullptr if this observed object does not observe an object
			[[nodiscard]] inline auto Object() const noexcept	
			{
				return managed_object_;
			}

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


			/*
				Observing/releasing
			*/

			//Observe the given object
			//If another object is already being observed, that object is released automatically (if possible by the requirements)
			//Returns true if the object has successfully been observed, or change requirements
			inline auto Observe(T &object)
			{
				if (managed_object_ != &object)
				{
					if (auto owner = object.Owner(); owner && this->Subscribe(*owner))
						managed_object_ = &object;
				}

				return !!managed_object_;
			}

			//Releases the object being observed
			//Returns true if the object has successfully been released, or change requirements
			inline auto Release() noexcept
			{
				if (this->Unsubscribe())
					Unsubscribed();

				return !managed_object_;
			}
	};
} //ion::managed

#endif