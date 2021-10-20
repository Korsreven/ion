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

namespace gui_controller::detail
{

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
			focused_frame_->Defocus();

		focused_frame_ = &frame;
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

}

void GuiController::Deactivated(GuiFrame &frame) noexcept
{

}


void GuiController::Focused(GuiFrame &frame) noexcept
{
	if (frame.IsFocused() && !focused_frame_)
		focused_frame_ = &frame;
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