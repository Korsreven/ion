/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonGuiFrameListener.h
-------------------------------------------
*/

#ifndef ION_GUI_FRAME_LISTENER_H
#define ION_GUI_FRAME_LISTENER_H

#include "IonListener.h"

namespace ion::gui
{
	class GuiFrame; //Forward declaration
}

namespace ion::events::listeners
{
	//A class representing a listener that listens to events from a GUI frame
	struct GuiFrameListener : Listener<GuiFrameListener>
	{
		/*
			Events
		*/

		//Called right after a gui frame has been enabled, with a mutable reference to the frame that was enabled
		virtual void Enabled([[maybe_unused]] gui::GuiFrame &frame) noexcept
		{
			//Optional to override
		}

		//Called right after a gui frame has been disabled, with a mutable reference to the frame that was disabled
		virtual void Disabled([[maybe_unused]] gui::GuiFrame &frame) noexcept
		{
			//Optional to override
		}


		//Called right after a gui frame has been activated, with a mutable reference to the frame that was activated
		virtual void Activated([[maybe_unused]] gui::GuiFrame &frame) noexcept
		{
			//Optional to override
		}

		//Called right after a gui frame has been deactivated, with a mutable reference to the frame that was deactivated
		virtual void Deactivated([[maybe_unused]] gui::GuiFrame &frame) noexcept
		{
			//Optional to override
		}


		//Called right after a gui frame has been focused, with a mutable reference to the frame that was focused
		virtual void Focused([[maybe_unused]] gui::GuiFrame &frame) noexcept
		{
			//Optional to override
		}

		//Called right after a gui frame has been defocused, with a mutable reference to the frame that was defocused
		virtual void Defocused([[maybe_unused]] gui::GuiFrame &frame) noexcept
		{
			//Optional to override
		}
	};
} //ion::events::listeners

#endif