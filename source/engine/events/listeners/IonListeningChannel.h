/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonListeningChannel.h
-------------------------------------------
*/

#ifndef ION_LISTENING_CHANNEL_H
#define ION_LISTENING_CHANNEL_H

#include <type_traits>

#include "events/listeners/IonListener.h"
#include "events/listeners/IonListenerInterface.h"

namespace ion::events::listeners
{
	namespace listening_channel
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
	} //listening_channel


	template <typename T,
		typename = std::enable_if_t<listening_channel::detail::has_listener_type_v<T>>>
	class ListeningChannel : public T::listener_type
	{
		static_assert(std::is_same_v<events::listeners::ListenerInterface<typename T::listener_type>, T>);

		private:

			T *publisher_ = nullptr;
			listening_channel::SubscriptionContract contract_ = listening_channel::SubscriptionContract::Cancelable;


			inline auto DoSubscribe(T &publisher)
			{
				if (!publisher_)
				{
					publisher_ = listening_channel::detail::subscribe_to_publisher(publisher, *this);
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
					publisher_ = listening_channel::detail::unsubscribe_from_publisher(*publisher_, *this);
					return !publisher_;
				}
				else
					return false;
			}

		protected:

			/*
				Events
			*/

			//See Listener::Unsubscribed for more details
			void Unsubscribed(events::listeners::ListenerInterface<typename T::listener_type>&) noexcept override
			{
				publisher_ = nullptr;
			}

			//See Listener::Unsubscribable for more details
			bool Unsubscribable(events::listeners::ListenerInterface<typename T::listener_type>&) noexcept override
			{
				return contract_ == listening_channel::SubscriptionContract::Cancelable;
			}

		public:

			//Default constructor
			ListeningChannel() = default;

			//Construct a new listening channel with the given subscription contract
			ListeningChannel(listening_channel::SubscriptionContract contract) noexcept :
				contract_{contract}
			{
				//Empty
			}

			//Construct a new listening channel with the given publisher and subscription contract
			ListeningChannel(T &publisher, listening_channel::SubscriptionContract contract = listening_channel::SubscriptionContract::Cancelable) :
				publisher_{listening_channel::detail::subscribe_to_publisher(publisher, *this)},
				contract_{contract}
			{
				//Empty
			}

			//Copy constructor
			ListeningChannel(const ListeningChannel &rhs) :
				publisher_(rhs.publisher_ ? listening_channel::detail::subscribe_to_publisher(*rhs.publisher_, *this) : nullptr),
				contract_{rhs.contract_}
			{
				//Empty
			}

			//Move constructor
			ListeningChannel(ListeningChannel &&rhs) :
				publisher_(rhs.publisher_ ? listening_channel::detail::subscribe_to_publisher(*rhs.publisher_, *this) : nullptr),
				contract_{rhs.contract_}
			{
				if (publisher_)
					rhs.DoUnsubscribe(true);
			}

			//Destructor
			~ListeningChannel() noexcept
			{
				DoUnsubscribe(true);
			}


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const ListeningChannel &rhs)
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
			inline auto& operator=(ListeningChannel &&rhs)
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

			//Start a new subscription with the given publisher
			//If another publisher is being subscribed to, that publisher is unsubscribed automatically (if possible by the contract)
			//Returns true if the subscription has successfully been started, or change contract
			inline auto StartSubscription(T &publisher)
			{
				if (publisher_ != &publisher)
				{
					DoUnsubscribe();
					DoSubscribe(publisher);
				}

				return !!publisher_;
			}

			//Cancel subscription with the given publisher
			//If another publisher is being subscribed to, that publisher is unsubscribed automatically (if possible by the contract)
			//Returns true if the subscription has successfully been canceled, or change contract
			inline auto CancelSubscription() noexcept
			{
				DoUnsubscribe();
				return !publisher_;
			}

			//Sets the subscription contract for this listening channel
			inline void Contract(listening_channel::SubscriptionContract contract) noexcept
			{
				contract_ = contract;
			}


			/*
				Observers
			*/

			//Returns a pointer to a mutable publisher
			//Returns nullptr if this listening channel does not have a publisher
			[[nodiscard]] inline auto Publisher() noexcept	
			{
				return publisher_;
			}

			//Returns a pointer to an immutable publisher
			//Returns nullptr if this listening channel does not have a publisher
			[[nodiscard]] inline const auto Publisher() const noexcept	
			{
				return publisher_;
			}

			//Returns the subscription contract for this listening channel
			[[nodiscard]] inline auto Contract() const noexcept
			{
				return contract_;
			}


			/*
				Active
			*/

			//Returns true if this listening channel has an active subscription
			[[nodiscard]] inline auto Active() const noexcept
			{
				return !!publisher_;
			}
	};
} //ion::events::listeners

#endif