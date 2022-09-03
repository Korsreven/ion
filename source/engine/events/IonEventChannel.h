/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events
File:	IonEventChannel.h
-------------------------------------------
*/

#ifndef ION_EVENT_CHANNEL_H
#define ION_EVENT_CHANNEL_H

#include <type_traits>

#include "IonListenable.h"
#include "listeners/IonListener.h"

namespace ion::events
{
	namespace event_channel
	{
		enum class SubscriptionContract : bool
		{
			Cancelable,
			NonCancelable
		};

		namespace detail
		{
			template <typename T, typename = void>
			struct has_listener_type : std::false_type {};

			template <typename T>
			struct has_listener_type<T, std::void_t<typename T::listener_type>> : std::true_type {};

			template <typename T>
			constexpr auto has_listener_type_v = has_listener_type<T>::value;


			template <typename T, typename U>
			inline auto subscribe_to_publisher(T &publisher, U &listener) -> T*
			{
				//Try to subscribe to publisher
				if (publisher.Subscribe(listener))
					return &publisher; //Subscribed

				return nullptr;
			}

			template <typename T, typename U>
			inline auto unsubscribe_from_publisher(T &publisher, U &listener) noexcept -> T*
			{
				//Try to unsubscribe from publisher
				if (publisher.Unsubscribe(listener))
					return nullptr; //Unsubscribed

				return &publisher;
			}
		} //detail
	} //event_channel


	//A class representing a channel that can listen and push events from a subscribed publisher (listenable) to a derived object
	//Can be used instead of directly subscribing to a listenable, if a subscription contract is needed
	template <typename ListenableT, typename PublisherT = ListenableT,
		typename = std::enable_if_t<event_channel::detail::has_listener_type_v<ListenableT>>>
	class EventChannel : public ListenableT::listener_type
	{
		static_assert(std::is_same_v<events::Listenable<typename ListenableT::listener_type>, ListenableT> ||
					  std::is_base_of_v<events::Listenable<typename ListenableT::listener_type>, ListenableT>);
		static_assert(std::is_same_v<ListenableT, PublisherT> ||
					  std::is_base_of_v<ListenableT, PublisherT>);

		public:

			using listenable_type = events::Listenable<typename ListenableT::listener_type>;

		private:

			ListenableT *publisher_ = nullptr;
			event_channel::SubscriptionContract contract_ = event_channel::SubscriptionContract::Cancelable;

		protected:

			inline auto DoSubscribe(ListenableT &publisher)
			{
				if (!publisher_)
				{
					publisher_ = event_channel::detail::subscribe_to_publisher(publisher, *this);
					return !!publisher_;
				}
				else
					return false;
			}

			inline auto DoUnsubscribe(bool forced = false) noexcept
			{
				if (forced)
					this->Listening(false);

				if (publisher_)
				{
					publisher_ = event_channel::detail::unsubscribe_from_publisher(*publisher_, *this);
					return !publisher_;
				}
				else
					return false;
			}


			/*
				Events
			*/

			//See Listener::Unsubscribed for more details
			void Unsubscribed(listenable_type&) noexcept override
			{
				publisher_ = nullptr;
				Unsubscribed();
			}

			//See Listener::Unsubscribable for more details
			bool Unsubscribable(listenable_type&) noexcept override
			{
				return contract_ == event_channel::SubscriptionContract::Cancelable;
			}

			//See Listener::SubscriberMoved for more details
			void SubscriberMoved(listenable_type &listenable) noexcept override
			{
				publisher_ = static_cast<ListenableT*>(&listenable);
			}


			virtual void Unsubscribed() noexcept
			{
				//Optional to override
			}

		public:

			//Default constructor
			EventChannel() = default;

			//Construct a new event channel with the given subscription contract
			EventChannel(event_channel::SubscriptionContract contract) noexcept :
				contract_{contract}
			{
				//Empty
			}

			//Construct a new event channel with the given publisher and subscription contract
			EventChannel(ListenableT &publisher, event_channel::SubscriptionContract contract = event_channel::SubscriptionContract::Cancelable) :

				publisher_{event_channel::detail::subscribe_to_publisher(publisher, *this)},
				contract_{contract}
			{
				//Empty
			}

			//Construct a new event channel with the given publisher and subscription contract
			EventChannel(ListenableT *publisher, event_channel::SubscriptionContract contract = event_channel::SubscriptionContract::Cancelable) :

				publisher_{publisher ? event_channel::detail::subscribe_to_publisher(*publisher, *this) : nullptr},
				contract_{contract}
			{
				//Empty
			}

			//Copy constructor
			EventChannel(const EventChannel &rhs) :

				publisher_(rhs.publisher_ ? event_channel::detail::subscribe_to_publisher(*rhs.publisher_, *this) : nullptr),
				contract_{rhs.contract_}
			{
				//Empty
			}

			//Move constructor
			EventChannel(EventChannel &&rhs) :

				publisher_(rhs.publisher_ ? event_channel::detail::subscribe_to_publisher(*rhs.publisher_, *this) : nullptr),
				contract_{rhs.contract_}
			{
				if (publisher_)
					rhs.DoUnsubscribe(true);
			}

			//Destructor
			~EventChannel() noexcept
			{
				DoUnsubscribe(true);
			}


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const EventChannel &rhs)
			{
				if (this != &rhs)
				{
					DoUnsubscribe();

					if (rhs.publisher_)
						DoSubscribe(*rhs.publisher_);

					contract_ = rhs.contract_;
				}

				return *this;
			}

			//Move assignment
			inline auto& operator=(EventChannel &&rhs)
			{
				if (this != &rhs)
				{
					DoUnsubscribe();

					if (rhs.publisher_)
					{
						if (DoSubscribe(*rhs.publisher_))
							rhs.DoUnsubscribe(true);
					}

					contract_ = rhs.contract_;
				}

				return *this;
			}


			/*
				Modifiers
			*/

			//Subscribe to the given publisher
			//If another publisher is being subscribed to, that publisher is unsubscribed automatically (if possible by the contract)
			//Returns true if the publisher has successfully been subscribed to, or change contract
			inline auto Subscribe(ListenableT &publisher)
			{
				if (publisher_ != &publisher)
				{
					DoUnsubscribe();
					DoSubscribe(publisher);
				}

				return !!publisher_;
			}

			//Unsubscribe from the given publisher
			//Returns true if the publisher has successfully been unsubscribed from, or change contract
			inline auto Unsubscribe() noexcept
			{
				DoUnsubscribe();
				return !publisher_;
			}

			//Sets the subscription contract for this event channel
			inline void Contract(event_channel::SubscriptionContract contract) noexcept
			{
				contract_ = contract;
			}


			/*
				Observers
			*/

			//Returns a pointer to the publisher
			//Returns nullptr if this event channel does not have a publisher
			[[nodiscard]] inline auto Publisher() const noexcept	
			{
				return publisher_;
			}

			//Returns the subscription contract for this event channel
			[[nodiscard]] inline auto Contract() const noexcept
			{
				return contract_;
			}


			/*
				Active
			*/

			//Returns true if this event channel is active (has a publisher)
			[[nodiscard]] inline auto Active() const noexcept
			{
				return !!publisher_;
			}
	};
} //ion::events

#endif