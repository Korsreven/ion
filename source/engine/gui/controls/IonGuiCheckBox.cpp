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

#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "gui/IonGuiController.h"
#include "gui/IonGuiFrame.h"
#include "gui/IonGuiPanel.h"

namespace ion::gui::controls
{

using namespace gui_check_box;

namespace gui_check_box::detail
{

void resize_skin(CheckBoxSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;
	auto delta_position = delta_size * 0.5_r;
	auto [delta_width, delta_height] = delta_size.XY();

	if (skin.ExtraParts)
	{
		auto &center = skin.ExtraParts.CheckMark ?
			skin.ExtraParts.CheckMark->Position() :
			vector3::Zero;
		
		gui_control::detail::resize_part(skin.ExtraParts.CheckMark, delta_size, vector2::Zero, center); //Check mark
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
	Changed();
	GuiControl::Clicked(); //Use base functionality
}

void GuiCheckBox::StateChanged() noexcept
{
	if (checked_)
		UpdateState();

	GuiControl::StateChanged(); //Use base functionality
}

void GuiCheckBox::Resized(const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	detail::resize_skin(skin_, from_size, to_size);
	GuiControl::Resized(from_size, to_size); //Use base functionality
}


void GuiCheckBox::Checked() noexcept
{
	UpdateState();

	//User callback
	if (on_check_)
		(*on_check_)(*this);
}

void GuiCheckBox::Unchecked() noexcept
{
	UpdateState();

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

void GuiCheckBox::UpdateState() noexcept
{
	if (visible_)
		SetSkinState(GuiControl::state_, skin_);
}


/*
	Skins
*/

void GuiCheckBox::AttachSkin()
{
	GuiControl::AttachSkin(); //Use base functionality

	if (skin_.ExtraParts)
	{
		if (auto node = skin_.ExtraParts->ParentNode(); node)
			node->DetachObject(*skin_.ExtraParts.ModelObject);
		
		if (node_) //Create node for all extra parts
			node_->CreateChildNode(node_->Visible())->AttachObject(*skin_.ExtraParts.ModelObject);
	}

	UpdateState();
}

void GuiCheckBox::DetachSkin() noexcept
{
	if (skin_.ExtraParts)
	{
		if (auto node = skin_.ExtraParts->ParentNode(); node_ && node)
			node_->RemoveChildNode(*node); //Remove extra parts node
	}

	GuiControl::DetachSkin(); //Use base functionality
}

void GuiCheckBox::RemoveSkin() noexcept
{
	DetachSkin();

	if (skin_.ExtraParts)
		skin_.ExtraParts->Owner()->RemoveModel(*skin_.ExtraParts.ModelObject); //Remove all extra parts

	GuiControl::RemoveSkin(); //Use base functionality
}


//Public

GuiCheckBox::GuiCheckBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	CheckBoxSkin skin) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin)}
{
	DefaultSetup();
}

GuiCheckBox::GuiCheckBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	CheckBoxSkin skin, const Vector2 &size) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size}
{
	DefaultSetup();
}

GuiCheckBox::GuiCheckBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	CheckBoxSkin skin, gui_control::Areas areas) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(areas)}
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