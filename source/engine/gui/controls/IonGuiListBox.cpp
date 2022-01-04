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

#include <algorithm>

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

	for (auto &icon : skin.Icons)
	{
		if (icon)
			gui_control::detail::resize_sprite(*icon, delta_size, delta_position, vector2::Zero);
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
		UpdateItems();
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
	//Todo
	UpdateSelection();
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
	Items
	Adding/inserting
*/

void GuiListBox::AddItem(std::string content, NonOwningPtr<graphics::materials::Material> icon)
{
	InsertItem(std::ssize(items_), {std::move(content), icon});
}

void GuiListBox::AddItem(ListBoxItem item)
{
	InsertItem(std::ssize(items_), std::move(item));
}

void GuiListBox::AddItems(ListBoxItems items)
{
	InsertItems(std::ssize(items_), std::move(items));
}


void GuiListBox::InsertItem(int off, std::string content, NonOwningPtr<graphics::materials::Material> icon)
{
	InsertItem(off, {std::move(content), icon});
}

void GuiListBox::InsertItem(int off, ListBoxItem item)
{
	InsertItems(off, ListBoxItems{item});
}

void GuiListBox::InsertItems(int off, ListBoxItems items)
{
	if (off >= 0)
	{
		off = std::clamp(off, 0, std::ssize(items_));
		items_.insert(std::begin(items_) + off, std::begin(items), std::end(items));

		if (item_index_ && *item_index_ >= off)
			*item_index_ += std::ssize(items);

		UpdateItems();
	}
}


/*
	Items
	Replacing
*/

void GuiListBox::ReplaceItem(int off, std::string content, NonOwningPtr<graphics::materials::Material> icon)
{
	ReplaceItems(off, off + 1, std::move(content), icon);
}

void GuiListBox::ReplaceItem(int off, ListBoxItem item)
{
	ReplaceItems(off, off + 1, std::move(item));
}

void GuiListBox::ReplaceItem(int off, ListBoxItems items)
{
	ReplaceItems(off, off + 1, std::move(items));
}


void GuiListBox::ReplaceItems(int first, int last, std::string content, NonOwningPtr<graphics::materials::Material> icon)
{
	ReplaceItems(first, last, {std::move(content), icon});
}

void GuiListBox::ReplaceItems(int first, int last, ListBoxItem item)
{
	ReplaceItems(first, last, ListBoxItems{item});
}

void GuiListBox::ReplaceItems(int first, int last, ListBoxItems items)
{
	if (first >= 0 && first < last)
	{
		RemoveItems(first, last);
		InsertItems(first, std::move(items));
		UpdateItems();
	}
}


/*
	Items
	Removing
*/

void GuiListBox::ClearItems() noexcept
{
	items_.clear();
	items_.shrink_to_fit();

	if (item_index_)
	{
		item_index_ = {};
		ItemDeselected();
	}

	UpdateItems();
}

void GuiListBox::RemoveItem(int off) noexcept
{
	RemoveItems(off, off + 1);
}

void GuiListBox::RemoveItems(int first, int last) noexcept
{
	if (first >= 0 && first < last)
	{
		last = std::clamp(last, first, std::ssize(items_));
		items_.erase(std::begin(items_) + first, std::begin(items_) + last);

		if (item_index_)
		{
			if (*item_index_ >= first && *item_index_ < last)
			{
				item_index_ = {}; //Deselect
				ItemDeselected();
			}
			else if (*item_index_ >= last)
				*item_index_ -= last - first;
		}

		UpdateItems();
	}
}


/*
	Key events
*/

bool GuiListBox::KeyReleased(KeyButton button) noexcept
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