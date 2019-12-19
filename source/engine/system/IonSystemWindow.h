/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system
File:	IonSystemWindow.h
-------------------------------------------
*/

#ifndef ION_SYSTEM_WINDOW_H
#define ION_SYSTEM_WINDOW_H

#include "IonSystemAPI.h"
#include "events/listeners/IonListenerInterface.h"
#include "events/listeners/IonWindowListener.h"
#include "graphics/IonGraphicsAPI.h"
#include "system/events/listeners/IonSystemMessageListener.h"

namespace ion::system
{
	namespace window::detail
	{

	} //window::detail


	class Window final :
		protected ion::events::listeners::ListenerInterface<events::listeners::MessageListener>,
		protected ion::events::listeners::ListenerInterface<ion::events::listeners::WindowListener>
	{
		private:



		protected:

			using MessageEventsBase = ion::events::listeners::ListenerInterface<events::listeners::MessageListener>; 
			using WindowEventsBase = ion::events::listeners::ListenerInterface<ion::events::listeners::WindowListener>;

		public:

			Window() = default;

			//Deleted copy constructor
			Window(const Window&) = delete;

			//Default move constructor
			Window(Window &&rhs) = default;

			//Destructor
			//~Window();


			/*
				Operators
			*/

			//Deleted copy assignment
			Window& operator=(const Window&) = delete;

			//Default move assignment
			Window& operator=(Window&&) = default;


			/*
				Observers
			*/

			//Return a mutable reference to the message events of this input listener
			[[nodiscard]] inline auto& MessageEvents() noexcept
			{
				return static_cast<MessageEventsBase&>(*this);
			}

			//Return a immutable reference to the message events of this input listener
			[[nodiscard]] inline const auto& MessageEvents() const noexcept
			{
				return static_cast<const MessageEventsBase&>(*this);
			}


			//Return a mutable reference to the window events of this input listener
			[[nodiscard]] inline auto& Events() noexcept
			{
				return static_cast<WindowEventsBase&>(*this);
			}

			//Return a immutable reference to the window events of this input listener
			[[nodiscard]] inline auto& Events() const noexcept
			{
				return static_cast<const WindowEventsBase&>(*this);
			}
	};

} //ion::system

#endif