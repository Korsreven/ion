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
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "utilities/IonStringUtility.h"

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


/*
	Lines
*/

std::string item_content_to_text_content(const gui_list_box::ListBoxItems &items)
{
	using namespace utilities;
	auto content = items.front().Content;

	for (auto iter = std::begin(items) + 1, end = std::end(items); iter != end; ++iter)
		content += "\n" + string::RemoveNonPrintableCopy(iter->Content);

	return content;
}


Vector2 lines_offset(ListBoxItemAlignment item_alignment, const Vector2 &size, const Vector2 &border_size) noexcept
{
	auto half_size = size * 0.5_r;

	switch (item_alignment)
	{
		case ListBoxItemAlignment::Left:
		return {-half_size.X() - border_size.X(), 0.0_r};

		case ListBoxItemAlignment::Right:
		return {half_size.X() + border_size.X(), 0.0_r};

		case ListBoxItemAlignment::Center:
		default:
		return vector2::Zero;
	}
}

Vector2 lines_area_offset(ListBoxItemAlignment item_alignment, const Vector2 &size, const Vector2 &border_size) noexcept
{
	switch (item_alignment)
	{
		case ListBoxItemAlignment::Left:
		return {-size.X() - border_size.X(), 0.0_r};

		case ListBoxItemAlignment::Right:
		return {size.X() + border_size.X(), 0.0_r};

		case ListBoxItemAlignment::Center:
		default:
		return vector2::Zero;
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
		UpdateLines();
	}

	GuiControl::Resized(from_size, to_size); //Use base functionality
}


void GuiListBox::Scrolled(int delta) noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Lines text
		if (auto &lines = skin->Lines->Get(); lines)
		{
			auto from_line = lines->FromLine();

			//Scrolled up
			if (delta < 0)
			{
				lines->FromLine(
					from_line > delta ?
					from_line + delta : 0);
			}
			//Scrolled down
			else if (delta > 0)
			{
				auto line_count = lines->LineCount();
				auto displayed_line_capacity = lines->DisplayedLineCapacity().value_or(line_count);
				auto max_from_line = 
					line_count > displayed_line_capacity ?
					line_count - displayed_line_capacity : 0;

				lines->FromLine(
					from_line + delta < max_from_line ?
					from_line + delta : max_from_line);
			}

			UpdateScrollBar();
		}
	}
}

int GuiListBox::TotalElements() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Get() will not reload vertex streams when called from an immutable reference
		if (const auto &c_part = *skin->Lines.TextObject; c_part.Get())
			return c_part.Get()->LineCount();
	}

	return 0;
}

int GuiListBox::ElementsInView() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Get() will not reload vertex streams when called from an immutable reference
		if (const auto &c_part = *skin->Lines.TextObject; c_part.Get())
			return c_part.Get()->DisplayedLineCapacity().value_or(c_part.Get()->LineCount());
	}

	return 0;
}


int GuiListBox::ScrollPosition() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Get() will not reload vertex streams when called from an immutable reference
		if (const auto &c_part = *skin->Lines.TextObject; c_part.Get())
			return c_part.Get()->FromLine();
	}

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

void GuiListBox::AttachSkin()
{
	GuiControl::AttachSkin(); //Use base functionality

	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin)
	{
		if (skin->Lines)
		{
			if (auto node = skin->Lines->ParentNode(); node)
				node->DetachObject(*skin->Lines.TextObject);
		
			if (node_) //Create node for lines
				node_->CreateChildNode(node_->Visible())->AttachObject(*skin->Lines.TextObject);
		}
	}
}

void GuiListBox::DetachSkin() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin)
	{
		if (skin->Lines)
		{
			if (auto node = skin->Lines->ParentNode(); node_ && node)
				node_->RemoveChildNode(*node); //Remove lines node
		}
	}

	GuiControl::DetachSkin(); //Use base functionality
}

void GuiListBox::RemoveSkin() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin)
	{
		DetachSkin();

		if (skin->Lines)
		{
			skin->Lines->Owner()->RemoveText(*skin->Lines.TextObject); //Remove lines
			skin->Lines = {};
		}
	}

	GuiControl::RemoveSkin(); //Use base functionality
}


void GuiListBox::UpdateLines() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Lines text
		if (auto &lines = skin->Lines->Get(); lines)
		{
			auto area_size = gui_control::detail::get_inner_size(*skin_, false).value_or(vector2::Zero);
			auto border_size = gui_control::detail::get_border_size(*skin_, false).value_or(vector2::Zero);
			auto center = gui_control::detail::get_center_area(*skin_, false).value_or(aabb::Zero).Center();

			//Area size
			if (area_size != vector2::Zero)
			{
				auto ortho_viewport_ratio =
					[&]() noexcept
					{
						//Adjust area size from ortho to viewport space
						if (auto scene_manager = skin->Lines->Owner(); scene_manager)
						{
							if (auto viewport = scene_manager->ConnectedViewport(); viewport)
								return viewport->OrthoToViewportRatio();
						}

						return vector2::UnitScale;
					}();

				lines->Overflow(graphics::fonts::text::TextOverflow::WordTruncate);
				lines->AreaSize(area_size * ortho_viewport_ratio);

				//if (auto node = skin->Lines->ParentNode(); node)
				//	node->Position(center + detail::lines_area_offset(item_alignment_, area_size, border_size));
			}
			else
			{
				/*auto viewport_ortho_ratio =
					[&]() noexcept
					{
						//Adjust area size from ortho to viewport space
						if (auto scene_manager = skin->Lines->Owner(); scene_manager)
						{
							if (auto viewport = scene_manager->ConnectedViewport(); viewport)
								return viewport->ViewportToOrthoRatio();
						}

						return vector2::UnitScale;
					}();*/

				lines->AreaSize({});

				if (auto node = skin->Lines->ParentNode(); node)
					node->Position(center + detail::lines_offset(item_alignment_, area_size, border_size));
			}

			//Line height factor
			lines->LineHeightFactor(item_height_factor_.value_or(detail::default_item_height_factor));

			//Line height factor
			lines->Padding(item_padding_.value_or(detail::default_item_padding_size));

			//Alignment
			lines->Alignment(detail::item_alignment_to_text_alignment(item_alignment_));

			//Content
			if (std::size(lines->FormattedLines()) != std::size(items_))
				lines->Content(detail::item_content_to_text_content(items_));
		}
	}

	UpdateSelection();
}

void GuiListBox::UpdateSelection() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Selection && item_index_)
	{
		//Todo
	}
}


/*
	Lines
*/

void GuiListBox::InsertLines(int off, const gui_list_box::ListBoxItems &items)
{
	using namespace utilities;

	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin && skin->Lines)
		skin->Lines->Get()->InsertLine(off, detail::item_content_to_text_content(items));
}

void GuiListBox::ReplaceLines(int first, int last, const gui_list_box::ListBoxItems &items)
{
	using namespace utilities;

	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin && skin->Lines)
		skin->Lines->Get()->ReplaceLines(first, last, detail::item_content_to_text_content(items));
}

void GuiListBox::RemoveLines(int first, int last) noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin && skin->Lines)
		skin->Lines->Get()->RemoveLines(first, last);
}

void GuiListBox::ClearLines() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin && skin->Lines)
		skin->Lines->Get()->Clear();
}


//Public

GuiListBox::GuiListBox(std::string name, std::optional<std::string> caption,
	OwningPtr<ListBoxSkin> skin, gui_control::BoundingBoxes hit_boxes) :
	GuiScrollable{std::move(name), std::move(caption), {}, std::move(skin), std::move(hit_boxes)}
{
	DefaultSetup();
}

GuiListBox::GuiListBox(std::string name, std::optional<std::string> caption,
	OwningPtr<ListBoxSkin> skin, const Vector2 &size, gui_control::BoundingBoxes hit_boxes) :
	GuiScrollable{std::move(name), std::move(caption), {}, std::move(skin), size, std::move(hit_boxes)}
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

		//Adjust item index
		if (item_index_ && *item_index_ >= off)
			*item_index_ += std::ssize(items);

		InsertLines(off, items);
		UpdateLines();
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
		last = std::clamp(last, first, std::ssize(items_));
		items_.erase(std::begin(items_) + first, std::begin(items_) + last);
		items_.insert(std::begin(items_) + first, std::begin(items), std::end(items));

		//Adjust item index
		if (item_index_)
		{
			if (*item_index_ >= first && *item_index_ < last &&
				*item_index_ - first >= std::ssize(items))
			{
				item_index_ = {}; //Deselect
				ItemDeselected();
			}
			else if (*item_index_ >= last)
				*item_index_ += std::ssize(items) - (last - first);
		}
		
		ReplaceLines(first, last, items);
		UpdateLines();
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

	ClearLines();
	UpdateLines();
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

		//Adjust item index
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

		RemoveLines(first, last);
		UpdateLines();
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

} //ion::gui::controls