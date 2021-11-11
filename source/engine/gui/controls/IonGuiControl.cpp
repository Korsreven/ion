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

#include "graphics/materials/IonMaterial.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/utilities/IonMatrix3.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector3.h"
#include "graphics/utilities/IonObb.h"
#include "gui/IonGuiFrame.h"
#include "gui/IonGuiPanelContainer.h"
#include "types/IonTypeTraits.h"

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

detail::control_visual_state_parts& GuiControl::GetVisualState(ControlState state,
	detail::control_visual_states_with_parts &visual_states) noexcept
{
	auto &visual_state = detail::control_state_to_visual_state(state, visual_states);

	//Fallback
	if (!visual_state.node)
	{
		//Check hovered
		if (hovered_ && state != ControlState::Hovered)
		{
			if (visual_states.hovered.node)
				return visual_states.hovered; //Display hovered state instead
		}

		//Check focused
		if (focused_ && state != ControlState::Focused)
		{
			if (visual_states.focused.node)
				return visual_states.focused; //Display focused state instead
		}

		//Check enabled
		if (state != ControlState::Enabled)
		{
			if (visual_states.enabled.node)
				return visual_states.enabled; //Display enabled state instead
		}
	}

	return visual_state;
}

NonOwningPtr<graphics::materials::Material> GuiControl::GetVisualState(gui_control::ControlState state,
	detail::visual_part_with_states &visual_states) noexcept
{
	auto visual_state = detail::control_state_to_visual_state(state, visual_states);

	//Fallback
	if (!visual_state)
	{
		//Check hovered
		if (hovered_ && state != ControlState::Hovered)
		{
			if (visual_states.hovered)
				return visual_states.hovered; //Display hovered state instead
		}

		//Check focused
		if (focused_ && state != ControlState::Focused)
		{
			if (visual_states.focused)
				return visual_states.focused; //Display focused state instead
		}

		//Check enabled
		if (state != ControlState::Enabled)
		{
			if (visual_states.enabled)
				return visual_states.enabled; //Display enabled state instead
		}
	}

	return visual_state;
}


void GuiControl::SetStateMaterial(NonOwningPtr<graphics::materials::Material> material, SceneNode &node) noexcept
{
	//Todo
}

void GuiControl::SetVisualState(ControlState state, detail::control_visual_states_with_parts &visual_states) noexcept
{	
	if (node_)
	{
		//Hide enabled state
		if (visual_states.enabled.node)
			visual_states.enabled.node->Visible(false);

		//Hide disabled state
		if (visual_states.disabled.node)
			visual_states.disabled.node->Visible(false);

		//Hide focused state
		if (visual_states.focused.node)
			visual_states.focused.node->Visible(false);

		//Hide pressed state
		if (visual_states.pressed.node)
			visual_states.pressed.node->Visible(false);

		//Hide hovered state
		if (visual_states.hovered.node)
			visual_states.hovered.node->Visible(false);


		//Show new visual state
		if (auto &visual_state = GetVisualState(state, visual_states); visual_state.node)
			visual_state.node->Visible(true);
	}
}

void GuiControl::SetVisualState(ControlState state, detail::control_visual_parts_with_states &visual_states) noexcept
{
	if (node_)
	{
		//Show new center visual state
		if (auto material = GetVisualState(state, visual_states.parts.center); material)
			SetStateMaterial(material, *visual_states.parts.center.node);


		//Show new top visual state
		if (auto material = GetVisualState(state, visual_states.parts.top); material)
			SetStateMaterial(material, *visual_states.parts.top.node);

		//Show new left visual state
		if (auto material = GetVisualState(state, visual_states.parts.left); material)
			SetStateMaterial(material, *visual_states.parts.left.node);

		//Show new bottom visual state
		if (auto material = GetVisualState(state, visual_states.parts.bottom); material)
			SetStateMaterial(material, *visual_states.parts.bottom.node);

		//Show new right visual state
		if (auto material = GetVisualState(state, visual_states.parts.right); material)
			SetStateMaterial(material, *visual_states.parts.right.node);


		//Show new top-left visual state
		if (auto material = GetVisualState(state, visual_states.parts.top_left); material)
			SetStateMaterial(material, *visual_states.parts.top_left.node);

		//Show new bottom-left visual state
		if (auto material = GetVisualState(state, visual_states.parts.bottom_left); material)
			SetStateMaterial(material, *visual_states.parts.bottom_left.node);

		//Show new top-right visual state
		if (auto material = GetVisualState(state, visual_states.parts.top_right); material)
			SetStateMaterial(material, *visual_states.parts.top_right.node);

		//Show new bottom-right visual state
		if (auto material = GetVisualState(state, visual_states.parts.bottom_right); material)
			SetStateMaterial(material, *visual_states.parts.bottom_right.node);
	}
}


void GuiControl::SetState(gui_control::ControlState state) noexcept
{
	if (visible_)
	{
		std::visit(
			[&](auto &&visual_states) noexcept
			{
				SetVisualState(state, visual_states);
			}, visual_states_);
	}

	state_ = state;
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

GuiControl::GuiControl(std::string name, Areas areas) :

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
		enabled_ && focusable_ && visible_)
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


void GuiControl::Visible(bool visible) noexcept
{
	if (visible_ != visible)
	{
		visible_ = visible;

		if (!visible && focused_)
			Defocus();

		if (node_)
			node_->Visible(visible, !visible);

		SetState(state_);
	}
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