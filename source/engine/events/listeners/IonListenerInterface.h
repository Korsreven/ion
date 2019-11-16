/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonListenerInterface.h
-------------------------------------------
*/

#ifndef ION_LISTENER_INTERFACE_H
#define ION_LISTENER_INTERFACE_H

#include <algorithm>
#include <functional>
#include <optional>
#include <type_traits>
#include <vector>

#include "IonListener.h"
#include "adaptors/ranges/IonDereferenceIterable.h"

namespace ion::events::listeners
{
	namespace listener_interface::detail
	{
		template <typename T>
		using container_type = std::vector<T*>; //Non-owning


		//This type is very similar to std::monostate
		//But we do not want to #include <variant> just to get it
		struct empty final {};

		template <typename T>
		struct notify_result
		{
			using type = T;
		};

		template <>
		struct notify_result<void>
		{
			using type = empty;
		};

		template <typename T>
		using notify_result_t = typename notify_result<T>::type;
	} //listener_interface::detail


	template <typename T>
	class ListenerInterface
	{
		static_assert(std::is_base_of_v<Listener<T>, T>);

		private:

			listener_interface::detail::container_type<T> listeners_;

		protected:

			/*
				Notifying
			*/

			template <typename Event, typename... Args>
			auto Notify(Event &&event, T &listener, Args &&...args) noexcept
			{
				static_assert(std::is_invocable_v<Event, T, Args...>);

				if (listener.Listening())
				{
					if constexpr (std::is_same_v<std::invoke_result_t<Event, T, Args...>, void>)
					{
						std::invoke(std::forward<Event>(event), std::forward<T>(listener), std::forward<Args>(args)...);
						return std::make_optional(listener_interface::detail::notify_result_t<void>{});
					}
					else
						return std::make_optional(
							std::invoke(std::forward<Event>(event), std::forward<T>(listener), std::forward<Args>(args)...));
				}

				return std::optional<listener_interface::detail::notify_result_t<std::invoke_result_t<Event, T, Args...>>>{};
			}


			auto NotifySubscribable(T &listener) noexcept
			{
				return Notify(&T::Subscribable, listener, *this).value_or(true);
			}

			void NotifySubscribed(T &listener) noexcept
			{
				Notify(&T::Subscribed, listener, *this);
			}


			auto NotifyUnsubscribable(T &listener) noexcept
			{
				return Notify(&T::Unsubscribable, listener, *this).value_or(true);
			}

			void NotifyUnsubscribed(T &listener) noexcept
			{
				Notify(&T::Unsubscribed, listener, *this);
			}

			void NotifyUnsubscribableAll() noexcept
			{
				for (auto &listener : listeners_)
					NotifyUnsubscribable(*listener);
			}

			void NotifyUnsubscribedAll(decltype(listeners_) &listeners) noexcept
			{
				for (auto &listener : listeners)
					NotifyUnsubscribed(*listener);
			}


			/*
				Clearing
			*/

			void Tidy() noexcept
			{
				NotifyUnsubscribableAll();
				decltype(listeners_) listeners = std::move(listeners_);
				listeners_.shrink_to_fit();
				NotifyUnsubscribedAll(listeners);
			}

		public:

			//Default constructor
			ListenerInterface() = default;

			//Deleted copy constructor
			ListenerInterface(const ListenerInterface&) = delete;

			//Default move constructor
			ListenerInterface(ListenerInterface&&) = default;

			//Destructor
			~ListenerInterface() noexcept
			{
				Tidy();
			}


			/*
				Operators
			*/

			//Deleted copy assignment
			ListenerInterface& operator=(const ListenerInterface&) = delete;

			//Move assignment
			inline auto& operator=(ListenerInterface &&rhs) noexcept
			{
				Tidy();
				listeners_ = std::move(rhs.listeners_);
				return *this;
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all listeners subscribed to this listener interface
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Listeners() noexcept
			{
				return adaptors::ranges::DereferenceIterable<listener_interface::detail::container_type<T>&>{listeners_};
			}

			//Returns an immutable range of all listeners subscribed to this listener interface
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Listeners() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const listener_interface::detail::container_type<T>&>{listeners_};
			}


			/*
				Clearing
			*/

			//Unsubscribe all unsubscribable listeners from this interface
			void Clear() noexcept
			{
				//[listeners to keep, listeners to unsubscribe]
				auto iter =
					std::stable_partition(std::begin(listeners_), std::end(listeners_),
						[&](auto &listener) noexcept
						{
							return !NotifyUnsubscribable(*listener);
						});

				decltype(listeners_) listeners;
				std::move(iter, std::end(listeners_), std::back_inserter(listeners));
				listeners_.erase(iter, std::end(listeners_)); //Erase the moved range
				listeners_.shrink_to_fit();
				NotifyUnsubscribedAll(listeners);
			}


			/*
				Subscribing
			*/

			//Subscribe a subscribable listener to this interface
			auto Subscribe(T &listener) noexcept
			{
				auto iter =
					std::find_if(std::cbegin(listeners_), std::cend(listeners_),
						[&](auto &x) noexcept
						{
							return x == &listener;
						});

				//Listener not already subscribed
				if (iter == std::end(listeners_) &&
					NotifyUnsubscribable(listener))
				{
					listeners_.push_back(&listener);
					NotifySubscribed(listener);
					return true;
				}
				else
					return false;
			}


			/*
				Unsubscribing
			*/

			//Unsubscribe a unsubscribable listener from this interface
			auto Unsubscribe(T &listener) noexcept
			{
				auto iter =
					std::find_if(std::begin(listeners_), std::end(listeners_),
						[&](auto &x) noexcept
						{
							return x == &listener;
						});

				//Listener found
				if (iter != std::end(listeners_) &&
					NotifyUnsubscribable(listener))
				{
					listeners_.erase(iter);
					NotifyUnsubscribed(listener);
					return true;
				}
				else
					return false;
			}
	};
} //ion::events::listeners

#endif