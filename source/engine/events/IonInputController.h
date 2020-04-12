/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events
File:	IonInputController.h
-------------------------------------------
*/

#ifndef ION_INPUT_CONTROLLER_H
#define ION_INPUT_CONTROLLER_H

#include <vector>

#include "events/listeners/IonWindowListener.h"
#include "system/events/listeners/IonSystemInputListener.h"

namespace ion::events
{
	using graphics::utilities::Vector2;

	namespace input_controller::detail
	{

	} //input_controller::detail


	class InputController final :
		private listeners::WindowListener,
		public system::events::listeners::InputListener
	{
		private:

			Vector2 mouse_position_;
			std::vector<listeners::KeyButton> key_buttons_;
			std::vector<listeners::MouseButton> mouse_buttons_;


			//See Listener<T>::Unsubscribing for more details
			//Make sure that if this input listener is about to unsubscribe from the system window, cancel it
			bool Unsubscribable(ion::events::Listenable<WindowListener>&) noexcept override final;


			/*
				Window listener events
			*/

			//See WindowListener::WindowActionReceived for more details
			void WindowActionReceived(listeners::WindowAction action) noexcept override final;


			/*
				Key listener events
			*/

			//See InputListener::KeyPressed for more details
			void KeyPressed(listeners::KeyButton button) noexcept override final;

			//See InputListener::KeyReleased for more details
			void KeyReleased(listeners::KeyButton button) noexcept override final;


			//See InputListener::CharacterPressed for more details
			void CharacterPressed(char character) noexcept override final;


			/*
				Mouse listener events
			*/

			//See InputListener::MousePressed for more details
			void MousePressed(listeners::MouseButton button, Vector2 position) noexcept override final;

			//See InputListener::MouseReleased for more details
			void MouseReleased(listeners::MouseButton button, Vector2 position) noexcept override final;

			//See InputListener::MouseMoved for more details
			void MouseMoved(Vector2 position) noexcept override final;


			//See InputListener::MouseWheelRolled for more details
			void MouseWheelRolled(int delta, Vector2 position) noexcept override final;

		public:

			//Create input controller on top of the given system window
			explicit InputController(graphics::render::RenderWindow &render_window) noexcept;

			//Deleted copy constructor
			InputController(const InputController&) = delete;

			//Deleted move constructor
			InputController(InputController&&) = delete;

			//Destructor
			~InputController();


			/*
				Operators
			*/

			//Deleted copy assignment
			InputController& operator=(const InputController&) = delete;

			//Deleted move assignment
			InputController& operator=(InputController&&) = delete;


			/*
				Observers
			*/

			//Returns the current mouse position
			[[nodiscard]] inline auto& MousePosition() const noexcept
			{
				return mouse_position_;
			}


			/*
				Button pressed
			*/

			//Returns true if the current key button is pressed
			[[nodiscard]] bool IsButtonPressed(listeners::KeyButton button) const noexcept;

			//Returns true if the current mouse button is pressed
			[[nodiscard]] bool IsButtonPressed(listeners::MouseButton button) const noexcept;


			/*
				Release pressed buttons
			*/

			//Release all pressed buttons
			void ReleaseButtons() noexcept;

			//Release all pressed key buttons
			void ReleaseKeyButtons() noexcept;

			//Release all pressed mouse buttons
			void ReleaseMouseButtons() noexcept;
	};
} //ion::events

#endif