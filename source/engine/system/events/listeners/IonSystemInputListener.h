/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system/events/listeners
File:	IonSystemInputListener.h
-------------------------------------------
*/

#ifndef _ION_SYSTEM_INPUT_LISTENER_
#define _ION_SYSTEM_INPUT_LISTENER_

#include "events/listeners/IonListenerInterface.h"
#include "system/events/IonSystemInput.h"
#include "system/events/listeners/IonSystemMessageListener.h"

//Forward declaration
namespace ion::system
{
	class Window;
} //ion::system

namespace ion::system::events::listeners
{
	namespace input_listener::detail
	{
	} //input_listener::detail


	class InputListener :
		private MessageListener,
		protected ion::events::listeners::ListenerInterface<ion::events::listeners::KeyListener>,
		protected ion::events::listeners::ListenerInterface<ion::events::listeners::MouseListener>,

		//Derives from key and mouse listener to be able to override all of the events,
		//thus to enforce the same function signatures in compile time
		private ion::events::listeners::KeyListener, private ion::events::listeners::MouseListener
	{
		private:

			//See Listener<T>::Unsubscribable for more details
			//Make sure that if this input listener is about to unsubscribe from the system window, cancel it
			bool Unsubscribable(ion::events::listeners::ListenerInterface<MessageListener>&) noexcept override final;


			/*
				Message listener events
			*/

			#ifdef _WIN32
			//See MessageListener::MessageReceived for more details
			//Handle all key and mouse input messages from the system window
			bool MessageReceived(HWND, UINT message, WPARAM w_param, LPARAM l_param) noexcept override final;
			#endif


			//Return true if position is outside window
			bool IsInsideWindow(real x, real y) const noexcept;

			//Return a viewport adjusted position
			graphics::utilities::Vector2 ViewportAdjusted(real x, real y) const noexcept;

		protected:

			using KeyEventsBase = ion::events::listeners::ListenerInterface<ion::events::listeners::KeyListener>; 
			using MouseEventsBase = ion::events::listeners::ListenerInterface<ion::events::listeners::MouseListener>;

			system::Window &window_;


			/*
				Key listener events
			*/

			//See KeyListener::KeyPressed for more details
			//Calls KeyPressed on all KeyListeners that subscibes to this input listener
			virtual void KeyPressed(ion::events::listeners::KeyButton button) noexcept override;

			//See KeyListener::KeyReleased for more details
			//Calls KeyReleased on all KeyListeners that subscibes to this input listener
			virtual void KeyReleased(ion::events::listeners::KeyButton button) noexcept override;


			//See KeyListener::CharacterPressed for more details
			//Calls CharacterPressed on all KeyListeners that subscibes to this input listener
			virtual void CharacterPressed(char character) noexcept override;


			/*
				Mouse listener events
			*/

			//See MouseListener::MousePressed for more details
			//Calls MousePressed on all MouseListener that subscibes to this input listener
			virtual void MousePressed(ion::events::listeners::MouseButton button, graphics::utilities::Vector2 position) noexcept override;

			//See MouseListener::MouseReleased for more details
			//Calls MouseReleased on all MouseListener that subscibes to this input listener
			virtual void MouseReleased(ion::events::listeners::MouseButton button, graphics::utilities::Vector2 position) noexcept override;

			//See MouseListener::MouseMoved for more details
			//Calls MouseMoved on all MouseListener that subscibes to this input listener
			virtual void MouseMoved(graphics::utilities::Vector2 position) noexcept override;


			//See MouseListener::MouseWheelRolled for more details
			//Calls MouseWheelRolled on all MouseListener that subscibes to this input listener
			virtual void MouseWheelRolled(int delta, graphics::utilities::Vector2 position) noexcept override;

		public:

			//Create input listener on top of the given system window
			explicit InputListener(system::Window &window) noexcept;

			//Deleted copy constructor
			InputListener(const InputListener&) = delete;

			//Deleted move constructor
			InputListener(InputListener&&) = delete;

			//Virtual destructor
			virtual ~InputListener();


			/*
				Operators
			*/

			//Deleted copy assignment
			InputListener& operator=(const InputListener&) = delete;

			//Deleted move assignment
			InputListener& operator=(InputListener&&) = delete;


			/*
				Observers
			*/

			//Return a mutable reference to the key base of this input listener
			[[nodiscard]] inline auto& KeyEvents() noexcept
			{
				return static_cast<KeyEventsBase&>(*this);
			}

			//Return a immutable reference to the key base of this input listener
			[[nodiscard]] inline const auto& KeyEvents() const noexcept
			{
				return static_cast<const KeyEventsBase&>(*this);
			}


			//Return a mutable reference to the mouse base of this input listener
			[[nodiscard]] inline auto& MouseEvents() noexcept
			{
				return static_cast<MouseEventsBase&>(*this);
			}

			//Return a immutable reference to the mouse base of this input listener
			[[nodiscard]] inline auto& MouseEvents() const noexcept
			{
				return static_cast<const MouseEventsBase&>(*this);
			}
	};
} //ion::system::events::listeners

#endif