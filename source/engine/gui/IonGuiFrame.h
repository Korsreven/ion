/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiFrame.h
-------------------------------------------
*/

#ifndef ION_GUI_FRAME_H
#define ION_GUI_FRAME_H

#include <string>
#include <string_view>

#include "IonGuiPanel.h"
#include "IonGuiPanelContainer.h"
#include "controls/IonGuiControl.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::gui
{
	namespace gui_frame::detail
	{
	} //gui_frame::detail


	class GuiFrame : public GuiPanelContainer
	{
		private:



		public:

			//Construct a frame with the given name
			GuiFrame(std::string name);


			/*
				Modifiers
			*/




			/*
				Observers
			*/


	};
} //ion::gui

#endif