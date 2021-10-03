/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiPanelContainer.cpp
-------------------------------------------
*/

#include "IonGuiPanelContainer.h"

#include "IonGuiPanel.h"
#include "controls/IonGuiControl.h"

namespace ion::gui
{

namespace gui_panel_container::detail
{

} //gui_panel_container::detail


/*
	Controls
	Retrieving
*/

NonOwningPtr<controls::GuiControl> GuiPanelContainer::GetControl(std::string_view name) noexcept
{
	return static_pointer_cast<controls::GuiControl>(GetComponent(name));
}

NonOwningPtr<const controls::GuiControl> GuiPanelContainer::GetControl(std::string_view name) const noexcept
{
	return static_pointer_cast<const controls::GuiControl>(GetComponent(name));
}


/*
	Controls
	Removing
*/

void GuiPanelContainer::ClearControls() noexcept
{
	return ClearComponents();
}

bool GuiPanelContainer::RemoveControl(controls::GuiControl &control) noexcept
{
	return RemoveComponent(control);
}

bool GuiPanelContainer::RemoveControl(std::string_view name) noexcept
{
	return RemoveComponent(name);
}


/*
	Panels
	Creating
*/

NonOwningPtr<GuiPanel> GuiPanelContainer::CreatePanel(std::string name)
{
	return CreateComponent<GuiPanel>(std::move(name));
}

NonOwningPtr<GuiPanel> GuiPanelContainer::CreatePanel(GuiPanel &&panel)
{
	return CreateComponent<GuiPanel>(std::move(panel));
}


/*
	Panels
	Retrieving
*/

NonOwningPtr<GuiPanel> GuiPanelContainer::GetPanel(std::string_view name) noexcept
{
	return static_pointer_cast<GuiPanel>(GetComponent(name));
}

NonOwningPtr<const GuiPanel> GuiPanelContainer::GetPanel(std::string_view name) const noexcept
{
	return static_pointer_cast<const GuiPanel>(GetComponent(name));
}


/*
	Panels
	Removing
*/

void GuiPanelContainer::ClearPanels() noexcept
{
	return ClearComponents();
}

bool GuiPanelContainer::RemovePanel(GuiPanel &panel) noexcept
{
	return RemoveComponent(panel);
}

bool GuiPanelContainer::RemovePanel(std::string_view name) noexcept
{
	return RemoveComponent(name);
}

} //ion::gui