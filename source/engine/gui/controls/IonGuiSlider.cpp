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

/*
	Skins
*/

void resize_handle(SliderSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;
	auto delta_position = delta_size * 0.5_r;

	if (skin.Handle)
	{
		auto &center = skin.Handle->Position();
		gui_control::detail::resize_part(skin.Handle, delta_size, delta_position, center);
	}
}

void resize_skin(SliderSkin &skin, SliderType type, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;

	if (skin.Handle)
	{
		auto [width, height] = skin.Handle->Size().XY();

		auto aspect_ratio =
			type == SliderType::Vertical ?
			height / width :
			width / height;

		//Handle should keep proportions when resized
		auto handle_new_size =
			type == SliderType::Vertical ?
			Vector2{width + delta_size.X(), (width + delta_size.X()) * aspect_ratio} :
			Vector2{(height + delta_size.Y()) * aspect_ratio, height + delta_size.Y()};

		detail::resize_handle(skin, skin.Handle->Size(), handle_new_size);
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
		detail::resize_skin(static_cast<SliderSkin&>(*skin_), type_, from_size, to_size);
		UpdateHandle();
	}

	GuiControl::Resized(from_size, to_size); //Use base functionality
}

void GuiSlider::Slid([[maybe_unused]] int delta) noexcept
{
	Changed();
}


/*
	States
*/

void GuiSlider::SetSkinState(gui_control::ControlState state, SliderSkin &skin) noexcept
{
	if (skin.Handle)
	{
		auto [min, max] = Range();

		if (min != max)
			SetPartState(state, skin.Handle);

		skin.Handle->Visible(min != max);
	}
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

OwningPtr<gui_control::ControlSkin> GuiSlider::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<SliderSkin*>(skin.get()))
	{
		auto slider_skin = make_owning<SliderSkin>();
		slider_skin->Assign(*skin);
		return slider_skin;
	}
	else
		return skin;
}


void GuiSlider::RotateHandle() noexcept
{
	if (size_ && skin_)
	{
		auto [width, height] = size_->XY();
		detail::resize_skin(static_cast<SliderSkin&>(*skin_), type_, *size_, {height, width});

		if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
		{
			auto [handle_width, handle_height] = skin.Handle->Size().XY();
			detail::resize_handle(skin, skin.Handle->Size(), {handle_height, handle_width});
		}
	}
}

void GuiSlider::RotateSkin() noexcept
{
	if (size_)
		Size({size_->Y(), size_->X()});
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
				
				auto handle_position =
					flipped_ ?
					math::Lerp(max - handle_half_size, min + handle_half_size, Percent()) :
					math::Lerp(min + handle_half_size, max - handle_half_size, Percent());

				auto center = area->Center();
				skin.Handle->Position(
					type_ == SliderType::Vertical ?
					Vector2{center.X(), handle_position} :
					Vector2{handle_position, center.Y()});
			}
		}
	}
}


//Public

GuiSlider::GuiSlider(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, SliderType type, gui_control::BoundingBoxes hit_boxes) :

	GuiControl{std::move(name), size, std::move(caption), std::move(tooltip), std::move(hit_boxes)},
	type_{type}
{
	DefaultSetup();
}

GuiSlider::GuiSlider(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, std::optional<std::string> tooltip, SliderType type, gui_control::BoundingBoxes hit_boxes) :

	GuiControl{std::move(name), skin, size, std::move(caption), std::move(tooltip), std::move(hit_boxes)},
	type_{type}
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
			if (skin_node_)
			{
				skin.Handle->Prepare();

				//Check for intersection
				if (skin.Handle->AxisAlignedBoundingBox().TransformCopy(
					Matrix3::Transformation(skin_node_->FullTransformation())).Intersects(point))
					return skin_node_->AxisAligned() ||
						Obb{skin.Handle->AxisAlignedBoundingBox()}.TransformCopy(
						Matrix3::Transformation(skin_node_->FullTransformation())).Intersects(point);
			}
		}
	}

	return false;
}


/*
	Key events
*/

bool GuiSlider::KeyReleased(KeyButton button) noexcept
{
	if (auto [min, max] = Range(); min != max)
	{
		if (flipped_)
		{
			if (type_ == SliderType::Vertical)
			{
				switch (button)
				{
					//Flip direction
					case KeyButton::UpArrow:
					case KeyButton::DownArrow:
					button = detail::flip_arrow_keys(button);
					break;
				}
			}
			else
			{
				switch (button)
				{
					//Flip direction
					case KeyButton::LeftArrow:
					case KeyButton::RightArrow:
					button = detail::flip_arrow_keys(button);
					break;
				}
			}
		}

		switch (button)
		{
			//Increase
			case KeyButton::UpArrow:
			case KeyButton::RightArrow:
			{
				if (Position() < max)
					Position(Position() + step_by_amount_);

				return true;
			}

			//Decrease
			case KeyButton::DownArrow:
			case KeyButton::LeftArrow:
			{
				if (Position() > min)
					Position(Position() - step_by_amount_);

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
				auto handle_position = skin.Handle->Position();

				if (skin_node_)
				{
					position = //Make position relative to skin
						(position - skin_node_->DerivedPosition()).
						RotateCopy(-skin_node_->DerivedRotation(), vector2::Zero);
					handle_position *= skin_node_->DerivedScaling();
				}

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

	return false;
}

bool GuiSlider::MouseMoved(Vector2 position) noexcept
{
	if (auto [min, max] = Range(); min != max && dragged_ && skin_)
	{
		if (auto &skin = static_cast<SliderSkin&>(*skin_); skin.Handle)
		{
			if (auto size = InnerSize(); size)
			{
				if (skin_node_)
				{
					position = //Make position relative to skin
						(position - skin_node_->DerivedPosition()).
						RotateCopy(-skin_node_->DerivedRotation(), vector2::Zero);
					size = (*size - skin.Handle->Size()) * skin_node_->DerivedScaling();
				}

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