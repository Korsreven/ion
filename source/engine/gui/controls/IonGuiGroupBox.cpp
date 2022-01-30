/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiGroupBox.cpp
-------------------------------------------
*/

#include "IonGuiGroupBox.h"

#include "graphics/scene/graph/IonSceneNode.h"
#include "gui/IonGuiPanelContainer.h"

namespace ion::gui::controls
{

using namespace gui_group_box;

namespace gui_group_box::detail
{
} //gui_group_box::detail


//Private

void GuiGroupBox::DefaultSetup() noexcept
{
	Focusable(false);
	CaptionLayout(gui_control::ControlCaptionLayout::OutsideTopCenter);
}


//Public

GuiGroupBox::GuiGroupBox(std::string name, std::optional<std::string> caption,
	OwningPtr<GroupBoxSkin> skin, gui_control::BoundingBoxes hit_boxes) :
	GuiControl{std::move(name), std::move(caption), {}, std::move(skin), std::move(hit_boxes)}
{
	DefaultSetup();
}

GuiGroupBox::GuiGroupBox(std::string name, std::optional<std::string> caption,
	OwningPtr<GroupBoxSkin> skin, const Vector2 &size, gui_control::BoundingBoxes hit_boxes) :
	GuiControl{std::move(name), std::move(caption), {}, std::move(skin), size, std::move(hit_boxes)}
{
	DefaultSetup();
}


GuiGroupBox::~GuiGroupBox() noexcept
{
	ClearControls();
}


/*
	Controls
	Adding
*/

bool GuiGroupBox::AddControl(NonOwningPtr<controls::GuiControl> control)
{
	if (control && control->Node() &&
		control.get() != this && //Cannot add itself
		control->Parent() != this && //Cannot add same control twice
		control->Owner() == Owner() && //Control and group box has same owner
		control->Parent() == Owner() && //Parent of the control is the owner of the group box
		control->Node()->ParentNode() == Owner()->Node().get())
			//Parent node of the control is the node of the owner of the group box
	{
		control->Parent(*this); //Adopt
		controls_.push_back(control);
		return true;
	}
	else
		return false;
}

bool GuiGroupBox::AddControl(std::string_view name)
{
	if (auto control = GetControl(name); control)
		return AddControl(control);
	else
		return false;
}


/*
	Controls
	Retrieving
*/

NonOwningPtr<controls::GuiControl> GuiGroupBox::GetControl(std::string_view name) noexcept
{
	return detail::get_control_by_name(name, controls_);
}

NonOwningPtr<const controls::GuiControl> GuiGroupBox::GetControl(std::string_view name) const noexcept
{
	return detail::get_control_by_name(name, controls_);
}


/*
	Controls
	Removing
*/

void GuiGroupBox::ClearControls() noexcept
{
	for (auto &control : controls_)
	{
		if (control)
			control->Parent(*control->Owner()); //Orphan
	}

	controls_.clear();
	controls_.shrink_to_fit();
}

bool GuiGroupBox::RemoveControl(controls::GuiControl &control) noexcept
{
	auto iter =
		std::find_if(std::begin(controls_), std::end(controls_),
			[&](auto &x) noexcept
			{
				return x.get() == &control;
			});

	//Control found
	if (iter != std::end(controls_))
	{
		control.Parent(*control.Owner()); //Orphan
		controls_.erase(iter);
		return true;
	}
	else
		return false;
}

bool GuiGroupBox::RemoveControl(std::string_view name) noexcept
{
	if (auto control = GetControl(name); control)
		return RemoveControl(*control);
	else
		return false;
}

} //ion::gui::controls