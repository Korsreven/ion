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

#include <type_traits>

#include "IonManagedObject.h"
#include "events/IonEventChannel.h"

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
					return observed_object::ObjectRequirement::Mandatory;

					case events::event_channel::SubscriptionContract::Cancelable:
					default:
					return observed_object::ObjectRequirement::Optional;
				}
			}

			inline auto as_subscription_contract(ObjectRequirement requirement) noexcept
			{
				switch (requirement)
				{
					case observed_object::ObjectRequirement::Mandatory:
					return events::event_channel::SubscriptionContract::NonCancelable;

					case observed_object::ObjectRequirement::Optional:
					default:
					return events::event_channel::SubscriptionContract::Cancelable;
				}
			}
		} //detail
	} //observed_object


	template <typename T,
		typename = std::enable_if_t<observed_object::detail::has_owner_type_v<T>>>
	class ObservedObject final : protected events::EventChannel<typename T::owner_type>
	{
		static_assert(std::is_base_of_v<ManagedObject<typename T::owner_type>, T>);

		private:

			T *managed_object_;

		protected:

			/*
				Events
			*/

			//See ManagedObjectListener::ObjectRemoved for more details
			void ObjectRemoved(T &object) noexcept override
			{
				if (managed_object_ == &object)
				{
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
				managed_object_ = nullptr;
			}

		public:

			//Default constructor
			ObservedObject() = default;

			//Construct a new empty observed object with the given requirement
			ObservedObject(observed_object::ObjectRequirement requirement) noexcept :
				events::EventChannel<typename T::owner_type>{observed_object::detail::as_subscription_contract(requirement)}
			{
				//Empty
			}

			//Construct a new observed object with the given object and requirement
			ObservedObject(T &object, observed_object::ObjectRequirement requirement = observed_object::ObjectRequirement::Optional) :

				events::EventChannel<typename T::owner_type>{object.Owner(), observed_object::detail::as_subscription_contract(requirement)},
				managed_object_{this->Active() ? &object : nullptr}
			{
				//Empty
			}

			//Copy constructor
			ObservedObject(const ObservedObject &rhs) :

				events::EventChannel<typename T::owner_type>{rhs},
				managed_object_{this->Active() ? rhs.managed_object_ : nullptr}
			{
				//Empty
			}

			//Move constructor
			ObservedObject(ObservedObject &&rhs) :

				events::EventChannel<typename T::owner_type>{std::move(rhs)},
				managed_object_{this->Active() ? std::exchange(rhs.managed_object_, nullptr) : nullptr}
			{
				//Empty
			}


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const ObservedObject &rhs)
			{
				events::EventChannel<typename T::owner_type>::operator=(rhs);

				if (this != &rhs)
					managed_object_ = this->Active() ? rhs.managed_object_ : nullptr;

				return *this;
			}

			//Move assignment
			inline auto& operator=(ObservedObject &&rhs)
			{
				events::EventChannel<typename T::owner_type>::operator=(std::move(rhs));

				if (this != &rhs)
					managed_object_ = this->Active() ? std::exchange(rhs.managed_object_, nullptr) : nullptr;

				return *this;
			}


			//Returns true if this observed object is observing an object
			inline operator bool() const noexcept
			{
				return !!managed_object_;
			}

			//Returns a pointer to a mutable object
			//Returns nullptr if this observed object is not observing an object
			inline auto operator->() noexcept
			{
				return managed_object_;
			}

			//Returns a pointer to an immutable object
			//Returns nullptr if this observed object is not observing an object
			inline const auto operator->() const noexcept
			{
				return managed_object_;
			}


			/*
				Modifiers
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

			//Release the observed object
			//Returns true if the object has successfully been released, or change requirements
			inline auto Release() noexcept
			{
				if (this->Unsubscribe())
					managed_object_ = nullptr;

				return !managed_object_;
			}

			//Sets the object requirement for this observed object
			inline void Requirement(observed_object::ObjectRequirement requirement)
			{
				this->Contract(observed_object::detail::as_subscription_contract(requirement));
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
			[[nodiscard]] inline const auto Object() const noexcept	
			{
				return managed_object_;
			}

			//Returns the object requirement for this observed object
			[[nodiscard]] inline auto Requirement() const noexcept
			{
				return as_object_requirement(this->Contract());
			}
	};
} //ion::managed

#endif