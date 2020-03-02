/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonMouseListener.h
-------------------------------------------
*/

#ifndef ION_MOUSE_LISTENER_H
#define ION_MOUSE_LISTENER_H

#include "IonListener.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::events::listeners
{
	enum class MouseButton
	{
		Left,
		Right,
		Middle,

		X1,
		X2
	};

	struct MouseListener : Listener<MouseListener>
	{
		/*
			Events
		*/

		//Called when a mouse button is pressed, with the button that was pressed and the position of the mouse
		virtual void MousePressed(MouseButton button, const graphics::utilities::Vector2 &position) noexcept = 0;

		//Called when a mouse button is released, with the button that was released and the position of the mouse
		virtual void MouseReleased(MouseButton button, const graphics::utilities::Vector2 &position) noexcept = 0;

		//Called when the mouse is moved, with the position of the mouse
		virtual void MouseMoved(const graphics::utilities::Vector2 &position) noexcept = 0;


		//Called when the mouse wheel is rolled, with the scroll delta and the position of the mouse
		//Positive delta indicates forward rotation (away from the user)
		//Negative delta indicates backward rotation (toward the user)
		virtual void MouseWheelRolled([[maybe_unused]] int delta, [[maybe_unused]] const graphics::utilities::Vector2 &position) noexcept
		{
			//Optional to override
		}
	};
} //ion::events::listeners

#endif