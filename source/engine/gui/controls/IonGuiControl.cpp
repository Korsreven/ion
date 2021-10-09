/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiControl.cpp
-------------------------------------------
*/

#include "IonGuiControl.h"

#include "gui/IonGuiPanelContainer.h"

namespace ion::gui::controls
{

namespace gui_control::detail
{

} //gui_control::detail

//Protected

/*
	Events
*/

void GuiControl::Enabled() noexcept
{
	if (auto owner = Owner(); owner)
		owner->Enabled(*this, true);
}

void GuiControl::Disabled() noexcept
{
	if (focused_)
	{
		focused_ = false;
		Defocused();
	}

	if (auto owner = Owner(); owner)
		owner->Enabled(*this, false);
}


void GuiControl::Focused() noexcept
{
	if (auto owner = Owner(); owner)
		owner->Focused(*this, true);
}

void GuiControl::Defocused() noexcept
{
	if (auto owner = Owner(); owner)
		owner->Focused(*this, false);
}


//Public

GuiControl::GuiControl(std::string name) :
	GuiComponent{std::move(name)}
{
	//Empty
}


/*
	Observers
*/

GuiPanelContainer* GuiControl::Owner() const noexcept
{
	return static_cast<GuiPanelContainer*>(owner_);
}


/*
	Tabulating
*/

void GuiControl::TabOrder(int order) noexcept
{
	if (auto owner = Owner(); owner)
		owner->TabOrder(*this, order);
}

std::optional<int> GuiControl::TabOrder() const noexcept
{
	if (auto owner = Owner(); owner)
		return owner->TabOrder(*this);
	else
		return std::nullopt;
}

} //ion::gui::controls