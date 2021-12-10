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
} //gui_control::detail


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
			if (auto panel = parent_frame->GetPanel(name); panel)
				panel->Show();
			break;

			case ButtonActionType::HidePanel:
			if (auto panel = parent_frame->GetPanel(name); panel)
				panel->Hide();
			break;

			case ButtonActionType::EnablePanel:
			if (auto panel = parent_frame->GetPanel(name); panel)
				panel->Enable();
			break;

			case ButtonActionType::DisablePanel:
			if (auto panel = parent_frame->GetPanel(name); panel)
				panel->Disable();
			break;


			//Control actions
			case ButtonActionType::ShowControl:
			if (auto control = owner->GetControl(name); control)
				control->Show();
			break;

			case ButtonActionType::HideControl:
			if (auto control = owner->GetControl(name); control)
				control->Hide();
			break;

			case ButtonActionType::EnableControl:
			if (auto control = owner->GetControl(name); control)
				control->Enable();
			break;

			case ButtonActionType::DisableControl:
			if (auto control = owner->GetControl(name); control)
				control->Disable();
			break;

			case ButtonActionType::FocusControl:
			if (auto control = owner->GetControl(name); control)
				control->Focus();
			break;

			case ButtonActionType::DefocusControl:
			if (auto control = owner->GetControl(name); control)
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


//Public

GuiButton::GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	gui_control::ControlSkin skin) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin)}
{
	//Empty
}

GuiButton::GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	gui_control::ControlSkin skin, const Vector2 &size) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size}
{
	//Empty
}

GuiButton::GuiButton(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	gui_control::ControlSkin skin, gui_control::Areas areas) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(areas)}
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