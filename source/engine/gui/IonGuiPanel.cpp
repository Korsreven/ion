/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiPanel.cpp
-------------------------------------------
*/

#include "IonGuiPanel.h"

namespace ion::gui
{

namespace gui_panel::detail
{

} //gui_panel::detail


GuiPanel::GuiPanel(std::string name) :
	GuiPanelContainer{std::move(name)}
{
	//Empty
}


/*
	Tabulating
*/

void GuiPanel::TabOrder(int order) noexcept
{
	if (auto owner = Owner(); owner)
		owner->TabOrder(*this, order);
}

std::optional<int> GuiPanel::TabOrder() const noexcept
{
	if (auto owner = Owner(); owner)
		return owner->TabOrder(*this);
	else
		return std::nullopt;
}

} //ion::gui