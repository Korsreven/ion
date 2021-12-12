/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonGuiControlListener.h
-------------------------------------------
*/

#ifndef ION_GUI_CONTROL_LISTENER_H
#define ION_GUI_CONTROL_LISTENER_H

#include "IonListener.h"

namespace ion::gui::controls
{
	class GuiControl; //Forward declaration
}

namespace ion::events::listeners
{
	struct GuiControlListener : Listener<GuiControlListener>
	{
		/*
			Events
		*/

		//Called right after a gui control has been enabled, with a mutable reference to the control that was enabled
		virtual void Enabled([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}

		//Called right after a gui control has been disabled, with a mutable reference to the control that was disabled
		virtual void Disabled([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}


		//Called right after a gui control has been shown, with a mutable reference to the control that was showed
		virtual void Shown([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}

		//Called right after a gui control has been hidden, with a mutable reference to the control that was hid
		virtual void Hidden([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}


		//Called right after a gui control has been focused, with a mutable reference to the control that was focused
		virtual void Focused([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}

		//Called right after a gui control has been defocused, with a mutable reference to the control that was defocused
		virtual void Defocused([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}


		//Called right after a gui control has been pressed, with a mutable reference to the control that was pressed
		virtual void Pressed([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}

		//Called right after a gui control has been released, with a mutable reference to the control that was released
		virtual void Released([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}

		//Called right after a gui control has been clicked, with a mutable reference to the control that was clicked
		//Namely after a complete press and release
		virtual void Clicked([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}


		//Called right after a gui control has been entered, with a mutable reference to the control that was entered
		//Namely when the mouse cursor has entered the control
		virtual void Entered([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}

		//Called right after a gui control has been exited, with a mutable reference to the control that was exited
		//Namely when the mouse cursor has exited the control
		virtual void Exited([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}


		//Called right after a gui control has been changed, with a mutable reference to the control that was changed
		virtual void Changed([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}

		//Called right after a gui control has changed state, with a mutable reference to the control that changed state
		virtual void StateChanged([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}


		//Called right after a gui control has been resized, with a mutable reference to the control that was resized
		virtual void Resized([[maybe_unused]] gui::controls::GuiControl &control) noexcept
		{
			//Optional to override
		}
	};
} //ion::events::listeners

#endif