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

void resize_bar(gui_control::ControlVisualPart &bar, const Vector2 &delta_size) noexcept
{
	if (bar)
	{
		auto &center = bar->Position();
		gui_control::detail::resize_part(bar, delta_size, vector2::Zero, center);
	}
}

void crop_bar(gui_control::ControlVisualPart &bar, ProgressBarType type, bool flipped, real percent) noexcept
{
	if (bar)
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

void update_bar(gui_control::ControlVisualPart &bar, ProgressBarType type, bool flipped, real percent, const Aabb &area) noexcept
{
	if (bar)
	{
		//Set bar size
		auto [width, height] = area.ToSize().XY();
		auto [bar_width, bar_height] = bar->Size().XY();

		if (type == ProgressBarType::Vertical)
			bar_height = height * percent;
		else
			bar_width = width * percent;

		if (auto bar_size = Vector2{bar_width, bar_height};
			bar_size != bar->Size())
		{
			detail::resize_bar(bar, bar_size - bar->Size());
			detail::crop_bar(bar, type, flipped, percent);
		}

		//Set bar position
		auto [min, max] =
			type == ProgressBarType::Vertical ?
			std::pair{area.Min().Y(), area.Max().Y()} :
			std::pair{area.Min().X(), area.Max().X()};

		auto bar_half_size =
			(type == ProgressBarType::Vertical ?
			bar->Size().Y() :
			bar->Size().X()) * 0.5_r;
				
		auto bar_position =
			flipped ?
			max - bar_half_size :
			min + bar_half_size;

		auto center = area.Center();
		bar->Position(
			type == ProgressBarType::Vertical ?
			Vector2{center.X(), bar_position} :
			Vector2{bar_position, center.Y()});
	}
}


void resize_skin(ProgressBarSkin &skin, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;
	resize_bar(skin.Bar, delta_size);
	resize_bar(skin.BarInterpolated, delta_size);
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


void GuiProgressBar::UpdateBar() noexcept
{
	if (skin_)
	{
		if (auto &skin = static_cast<ProgressBarSkin&>(*skin_); skin.Bar)
		{
			if (auto area = InnerArea(); area)
				detail::update_bar(skin.Bar, type_, flipped_, Percent(), *area);
		}
	}
}

void GuiProgressBar::UpdateBarInterpolated() noexcept
{
	if (skin_ && interpolated_position_)
	{
		if (auto &skin = static_cast<ProgressBarSkin&>(*skin_); skin.BarInterpolated)
		{
			if (auto area = InnerArea(); area)
				detail::update_bar(skin.BarInterpolated, type_, flipped_, InterpolatedPercent(), *area);
		}
	}
}

void GuiProgressBar::UpdateBars() noexcept
{
	UpdateBar();
	UpdateBarInterpolated();
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
	Observers
*/

real GuiProgressBar::InterpolatedPercent() const noexcept
{
	if (interpolated_position_)
	{
		auto [min, max] = Range();
		return max - min > 0.0_r ?
			(*interpolated_position_ - min) / (max - min) :
			1.0_r;
	}
	else
		return Percent();
}

real GuiProgressBar::InterpolatedPosition() const noexcept
{
	if (interpolated_position_)
		return *interpolated_position_;
	else
		return Position();
}


/*
	Frame events
*/

void GuiProgressBar::FrameStarted(duration time) noexcept
{
	if (visible_ && interpolated_position_)
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

			switch (phase_)
			{
				case detail::interpolation_phase::PreInterpolate:
				interpolated_position_ = {};
				break;	
			}
		}

		switch (phase_)
		{
			case detail::interpolation_phase::Interpolate:
			interpolated_position_ =
				[&]() noexcept
				{
					using namespace utilities;

					//Forward
					if (interpolated_position_ < Position())
						return math::Lerp(*interpolated_position_, Position(), phase_duration_.Percent());
					//Backward
					else if (interpolated_position_ > Position())
						return math::Lerp(Position(), *interpolated_position_, phase_duration_.Percent());
					else
						return *interpolated_position_;
				}();

			UpdateBarInterpolated();
			break;
		}
	}
}

} //ion::gui::controls