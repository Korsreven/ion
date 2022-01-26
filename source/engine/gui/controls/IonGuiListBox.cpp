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
#include "utilities/IonMath.h"
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
			gui_control::detail::resize_part(*icon, delta_size, delta_position, vector2::Zero);
	}
}


/*
	Items/lines
*/

void trim_item(gui_list_box::ListBoxItem &item) noexcept
{
	using namespace utilities;
	string::ReplaceAll(item.Content, "<br>", "\n");
	string::RemoveNonPrintable(item.Content);
}

void trim_items(gui_list_box::ListBoxItems &items) noexcept
{
	for (auto &item : items)
		trim_item(item);
}

std::string items_to_text_content(const gui_list_box::ListBoxItems &items)
{
	auto content = items.front().Content;

	for (auto iter = std::begin(items) + 1, end = std::end(items); iter != end; ++iter)
		content += "\n" + iter->Content;

	return content;
}


Vector2 lines_area_offset(ListBoxIconLayout icon_layout, const Vector2 &icon_column_size) noexcept
{
	auto half_size = icon_column_size * 0.5_r;

	switch (icon_layout)
	{
		case ListBoxIconLayout::Left:
		return {half_size.X(), 0.0_r};

		case ListBoxIconLayout::Right:
		return {-half_size.X(), 0.0_r};
		
		default:
		return {0.0_r, 0.0_r};
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
	static auto do_scroll = true;

	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines && do_scroll)
	{
		//Lines text
		if (auto &lines = skin->Lines->Get(); lines)
		{
			auto from_line = lines->FromLine();

			//Scrolled up
			if (delta < 0)
			{
				lines->FromLine(
					from_line > -delta ?
					from_line + delta : 0);
			}
			//Scrolled down
			else if (delta > 0)
			{
				auto line_count = lines->LineCount();
				auto displayed_line_count = lines->DisplayedLineCount();
				auto max_from_line = 
					line_count > displayed_line_count ?
					line_count - displayed_line_count : 0;

				lines->FromLine(
					from_line + delta < max_from_line ?
					from_line + delta : max_from_line);
			}

			UpdateIcons();
			UpdateSelection();

			do_scroll = false;
			UpdateScrollBar();
			do_scroll = true;
		}
	}
}

int GuiListBox::TotalElements() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines && skin->Lines->GetImmutable())
		return skin->Lines->GetImmutable()->LineCount();
	else
		return 0;
}

int GuiListBox::ElementsInView() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines && skin->Lines->GetImmutable())
		return skin->Lines->GetImmutable()->DisplayedLineCount();
	else
		return 0;
}

int GuiListBox::ScrollPosition() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines && skin->Lines->GetImmutable())
		return skin->Lines->GetImmutable()->FromLine();
	else
		return 0;
}


void GuiListBox::ItemSelected() noexcept
{
	Changed();
}

void GuiListBox::ItemDeselected() noexcept
{
	//Optional to override
}


/*
	States
*/

void GuiListBox::SetSkinState(gui_control::ControlState state, ListBoxSkin &skin) noexcept
{
	if (skin.Lines)
		SetCaptionState(state, skin.Lines);

	if (skin.Selection)
	{
		if (item_index_)
			SetPartState(state, skin.Selection);
		else
			skin.Selection->Visible(false);
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

	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin && skin_node_)
	{
		if (skin->Lines)
		{
			//Detach from previous parent (if any)
			if (auto node = skin->Lines->ParentNode(); node)
				node->DetachObject(*skin->Lines.TextObject);
		
			//Attach lines text
			skin_node_->AttachObject(*skin->Lines.TextObject);
		}
	}
}

void GuiListBox::DetachSkin() noexcept
{
	GuiControl::DetachSkin(); //Use base functionality
}

void GuiListBox::RemoveSkin() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin)
	{
		DetachSkin();

		if (skin->Lines && skin->Lines->Owner())
			skin->Lines->Owner()->RemoveText(*skin->Lines.TextObject); //Remove lines text
	}

	GuiControl::RemoveSkin(); //Use base functionality
}


void GuiListBox::UpdateLines() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Lines text
		if (auto &text = skin->Lines->Get(); text)
		{
			if (auto size = InnerSize(); size)
			{
				auto center = CenterArea().value_or(aabb::Zero).Center();

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

				auto icon_column_size = show_icons_ ?
					*size * Vector2{icon_column_width_.value_or(detail::default_icon_column_width_percent), 0.0_r} :
					vector2::Zero;

				text->Overflow(graphics::fonts::text::TextOverflow::WordTruncate);
				text->AreaSize((*size - icon_column_size) * ortho_viewport_ratio);
				text->LineHeightFactor(item_height_factor_.value_or(detail::default_item_height_factor));
				text->Padding(item_padding_.value_or(detail::default_item_padding_size));
				text->Alignment(detail::item_layout_to_text_alignment(item_layout_));

				//Refresh content
				if (text->LineCount() != std::ssize(items_))
					text->Content(detail::items_to_text_content(items_));

				skin->Lines->Position(center + detail::lines_area_offset(icon_layout_, icon_column_size));
			}
		}
	}

	UpdateIcons();
	UpdateSelection();
}

void GuiListBox::UpdateIcons() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Lines text
		if (auto &text = skin->Lines->GetImmutable();
			text && text->LineHeight())
		{
			if (auto size = InnerSize(); size)
			{
				auto [width, height] = size->XY();
				auto center = CenterArea().value_or(aabb::Zero).Center();

				auto viewport_ortho_ratio =
					[&]() noexcept
					{
						//Adjust area size from ortho to viewport space
						if (auto scene_manager = skin->Lines->Owner(); scene_manager)
						{
							if (auto viewport = scene_manager->ConnectedViewport(); viewport)
								return viewport->ViewportToOrthoRatio();
						}

						return vector2::UnitScale;
					}();

				auto item_height = *text->LineHeight() * viewport_ortho_ratio.Y();
				auto item_padding = text->Padding().Y() * viewport_ortho_ratio.Y();
				auto icon_padding = icon_padding_.value_or(detail::default_icon_padding_size) * viewport_ortho_ratio.Y();

				auto icon_column_percent = icon_column_width_.value_or(detail::default_icon_column_width_percent);
				auto icon_column_width = width * icon_column_percent;
				auto icon_max_size = Vector2{icon_column_width, item_height} - icon_padding * 2.0_r;
				icon_max_size.Floor(icon_max_size_.value_or(icon_max_size)).Ceil(vector2::Zero);

				auto item_count = show_icons_ ? text->DisplayedLineCount() : 0;

				//Show all icons in view
				for (auto item_off = text->FromLine(), icon_off = 0;
					icon_off < item_count; ++item_off, ++icon_off)
				{
					//Create icon sprite (if missing)
					if (icon_off == std::ssize(skin->Icons))
						skin->Icons.push_back(CreateIcon(items_[item_off].Icon));
					else if (!skin->Icons[icon_off])
						skin->Icons[icon_off] = CreateIcon(items_[item_off].Icon);
					else
						skin->Icons[icon_off]->SurfaceMaterial(items_[item_off].Icon);

					//Item has icon
					if (items_[item_off].Icon)
					{
						skin->Icons[icon_off]->AutoSize(true);
						skin->Icons[icon_off]->ResizeToFit(icon_max_size);
						
						skin->Icons[icon_off]->Position({
							icon_layout_ == ListBoxIconLayout::Right ?
							center.X() + width * 0.5_r - icon_column_width * 0.5_r : //Right
							center.X() - width * 0.5_r + icon_column_width * 0.5_r,  //Left
							height * 0.5_r - item_padding - item_height * 0.5_r - item_height * icon_off,
							skin->Icons[icon_off]->Position().Z()}
						);
					}
					
					skin->Icons[icon_off]->Visible(!!items_[item_off].Icon);
				}

				//Hide all icons out of view
				for (auto iter = std::begin(skin->Icons) + item_count,
					end = std::end(skin->Icons); iter != end; ++iter)
				{
					if (*iter)
						(*iter)->Visible(false);
				}
			}
		}
	}
}

void GuiListBox::UpdateSelection() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Selection)
	{
		auto show_selection = false;

		if (skin->Lines && item_index_)
		{
			//Lines text
			if (auto &text = skin->Lines->GetImmutable();
				text && text->LineHeight())
			{
				//Selected item is in view
				if (auto item_off = *item_index_ - text->FromLine();
					item_off >= 0 && item_off < text->DisplayedLineCount())
				{
					if (auto size = InnerSize(); size)
					{
						auto [width, height] = size->XY();
						auto center = CenterArea().value_or(aabb::Zero).Center();

						auto viewport_ortho_ratio =
							[&]() noexcept
							{
								//Adjust area size from ortho to viewport space
								if (auto scene_manager = skin->Lines->Owner(); scene_manager)
								{
									if (auto viewport = scene_manager->ConnectedViewport(); viewport)
										return viewport->ViewportToOrthoRatio();
								}

								return vector2::UnitScale;
							}();

						auto item_height = *text->LineHeight() * viewport_ortho_ratio.Y();
						auto item_padding = text->Padding().Y() * viewport_ortho_ratio.Y();
						auto item_selection_padding = selection_padding_.
							value_or(detail::default_selection_padding_size) * viewport_ortho_ratio;

						skin->Selection->Size(
							(Vector2{width, item_height} - item_selection_padding * 2.0_r).
							CeilCopy(vector2::Zero)
						);
						skin->Selection->Position({
							center.X(),
							height * 0.5_r - item_padding - item_height * 0.5_r - item_height * item_off,
							skin->Selection->Position().Z()}
						);
						
						show_selection = true;
					}
				}
			}
		}
		
		skin->Selection->Visible(show_selection);
	}
}


/*
	Lines
*/

void GuiListBox::InsertLines(int off, const gui_list_box::ListBoxItems &items)
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Lines text
		if (auto &text = skin->Lines->Get(); text)
			text->InsertLine(off, detail::items_to_text_content(items));
	}
}

void GuiListBox::ReplaceLines(int first, int last, const gui_list_box::ListBoxItems &items)
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Lines text
		if (auto &text = skin->Lines->Get(); text)
		{
			text->ReplaceLines(first, last, detail::items_to_text_content(items));

			auto count = text->LineCount();
			auto view_count = text->DisplayedLineCount();
			auto view_capacity = text->DisplayedLineCapacity().value_or(0);

			if (count > view_count && view_count < view_capacity)
				Scrolled(view_count - view_capacity);
		}
	}
}

void GuiListBox::RemoveLines(int first, int last) noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines)
	{
		//Lines text
		if (auto &text = skin->Lines->Get(); text)
		{
			text->RemoveLines(first, last);

			auto count = text->LineCount();
			auto view_count = text->DisplayedLineCount();
			auto view_capacity = text->DisplayedLineCapacity().value_or(0);

			if (count > view_count && view_count < view_capacity)
				Scrolled(view_count - view_capacity);
		}
	}
}

void GuiListBox::ClearLines() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
		skin && skin->Lines && skin->Lines->Get())
	{
		//Lines text
		if (auto &text = skin->Lines->Get(); text)
		{
			text->Clear();
			text->FromLine(0);
		}
	}
}


/*
	Icons
*/

NonOwningPtr<graphics::scene::shapes::Sprite> GuiListBox::CreateIcon(NonOwningPtr<graphics::materials::Material> material)
{
	if (skin_)
	{
		auto sprite = skin_->Parts->CreateMesh<graphics::scene::shapes::Sprite>(vector2::Zero, material);
		sprite->IncludeBoundingVolumes(false);
		return sprite;
	}
	else
		return nullptr;
}

void GuiListBox::RemoveIcons() noexcept
{
	if (auto skin = static_cast<ListBoxSkin*>(skin_.get()); skin)
	{
		for (auto &icon : skin->Icons)
		{
			if (icon)
				icon->Owner()->RemoveMesh(*icon); //Remove icon
		}

		skin->Icons.clear();
		skin->Icons.shrink_to_fit();
	}
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
	Modifiers
*/

void GuiListBox::ItemIndex(std::optional<int> index) noexcept
{
	if (index)
	{
		if (*index < 0 || std::empty(items_))
			index = {};
		else
			index = std::clamp(*index, 0, std::ssize(items_) - 1);
	}

	if (item_index_ != index)
	{
		//Go from or to an empty selection
		auto empty_selection =
			!item_index_ || !index;

		item_index_ = index;

		if (item_index_)
			ItemSelected();
		else
			ItemDeselected();

		if (empty_selection)
			SetState(state_);

		UpdateSelection();
	}
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
		detail::trim_items(items);

		if (!std::empty(items))
		{
			off = std::clamp(off, 0, std::ssize(items_));
			items_.insert(std::begin(items_) + off, std::begin(items), std::end(items));

			//Adjust item index
			if (item_index_ && *item_index_ >= off)
				ItemIndex(*item_index_ + std::ssize(items));

			InsertLines(off, items);
			UpdateLines();
		}
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
		detail::trim_items(items);

		last = std::clamp(last, first, std::ssize(items_));
		items_.erase(std::begin(items_) + first, std::begin(items_) + last);
		items_.insert(std::begin(items_) + first, std::begin(items), std::end(items));

		//Adjust item index
		if (item_index_)
		{
			if (*item_index_ >= first && *item_index_ < last &&
				*item_index_ - first >= std::ssize(items))
				ItemIndex({}); //Deselect
			else if (*item_index_ >= last)
				ItemIndex(*item_index_ + (std::ssize(items) - (last - first)));
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
	if (!std::empty(items_))
	{
		items_.clear();

		if (item_index_)
			ItemIndex({}); //Deselect

		ClearLines();
		RemoveIcons();
		UpdateLines();
	}

	items_.shrink_to_fit();
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
				ItemIndex({}); //Deselect
			else if (*item_index_ >= last)
				ItemIndex(*item_index_ - (last - first));
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
	auto update_selection =
		[&]() noexcept
		{
			switch (button)
			{
				//Move selection up
				case KeyButton::UpArrow:
				case KeyButton::LeftArrow:
				{
					if (!item_index_)
						ItemIndex(0);
					else if (*item_index_ > 0)
						ItemIndex(*item_index_ - 1);

					return true;
				}

				//Move selection down
				case KeyButton::DownArrow:
				case KeyButton::RightArrow:
				{
					if (!item_index_)
						ItemIndex(0);
					else if (*item_index_ + 1 < std::ssize(items_))
						ItemIndex(*item_index_ + 1);

					return true;
				}

				//Move selection to top
				case KeyButton::Home:
				{
					if (!std::empty(items_))
						ItemIndex(0);

					return true;
				}

				//Move selection to bottom
				case KeyButton::End:
				{
					if (!std::empty(items_))
						ItemIndex(std::ssize(items_) - 1);

					return true;
				}

				default:
				return false;
			}
		}();

	if (update_selection)
	{
		if (item_index_)
		{
			auto first = ScrollPosition();
			auto view_count = ElementsInView();

			if (*item_index_ < first)
				Scrolled(*item_index_ - first);
			else if (*item_index_ > first + view_count - 1)
				Scrolled(*item_index_ - (first + view_count - 1));
		}

		return true;
	}
	
	return GuiScrollable::KeyReleased(button); //Use base functionality
}


/*
	Mouse events
*/

bool GuiListBox::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	using namespace utilities;

	if (button == MouseButton::Left)
	{
		if (auto skin = static_cast<ListBoxSkin*>(skin_.get());
			skin && skin->Lines && skin->Selection)
		{
			//Lines text
			if (auto &text = skin->Lines->GetImmutable();
				text && text->LineHeight())
			{
				if (auto size = InnerSize(); size)
				{
					auto [width, height] = size->XY();
					auto center = CenterArea().value_or(aabb::Zero).Center();

					auto viewport_ortho_ratio =
						[&]() noexcept
						{
							//Adjust area size from ortho to viewport space
							if (auto scene_manager = skin->Lines->Owner(); scene_manager)
							{
								if (auto viewport = scene_manager->ConnectedViewport(); viewport)
									return viewport->ViewportToOrthoRatio();
							}

							return vector2::UnitScale;
						}();

					auto item_height = *text->LineHeight() * viewport_ortho_ratio.Y();
					auto item_padding = text->Padding().Y() * viewport_ortho_ratio.Y();
					
					if (skin_node_)
					{
						position = //Make position relative to lines
							(position - (skin_node_->DerivedPosition() + center)).
							RotateCopy(-skin_node_->DerivedRotation(), vector2::Zero);

						height *= skin_node_->DerivedScaling().Y();
						item_height *= skin_node_->DerivedScaling().Y();
						item_padding *= skin_node_->DerivedScaling().Y();
					}

					auto y = -position.Y() + height * 0.5_r - item_padding - item_height * 0.5_r;
					auto clicked_item_off = static_cast<int>(math::Round(y / item_height)) + text->FromLine();

					//Clicked item is in view
					if (auto item_off = clicked_item_off - text->FromLine();
						item_off >= 0 && item_off < text->DisplayedLineCount())

						ItemIndex(clicked_item_off); //Select item
				}
			}
		}
	}

	return false;
}

} //ion::gui::controls