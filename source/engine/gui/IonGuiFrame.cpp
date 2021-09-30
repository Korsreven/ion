/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiFrame.cpp
-------------------------------------------
*/

#include "IonGuiFrame.h"

#include "IonGuiController.h"
#include "graphics/scene/graph/IonSceneNode.h"

namespace ion::gui
{

namespace gui_frame::detail
{

} //gui_frame::detail


GuiFrame::GuiFrame(std::string name) :
	GuiContainer{std::move(name)}
{
	//Empty
}

GuiFrame::GuiFrame(GuiController &owner, std::string name) :
	GuiContainer{std::move(name)}
{
	if (owner.Node())
		node_ = owner.Node()->CreateChildNode();
}


/*
	Containers
	Retrieving
*/

NonOwningPtr<GuiContainer> GuiFrame::GetContainer(std::string_view name) noexcept
{
	return static_pointer_cast<GuiContainer>(Get(name));
}

NonOwningPtr<const GuiContainer> GuiFrame::GetContainer(std::string_view name) const noexcept
{
	return static_pointer_cast<const GuiContainer>(Get(name));
}


/*
	Containers
	Removing
*/

void GuiFrame::ClearContainers() noexcept
{
	Clear();
}

bool GuiFrame::RemoveContainer(GuiContainer &container) noexcept
{
	return Remove(container);
}

bool GuiFrame::RemoveContainer(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::gui