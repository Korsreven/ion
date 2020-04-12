/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events
File:	IonListenable.h
-------------------------------------------
*/

#ifndef ION_LISTENABLE_H
#define ION_LISTENABLE_H

#include <algorithm>
#include <type_traits>
#include <vector>

#include "IonEventGenerator.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "listeners/IonListener.h"

namespace ion::events
{
	namespace listenable::detail
	{
		template <typename T>
		using container_type = std::vector<T*>; //Non-owning
	} //listenable::detail


	template <typename T>
	class Listenable : protected EventGenerator<T>
	{
		static_assert(std::is_base_of_v<listeners::Listener<T>, T>);

		public:

			using listener_type = T;

		private:

			listenable::detail::container_type<T> listeners_;

		protected:

			/*
				Notifying
			*/

			auto NotifySubscribable(T &listener) noexcept
			{
				return this->Notify(&T::Subscribable, listener, *this).value_or(true);
			}

			void NotifySubscribed(T &listener) noexcept
			{
				this->Notify(&T::Subscribed, listener, *this);
			}

			auto NotifyUnsubscribable(T &listener) noexcept
			{
				return this->Notify(&T::Unsubscribable, listener, *this).value_or(true);
			}

			void NotifyUnsubscribed(T &listener) noexcept
			{
				this->Notify(&T::Unsubscribed, listener, *this);
			}

			void NotifySubscriberMoved(T &listener) noexcept
			{
				this->Notify(&T::SubscriberMoved, listener, *this);
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

			void NotifySubscriberMovedAll(decltype(listeners_) &listeners) noexcept
			{
				for (auto &listener : listeners)
					NotifySubscriberMoved(*listener);
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
			Listenable() = default;

			//Deleted copy constructor
			Listenable(const Listenable&) = delete;

			//Default move constructor
			Listenable(Listenable &&rhs) noexcept :
				listeners_{std::move(rhs.listeners_)}
			{
				NotifySubscriberMovedAll(listeners_);
			}

			//Destructor
			~Listenable() noexcept
			{
				Tidy();
			}


			/*
				Operators
			*/

			//Deleted copy assignment
			Listenable& operator=(const Listenable&) = delete;

			//Move assignment
			inline auto& operator=(Listenable &&rhs) noexcept
			{
				if (this != &rhs)
				{
					Tidy();
					listeners_ = std::move(rhs.listeners_);
					NotifySubscriberMovedAll(listeners_);
				}

				return *this;
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all listeners subscribed to this listener listenable
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Listeners() noexcept
			{
				return adaptors::ranges::DereferenceIterable<listenable::detail::container_type<T>&>{listeners_};
			}

			//Returns an immutable range of all listeners subscribed to this listener listenable
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Listeners() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const listenable::detail::container_type<T>&>{listeners_};
			}


			/*
				Subscribing
			*/

			//Subscribe a subscribable listener to this listenable
			auto Subscribe(T &listener)
			{
				auto iter =
					std::find_if(std::cbegin(listeners_), std::cend(listeners_),
						[&](auto &x) noexcept
						{
							return x == &listener;
						});

				//Listener not already subscribed
				if (iter == std::end(listeners_) &&
					NotifySubscribable(listener))
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

			//Unsubscribe a unsubscribable listener from this listenable
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

			//Unsubscribe all unsubscribable listeners from this listenable
			void UnsubscribeAll() noexcept
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
	};
} //ion::events

#endif