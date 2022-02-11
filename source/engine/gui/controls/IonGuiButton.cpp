/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiButton.cpp
-------------------------------------------
*/

#include "IonGuiButton.h"

#include "gui/IonGuiController.h"
#include "gui/IonGuiFrame.h"
#include "gui/IonGuiPanel.h"

namespace ion::gui::controls
{

using namespace gui_button;

namespace gui_button::detail
{
} //gui_button::detail


//Private

void GuiButton::ExecuteActions() noexcept
{
	auto owner = Owner();
	if (!owner)
		return;

	auto parent_frame = owner->ParentFrame();
	if (!parent_frame)
		return;

	auto controller = parent_frame->Owner();
	if (!controller)
		return;


	//Execute actions
	for (auto &[type, name] : actions_)
	{
		auto active_frame = controller->FocusedFrame();
		if (!active_frame)
			active_frame = parent_frame;


		switch (type)
		{
			//Controller actions
			case ButtonActionType::ShowGui:
			controller->Show();
			break;

			case ButtonActionType::HideGui:
			controller->Hide();
			break;

			case ButtonActionType::EnableGui:
			controller->Enable();
			break;

			case ButtonActionType::DisableGui:
			controller->Disable();
			break;


			//Frame actions
			case ButtonActionType::ShowFrame:
			if (auto frame = controller->GetFrame(name); frame)
				frame->Show();
			break;

			case ButtonActionType::ShowFrameModal:
			if (auto frame = controller->GetFrame(name); frame)
				frame->Show(gui_frame::FrameMode::Modal);
			break;

			case ButtonActionType::HideFrame:
			if (auto frame = controller->GetFrame(name); frame)
				frame->Hide();
			break;

			case ButtonActionType::EnableFrame:
			if (auto frame = controller->GetFrame(name); frame)
				frame->Enable();
			break;

			case ButtonActionType::DisableFrame:
			if (auto frame = controller->GetFrame(name); frame)
				frame->Disable();
			break;

			case ButtonActionType::FocusFrame:
			if (auto frame = controller->GetFrame(name); frame)
				frame->Focus();
			break;

			case ButtonActionType::DefocusFrame:
			if (auto frame = controller->GetFrame(name); frame)
				frame->Defocus();
			break;


			//Panel actions
			case ButtonActionType::ShowPanel:
			if (auto panel = active_frame->SearchPanel(name); panel)
				panel->Show();
			break;

			case ButtonActionType::HidePanel:
			if (auto panel = active_frame->SearchPanel(name); panel)
				panel->Hide();
			break;

			case ButtonActionType::EnablePanel:
			if (auto panel = active_frame->SearchPanel(name); panel)
				panel->Enable();
			break;

			case ButtonActionType::DisablePanel:
			if (auto panel = active_frame->SearchPanel(name); panel)
				panel->Disable();
			break;


			//Control actions
			case ButtonActionType::ShowControl:
			if (auto control = active_frame->SearchControl(name); control)
				control->Show();
			break;

			case ButtonActionType::HideControl:
			if (auto control = active_frame->SearchControl(name); control)
				control->Hide();
			break;

			case ButtonActionType::EnableControl:
			if (auto control = active_frame->SearchControl(name); control)
				control->Enable();
			break;

			case ButtonActionType::DisableControl:
			if (auto control = active_frame->SearchControl(name); control)
				control->Disable();
			break;

			case ButtonActionType::FocusControl:
			if (auto control = active_frame->SearchControl(name); control)
				control->Focus();
			break;

			case ButtonActionType::DefocusControl:
			if (auto control = active_frame->SearchControl(name); control)
				control->Defocus();
			break;
		}
	}
}


//Protected

/*
	Events
*/

void GuiButton::Clicked() noexcept
{
	if (!std::empty(actions_))
		ExecuteActions();

	GuiControl::Clicked(); //Use base functionality
}


/*
	Skins
*/

OwningPtr<gui_control::ControlSkin> GuiButton::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<ButtonSkin*>(skin.get()))
	{
		auto button_skin = make_owning<ButtonSkin>();
		button_skin->Assign(*skin);
		return button_skin;
	}
	else
		return skin;
}


//Public

GuiButton::GuiButton(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes) :
	GuiControl{std::move(name), size, std::move(caption), std::move(tooltip), std::move(hit_boxes)}
{
	//Empty
}

GuiButton::GuiButton(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, gui_control::BoundingBoxes hit_boxes) :
	GuiControl{std::move(name), skin, size, std::move(caption), std::move(tooltip), std::move(hit_boxes)}
{
	//Empty
}


/*
	Modifiers
*/

void GuiButton::AddAction(gui_button::ButtonAction action)
{
	actions_.push_back(action);
}

void GuiButton::AddActions(gui_button::ButtonActions actions)
{
	std::move(std::begin(actions), std::end(actions), std::back_inserter(actions_));
}


void GuiButton::ClearActions() noexcept
{
	actions_.clear();
	actions_.shrink_to_fit();
}

} //ion::gui::controls