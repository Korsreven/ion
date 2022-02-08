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
#include "memory/IonOwningPtr.h"

namespace ion::gui::controls
{
	namespace gui_label
	{
		struct LabelSkin : gui_control::ControlSkin
		{
			//Empty
		};


		namespace detail
		{
		} //detail
	} //gui_label


	class GuiLabel : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			/*
				Skins
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

		public:

			//Construct a label with the given name, caption, skin and hit boxes
			GuiLabel(std::string name, std::optional<std::string> caption,
				OwningPtr<gui_label::LabelSkin> skin, gui_control::BoundingBoxes hit_boxes = {});

			//Construct a label with the given name, caption, skin, size and hit boxes
			GuiLabel(std::string name, std::optional<std::string> caption,
				OwningPtr<gui_label::LabelSkin> skin, const Vector2 &size, gui_control::BoundingBoxes hit_boxes = {});


			/*
				Modifiers
			*/




			/*
				Observers
			*/


	};

} //ion::gui::controls

#endif