/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiController.cpp
-------------------------------------------
*/

#include "IonGuiController.h"

#include <algorithm>
#include <utility>

#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneNode.h"

namespace ion::gui
{

using namespace gui_controller;

namespace gui_controller::detail
{

bool is_frame_on_top(const GuiFrame &frame, const frames &frames) noexcept
{
	if (std::empty(frames))
		return false;
	else
		return std::find(std::begin(frames.back().frames), std::end(frames.back().frames), &frame) !=
			std::end(frames.back().frames);
}

bool is_frame_activated(const GuiFrame &frame, const frames &frames) noexcept
{
	//Search through all active frames
	for (auto &active_layer : frames)
	{
		//Search through active layer
		for (auto &active_frame : active_layer.frames)
		{
			if (active_frame == &frame)
				return true;
		}
	}

	return false;
}


void activate_frame(GuiFrame &frame, frames &to_frames) noexcept
{
	if (!is_frame_activated(frame, to_frames))
	{
		//Push new layer
		if (std::empty(to_frames) ||
			frame.Mode() == gui_frame::FrameMode::Modal)
			to_frames.emplace_back().frames.push_back(&frame);
		//Append to top layer
		else
			to_frames.back().frames.push_back(&frame);
	}
}

void deactivate_frame(GuiFrame &frame, frames &from_frames) noexcept
{
	//Search through all active frames (from top)
	for (auto riter = std::rbegin(from_frames),
		rend = std::rend(from_frames); riter != rend; ++riter)
	{
		auto &active_layer = *riter;

		//Search through active layer
		for (auto iter = std::begin(active_layer.frames),
			end = std::end(active_layer.frames); iter != end; ++iter)
		{
			if (*iter == &frame)
			{
				iter = active_layer.frames.erase(iter);

				//No frames left, remove empty layer
				if (std::empty(active_layer.frames))
					from_frames.erase(--riter.base());

				//Current frame removed, set new current frame
				else if (active_layer.current_frame == &frame)
					active_layer.current_frame =
						iter != std::end(active_layer.frames) ?
						*iter :
						nullptr;
				
				return;
			}
		}
	}
}


std::optional<frame_pointers::const_iterator> get_frame_iterator(const frames &frames, GuiFrame *frame) noexcept
{
	if (std::empty(frames))
		return {};
	else if (!frame && !frames.back().current_frame)
		return std::end(frames.back().frames);
	else
	{
		if (auto iter = std::find(std::begin(frames.back().frames), std::end(frames.back().frames),
			frame ? frame : frames.back().current_frame); iter != std::end(frames.back().frames))
			return iter;
		else
			return {};
	}
}

} //gui_controller::detail


//Private

GuiFrame* GuiController::NextFocusableFrame(GuiFrame *from_frame) const noexcept
{
	if (auto current_iter = detail::get_frame_iterator(active_frames_, from_frame); current_iter)
	{
		auto &top_frames = active_frames_.back().frames;
		auto iter = detail::get_next_frame_iterator(*current_iter, top_frames);

		if (*current_iter == std::end(top_frames))
			*current_iter = iter;

		do
		{
			if ((*iter)->IsFocusable())
				return *iter;
		} while ((iter = detail::get_next_frame_iterator(iter, top_frames)) != *current_iter);
	}

	return nullptr;
}

GuiFrame* GuiController::PreviousFocusableFrame(GuiFrame *from_frame) const noexcept
{
	if (auto current_iter = detail::get_frame_iterator(active_frames_, from_frame); current_iter)
	{
		auto &top_frames = active_frames_.back().frames;
		auto iter = detail::get_previous_frame_iterator(*current_iter, top_frames);

		if (*current_iter == std::end(top_frames))
			*current_iter = iter;

		do
		{
			if ((*iter)->IsFocusable())
				return *iter;
		} while ((iter = detail::get_previous_frame_iterator(iter, top_frames)) != *current_iter);
	}

	return nullptr;
}


/*
	Events
*/


void GuiController::Created(GuiComponent &component) noexcept
{
	GuiContainer::Created(component); //Use base functionality

	if (auto frame = dynamic_cast<GuiFrame*>(&component); frame)
		Created(*frame);
	else if (auto tooltip = dynamic_cast<controls::GuiTooltip*>(&component); tooltip)
		Created(*tooltip);
}

void GuiController::Created(GuiFrame &frame) noexcept
{
	//If the added frame is adopted
	frame.Deactivate();
	frames_.push_back(&frame);
}

void GuiController::Created(controls::GuiTooltip &tooltip) noexcept
{
	if (!active_tooltip_)
		active_tooltip_ = &tooltip;

	tooltips_.push_back(&tooltip);
}


void GuiController::Removed(GuiComponent &component) noexcept
{
	if (auto frame = dynamic_cast<GuiFrame*>(&component); frame)
		Removed(*frame);
	else if (auto tooltip = dynamic_cast<controls::GuiTooltip*>(&component); tooltip)
		Removed(*tooltip);

	GuiContainer::Removed(component); //Use base functionality
}

void GuiController::Removed(GuiFrame &frame) noexcept
{
	frame.Deactivate();

	auto iter =
		std::find_if(std::begin(frames_), std::end(frames_),
			[&](auto &x) noexcept
			{
				return x == &frame;
			});

	//Frame found
	if (iter != std::end(frames_))
		frames_.erase(iter);
}

void GuiController::Removed(controls::GuiTooltip &tooltip) noexcept
{
	if (active_tooltip_ == &tooltip)
		active_tooltip_ = nullptr;

	auto iter =
		std::find_if(std::begin(tooltips_), std::end(tooltips_),
			[&](auto &x) noexcept
			{
				return x == &tooltip;
			});

	//Tooltip found
	if (iter != std::end(tooltips_))
		tooltips_.erase(iter);
}


bool GuiController::Unsubscribable(Listenable<events::listeners::GuiFrameListener>&) noexcept
{
	//Cancel all unsubscribe attempts
	return false;
}


void GuiController::Enabled([[maybe_unused]] GuiFrame &frame) noexcept
{
	//Empty
}

void GuiController::Disabled([[maybe_unused]] GuiFrame &frame) noexcept
{
	//Empty
}


void GuiController::Activated(GuiFrame &frame) noexcept
{
	if (frame.IsActivated())
		detail::activate_frame(frame, active_frames_);
}

void GuiController::Deactivated(GuiFrame &frame) noexcept
{
	if (!frame.IsActivated() && !std::empty(active_frames_))
	{
		//Deactivate all other top frames first
		if (frame.Mode() == gui_frame::FrameMode::Modal)
		{
			for (auto &top_frame : active_frames_.back().frames)
			{
				if (top_frame != &frame)
					detail::deactivate_frame(*top_frame, active_frames_);
			}
		}

		detail::deactivate_frame(frame, active_frames_);
	}
}


void GuiController::Focused(GuiFrame &frame) noexcept
{
	if (frame.IsFocused() && focused_frame_ != &frame)
	{
		if (focused_frame_)
			focused_frame_->Defocus();

		focused_frame_ = &frame;

		if (!std::empty(active_frames_))
			active_frames_.back().current_frame = &frame;
	}
}

void GuiController::Defocused(GuiFrame &frame) noexcept
{
	if (!frame.IsFocused() && focused_frame_ == &frame)
		focused_frame_ = nullptr;
}


void GuiController::Enabled() noexcept
{
	GuiContainer::Enabled(); //Use base functionality
}

void GuiController::Disabled() noexcept
{
	GuiContainer::Disabled(); //Use base functionality
}


void GuiController::Shown() noexcept
{
	if (mouse_cursor_skin_)
	{
		if (auto node = mouse_cursor_skin_->ParentNode(); node)
			node->Visible(true);
	}

	for (auto &frame : Frames())
	{
		if (frame.IsVisible())
		{
			frame.GuiComponent::Hide(); //Force Show to trigger Shown event
			frame.GuiComponent::Show();
		}
	}

	GuiContainer::Shown(); //Use base functionality
}

void GuiController::Hidden() noexcept
{
	if (active_tooltip_)
		active_tooltip_->GuiComponent::Hide(); //Hide immediately

	if (mouse_cursor_skin_)
	{
		if (auto node = mouse_cursor_skin_->ParentNode(); node)
			node->Visible(false);
	}

	GuiContainer::Hidden(); //Use base functionality
}


/*
	Mouse cursor skin
*/

void GuiController::AttachMouseCursorSkin(real z_order)
{
	if (mouse_cursor_skin_)
	{
		if (auto node = mouse_cursor_skin_->ParentNode(); node)
			node->DetachObject(*mouse_cursor_skin_.ModelObject);
		
		if (node_) //Create node for cursor
		{
			auto mouse_cursor_node = node_->CreateChildNode({0.0_r, 0.0_r, z_order});
			mouse_cursor_node->AttachObject(*mouse_cursor_skin_.ModelObject);
			mouse_cursor_node->InheritRotation(false);
			mouse_cursor_node->RotationOrigin(scene_node::NodeRotationOrigin::Local);
		}
	}
}

void GuiController::DetachMouseCursorSkin() noexcept
{
	if (mouse_cursor_skin_)
	{
		if (auto node = mouse_cursor_skin_->ParentNode(); node_ && node)
			node_->RemoveChildNode(*node); //Remove cursor node
	}
}

void GuiController::RemoveMouseCursorSkin() noexcept
{
	DetachMouseCursorSkin();

	if (mouse_cursor_skin_)
		mouse_cursor_skin_->Owner()->RemoveModel(*mouse_cursor_skin_.ModelObject); //Remove cursor

	mouse_cursor_skin_ = {};
}


void GuiController::UpdateMouseCursor(const Vector2 &position) noexcept
{
	if (mouse_cursor_skin_)
	{
		if (auto node = mouse_cursor_skin_->ParentNode(); node)
		{
			auto [half_width, half_height] =
				(mouse_cursor_skin_->AxisAlignedBoundingBox().ToHalfSize() * node->DerivedScaling()).XY();

			//Adjust from center to hot spot
			auto hot_spot_off =
				[&]() noexcept -> Vector2
				{
					switch (mouse_cursor_hot_spot_)
					{
						case GuiMouseCursorHotSpot::TopLeft:
						return {half_width, -half_height};

						case GuiMouseCursorHotSpot::TopCenter:
						return {0.0_r, -half_height};

						case GuiMouseCursorHotSpot::TopRight:
						return {-half_width, -half_height};

						case GuiMouseCursorHotSpot::Left:
						return {half_width, 0.0_r};

						case GuiMouseCursorHotSpot::Right:
						return {-half_width, 0.0_r};

						case GuiMouseCursorHotSpot::BottomLeft:
						return {half_width, half_height};

						case GuiMouseCursorHotSpot::BottomCenter:
						return {0.0_r, half_height};

						case GuiMouseCursorHotSpot::BottomRight:
						return {-half_width, half_height};

						default:
						return vector2::Zero;
					}
				}();

			node->DerivedPosition(position + hot_spot_off);
		}
	}
}


//Public

GuiController::GuiController(SceneNode &parent_node)
{
	node_ = parent_node.CreateChildNode();
	FrameEvents().Subscribe(*this);
}

GuiController::~GuiController() noexcept
{
	RemoveMouseCursorSkin();
}


/*
	Modifiers
*/

void GuiController::ActiveTooltip(std::string_view name) noexcept
{
	if (active_tooltip_)
		active_tooltip_->GuiComponent::Hide(); //Hide immediately

	active_tooltip_ = GetTooltip(name).get();
}

void GuiController::MouseCursorSkin(gui_controller::GuiMouseCursorSkin skin, real z_order) noexcept
{
	if (mouse_cursor_skin_.ModelObject != skin.ModelObject)
	{
		RemoveMouseCursorSkin();
		mouse_cursor_skin_ = std::move(skin);
		AttachMouseCursorSkin(z_order);
	}
}


/*
	Observers
*/

bool GuiController::IsOnTop(const GuiFrame &frame) const noexcept
{
	return detail::is_frame_on_top(frame, active_frames_);
}


/*
	Tabulating
*/

void GuiController::TabForward() noexcept
{
	if (focused_frame_)
		focused_frame_->TabForward();
	else if (auto focusable_frame = NextFocusableFrame(); focusable_frame)
		focusable_frame->TabForward();
}

void GuiController::TabBackward() noexcept
{
	if (focused_frame_)
		focused_frame_->TabBackward();
	else if (auto focusable_frame = PreviousFocusableFrame(); focusable_frame)
		focusable_frame->TabBackward();
}


GuiFrame* GuiController::NextFocusableFrame() const noexcept
{
	return NextFocusableFrame(nullptr);
}

GuiFrame* GuiController::PreviousFocusableFrame() const noexcept
{
	return PreviousFocusableFrame(nullptr);
}


GuiFrame* GuiController::NextFocusableFrame(GuiFrame &from_frame) const noexcept
{
	return from_frame.IsActivated() ? NextFocusableFrame(&from_frame) : nullptr;
}

GuiFrame* GuiController::PreviousFocusableFrame(GuiFrame &from_frame) const noexcept
{
	return from_frame.IsActivated() ? PreviousFocusableFrame(&from_frame) : nullptr;
}


/*
	Frame events
*/

void GuiController::FrameStarted(duration time) noexcept
{
	for (auto &frame : Frames())
		frame.FrameStarted(time);

	if (active_tooltip_)
		active_tooltip_->FrameStarted(time);
}

void GuiController::FrameEnded(duration time) noexcept
{
	for (auto &frame : Frames())
		frame.FrameEnded(time);
}


/*
	Key events
*/

bool GuiController::KeyPressed(KeyButton button) noexcept
{
	if (!enabled_)
		return false;

	if (focused_frame_ &&
		focused_frame_->KeyPressed(button))
		return true; //Consumed
	
	else
	{
		switch (button)
		{
			case KeyButton::Shift:
			shift_pressed_ = true;
			return true; //Consumed
		}

		return false;
	}
}

bool GuiController::KeyReleased(KeyButton button) noexcept
{
	if (!enabled_)
		return false;

	if (focused_frame_ &&
		focused_frame_->KeyReleased(button))
		return true; //Consumed

	else
	{
		switch (button)
		{
			case KeyButton::Shift:
			shift_pressed_ = false;
			return true; //Consumed

			case KeyButton::Tab:
			{
				if (shift_pressed_)
					TabBackward();
				else
					TabForward();

				return true; //Consumed
			}

			case KeyButton::DownArrow:
			case KeyButton::RightArrow:
			TabForward();
			return true; //Consumed

			case KeyButton::UpArrow:
			case KeyButton::LeftArrow:
			TabBackward();
			return true; //Consumed
		}

		return false;
	}
}

bool GuiController::CharacterPressed(char character) noexcept
{
	if (!enabled_)
		return false;

	return focused_frame_ ?
		focused_frame_->CharacterPressed(character) :
		false;
}


/*
	Mouse events
*/

bool GuiController::MousePressed(MouseButton button, Vector2 position) noexcept
{
	if (!enabled_)
		return false;

	//Check focused frame first
	if (focused_frame_ &&
		focused_frame_->MousePressed(button, position))
		return true; //Consumed

	else if (!std::empty(active_frames_))
	{
		//Check all other top frames
		for (auto &top_frame : active_frames_.back().frames)
		{
			if (top_frame != focused_frame_ &&
				top_frame->MousePressed(button, position))
				return true; //Consumed
		}
	}

	return false;
}

bool GuiController::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	if (!enabled_)
		return false;

	//Check focused frame first
	if (focused_frame_ &&
		focused_frame_->MouseReleased(button, position))
		return true; //Consumed

	else if (!std::empty(active_frames_))
	{
		//Check all other top frames
		for (auto &top_frame : active_frames_.back().frames)
		{
			if (top_frame != focused_frame_ &&
				top_frame->MouseReleased(button, position))
				return true; //Consumed
		}
	}

	return false;
}

bool GuiController::MouseMoved(Vector2 position) noexcept
{
	UpdateMouseCursor(position);

	if (!enabled_)
		return false;

	if (active_tooltip_)
		active_tooltip_->MouseMoved(position);

	//Check focused frame first
	if (focused_frame_ &&
		focused_frame_->MouseMoved(position))
		return true; //Consumed

	else if (!std::empty(active_frames_))
	{
		//Check all other top frames
		for (auto &top_frame : active_frames_.back().frames)
		{
			if (top_frame != focused_frame_ &&
				top_frame->MouseMoved(position))
				return true; //Consumed
		}
	}

	return false;
}

bool GuiController::MouseWheelRolled(int delta, Vector2 position) noexcept
{
	if (!enabled_)
		return false;

	//Check focused frame first
	if (focused_frame_ &&
		focused_frame_->MouseWheelRolled(delta, position))
		return true; //Consumed

	else if (!std::empty(active_frames_))
	{
		//Check all other top frames
		for (auto &top_frame : active_frames_.back().frames)
		{
			if (top_frame != focused_frame_ &&
				top_frame->MouseWheelRolled(delta, position))
				return true; //Consumed
		}
	}

	return false;
}


/*
	Window events
*/

void GuiController::WindowActionReceived([[maybe_unused]] WindowAction action) noexcept
{

}


/*
	Frames
	Creating
*/

NonOwningPtr<GuiFrame> GuiController::CreateFrame(std::string name)
{
	return CreateComponent<GuiFrame>(std::move(name));
}

NonOwningPtr<GuiFrame> GuiController::CreateFrame(GuiFrame &&frame)
{
	return CreateComponent<GuiFrame>(std::move(frame));
}


/*
	Frames
	Retrieving
*/

NonOwningPtr<GuiFrame> GuiController::GetFrame(std::string_view name) noexcept
{
	return dynamic_pointer_cast<GuiFrame>(GetComponent(name));
}

NonOwningPtr<const GuiFrame> GuiController::GetFrame(std::string_view name) const noexcept
{
	return dynamic_pointer_cast<const GuiFrame>(GetComponent(name));
}


/*
	Frames
	Removing
*/

void GuiController::ClearFrames() noexcept
{
	active_frames_.clear();
	active_frames_.shrink_to_fit();

	auto frames = std::move(frames_);

	for (auto &frame : frames)
	{
		if (RemoveFrame(*frame))
			frame = nullptr;
	}

	frames.erase(
		std::remove(std::begin(frames), std::end(frames), nullptr),
		std::end(frames));

	frames_ = std::move(frames);
	frames_.shrink_to_fit();
}

bool GuiController::RemoveFrame(GuiFrame &frame) noexcept
{
	return RemoveComponent(frame);
}

bool GuiController::RemoveFrame(std::string_view name) noexcept
{
	return RemoveComponent(name);
}


/*
	Tooltips
	Creating
*/

NonOwningPtr<controls::GuiTooltip> GuiController::CreateTooltip(std::string name, controls::gui_control::ControlSkin skin)
{
	return CreateComponent<controls::GuiTooltip>(std::move(name), std::nullopt, std::move(skin));
}

NonOwningPtr<controls::GuiTooltip> GuiController::CreateTooltip(std::string name, controls::gui_control::ControlSkin skin, const Vector2 &size)
{
	return CreateComponent<controls::GuiTooltip>(std::move(name), std::nullopt, std::move(skin), size);
}

NonOwningPtr<controls::GuiTooltip> GuiController::CreateTooltip(controls::GuiTooltip &&tooltip)
{
	return CreateComponent<controls::GuiTooltip>(std::move(tooltip));
}


/*
	Tooltips
	Retrieving
*/

NonOwningPtr<controls::GuiTooltip> GuiController::GetTooltip(std::string_view name) noexcept
{
	return dynamic_pointer_cast<controls::GuiTooltip>(GetComponent(name));
}

NonOwningPtr<const controls::GuiTooltip> GuiController::GetTooltip(std::string_view name) const noexcept
{
	return dynamic_pointer_cast<const controls::GuiTooltip>(GetComponent(name));
}


/*
	Tooltips
	Removing
*/

void GuiController::ClearTooltips() noexcept
{
	auto tooltips = std::move(tooltips_);

	for (auto &tooltip : tooltips)
	{
		if (RemoveTooltip(*tooltip))
			tooltip = nullptr;
	}

	tooltips.erase(
		std::remove(std::begin(tooltips), std::end(tooltips), nullptr),
		std::end(tooltips));

	tooltips_ = std::move(tooltips);
	tooltips_.shrink_to_fit();
}

bool GuiController::RemoveTooltip(controls::GuiTooltip &tooltip) noexcept
{
	return RemoveComponent(tooltip);
}

bool GuiController::RemoveTooltip(std::string_view name) noexcept
{
	return RemoveComponent(name);
}


/*
	Components
	Removing (optimization)
*/

void GuiController::ClearComponents() noexcept
{
	active_frames_.clear();
	active_frames_.shrink_to_fit();

	frames_.clear();
	tooltips_.clear();
	GuiContainer::ClearComponents();
		//This will go much faster because frames and tooltips are pre-cleared
	
	//Non-removable components will still be present
	//Add them back to the controls/panels containers
	for (auto &component : Components())
		Created(component);

	frames_.shrink_to_fit();
	tooltips_.shrink_to_fit();
}

} //ion::gui