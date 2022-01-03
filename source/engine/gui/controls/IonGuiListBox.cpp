/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiListBox.cpp
-------------------------------------------
*/

#include "IonGuiListBox.h"

#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"

namespace ion::gui::controls
{

using namespace gui_list_box;

namespace gui_list_box::detail
{

/*
	Skins
*/

void resize_skin(ListBoxSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;
	auto delta_position = delta_size * 0.5_r;

	if (skin.Selection)
	{
		auto &center = skin.Selection->Position();
		gui_control::detail::resize_part(skin.Selection, delta_size, delta_position, center);
	}
}

} //gui_list_box::detail


//Private

void GuiListBox::DefaultSetup() noexcept
{
	CaptionLayout(gui_control::ControlCaptionLayout::OutsideTopCenter);
}


//Protected

/*
	Events
*/

void GuiListBox::Resized(Vector2 from_size, Vector2 to_size) noexcept
{
	if (skin_)
	{
		detail::resize_skin(static_cast<ListBoxSkin&>(*skin_), from_size, to_size);
		UpdateSelection();
	}

	GuiControl::Resized(from_size, to_size); //Use base functionality
}


void GuiListBox::Scrolled(int delta) noexcept
{
	//Todo
}

int GuiListBox::TotalElements() noexcept
{
	return 0;
}

int GuiListBox::ElementsInView() noexcept
{
	return 0;
}


int GuiListBox::ScrollPosition() noexcept
{
	return 0;
}


void GuiListBox::ItemSelected() noexcept
{
	Changed();
}

void GuiListBox::ItemDeselected() noexcept
{
	//Empty
}


/*
	States
*/

void GuiListBox::SetSkinState(gui_control::ControlState state, ListBoxSkin &skin) noexcept
{
	if (skin.Selection)
	{
		if (item_index_)
			SetPartState(state, skin.Selection);

		skin.Selection->Visible(item_index_.has_value());
	}
}

void GuiListBox::SetState(gui_control::ControlState state) noexcept
{
	GuiControl::SetState(state); //Use base functionality

	if (visible_ && skin_)
		SetSkinState(state, static_cast<ListBoxSkin&>(*skin_));
}


/*
	Skins
*/

void GuiListBox::UpdateSelection() noexcept
{
	
}

void GuiListBox::UpdateItems() noexcept
{
	
}


//Public

GuiListBox::GuiListBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ListBoxSkin> skin, gui_control::BoundingBoxes hit_boxes) :
	GuiScrollable{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(hit_boxes)}
{
	DefaultSetup();
}

GuiListBox::GuiListBox(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ListBoxSkin> skin, const Vector2 &size, gui_control::BoundingBoxes hit_boxes) :
	GuiScrollable{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size, std::move(hit_boxes)}
{
	DefaultSetup();
}


/*
	Key events
*/

bool GuiListBox::KeyReleased([[maybe_unused]] KeyButton button) noexcept
{
	return false;
}


/*
	Mouse events
*/

bool GuiListBox::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	return false;
}

bool GuiListBox::MouseWheelRolled(int delta, [[maybe_unused]] Vector2 position) noexcept
{
	return false;
}

} //ion::gui::controls