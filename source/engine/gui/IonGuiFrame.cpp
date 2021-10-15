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

bool GuiFrame::Unsubscribable(Listenable<events::listeners::GuiControlListener>&) noexcept
{
	//Cancel all unsubscribe attempts
	return false;
}


void GuiFrame::Enabled(controls::GuiControl &control) noexcept
{

}

void GuiFrame::Disabled(controls::GuiControl &control) noexcept
{

}


void GuiFrame::Focused(controls::GuiControl &control) noexcept
{

}

void GuiFrame::Defocused(controls::GuiControl &control) noexcept
{

}


void GuiFrame::Pressed(controls::GuiControl &control) noexcept
{

}

void GuiFrame::Released(controls::GuiControl &control) noexcept
{

}


void GuiFrame::Entered(controls::GuiControl &control) noexcept
{

}

void GuiFrame::Exited(controls::GuiControl &control) noexcept
{

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