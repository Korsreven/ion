/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiCheckBox.cpp
-------------------------------------------
*/

#include "IonGuiCheckBox.h"

#include "graphics/scene/shapes/IonSprite.h"
#include "gui/IonGuiController.h"
#include "gui/IonGuiFrame.h"
#include "gui/IonGuiPanel.h"

namespace ion::gui::controls
{

using namespace gui_check_box;

namespace gui_check_box::detail
{
} //gui_check_box::detail


//Protected

/*
	Events
*/

void GuiCheckBox::Clicked() noexcept
{
	Checked(!checked_);
	Changed();
	GuiControl::Clicked(); //Use base functionality
}


void GuiCheckBox::Checked() noexcept
{
	SetState(CheckBoxState::Checked);

	//User callback
	if (on_check_)
		(*on_check_)(*this);
}

void GuiCheckBox::Unchecked() noexcept
{
	SetState(CheckBoxState::Unchecked);

	//User callback
	if (on_uncheck_)
		(*on_uncheck_)(*this);
}


/*
	States
*/

void GuiCheckBox::SetSkinState(gui_control::ControlState state, CheckBoxSkin &skin) noexcept
{
	if (skin.ExtraParts)
	{
		if (checked_)
			SetPartState(state, skin.ExtraParts.CheckMark); //Check mark

		skin.ExtraParts.CheckMark->Visible(checked_);
	}
}

void GuiCheckBox::SetState(CheckBoxState state) noexcept
{
	if (visible_)
		SetSkinState(GuiControl::state_, skin_);

	if (state_ != state)
	{
		state_ = state;
		StateChanged();
	}
}


//Public

GuiCheckBox::GuiCheckBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	CheckBoxSkin skin) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin)}
{
	//Empty
}

GuiCheckBox::GuiCheckBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	CheckBoxSkin skin, const Vector2 &size) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size}
{
	//Empty
}

GuiCheckBox::GuiCheckBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	CheckBoxSkin skin, gui_control::Areas areas) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(areas)}
{
	//Empty
}


/*
	Modifiers
*/

void GuiCheckBox::Check() noexcept
{
	if (!checked_)
	{
		checked_ = true;
		Checked();
	}
}

void GuiCheckBox::Uncheck() noexcept
{
	if (checked_)
	{
		checked_ = false;
		Unchecked();
	}
}

} //ion::gui::controls