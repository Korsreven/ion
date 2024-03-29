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


	///@brief A class representing a GUI label with a caption
	class GuiLabel : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

			///@}

		public:

			///@brief Constructs a label with the given name, size, caption and hit boxes
			GuiLabel(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs a label with the given name, skin, size, caption and hit boxes
			GuiLabel(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {});
	};

} //ion::gui::controls

#endif