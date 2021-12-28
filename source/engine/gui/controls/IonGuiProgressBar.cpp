/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiProgressBar.cpp
-------------------------------------------
*/

#include "IonGuiProgressBar.h"

#include <algorithm>

#include "graphics/scene/IonModel.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "utilities/IonMath.h"

namespace ion::gui::controls
{

using namespace gui_progress_bar;

namespace gui_progress_bar::detail
{

void resize_skin(ProgressBarSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;

	if (skin.Bar)
	{
		auto &center = skin.Bar->Position();
		gui_control::detail::resize_part(skin.Bar, delta_size, vector2::Zero, center);
	}

	if (skin.BarInterpolated)
	{
		auto &center = skin.BarInterpolated->Position();
		gui_control::detail::resize_part(skin.BarInterpolated, delta_size, vector2::Zero, center);
	}
}

} //gui_progress_bar::detail


//Private

void GuiProgressBar::DefaultSetup() noexcept
{
	Focusable(false);
	CaptionLayout(gui_control::ControlCaptionLayout::OutsideTopCenter);
}


//Protected

/*
	Events
*/

void GuiProgressBar::Resized(Vector2 from_size, Vector2 to_size) noexcept
{
	if (skin_)
		detail::resize_skin(static_cast<ProgressBarSkin&>(*skin_), from_size, to_size);

	GuiControl::Resized(from_size, to_size); //Use base functionality
}

void GuiProgressBar::Progressed([[maybe_unused]] real delta) noexcept
{
	Changed();
}


/*
	States
*/

void GuiProgressBar::SetSkinState(gui_control::ControlState state, ProgressBarSkin &skin) noexcept
{
	auto [min, max] = Range();

	if (skin.Bar)
	{
		if (min != max)
			SetPartState(state, skin.Bar);

		skin.Bar->Visible(min != max);
	}

	if (skin.BarInterpolated)
	{
		if (min != max)
			SetPartState(state, skin.BarInterpolated);

		skin.BarInterpolated->Visible(min != max);
	}
}

void GuiProgressBar::SetState(gui_control::ControlState state) noexcept
{
	GuiControl::SetState(state); //Use base functionality

	if (visible_ && skin_)
		SetSkinState(state, static_cast<ProgressBarSkin&>(*skin_));
}


/*
	Skins
*/

void GuiProgressBar::RotateBars() noexcept
{
	if (size_ && skin_)
	{
		auto [width, height] = size_->XY();
		detail::resize_skin(static_cast<ProgressBarSkin&>(*skin_), *size_, {height, width});
	}
}

void GuiProgressBar::RotateSkin() noexcept
{
	if (size_)
		Size({size_->Y(), size_->X()});
}


void GuiProgressBar::UpdateBars() noexcept
{
	using namespace utilities;

	if (skin_)
	{
		if (auto &skin = static_cast<ProgressBarSkin&>(*skin_); skin.Bar)
		{
			//Set bar crop sizes
			if (auto area = InnerArea(); area)
			{
				auto [min, max] =
					type_ == ProgressBarType::Vertical ?
					std::pair{area->Min().Y(), area->Max().Y()} :
					std::pair{area->Min().X(), area->Max().X()};

				auto handle_half_size =
					(type_ == ProgressBarType::Vertical ?
					skin.Bar->Size().Y() :
					skin.Bar->Size().X()) * 0.5_r;
				
				auto bar_position =
					flipped_ ?
					math::Lerp(max - handle_half_size, min + handle_half_size, Percent()) :
					math::Lerp(min + handle_half_size, max - handle_half_size, Percent());

				auto center = area->Center();
				skin.Bar->Position(
					type_ == ProgressBarType::Vertical ?
					Vector2{center.X(), bar_position} :
					Vector2{bar_position, center.Y()});
			}
		}
	}
}


//Public

GuiProgressBar::GuiProgressBar(std::string name, std::optional<std::string> caption, OwningPtr<ProgressBarSkin> skin,
	ProgressBarType type) :

	GuiControl{std::move(name), std::move(caption), {}, std::move(skin)},
	type_{type}
{
	DefaultSetup();
}

GuiProgressBar::GuiProgressBar(std::string name, std::optional<std::string> caption, OwningPtr<ProgressBarSkin> skin, const Vector2 &size,
	ProgressBarType type) :

	GuiControl{std::move(name), std::move(caption), {}, std::move(skin), size},
	type_{type}
{
	DefaultSetup();
}


/*
	Modifiers
*/

void GuiProgressBar::Percent(real percent) noexcept
{
	using namespace utilities;
	Position(
		math::Round(math::Lerp(
			progress_.Min(),
			progress_.Max(),
			percent))
		);
}

} //ion::gui::controls