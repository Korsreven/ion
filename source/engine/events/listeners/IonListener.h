/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonListener.h
-------------------------------------------
*/

#ifndef ION_LISTENER_H
#define ION_LISTENER_H

#include <type_traits>

namespace ion::events
{
	template <typename T>
	class Listenable;
} //ion::events

namespace ion::events::listeners
{
	//A base class representing something that can listen to a listenable
	//Listeners can be subscribe to listenables, to receive events
	//When not listening, no events will be received
	template <typename T>
	class Listener
	{
		private:
		
			bool listening_ = true;

		public:

			/*
				Modifiers
			*/

			//Sets if this listener is listening or not
			inline void Listening(bool listening) noexcept
			{
				listening_ = listening;
			}


			/*
				Observers
			*/

			//Returns if this listener is listening or not
			//If not listening, this listener will not receive any events
			[[nodiscard]] inline auto Listening() const noexcept
			{
				return listening_;
			}


			/*
				Events
			*/

			//Called right before this listener is subscribed, with a reference to the listenable
			//Returns false from this function if the subscription should be canceled
			virtual bool Subscribable([[maybe_unused]] Listenable<T> &listenable) noexcept
			{
				//Optional to override
				return true;
			}

			//Called right after this listener has been subscribed, with a reference to the listenable
			virtual void Subscribed([[maybe_unused]] Listenable<T> &listenable) noexcept
			{
				//Optional to override
			}


			//Called right before this listener is unsubscribed, with a reference to the listenable
			//Returns false from this function if the unsubscription should be canceled
			virtual bool Unsubscribable([[maybe_unused]] Listenable<T> &listenable) noexcept
			{
				//Optional to override
				return true;
			}

			//Called right after this listener has been unsubscribed, with a reference to the listenable
			virtual void Unsubscribed([[maybe_unused]] Listenable<T> &listenable) noexcept
			{
				//Optional to override
			}


			//Called right after this subscriber has been moved, with a reference to the new listenable
			//A subscriber is a listener that has an active subscription
			virtual void SubscriberMoved([[maybe_unused]] Listenable<T> &listenable) noexcept
			{
				//Optional to override
			}
	};
} //ion::events::listeners

#endif