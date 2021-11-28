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
		return std::nullopt;
	else if (!frame && !frames.back().current_frame)
		return std::end(frames.back().frames);
	else
	{
		if (auto iter = std::find(std::begin(frames.back().frames), std::end(frames.back().frames),
			frame ? frame : frames.back().current_frame); iter != std::end(frames.back().frames))
			return iter;
		else
			return std::nullopt;
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
	if (auto frame = dynamic_cast<GuiFrame*>(&component); frame)
		Created(*frame);
}

void GuiController::Created(GuiFrame &frame) noexcept
{
	//If the added frame is adopted
	frame.Deactivate();
}


void GuiController::Removed(GuiComponent &component) noexcept
{
	if (auto frame = dynamic_cast<GuiFrame*>(&component); frame)
		Removed(*frame);
}

void GuiController::Removed(GuiFrame &frame) noexcept
{
	frame.Deactivate();
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


/*
	Cursor skin
*/

void GuiController::AttachCursorSkin()
{
	if (cursor_skin_)
	{
		if (auto node = cursor_skin_->ParentNode(); node)
			node->DetachObject(*cursor_skin_.ModelObject);
		
		if (node_) //Create node for cursor
		{
			auto cursor_node = node_->CreateChildNode();
			cursor_node->AttachObject(*cursor_skin_.ModelObject);
			cursor_node->InheritRotation(false);
		}
	}
}

void GuiController::DetachCursorSkin() noexcept
{
	if (cursor_skin_)
	{
		if (auto node = cursor_skin_->ParentNode(); node_ && node)
			node_->RemoveChildNode(*node); //Remove cursor node
	}
}

void GuiController::RemoveCursorSkin() noexcept
{
	DetachCursorSkin();

	if (cursor_skin_)
		cursor_skin_->Owner()->RemoveModel(*cursor_skin_.ModelObject); //Remove cursor

	cursor_skin_ = {};
}


//Public

GuiController::GuiController(SceneNode &parent_node)
{
	node_ = parent_node.CreateChildNode();
	FrameEvents().Subscribe(*this);
}

GuiController::~GuiController() noexcept
{
	RemoveCursorSkin();
}


/*
	Modifiers
*/

void GuiController::CursorSkin(gui_controller::CursorSkin cursor_skin) noexcept
{
	if (cursor_skin_.ModelObject != cursor_skin.ModelObject)
	{
		RemoveCursorSkin();
		cursor_skin_ = std::move(cursor_skin);
		AttachCursorSkin();
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
		static_cast<GuiFrame&>(frame).FrameStarted(time);
}

void GuiController::FrameEnded(duration time) noexcept
{
	for (auto &frame : Frames())
		static_cast<GuiFrame&>(frame).FrameEnded(time);
}


/*
	Key events
*/

bool GuiController::KeyPressed(KeyButton button) noexcept
{
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
	return focused_frame_ ?
		focused_frame_->CharacterPressed(character) :
		false;
}


/*
	Mouse events
*/

bool GuiController::MousePressed(MouseButton button, Vector2 position) noexcept
{
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
				static_cast<GuiFrame*>(top_frame)->MousePressed(button, position))
				return true; //Consumed
		}
	}

	return false;
}

bool GuiController::MouseReleased(MouseButton button, Vector2 position) noexcept
{
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
				static_cast<GuiFrame*>(top_frame)->MouseReleased(button, position))
				return true; //Consumed
		}
	}

	return false;
}

bool GuiController::MouseMoved(Vector2 position) noexcept
{
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
				static_cast<GuiFrame*>(top_frame)->MouseMoved(position))
				return true; //Consumed
		}
	}

	return false;
}

bool GuiController::MouseWheelRolled(int delta, Vector2 position) noexcept
{
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
				static_cast<GuiFrame*>(top_frame)->MouseWheelRolled(delta, position))
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
	return static_pointer_cast<GuiFrame>(GetComponent(name));
}

NonOwningPtr<const GuiFrame> GuiController::GetFrame(std::string_view name) const noexcept
{
	return static_pointer_cast<const GuiFrame>(GetComponent(name));
}


/*
	Frames
	Removing
*/

void GuiController::ClearFrames() noexcept
{
	active_frames_.clear();
	active_frames_.shrink_to_fit();
	ClearComponents();
}

bool GuiController::RemoveFrame(GuiFrame &frame) noexcept
{
	return RemoveComponent(frame);
}

bool GuiController::RemoveFrame(std::string_view name) noexcept
{
	return RemoveComponent(name);
}

} //ion::gui