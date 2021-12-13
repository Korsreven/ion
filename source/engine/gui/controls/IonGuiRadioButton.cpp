/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiRadioButton.cpp
-------------------------------------------
*/

#include "IonGuiRadioButton.h"

#include "gui/IonGuiPanelContainer.h"

namespace ion::gui::controls
{

using namespace gui_radio_button;

namespace gui_radio_button::detail
{
} //gui_radio_button::detail


//Protected

/*
	Events
*/

void GuiRadioButton::Checked() noexcept
{
	UnselectEqualTag(tag_);
	GuiCheckBox::Checked(); //Use base functionality
}

void GuiRadioButton::Unchecked() noexcept
{
	checked_ = true; //Not allowed to unselect a radio button
	//Do not use base functionality
}


/*
	Tags
*/

void GuiRadioButton::Unselect() noexcept
{
	checked_ = false;
	GuiCheckBox::Unchecked(); //Use base functionality
}

void GuiRadioButton::UnselectEqualTag(int tag) noexcept
{
	if (auto owner = Owner(); owner)
	{
		//Unselect radio button with equal tag
		for (auto &control : owner->Controls())
		{
			if (auto radio_button = dynamic_cast<GuiRadioButton*>(&control);
				radio_button && this != radio_button &&
				radio_button->Tag() == tag && radio_button->IsSelected())
			{
				radio_button->Unselect();
				break;
			}
		}
	}
}

bool GuiRadioButton::UniqueTag(int tag) const noexcept
{
	if (auto owner = Owner(); owner)
	{
		for (auto &control : owner->Controls())
		{
			if (auto radio_button = dynamic_cast<GuiRadioButton*>(&control);
				radio_button && this != radio_button &&
				radio_button->Tag() == tag)

				return false; //Not unique
		}
	}

	return true;
}


//Public

GuiRadioButton::GuiRadioButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<RadioButtonSkin> skin) :
	GuiCheckBox{std::move(name), std::move(caption), std::move(tooltip), std::move(skin)}
{
	//Empty
}

GuiRadioButton::GuiRadioButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<RadioButtonSkin> skin, const Vector2 &size) :
	GuiCheckBox{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size}
{
	//Empty
}

GuiRadioButton::GuiRadioButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<RadioButtonSkin> skin, gui_control::Areas areas) :
	GuiCheckBox{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(areas)}
{
	//Empty
}


/*
	Modifiers
*/

void GuiRadioButton::Select() noexcept
{
	Check();
}

void GuiRadioButton::Tag(int tag) noexcept
{
	if (tag_ != tag)
	{
		if (IsSelected() && !UniqueTag(tag))
			Unselect();

		tag_ = tag;
	}
}

} //ion::gui::controls