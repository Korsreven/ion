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

#include "graphics/scene/IonModel.h"
#include "graphics/scene/graph/IonSceneNode.h"

namespace ion::gui::controls
{

using namespace gui_mouse_cursor;

namespace gui_mouse_cursor::detail
{

Vector2 hot_spot_offset(MouseCursorHotSpot hot_spot, const Vector2 &cursor_size) noexcept
{
	auto [half_width, half_height] = (cursor_size * 0.5_r).XY();

	switch (hot_spot)
	{
		case MouseCursorHotSpot::TopLeft:
		return {half_width, -half_height};

		case MouseCursorHotSpot::TopCenter:
		return {0.0_r, -half_height};

		case MouseCursorHotSpot::TopRight:
		return {-half_width, -half_height};

		case MouseCursorHotSpot::Left:
		return {half_width, 0.0_r};

		case MouseCursorHotSpot::Right:
		return {-half_width, 0.0_r};

		case MouseCursorHotSpot::BottomLeft:
		return {half_width, half_height};

		case MouseCursorHotSpot::BottomCenter:
		return {0.0_r, half_height};

		case MouseCursorHotSpot::BottomRight:
		return {-half_width, half_height};

		default:
		return vector2::Zero;
	}
}

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


void GuiMouseCursor::AttachSkin()
{
	GuiControl::AttachSkin(); //Use base functionality

	if (node_)
	{
		node_->InheritRotation(false);
		node_->RotationOrigin(scene_node::NodeRotationOrigin::Local);
	}
}

void GuiMouseCursor::UpdatePosition(Vector2 position) noexcept
{
	if (node_ && skin_)
	{
		auto cursor_size =
			skin_->Parts->AxisAlignedBoundingBox().ToSize() * node_->DerivedScaling();

		//Adjust from center to hot spot
		auto hot_spot_off =
			detail::hot_spot_offset(hot_spot_, cursor_size);
		node_->DerivedPosition(position + hot_spot_off);
	}
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


/*
	Mouse events
*/

bool GuiMouseCursor::MouseMoved(Vector2 position) noexcept
{
	if (visible_)
	{
		UpdatePosition(position);
		return true;
	}
	else
		return false;
}

} //ion::gui::controls