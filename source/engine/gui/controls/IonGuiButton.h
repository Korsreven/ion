/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiButton.h
-------------------------------------------
*/

#ifndef ION_GUI_BUTTON_H
#define ION_GUI_BUTTON_H

#include <optional>
#include <string>

#include "IonGuiControl.h"

namespace ion::gui::controls
{
	namespace gui_button::detail
	{
	} //gui_button::detail

	class GuiButton : public GuiControl
	{
		protected:

			//Empty

		public:

			//Construct a button with the given name, caption, tooltip and skin
			GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				gui_control::ControlSkin skin);

			//Construct a button with the given name, caption, tooltip, skin and size
			GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				gui_control::ControlSkin skin, const Vector2 &size);

			//Construct a button with the given name, caption, tooltip, skin and hit areas
			GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
				gui_control::ControlSkin skin, gui_control::Areas areas);


			/*
				Modifiers
			*/




			/*
				Observers
			*/


	};

} //ion::gui::controls

#endif