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

using namespace gui_control;

namespace gui_control::detail
{

} //gui_control::detail

//Protected

/*
	Events
*/

void GuiControl::Enabled() noexcept
{
	SetState(ControlState::Enabled);

	if (auto owner = Owner(); owner)
		owner->Enabled(*this, true);

	GuiComponent::Enabled(); //Use base functionality
}

void GuiControl::Disabled() noexcept
{
	if (/*hover*/false)
		Exited();

	if (focused_)
	{
		focused_ = false;
		Defocused();
	}

	SetState(ControlState::Disabled);

	if (auto owner = Owner(); owner)
		owner->Enabled(*this, false);

	GuiComponent::Disabled(); //Use base functionality
}


void GuiControl::Focused() noexcept
{
	if (state_ == ControlState::Enabled)
		SetState(ControlState::Focused);

	if (auto owner = Owner(); owner)
		owner->Focused(*this, true);

	//User callback
	if (on_focus_)
		(*on_focus_)(*this);
}

void GuiControl::Defocused() noexcept
{
	if (state_ == ControlState::Focused)
		SetState(ControlState::Enabled);

	if (auto owner = Owner(); owner)
		owner->Focused(*this, false);

	//User callback
	if (on_defocus_)
		(*on_defocus_)(*this);
}

void GuiControl::Pressed() noexcept
{
	if (!focused_ && focusable_)
		Focused(true);

	SetState(ControlState::Pressed);

	//User callback
	if (on_press_)
		(*on_press_)(*this);
}

void GuiControl::Released() noexcept
{
	if (/*hover*/false)
		SetState(ControlState::Hover);
	else if (focused_)
		SetState(ControlState::Focused);
	else
		SetState(ControlState::Enabled);

	//User callback
	if (on_release_)
		(*on_release_)(*this);
}

void GuiControl::Entered() noexcept
{
	if (state_ != ControlState::Pressed)
		SetState(ControlState::Hover);

	//User callback
	if (on_enter_)
		(*on_enter_)(*this);
}

void GuiControl::Exited() noexcept
{
	if (state_ != ControlState::Pressed)
	{
		if (focused_)
			SetState(ControlState::Focused);
		else
			SetState(ControlState::Enabled);
	}

	//User callback
	if (on_exit_)
		(*on_exit_)(*this);
}

void GuiControl::Changed() noexcept
{
	//User callback
	if (on_change_)
		(*on_change_)(*this);
}


/*
	States
*/

void GuiControl::SetState(ControlState state) noexcept
{
	if (state_ != state)
	{
		state_ = state;
	}
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


/*
	Frame events
*/

void GuiControl::FrameStarted([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}

void GuiControl::FrameEnded([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}


/*
	Key events
*/

void GuiControl::KeyPressed([[maybe_unused]] KeyButton button) noexcept
{
	//Optional to override
}

void GuiControl::KeyReleased([[maybe_unused]] KeyButton button) noexcept
{
	//Optional to override
}

void GuiControl::CharacterPressed([[maybe_unused]] char character) noexcept
{
	//Optional to override
}


/*
	Mouse events
*/

void GuiControl::MousePressed([[maybe_unused]] MouseButton button, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
}

void GuiControl::MouseReleased([[maybe_unused]] MouseButton button, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
}

void GuiControl::MouseMoved([[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
}

void GuiControl::MouseWheelRolled([[maybe_unused]] int delta, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
}

} //ion::gui::controls