/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/skins
File:	IonGuiSkin.h
-------------------------------------------
*/

#ifndef ION_GUI_SKIN_H
#define ION_GUI_SKIN_H

#include "managed/IonManagedObject.h"

namespace ion::gui::skins
{
	//Forward declarations
	class GuiTheme;

	namespace gui_skin::detail
	{
	} //gui_skin::detail


	class GuiSkin final : public managed::ManagedObject<GuiTheme>
	{
		private:



		public:

			using managed::ManagedObject<GuiTheme>::ManagedObject;


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			
	};
} //ion::gui::skins

#endif