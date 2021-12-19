/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiSlider.cpp
-------------------------------------------
*/

#include "IonGuiSlider.h"

#include <algorithm>

#include "graphics/scene/IonModel.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "utilities/IonMath.h"

namespace ion::gui::controls
{

using namespace gui_slider;
using namespace types::type_literals;

namespace gui_slider::detail
{

void resize_skin(SliderSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;
	auto delta_position = delta_size * 0.5_r;

	if (skin.Handle)
	{
		auto &center = skin.Handle->Position();
		gui_control::detail::resize_part(skin.Handle, delta_size, delta_position, center);
	}
}

} //gui_slider::detail


//Private

void GuiSlider::DefaultSetup() noexcept
{
	CaptionLayout(gui_control::ControlCaptionLayout::OutsideTopCenter);
}


//Protected

/*
	Events
*/

void GuiSlider::Resized(Vector2 from_size, Vector2 to_size) noexcept
{
	if (skin_)
	{
		if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
		{
			auto [width, height] = skin.Handle->Size().XY();
			auto delta_size = to_size - from_size;

			auto aspect_ratio =
				type_ == SliderType::Vertical ?
				height / width :
				width / height;

			//Handle should only resize to keep proportions
			auto handle_new_size =
				type_ == SliderType::Vertical ?
				Vector2{width + delta_size.X(), (width + delta_size.X()) * aspect_ratio} :
				Vector2{(height + delta_size.Y()) * aspect_ratio, height + delta_size.Y()};

			detail::resize_skin(skin, skin.Handle->Size(), handle_new_size);
		}
	}

	GuiControl::Resized(from_size, to_size); //Use base functionality
	UpdateHandle();
}


/*
	States
*/

void GuiSlider::SetSkinState(gui_control::ControlState state, SliderSkin &skin) noexcept
{
	if (skin.Handle)
		SetPartState(state, skin.Handle);
}

void GuiSlider::SetState(gui_control::ControlState state) noexcept
{
	GuiControl::SetState(state); //Use base functionality

	if (visible_ && skin_)
		SetSkinState(state, static_cast<SliderSkin&>(*skin_));
}


/*
	Skins
*/

void GuiSlider::FlipHandle() noexcept
{
	if (auto size = Size(); size && skin_)
	{
		auto [width, height] = size->XY();
		Resized(*size, {height, width});

		if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
		{
			auto [handle_width, handle_height] = skin.Handle->Size().XY();
			detail::resize_skin(skin, skin.Handle->Size(), {handle_height, handle_width});
		}
	}
}

void GuiSlider::UpdateHandle() noexcept
{
	using namespace utilities;

	if (skin_)
	{
		if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
		{
			//Set handle position
			if (auto area = InnerArea(); area)
			{
				auto [min, max] =
					type_ == SliderType::Vertical ?
					std::pair{area->Min().Y(), area->Max().Y()} :
					std::pair{area->Min().X(), area->Max().X()};

				auto handle_half_size =
					(type_ == SliderType::Vertical ?
					skin.Handle->Size().Y() :
					skin.Handle->Size().X()) * 0.5_r;
				
				auto handle_pos =
					flipped_ ?
					math::Lerp(max - handle_half_size, min + handle_half_size, Percent()) :
					math::Lerp(min + handle_half_size, max - handle_half_size, Percent());

				skin.Handle->Position(
					type_ == SliderType::Vertical ?
					Vector2{0.0_r, handle_pos} :
					Vector2{handle_pos, 0.0_r});
			}
		}
	}
}


//Public

GuiSlider::GuiSlider(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<SliderSkin> skin) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin)}
{
	DefaultSetup();
}

GuiSlider::GuiSlider(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<SliderSkin> skin, const Vector2 &size) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size}
{
	DefaultSetup();
}

GuiSlider::GuiSlider(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<SliderSkin> skin, gui_control::Areas areas) :
	GuiControl{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(areas)}
{
	DefaultSetup();
}


/*
	Modifiers
*/

void GuiSlider::Percent(real percent) noexcept
{
	using namespace utilities;
	Position(static_cast<int>(
		math::Round(math::Lerp(
			static_cast<real>(progress_.Min()),
			static_cast<real>(progress_.Max()),
			percent))
		));
}


/*
	Intersecting
*/

bool GuiSlider::IntersectsHandle(const Vector2 &point) const noexcept
{
	if (node_ && visible_ && skin_)
	{
		if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
		{
			if (auto node = skin.Parts->ParentNode(); node)
			{
				skin.Handle->Prepare();

				//Check for intersection
				if (skin.Handle->AxisAlignedBoundingBox().TransformCopy(node->FullTransformation()).Intersects(point))
					return node->AxisAligned() ||
						skin.Handle->OrientedBoundingBox().TransformCopy(node->FullTransformation()).Intersects(point);
			}
		}
	}

	return false;
}


/*
	Key events
*/

bool GuiSlider::KeyReleased([[maybe_unused]] KeyButton button) noexcept
{
	if (auto [min, max] = Range(); min != max)
	{
		if (flipped_)
			button = detail::flip_arrow_keys(button);

		switch (button)
		{
			//Increase
			case KeyButton::UpArrow:
			case KeyButton::RightArrow:
			{
				if (Position() < max)
				{
					Position(Position() + step_by_amount_);
					Changed();
				}

				return true;
			}

			//Decrease
			case KeyButton::DownArrow:
			case KeyButton::LeftArrow:
			{
				if (Position() > min)
				{
					Position(Position() - step_by_amount_);
					Changed();
				}

				return true;
			}
		}
	}

	return false;
}


/*
	Mouse events
*/

bool GuiSlider::MousePressed(MouseButton button, Vector2 position) noexcept
{
	if (button == MouseButton::Left)
		dragged_ = IntersectsHandle(position);

	return false;
}

bool GuiSlider::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	if (button == MouseButton::Left)
	{
		if (dragged_)
			dragged_ = false;

		else if (auto [min, max] = Range(); min != max && skin_)
		{
			if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
			{
				//Make position relative to handle
				if (auto node = skin.Parts->ParentNode(); node)
				{
					auto handle_position =
						skin.Handle->Position() * node->DerivedScaling();
					position -= node->DerivedPosition();

					auto delta_position =
						(type_ == SliderType::Horizontal && position.X() < handle_position.X()) ||
						(type_ == SliderType::Vertical && position.Y() < handle_position.Y()) ?
						-step_by_amount_ : step_by_amount_;

					if (flipped_)
						Position(Position() - delta_position);
					else
						Position(Position() + delta_position);
				}
			}
		}
	}

	return false;
}

bool GuiSlider::MouseMoved(Vector2 position) noexcept
{
	if (auto [min, max] = Range(); min != max && dragged_ && skin_)
	{
		if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
		{
			//Make position relative to handle
			if (auto node = skin.Parts->ParentNode(); node)
			{
				//Set handle position
				if (auto size = InnerSize(); size)
				{
					size = (*size - skin.Handle->Size()) * node->DerivedScaling();
					position -= node->DerivedPosition();

					auto percent =
						type_ == SliderType::Vertical ?
						(position.Y() + size->Y() * 0.5_r) / size->Y() :
						(position.X() + size->X() * 0.5_r) / size->X();

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

bool GuiSlider::MouseWheelRolled(int delta, [[maybe_unused]] Vector2 position) noexcept
{
	//Up or down
	if (delta != 0)
		return KeyReleased(delta > 0 ? KeyButton::UpArrow : KeyButton::DownArrow);
	else
		return false;
}

} //ion::gui::controls