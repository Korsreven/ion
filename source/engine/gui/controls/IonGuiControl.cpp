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

#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/utilities/IonMatrix3.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector3.h"
#include "graphics/utilities/IonObb.h"
#include "gui/IonGuiFrame.h"
#include "gui/IonGuiPanelContainer.h"

namespace ion::gui::controls
{

using namespace gui_control;

namespace gui_control::detail
{

void resize_area(Aabb &area, const Vector2 &from_size, const Vector2 &to_size)
{
	area.Transform(Matrix3::Transformation(0.0_r, to_size / from_size, vector2::Zero));
}

void resize_areas(Areas &areas, const Vector2 &from_size, const Vector2 &to_size)
{
	for (auto &area : areas)
		resize_area(area, from_size, to_size);
}

} //gui_control::detail

//Protected

/*
	Events
*/

void GuiControl::Enabled() noexcept
{
	SetState(ControlState::Enabled);
	NotifyControlEnabled();
	GuiComponent::Enabled(); //Use base functionality
}

void GuiControl::Disabled() noexcept
{
	Reset();
	SetState(ControlState::Disabled);
	NotifyControlDisabled();
	GuiComponent::Disabled(); //Use base functionality
}


void GuiControl::Focused() noexcept
{
	if (state_ == ControlState::Enabled)
		SetState(ControlState::Focused);

	NotifyControlFocused();
}

void GuiControl::Defocused() noexcept
{
	if (state_ == ControlState::Focused)
		SetState(ControlState::Enabled);

	NotifyControlDefocused();
}


void GuiControl::Pressed() noexcept
{
	Focus();
	SetState(ControlState::Pressed);
	NotifyControlPressed();
}

void GuiControl::Released() noexcept
{
	if (hovered_)
		SetState(ControlState::Hovered);
	else if (focused_)
		SetState(ControlState::Focused);
	else
		SetState(ControlState::Enabled);

	NotifyControlReleased();
}

void GuiControl::Clicked() noexcept
{
	NotifyControlClicked();
}


void GuiControl::Entered() noexcept
{
	if (state_ != ControlState::Pressed)
		SetState(ControlState::Hovered);

	NotifyControlEntered();
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

	NotifyControlExited();
}


void GuiControl::Changed() noexcept
{
	NotifyControlChanged();
}

void GuiControl::Resized() noexcept
{
	NotifyControlResized();
}


/*
	Notifying
*/

void GuiControl::NotifyControlEnabled() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Enabled, std::ref(*this));
	}
}

void GuiControl::NotifyControlDisabled() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Disabled, std::ref(*this));
	}
}


void GuiControl::NotifyControlFocused() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Focused, std::ref(*this));
	}

	//User callback
	if (on_focus_)
		(*on_focus_)(*this);
}

void GuiControl::NotifyControlDefocused() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Defocused, std::ref(*this));
	}

	//User callback
	if (on_defocus_)
		(*on_defocus_)(*this);
}


void GuiControl::NotifyControlPressed() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Pressed, std::ref(*this));
	}

	//User callback
	if (on_press_)
		(*on_press_)(*this);
}

void GuiControl::NotifyControlReleased() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Released, std::ref(*this));
	}

	//User callback
	if (on_release_)
		(*on_release_)(*this);
}

void GuiControl::NotifyControlClicked() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Clicked, std::ref(*this));
	}

	//User callback
	if (on_click_)
		(*on_click_)(*this);
}


void GuiControl::NotifyControlEntered() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Entered, std::ref(*this));
	}

	//User callback
	if (on_enter_)
		(*on_enter_)(*this);
}

void GuiControl::NotifyControlExited() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Exited, std::ref(*this));
	}

	//User callback
	if (on_exit_)
		(*on_exit_)(*this);
}


void GuiControl::NotifyControlChanged() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Changed, std::ref(*this));
	}

	//User callback
	if (on_change_)
		(*on_change_)(*this);
}

void GuiControl::NotifyControlResized() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Resized, std::ref(*this));
	}

	//User callback
	if (on_resize_)
		(*on_resize_)(*this);
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

GuiControl::GuiControl(std::string name, const Vector2 &size) :

	GuiComponent{std::move(name)},
	clickable_areas_{Aabb::Size(size)}
{
	//Empty
}

GuiControl::GuiControl(std::string name, gui_control::Areas areas) :

	GuiComponent{std::move(name)},
	clickable_areas_{std::move(areas)}
{
	//Empty
}


/*
	Modifiers
*/

void GuiControl::Focus() noexcept
{
	if (!focused_ &&
		enabled_ && focusable_)
	{
		if (auto owner = Owner(); !owner || owner->IsFocusable())
		{
			focused_ = true;
			Focused();
		}
	}
}

void GuiControl::Defocus() noexcept
{
	if (focused_)
	{
		focused_ = false;
		Defocused();
	}
}


void GuiControl::Press() noexcept
{
	if (!pressed_ && enabled_)
	{
		if (auto owner = Owner(); !owner || owner->IsFocusable())
		{
			pressed_ = true;
			Pressed();
		}
	}
}

void GuiControl::Release() noexcept
{
	if (pressed_)
	{
		pressed_ = false;
		Released();
	}
}

void GuiControl::Click() noexcept
{
	if (enabled_)
		Clicked();
}


void GuiControl::Enter() noexcept
{
	if (!hovered_ && enabled_)
	{
		if (auto owner = Owner(); !owner || owner->IsFocusable())
		{
			hovered_ = true;
			Entered();
		}
	}
}

void GuiControl::Exit() noexcept
{
	if (hovered_)
	{
		hovered_ = false;
		Exited();
	}
}


void GuiControl::Reset() noexcept
{
	Release();
	Defocus();
	Exit();
}


void GuiControl::Size(const Vector2 &size) noexcept
{
	if (auto current_size = Size(); current_size != size)
	{
		if (std::empty(clickable_areas_))
		{
			clickable_areas_.push_back(Aabb::Size(size));
			Resized();
		}
		else if (std::size(clickable_areas_) == 1)
		{
			clickable_areas_.back() = Aabb::Size(size);
			Resized();
		}

		//Multiple areas
		else if (current_size.X() != 0.0_r && current_size.Y() != 0.0_r)
		{
			detail::resize_areas(clickable_areas_, current_size, size);
			Resized();
		}
	}
}


/*
	Observers
*/

Vector2 GuiControl::Size() const noexcept
{
	if (std::empty(clickable_areas_))
		return vector2::Zero;
	else if (std::size(clickable_areas_) == 1)
		return clickable_areas_.back().ToSize();

	else //Multiple areas
	{
		auto merged_area = aabb::Zero;

		for (auto &area : clickable_areas_)
			merged_area.Merge(area);

		return merged_area.ToSize();
	}
}


GuiPanelContainer* GuiControl::Owner() const noexcept
{
	return static_cast<GuiPanelContainer*>(owner_);
}


/*
	Intersecting
*/

bool GuiControl::Intersects(const Vector2 &point) const noexcept
{
	if (node_)
	{
		for (auto &area : clickable_areas_)
		{
			if (Obb{area}.Transform(node_->FullTransformation()).Intersects(point))
				return true;
		}
	}

	return false;
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

bool GuiControl::KeyPressed([[maybe_unused]] KeyButton button) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::KeyReleased([[maybe_unused]] KeyButton button) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::CharacterPressed([[maybe_unused]] char character) noexcept
{
	//Optional to override
	return false;
}


/*
	Mouse events
*/

bool GuiControl::MousePressed([[maybe_unused]] MouseButton button, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::MouseReleased([[maybe_unused]] MouseButton button, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::MouseMoved([[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::MouseWheelRolled([[maybe_unused]] int delta, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

} //ion::gui::controls