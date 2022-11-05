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

#ifndef ION_SYSTEM_INPUT_LISTENER_H
#define ION_SYSTEM_INPUT_LISTENER_H

#include "IonSystemMessageListener.h"
#include "events/IonListenable.h"
#include "system/IonSystemAPI.h"
#include "system/events/IonSystemInput.h"

namespace ion::graphics::render
{
	class RenderWindow; //Forward declaration
	class Viewport; //Forward declaration
}

namespace ion::system::events::listeners
{
	using graphics::utilities::Vector2;

	namespace input_listener::detail
	{
	} //input_listener::detail


	///@brief A class representing an input listener that listens to system specific inputs
	///@details When an input event has been processed, it generates either a key or mouse event that can be listened to.
	///Functions, classes and class members may need different implementation based on the underlying OS.
	///System specific code should have its own define directive
	class InputListener :
		private MessageListener,
		protected ion::events::Listenable<ion::events::listeners::KeyListener>,
		protected ion::events::Listenable<ion::events::listeners::MouseListener>,

		//Derives from key and mouse listener to be able to override all of the events,
		//thus to enforce the same function signatures in compile time
		private ion::events::listeners::KeyListener, private ion::events::listeners::MouseListener
	{
		private:

			///@brief See Listener<T>::Unsubscribable for more details
			///@details Make sure that if this input listener is about to unsubscribe from the system window, cancel it
			bool Unsubscribable(ion::events::Listenable<MessageListener>&) noexcept override final;


			/**
				@name Message listener events
				@{
			*/

			#ifdef ION_WIN32
			///@brief See MessageListener::MessageReceived for more details
			///@details Handle all key and mouse input messages from the system window
			bool MessageReceived(HWND, UINT message, WPARAM w_param, LPARAM l_param) noexcept override final;
			#endif


			///@brief Returns true if position is inside window
			bool IsInsideWindow(Vector2 position) const noexcept;

			///@brief Returns true if position is inside viewport
			bool IsInsideViewport(Vector2 position) const noexcept;

			///@brief Returns a view adjusted position
			Vector2 ViewAdjusted(Vector2 position) const noexcept;

			///@}

		protected:

			using KeyEventsBase = ion::events::Listenable<ion::events::listeners::KeyListener>; 
			using MouseEventsBase = ion::events::Listenable<ion::events::listeners::MouseListener>;

			graphics::render::RenderWindow &render_window_;


			/**
				@name Key listener events
				@{
			*/

			///@brief See KeyListener::KeyPressed for more details
			///@details Calls KeyPressed on all KeyListeners that subscibes to this input listener
			virtual void KeyPressed(ion::events::listeners::KeyButton button) noexcept override;

			///@brief See KeyListener::KeyReleased for more details
			///@details Calls KeyReleased on all KeyListeners that subscibes to this input listener
			virtual void KeyReleased(ion::events::listeners::KeyButton button) noexcept override;


			///@brief See KeyListener::CharacterPressed for more details
			///@details Calls CharacterPressed on all KeyListeners that subscibes to this input listener
			virtual void CharacterPressed(char character) noexcept override;

			///@}

			/**
				@name Mouse listener events
				@{
			*/

			///@brief See MouseListener::MousePressed for more details
			///@details Calls MousePressed on all MouseListener that subscibes to this input listener
			virtual void MousePressed(ion::events::listeners::MouseButton button, Vector2 position) noexcept override;

			///@brief See MouseListener::MouseReleased for more details
			///@details Calls MouseReleased on all MouseListener that subscibes to this input listener
			virtual void MouseReleased(ion::events::listeners::MouseButton button, Vector2 position) noexcept override;

			///@brief See MouseListener::MouseMoved for more details
			///@details Calls MouseMoved on all MouseListener that subscibes to this input listener
			virtual void MouseMoved(Vector2 position) noexcept override;


			///@brief See MouseListener::MouseWheelRolled for more details
			///@details Calls MouseWheelRolled on all MouseListener that subscibes to this input listener
			virtual void MouseWheelRolled(int delta, Vector2 position) noexcept override;

			///@}

		public:

			///@brief Constructs an input listener on top of the given system window
			explicit InputListener(graphics::render::RenderWindow &render_window) noexcept;

			///@brief Deleted copy constructor
			InputListener(const InputListener&) = delete;

			///@brief Deleted move constructor
			InputListener(InputListener&&) = delete;

			///@brief Virtual destructor
			virtual ~InputListener();


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			InputListener& operator=(const InputListener&) = delete;

			///@brief Deleted move assignment
			InputListener& operator=(InputListener&&) = delete;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns a mutable reference to the key base of this input listener
			[[nodiscard]] inline auto& KeyEvents() noexcept
			{
				return static_cast<KeyEventsBase&>(*this);
			}

			///@brief Returns an immutable reference to the key base of this input listener
			[[nodiscard]] inline auto& KeyEvents() const noexcept
			{
				return static_cast<const KeyEventsBase&>(*this);
			}


			///@brief Returns a mutable reference to the mouse base of this input listener
			[[nodiscard]] inline auto& MouseEvents() noexcept
			{
				return static_cast<MouseEventsBase&>(*this);
			}

			///@brief Returns an immutable reference to the mouse base of this input listener
			[[nodiscard]] inline auto& MouseEvents() const noexcept
			{
				return static_cast<const MouseEventsBase&>(*this);
			}

			///@}
	};
} //ion::system::events::listeners

#endif