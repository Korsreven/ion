/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiContainer.h
-------------------------------------------
*/

#ifndef ION_GUI_CONTAINER_H
#define ION_GUI_CONTAINER_H

#include "IonGuiComponent.h"
#include "managed/IonObjectManager.h"

namespace ion::gui
{
	namespace gui_container::detail
	{
	} //gui_container::detail


	class GuiContainer :
		public GuiComponent,
		public managed::ObjectManager<GuiComponent, GuiContainer>
	{
		private:



		public:

			using GuiComponent::GuiComponent;


			/*
				Modifiers
			*/




			/*
				Observers
			*/


	};
} //ion::gui

#endif