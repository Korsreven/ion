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

namespace ion::gui::controls
{

using namespace gui_check_box;

namespace gui_check_box::detail
{

void resize_skin(CheckBoxSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;

	if (skin.CheckMark)
	{
		auto &center = skin.CheckMark->Position();
		gui_control::detail::resize_part(skin.CheckMark, delta_size, vector2::Zero, center);
	}
}

} //gui_check_box::detail


//Private

void GuiCheckBox::DefaultSetup() noexcept
{
	CaptionLayout(gui_control::ControlCaptionLayout::OutsideRightCenter);
}


//Protected

/*
	Events
*/

void GuiCheckBox::Clicked() noexcept
{
	Checked(!checked_);
	GuiControl::Clicked(); //Use base functionality
}

void GuiCheckBox::Resized(Vector2 from_size, Vector2 to_size) noexcept
{
	if (skin_)
		detail::resize_skin(static_cast<CheckBoxSkin&>(*skin_), from_size, to_size);

	GuiControl::Resized(from_size, to_size); //Use base functionality
}


void GuiCheckBox::Checked() noexcept
{
	Changed();
	SetState(state_);

	//User callback
	if (on_check_)
		(*on_check_)(*this);
}

void GuiCheckBox::Unchecked() noexcept
{
	Changed();
	SetState(state_);

	//User callback
	if (on_uncheck_)
		(*on_uncheck_)(*this);
}


/*
	States
*/

void GuiCheckBox::SetSkinState(gui_control::ControlState state, CheckBoxSkin &skin) noexcept
{
	if (skin.CheckMark)
	{
		if (checked_)
			SetPartState(state, skin.CheckMark);

		skin.CheckMark->Visible(checked_);
	}
}

void GuiCheckBox::SetState(gui_control::ControlState state) noexcept
{
	GuiControl::SetState(state); //Use base functionality

	if (visible_ && skin_)
		SetSkinState(state, static_cast<CheckBoxSkin&>(*skin_));
}


/*
	Skins
*/

OwningPtr<gui_control::ControlSkin> GuiCheckBox::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<CheckBoxSkin*>(skin.get()))
	{
		auto check_box_skin = make_owning<CheckBoxSkin>();
		check_box_skin->Assign(*skin);
		return check_box_skin;
	}
	else
		return skin;
}


//Public

GuiCheckBox::GuiCheckBox(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes) :
	GuiControl{std::move(name), size, std::move(caption), std::move(tooltip), std::move(hit_boxes)}
{
	DefaultSetup();
}

GuiCheckBox::GuiCheckBox(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes) :
	GuiControl{std::move(name), skin, size, std::move(caption), std::move(tooltip), std::move(hit_boxes)}
{
	DefaultSetup();
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