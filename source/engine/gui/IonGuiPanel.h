/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiPanel.h
-------------------------------------------
*/

#ifndef ION_GUI_PANEL_H
#define ION_GUI_PANEL_H

#include "IonGuiContainer.h"
#include "controls/IonGuiControl.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObjectManager.h"

namespace ion::gui
{
	namespace gui_panel::detail
	{
	} //gui_panel::detail


	class GuiPanel :
		public GuiContainer,
		public managed::ObjectManager<controls::GuiControl, GuiContainer>
	{
		private:



		public:

			//Default constructor
			GuiPanel() = default;


			/*
				Modifiers
			*/




			/*
				Observers
			*/


	};
} //ion::gui

#endif