/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiScrollBar.cpp
-------------------------------------------
*/

#include "IonGuiScrollBar.h"

#include <algorithm>

#include "IonGuiScrollable.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "gui/IonGuiPanelContainer.h"

namespace ion::gui::controls
{

using namespace gui_scroll_bar;

namespace gui_scroll_bar::detail
{
} //gui_scroll_bar::detail


//Private

void GuiScrollBar::DefaultSetup() noexcept
{
	flipped_ =
		type_ == gui_slider::SliderType::Vertical;
}


//Protected

/*
	Events
*/

void GuiScrollBar::Slid(int delta) noexcept
{
	if (scrollable_)
		scrollable_->Scroll(delta);

	GuiSlider::Slid(delta); //Use base functionality
}


/*
	Skins
*/

OwningPtr<gui_control::ControlSkin> GuiScrollBar::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<ScrollBarSkin*>(skin.get()))
	{
		auto scroll_bar_skin = make_owning<ScrollBarSkin>();
		scroll_bar_skin->Assign(*skin);
		return scroll_bar_skin;
	}
	else
		return skin;
}

void GuiScrollBar::UpdateHandle() noexcept
{
	if (auto [min, max] = Range(); min != max && skin_)
	{
		if (auto &skin = static_cast<ScrollBarSkin&>(*skin_); skin.Handle)
		{
			//Set handle size
			if (auto size = ContentSize(); size)
			{
				auto [width, height] = size->XY();
				auto [handle_width, handle_height] = skin.Handle->Size().XY();

				auto count = scrollable_ ? scrollable_->TotalElements() : max - min;
				auto view_count = scrollable_ ? scrollable_->ElementsInView() : 1;

				if (type_ == gui_slider::SliderType::Vertical)
				{
					handle_height = static_cast<real>(view_count) / count * height;
					handle_height = std::clamp(handle_height, height * handle_size_.first, height * handle_size_.second);
				}
				else
				{
					handle_width = static_cast<real>(view_count) / count * width;
					handle_width = std::clamp(handle_width, width * handle_size_.first, width * handle_size_.second);
				}

				if (auto handle_size = Vector2{handle_width, handle_height};
					handle_size != skin.Handle->Size())
					gui_slider::detail::resize_handle(skin, skin.Handle->Size(), handle_size);
			}
		}
	}

	GuiSlider::UpdateHandle(); //Use base functionality
}


//Public

GuiScrollBar::GuiScrollBar(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, gui_slider::SliderType type, gui_control::BoundingBoxes hit_boxes) noexcept :
	GuiSlider{std::move(name), size, std::move(caption), {}, type, std::move(hit_boxes)}
{
	DefaultSetup();
}

GuiScrollBar::GuiScrollBar(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, gui_slider::SliderType type, gui_control::BoundingBoxes hit_boxes) :
	GuiSlider{std::move(name), skin, size, std::move(caption), {}, type, std::move(hit_boxes)}
{
	DefaultSetup();
}


GuiScrollBar::~GuiScrollBar() noexcept
{
	AttachedScrollable(nullptr); //Detach (if any)
}


/*
	Scrollable
*/

void GuiScrollBar::AttachedScrollable(NonOwningPtr<GuiScrollable> scrollable) noexcept
{
	if (scrollable_ != scrollable)
	{
		//Attach
		if (scrollable)
		{
			AttachedScrollable(nullptr); //Detach previous (if any)
			scrollable_ = scrollable;
			Focusable(false);

			if (auto owner = Owner(); owner)
				scrollable->AttachedScrollBar(
					owner->GetControlAs<GuiScrollBar>(*Name())
				);
		}
		
		else //Detach
		{
			auto ptr = scrollable_.get();
			scrollable_ = nullptr;
			Focusable(true);

			if (ptr)
				ptr->AttachedScrollBar(nullptr);
		}

		UpdateHandle();
	}
}


/*
	Mouse events
*/

bool GuiScrollBar::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	if (button == MouseButton::Left)
	{
		if (dragged_)
			dragged_ = false;

		else if (auto [min, max] = Range(); min != max && skin_)
		{
			if (auto &skin = static_cast<ScrollBarSkin&>(*skin_); skin.Handle)
			{
				if (auto size = ContentSize(); size)
				{
					auto handle_size = skin.Handle->Size();
					auto handle_position = skin.Handle->Position();

					if (skin_node_)
					{
						position = //Make position relative to skin
							(position - skin_node_->DerivedPosition()).
							RotateCopy(-skin_node_->DerivedRotation(), vector2::Zero);
						size = (*size - skin.Handle->Size()) * skin_node_->DerivedScaling();

						handle_size *= skin_node_->DerivedScaling();
						handle_position *= skin_node_->DerivedScaling();
					}

					handle_position +=
						(type_ == gui_slider::SliderType::Horizontal && position.X() < handle_position.X()) ||
						(type_ == gui_slider::SliderType::Vertical && position.Y() < handle_position.Y()) ?
						-handle_size : handle_size;

					auto percent =
						type_ == gui_slider::SliderType::Vertical ?
						(handle_position.Y() + size->Y() * 0.5_r) / size->Y() :
						(handle_position.X() + size->X() * 0.5_r) / size->X();

					if (flipped_)
						Percent(1.0_r - percent);
					else
						Percent(percent);
				}
			}
		}
	}

	return false;
}

} //ion::gui::controls