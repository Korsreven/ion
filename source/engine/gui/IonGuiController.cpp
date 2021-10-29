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


void activate_frame(GuiFrame &frame, frames &to_frames, bool modal) noexcept
{
	if (!is_frame_activated(frame, to_frames))
	{
		//Push new layer
		if (modal || std::empty(to_frames))
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

		for (auto iter = detail::get_next_frame_iterator(*current_iter, top_frames);
			iter != *current_iter; iter = detail::get_next_frame_iterator(iter, top_frames))
		{
			if (iter != std::end(top_frames) &&
				(*iter)->IsFocusable())
				return *iter;
		}
	}

	return nullptr;
}

GuiFrame* GuiController::PreviousFocusableFrame(GuiFrame *from_frame) const noexcept
{
	if (auto current_iter = detail::get_frame_iterator(active_frames_, from_frame); current_iter)
	{
		auto &top_frames = active_frames_.back().frames;

		for (auto iter = detail::get_previous_frame_iterator(*current_iter, top_frames);
			iter != *current_iter; iter = detail::get_previous_frame_iterator(iter, top_frames))
		{
			if (iter != std::end(top_frames) &&
				(*iter)->IsFocusable())
				return *iter;
		}
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
		detail::activate_frame(frame, active_frames_, false);
}

void GuiController::Deactivated(GuiFrame &frame) noexcept
{
	if (!frame.IsActivated())
		detail::deactivate_frame(frame, active_frames_);
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


//Public

GuiController::GuiController(SceneNode &parent_node)
{
	node_ = parent_node.CreateChildNode();
	FrameEvents().Subscribe(*this);
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
	return NextFocusableFrame(&from_frame);
}

GuiFrame* GuiController::PreviousFocusableFrame(GuiFrame &from_frame) const noexcept
{
	return PreviousFocusableFrame(&from_frame);
}


/*
	Frame events
*/

void GuiController::FrameStarted(duration time) noexcept
{

}

void GuiController::FrameEnded(duration time) noexcept
{

}


/*
	Key events
*/

void GuiController::KeyPressed(KeyButton button) noexcept
{

}

void GuiController::KeyReleased(KeyButton button) noexcept
{

}

void GuiController::CharacterPressed(char character) noexcept
{

}


/*
	Mouse events
*/

void GuiController::MousePressed(MouseButton button, Vector2 position) noexcept
{

}

void GuiController::MouseReleased(MouseButton button, Vector2 position) noexcept
{

}

void GuiController::MouseMoved(Vector2 position) noexcept
{

}

void GuiController::MouseWheelRolled(int delta, Vector2 position) noexcept
{

}


/*
	Window events
*/

void GuiController::WindowActionReceived(WindowAction action) noexcept
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