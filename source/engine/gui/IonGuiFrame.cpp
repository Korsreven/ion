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

using namespace gui_frame;

namespace gui_frame::detail
{

void get_ordered_controls(GuiPanelContainer &owner, control_pointers &controls)
{
	for (auto &component : owner.OrderedComponents())
	{
		if (auto control = dynamic_cast<controls::GuiControl*>(&component); control)
			controls.push_back(control);
		else if (auto panel = dynamic_cast<GuiPanel*>(&component); panel)
			get_ordered_controls(*panel, controls); //Recursive
	}
}

control_pointers get_ordered_controls(GuiPanelContainer &owner)
{
	control_pointers controls;
	get_ordered_controls(owner, controls);
	return controls;
}


std::optional<control_pointers::iterator> get_current_control_iterator(control_pointers &controls,
	controls::GuiControl *focused_control) noexcept
{
	if (std::empty(controls))
		return std::nullopt;
	else if (!focused_control)
		return std::end(controls);
	else
	{
		if (auto iter = std::find(std::begin(controls), std::end(controls), focused_control);
			iter != std::end(controls))
			return iter;
		else
			return std::nullopt;
	}
}

} //gui_frame::detail


//Private

bool GuiFrame::TabForward(GuiFrame &from_frame) noexcept
{
	auto controls = detail::get_ordered_controls(*this);

	if (auto current_iter = detail::get_current_control_iterator(controls, focused_control_); current_iter)
	{
		for (auto iter = detail::get_next_control_iterator(*current_iter, controls);
			iter != *current_iter; iter = detail::get_next_control_iterator(iter, controls))
		{
			//Tab to next frame
			if (iter == std::end(controls))
			{
				Owner()->TabForward();

				auto found = false;
				if (auto focused_frame = Owner()->FocusedFrame();
					focused_frame && focused_frame != &from_frame)

					found = focused_frame->TabForward(from_frame); //Recursive

				if (found || this != &from_frame)
					return found; //Unwind

				continue;
			}

			(*iter)->Focus();

			if ((*iter)->IsFocused())
				return true;
		}
	}

	return false;
}

bool GuiFrame::TabBackward(GuiFrame &from_frame) noexcept
{
	auto controls = detail::get_ordered_controls(*this);

	if (auto current_iter = detail::get_current_control_iterator(controls, focused_control_); current_iter)
	{
		for (auto iter = detail::get_previous_control_iterator(*current_iter, controls);
			iter != *current_iter; iter = detail::get_previous_control_iterator(iter, controls))
		{
			//Tab to previous frame
			if (iter == std::end(controls) - 1)
			{
				Owner()->TabBackward();

				auto found = false;
				if (auto focused_frame = Owner()->FocusedFrame();
					focused_frame && focused_frame != &from_frame)
					
					found = focused_frame->TabBackward(from_frame); //Recursive

				if (found || this != &from_frame)
					return found; //Unwind
			}

			(*iter)->Focus();

			if ((*iter)->IsFocused())
				return true;
		}
	}

	return false;
}


//Protected

/*
	Events
*/

void GuiFrame::Created(controls::GuiControl &control) noexcept
{
	GuiPanelContainer::Created(control); //Use base functionality

	//Added control could be adopted
	control.Reset();
}

void GuiFrame::Removed(controls::GuiControl &control) noexcept
{
	//Execute opposite events based on its current state
	control.Reset();

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
	if (control.IsFocused() && focused_control_ != &control)
	{
		if (focused_control_)
			focused_control_->Defocus();

		Focus();
		focused_control_ = &control;
	}
}

void GuiFrame::Defocused(controls::GuiControl &control) noexcept
{
	if (!control.IsFocused() && focused_control_ == &control)
		focused_control_ = nullptr;
}


void GuiFrame::Pressed(controls::GuiControl &control) noexcept
{
	if (control.IsPressed() && pressed_control_ != &control)
	{
		if (pressed_control_)
			pressed_control_->Release();

		pressed_control_ = &control;
	}
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
	if (control.IsHovered() && hovered_control_ != &control)
	{
		if (hovered_control_)
			hovered_control_->Exit();

		hovered_control_ = &control;
	}
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
	NotifyFrameDisabled();
	GuiComponent::Disabled(); //Use base functionality
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
	if (pressed_control_)
		pressed_control_->Release();

	if (focused_control_)
		focused_control_->Defocus();

	if (hovered_control_)
		hovered_control_->Exit();

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
	Modifiers
*/

void GuiFrame::Activate() noexcept
{
	if (!activated_)
	{
		activated_ = true;
		Activated();
	}
}

void GuiFrame::Deactivate() noexcept
{
	if (activated_)
	{
		activated_ = false;
		Deactivated();
	}
}


void GuiFrame::Focus() noexcept
{
	if (!focused_ && IsFocusable())
	{
		focused_ = true;
		Focused();
	}
}

void GuiFrame::Defocus() noexcept
{
	if (focused_)
	{
		focused_ = false;
		Defocused();
	}
}


/*
	Observers
*/

bool GuiFrame::IsFocusable() const noexcept
{
	if (enabled_ && activated_)
	{
		if (auto owner = Owner(); owner)
			return owner->IsEnabled() && owner->IsOnTop(*this);
		else
			return true;
	}
	else
		return false;
}

bool GuiFrame::IsOnTop() const noexcept
{
	if (activated_)
	{
		if (auto owner = Owner(); owner)
			return owner->IsOnTop(*this);
		else
			return true;
	}
	else
		return false;
}


GuiController* GuiFrame::Owner() const noexcept
{
	return static_cast<GuiController*>(owner_);
}


/*
	Tabulating
*/

void GuiFrame::TabForward() noexcept
{
	TabForward(*this);
}

void GuiFrame::TabBackward() noexcept
{
	TabBackward(*this);
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