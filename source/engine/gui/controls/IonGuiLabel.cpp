/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiLabel.cpp
-------------------------------------------
*/

#include "IonGuiLabel.h"

namespace ion::gui::controls
{

namespace gui_label::detail
{
} //gui_label::detail


//Private

void GuiLabel::DefaultSetup() noexcept
{
	Focusable(false);
}


//Public

GuiLabel::GuiLabel(std::string name, std::optional<std::string> caption, gui_control::ControlSkin skin) :
	GuiControl{std::move(name), std::move(caption), {}, std::move(skin)}
{
	DefaultSetup();
}

GuiLabel::GuiLabel(std::string name, std::optional<std::string> caption, gui_control::ControlSkin skin, const Vector2 &size) :
	GuiControl{std::move(name), std::move(caption), {}, std::move(skin), size}
{
	DefaultSetup();
}

GuiLabel::GuiLabel(std::string name, std::optional<std::string> caption, gui_control::ControlSkin skin, gui_control::Areas areas) :
	GuiControl{std::move(name), std::move(caption), {}, std::move(skin), std::move(areas)}
{
	DefaultSetup();
}

} //ion::gui::controls