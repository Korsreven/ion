/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiLabel.h
-------------------------------------------
*/

#ifndef ION_GUI_LABEL_H
#define ION_GUI_LABEL_H

#include <optional>
#include <string>

#include "IonGuiControl.h"

namespace ion::gui::controls
{
	namespace gui_label::detail
	{
	} //gui_label::detail


	class GuiLabel : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			//Empty

		public:

			//Construct a label with the given name, caption and skin
			GuiLabel(std::string name, std::optional<std::string> caption, gui_control::ControlSkin skin);

			//Construct a label with the given name, caption, skin and size
			GuiLabel(std::string name, std::optional<std::string> caption, gui_control::ControlSkin skin, const Vector2 &size);

			//Construct a label with the given name, caption, skin and hit areas
			GuiLabel(std::string name, std::optional<std::string> caption, gui_control::ControlSkin skin, gui_control::Areas areas);


			/*
				Modifiers
			*/




			/*
				Observers
			*/


	};

} //ion::gui::controls

#endif