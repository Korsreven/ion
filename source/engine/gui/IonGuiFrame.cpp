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

namespace ion::gui
{

namespace gui_frame::detail
{

} //gui_frame::detail


//Protected

/*
	Events
*/

void GuiFrame::Created(controls::GuiControl &control) noexcept
{
	GuiPanelContainer::Created(control); //Use base functionality

	//In case the created control is actually adopted

	if (control.IsFocused())
	{
		if (focused_control_)
			focused_control_->Defocus();

		focused_control_ = &control;
	}

	if (control.IsPressed())
	{
		if (pressed_control_)
			pressed_control_->Release();

		pressed_control_ = &control;
	}

	if (control.IsHovered())
	{
		if (hovered_control_)
			hovered_control_->Exit();

		hovered_control_ = &control;
	}
}

void GuiFrame::Removed(controls::GuiControl &control) noexcept
{
	if (focused_control_ == &control)
		focused_control_ = nullptr;

	if (pressed_control_ == &control)
		pressed_control_ = nullptr;

	if (hovered_control_ == &control)
		hovered_control_ = nullptr;

	GuiPanelContainer::Removed(control); //Use base functionality
}


bool GuiFrame::Unsubscribable(Listenable<events::listeners::GuiControlListener>&) noexcept
{
	//Cancel all unsubscribe attempts
	return false;
}


void GuiFrame::Enabled([[maybe_unused]] controls::GuiControl &control) noexcept
{
	//Empty
}

void GuiFrame::Disabled([[maybe_unused]] controls::GuiControl &control) noexcept
{
	//Empty
}


void GuiFrame::Focused(controls::GuiControl &control) noexcept
{
	if (control.IsFocused() && !focused_control_)
		focused_control_ = &control;
}

void GuiFrame::Defocused(controls::GuiControl &control) noexcept
{
	if (!control.IsFocused() && focused_control_ == &control)
		focused_control_ = nullptr;
}


void GuiFrame::Pressed(controls::GuiControl &control) noexcept
{
	if (control.IsPressed() && !pressed_control_)
		pressed_control_ = &control;
}

void GuiFrame::Released(controls::GuiControl &control) noexcept
{
	if (!control.IsPressed() && pressed_control_ == &control)
		pressed_control_ = nullptr;
}

void GuiFrame::Clicked([[maybe_unused]] controls::GuiControl &control) noexcept
{
	//Empty
}


void GuiFrame::Entered(controls::GuiControl &control) noexcept
{
	if (control.IsHovered() && !hovered_control_)
		hovered_control_ = &control;
}

void GuiFrame::Exited(controls::GuiControl &control) noexcept
{
	if (!control.IsHovered() && hovered_control_ == &control)
		hovered_control_ = nullptr;
}


void GuiFrame::Enabled() noexcept
{
	NotifyFrameEnabled();
	GuiComponent::Enabled(); //Use base functionality
}

void GuiFrame::Disabled() noexcept
{
	Defocus();
	Deactivate();

	NotifyFrameDisabled();
	GuiComponent::Enabled(); //Use base functionality
}


void GuiFrame::Activated() noexcept
{
	NotifyFrameActivated();
}

void GuiFrame::Deactivated() noexcept
{
	Defocus();
	NotifyFrameDeactivated();
}


void GuiFrame::Focused() noexcept
{
	NotifyFrameFocused();
}

void GuiFrame::Defocused() noexcept
{
	NotifyFrameDefocused();
}


/*
	Notifying
*/

void GuiFrame::NotifyFrameEnabled() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Enabled, std::ref(*this));
}

void GuiFrame::NotifyFrameDisabled() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Disabled, std::ref(*this));
}


void GuiFrame::NotifyFrameActivated() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Activated, std::ref(*this));

	//User callback
	if (on_activate_)
		(*on_activate_)(*this);
}

void GuiFrame::NotifyFrameDeactivated() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Deactivated, std::ref(*this));

	//User callback
	if (on_deactivate_)
		(*on_deactivate_)(*this);
}


void GuiFrame::NotifyFrameFocused() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Focused, std::ref(*this));

	//User callback
	if (on_focus_)
		(*on_focus_)(*this);
}

void GuiFrame::NotifyFrameDefocused() noexcept
{
	if (auto owner = Owner(); owner)
		FrameEventsGeneratorBase::NotifyAll(owner->FrameEvents().Listeners(),
			&events::listeners::GuiFrameListener::Defocused, std::ref(*this));

	//User callback
	if (on_defocus_)
		(*on_defocus_)(*this);
}


//Public

GuiFrame::GuiFrame(std::string name) :
	GuiPanelContainer{std::move(name)}
{
	ControlEvents().Subscribe(*this);
}


/*
	Observers
*/

GuiController* GuiFrame::Owner() const noexcept
{
	return static_cast<GuiController*>(owner_);
}


/*
	Frame events
*/

void GuiFrame::FrameStarted(duration time) noexcept
{

}

void GuiFrame::FrameEnded(duration time) noexcept
{

}


/*
	Key events
*/

void GuiFrame::KeyPressed(KeyButton button) noexcept
{

}

void GuiFrame::KeyReleased(KeyButton button) noexcept
{

}

void GuiFrame::CharacterPressed(char character) noexcept
{

}


/*
	Mouse events
*/

void GuiFrame::MousePressed(MouseButton button, Vector2 position) noexcept
{

}

void GuiFrame::MouseReleased(MouseButton button, Vector2 position) noexcept
{

}

void GuiFrame::MouseMoved(Vector2 position) noexcept
{

}

void GuiFrame::MouseWheelRolled(int delta, Vector2 position) noexcept
{

}

} //ion::gui