/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiButton.cpp
-------------------------------------------
*/

#include "IonGuiButton.h"

namespace ion::gui::controls
{

namespace gui_button::detail
{
} //gui_control::detail


//Public

GuiButton::GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	gui_control::ControlSkin skin) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin)}
{
	//Empty
}

GuiButton::GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	gui_control::ControlSkin skin, const Vector2 &size) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size}
{
	//Empty
}

GuiButton::GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	gui_control::ControlSkin skin, gui_control::Areas areas) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(areas)}
{
	//Empty
}

} //ion::gui::controls