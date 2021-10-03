/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiContainer.cpp
-------------------------------------------
*/

#include "IonGuiContainer.h"

namespace ion::gui
{

namespace gui_container::detail
{

} //gui_container::detail


/*
	Components
	Retrieving
*/

NonOwningPtr<GuiComponent> GuiContainer::GetComponent(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const GuiComponent> GuiContainer::GetComponent(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Components
	Removing
*/

void GuiContainer::ClearComponents() noexcept
{
	Clear();
}

bool GuiContainer::RemoveComponent(GuiComponent &component) noexcept
{
	return Remove(component);
}

bool GuiContainer::RemoveComponent(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::gui