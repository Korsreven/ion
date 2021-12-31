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

/*
	Skins
*/

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

void crop_bar(gui_control::ControlVisualPart &bar, ProgressBarType type, bool flipped, real percent) noexcept
{
	if (bar.SpriteObject)
	{
		switch (type)
		{
			case ProgressBarType::Vertical:
			{
				if (flipped)
					bar->Crop(Aabb{{0.0_r, 1.0_r - percent}, {1.0_r, 1.0_r}});
				else
					bar->Crop(Aabb{{0.0_r, 0.0_r}, {1.0_r, percent}});

				break;
			}

			case ProgressBarType::Horizontal:
			{
				if (flipped)
					bar->Crop(Aabb{{1.0_r - percent, 0.0_r}, {1.0_r, 1.0_r}});
				else
					bar->Crop(Aabb{{0.0_r, 0.0_r}, {percent, 1.0_r}});

				break;
			}
		}
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
			auto percent = Percent();

			//Set bar size
			if (auto size = InnerSize(); size)
			{
				auto [width, height] = size->XY();
				auto [bar_width, bar_height] = skin.Bar->Size().XY();

				if (type_ == ProgressBarType::Vertical)
					bar_height = height * percent;
				else
					bar_width = width * percent;

				if (auto bar_size = Vector2{bar_width, bar_height};
					bar_size != skin.Bar->Size())
				{
					detail::resize_skin(skin, skin.Bar->Size(), bar_size);
					detail::crop_bar(skin.Bar, type_, flipped_, percent);
				}
			}

			//Set bar position
			if (auto area = InnerArea(); area)
			{
				auto [min, max] =
					type_ == ProgressBarType::Vertical ?
					std::pair{area->Min().Y(), area->Max().Y()} :
					std::pair{area->Min().X(), area->Max().X()};

				auto bar_half_size =
					(type_ == ProgressBarType::Vertical ?
					skin.Bar->Size().Y() :
					skin.Bar->Size().X()) * 0.5_r;
				
				auto bar_position =
					flipped_ ?
					max - bar_half_size :
					min + bar_half_size;

				auto center = area->Center();
				skin.Bar->Position(
					type_ == ProgressBarType::Vertical ?
					Vector2{center.X(), bar_position} :
					Vector2{bar_position, center.Y()});
			}
		}
	}
}


/*
	Phase
*/

void GuiProgressBar::SetPhase(detail::interpolation_phase phase) noexcept
{
	phase_ = phase;
	UpdatePhaseDuration();
}

void GuiProgressBar::UpdatePhaseDuration() noexcept
{
	auto limit =
		[&]() noexcept
		{
			switch (phase_)
			{
				case detail::interpolation_phase::PreInterpolate:
				return interpolation_delay_;

				case detail::interpolation_phase::Interpolate:
				default:
				return interpolation_time_;
			}
		}();

	phase_duration_.Limit(limit);
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


/*
	Frame events
*/

void GuiProgressBar::FrameStarted(duration time) noexcept
{
	if (visible_)
	{
		if (phase_duration_ += time)
		{
			phase_duration_.ResetWithCarry();

			//Switch to next phase
			SetPhase(
				[&]() noexcept
				{
					switch (phase_)
					{
						case detail::interpolation_phase::PreInterpolate:
						return detail::interpolation_phase::Interpolate;

						case detail::interpolation_phase::Interpolate:
						default:
						return detail::interpolation_phase::PreInterpolate;
					}
				}());
		}

		switch (phase_)
		{
			case detail::interpolation_phase::PreInterpolate:
			//Todo
			break;

			case detail::interpolation_phase::Interpolate:
			//Todo
			break;		
		}
	}
}

} //ion::gui::controls