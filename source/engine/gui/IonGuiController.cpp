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

void activate_frame(GuiFrame &frame, frames &to_frames, bool modal) noexcept
{
	//Push new layer
	if (modal || std::empty(to_frames))
		to_frames.emplace_back(&frame).frames.push_back(&frame);
	//Append to top layer
	else
		to_frames.back().frames.push_back(&frame);
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
						*(iter - 1);
				
				return;
			}
		}
	}
}


bool is_frame_focusable(const GuiFrame &frame, const frames &frames) noexcept
{
	if (std::empty(frames))
		return false;
	else
		return std::find(std::begin(frames.back().frames), std::end(frames.back().frames), &frame) !=
			std::end(frames.back().frames);
}

} //gui_controller::detail


//Private

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
	//In case the created frame is actually adopted

	if (frame.IsFocused())
	{
		if (focused_frame_)
		{
			focused_frame_->Defocus();
			focused_frame_ = nullptr;
		}

		detail::activate_frame(frame, active_frames_, false);
		Focused(frame);
	}
}


void GuiController::Removed(GuiComponent &component) noexcept
{
	if (auto frame = dynamic_cast<GuiFrame*>(&component); frame)
		Removed(*frame);
}

void GuiController::Removed(GuiFrame &frame) noexcept
{
	if (focused_frame_ == &frame)
		focused_frame_ = nullptr;

	detail::deactivate_frame(frame, active_frames_);
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
	detail::activate_frame(frame, active_frames_, false);
}

void GuiController::Deactivated(GuiFrame &frame) noexcept
{
	detail::deactivate_frame(frame, active_frames_);
}


void GuiController::Focused(GuiFrame &frame) noexcept
{
	if (frame.IsFocused() && !focused_frame_)
	{
		focused_frame_ = &frame;

		if (!std::empty(active_frames_))
			active_frames_.back().current_frame = &frame;
	}
}

void GuiController::Defocused(GuiFrame &frame) noexcept
{
	if (!frame.IsFocused() && focused_frame_ == &frame)
	{
		focused_frame_ = nullptr;

		if (!std::empty(active_frames_) &&
			active_frames_.back().current_frame == &frame)
			active_frames_.back().current_frame = nullptr;
	}
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

bool GuiController::IsFocusable(const GuiFrame &frame) const noexcept
{
	return detail::is_frame_focusable(frame, active_frames_);
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