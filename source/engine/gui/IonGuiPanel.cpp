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
	Modifiers
*/

void GuiPanel::Show() noexcept
{
	GuiPanelContainer::Show();

	//Show all controls that should be visible
	for (auto &control : Controls())
	{
		if (control.IsVisible())
			control.Show();
	}

	//Show all panels that should be visible
	for (auto &panel : Panels())
	{
		if (panel.IsVisible())
			panel.Show();
	}
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
		return {};
}


/*
	Frame events
*/

void GuiPanel::FrameStarted(duration time) noexcept
{
	for (auto &control : Controls())
		control.FrameStarted(time);

	for (auto &panel : Panels())
		panel.FrameStarted(time);
}

void GuiPanel::FrameEnded(duration time) noexcept
{
	for (auto &control : Controls())
		control.FrameEnded(time);

	for (auto &panel : Panels())
		panel.FrameEnded(time);
}

} //ion::gui