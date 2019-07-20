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

#ifndef _ION_LISTENER_
#define _ION_LISTENER_

namespace ion::events::listeners
{
	template <typename T>
	class ListenerInterface;


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

			//Called right before a listener is subscribed, with a reference to the listener interface
			//Return false from this function if the subscription should be canceled
			virtual bool Subscribable([[maybe_unused]] ListenerInterface<T> &listener_interface) noexcept
			{
				//Optional to override
				return true;
			}

			//Called right after a listener has been subscribed, with a reference to the listener interface
			virtual void Subscribed([[maybe_unused]] ListenerInterface<T> &listener_interface) noexcept
			{
				//Optional to override
			}


			//Called right before a listener is unsubscribed, with a reference to the listener interface
			//Return false from this function if the unsubscription should be canceled
			virtual bool Unsubscribable([[maybe_unused]] ListenerInterface<T> &listener_interface) noexcept
			{
				//Optional to override
				return true;
			}

			//Called right after a listener has been unsubscribed, with a reference to the listener interface
			virtual void Unsubscribed([[maybe_unused]] ListenerInterface<T> &listener_interface) noexcept
			{
				//Optional to override
			}
	};
} //ion::events::listeners

#endif