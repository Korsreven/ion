/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiMouseCursor.cpp
-------------------------------------------
*/

#include "IonGuiMouseCursor.h"

namespace ion::gui::controls
{

using namespace gui_mouse_cursor;

namespace gui_mouse_cursor::detail
{
} //gui_mouse_cursor::detail


//Private

void GuiMouseCursor::DefaultSetup() noexcept
{
	Enabled(false);
}


//Protected

/*
	Skins
*/

OwningPtr<gui_control::ControlSkin> GuiMouseCursor::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<MouseCursorSkin*>(skin.get()))
	{
		auto mouse_cursor_skin = make_owning<MouseCursorSkin>();
		mouse_cursor_skin->Assign(*skin);
		return mouse_cursor_skin;
	}
	else
		return skin;
}


//Public

GuiMouseCursor::GuiMouseCursor(std::string name, const std::optional<Vector2> &size) :
	GuiControl{std::move(name), size}
{
	DefaultSetup();
}

GuiMouseCursor::GuiMouseCursor(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size) :
	GuiControl{std::move(name), skin, size}
{
	DefaultSetup();
}

} //ion::gui::controls