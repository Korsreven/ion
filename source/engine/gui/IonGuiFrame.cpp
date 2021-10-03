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

namespace ion::gui
{

namespace gui_frame::detail
{

} //gui_frame::detail


GuiFrame::GuiFrame(std::string name) :
	GuiPanelContainer{std::move(name)}
{
	//Empty
}

} //ion::gui