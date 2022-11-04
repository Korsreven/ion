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

#include "listeners/IonWindowListener.h"
#include "system/events/listeners/IonSystemInputListener.h"

namespace ion::events
{
	using graphics::utilities::Vector2;

	namespace input_controller::detail
	{

	} //input_controller::detail


	///@brief A class representing a controller that listens to window inputs and manages those events
	///@details When an input event has been processed, it generates either a key or mouse event that can be listened to
	class InputController final :
		private listeners::WindowListener,
		public system::events::listeners::InputListener
	{
		private:

			Vector2 mouse_position_;
			std::vector<listeners::KeyButton> key_buttons_;
			std::vector<listeners::MouseButton> mouse_buttons_;


			///@brief See Listener<T>::Unsubscribable for more details
			///@details Make sure that if this input listener is about to unsubscribe from the system window, cancel it
			bool Unsubscribable(ion::events::Listenable<WindowListener>&) noexcept override final;


			/**
				@name Window listener events
				@{
			*/

			///@brief See WindowListener::WindowActionReceived for more details
			void WindowActionReceived(listeners::WindowAction action) noexcept override final;

			///@}

			/**
				@name Key listener events
				@{
			*/

			///@brief See InputListener::KeyPressed for more details
			void KeyPressed(listeners::KeyButton button) noexcept override final;

			///@brief See InputListener::KeyReleased for more details
			void KeyReleased(listeners::KeyButton button) noexcept override final;


			///@brief See InputListener::CharacterPressed for more details
			void CharacterPressed(char character) noexcept override final;

			///@}

			/**
				@name Mouse listener events
				@{
			*/

			///@brief See InputListener::MousePressed for more details
			void MousePressed(listeners::MouseButton button, Vector2 position) noexcept override final;

			///@brief See InputListener::MouseReleased for more details
			void MouseReleased(listeners::MouseButton button, Vector2 position) noexcept override final;

			///@brief See InputListener::MouseMoved for more details
			void MouseMoved(Vector2 position) noexcept override final;


			///@brief See InputListener::MouseWheelRolled for more details
			void MouseWheelRolled(int delta, Vector2 position) noexcept override final;

			///@}

		public:

			///@brief Constructs an input controller on top of the given system window
			explicit InputController(graphics::render::RenderWindow &render_window);

			///@brief Deleted copy constructor
			InputController(const InputController&) = delete;

			///@brief Deleted move constructor
			InputController(InputController&&) = delete;

			///@brief Destructor
			~InputController();


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			InputController& operator=(const InputController&) = delete;

			///@brief Deleted move assignment
			InputController& operator=(InputController&&) = delete;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the current mouse position
			[[nodiscard]] inline auto& MousePosition() const noexcept
			{
				return mouse_position_;
			}

			///@}

			/**
				@name Button pressed
				@{
			*/

			///@brief Returns true if the current key button is pressed
			[[nodiscard]] bool IsButtonPressed(listeners::KeyButton button) const noexcept;

			///@brief Returns true if the current mouse button is pressed
			[[nodiscard]] bool IsButtonPressed(listeners::MouseButton button) const noexcept;

			///@}

			/**
				@name Release pressed buttons
				@{
			*/

			///@brief Releases all pressed buttons
			void ReleaseButtons() noexcept;

			///@brief Releases all pressed key buttons
			void ReleaseKeyButtons() noexcept;

			///@brief Releases all pressed mouse buttons
			void ReleaseMouseButtons() noexcept;

			///@}
	};
} //ion::events

#endif