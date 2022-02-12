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
		{
			bar_width = width;
			bar_height = height * percent;
		}
		else
		{
			bar_width = width * percent;
			bar_height = height;
		}

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

OwningPtr<gui_control::ControlSkin> GuiProgressBar::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<ProgressBarSkin*>(skin.get()))
	{
		auto progress_bar_skin = make_owning<ProgressBarSkin>();
		progress_bar_skin->Assign(*skin);
		return progress_bar_skin;
	}
	else
		return skin;
}


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
	if (skin_)
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
	using namespace utilities;

	auto limit =
		[&]() noexcept
		{
			switch (phase_)
			{
				case detail::interpolation_phase::PreInterpolate:
				return interpolation_delay_;

				case detail::interpolation_phase::Interpolate:
				default:
				phase_duration_.Reset();
				return interpolation_time_ * math::Abs(Percent() - interpolated_percent_.value_or(0.0_r));
			}
		}();

	phase_duration_.Limit(limit);
}


void GuiProgressBar::StartInterpolation(real from_percent) noexcept
{
	if (skin_)
	{
		if (!interpolated_percent_)
			interpolated_percent_ = from_percent;

		if (auto &skin = static_cast<ProgressBarSkin&>(*skin_); skin.BarInterpolated &&
			(interpolation_type_ == BarInterpolationType::Bidirectional ||
			(interpolation_type_ == BarInterpolationType::Forward && *interpolated_percent_ < Percent()) ||
			(interpolation_type_ == BarInterpolationType::Backward && *interpolated_percent_ > Percent())))
		{
			if (phase_ == detail::interpolation_phase::Interpolate)
				SetPhase(detail::interpolation_phase::Interpolate); //Recalculate duration

			return; //Start interpolation
		}
	}

	interpolated_percent_ = {};
	SetPhase(detail::interpolation_phase::PreInterpolate);
	UpdateBarInterpolated(); //Do not interpolate
}


//Public

GuiProgressBar::GuiProgressBar(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, ProgressBarType type) :

	GuiControl{std::move(name), size, std::move(caption), {}},
	type_{type}
{
	DefaultSetup();
}

GuiProgressBar::GuiProgressBar(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, ProgressBarType type) :

	GuiControl{std::move(name), skin, size, std::move(caption), {}},
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
	Position(math::Round(math::Lerp(progress_.Min(), progress_.Max(), percent)));
}


/*
	Observers
*/

real GuiProgressBar::InterpolatedPercent() const noexcept
{
	return interpolated_percent_.value_or(Percent());
}

real GuiProgressBar::InterpolatedPosition() const noexcept
{
	using namespace utilities;

	if (interpolated_percent_)
		return math::Lerp(progress_.Min(), progress_.Max(), *interpolated_percent_);
	else
		return Position();
}


/*
	Frame events
*/

void GuiProgressBar::FrameStarted(duration time) noexcept
{
	if (visible_ && interpolated_percent_)
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
				interpolated_percent_ = Percent();
				UpdateBarInterpolated();
				interpolated_percent_ = {}; //Stop interpolation
				break;
			}
		}

		switch (phase_)
		{
			case detail::interpolation_phase::Interpolate:
			{
				using namespace utilities;

				auto delta_percent =
					std::clamp(
						interpolation_time_ > 0.0_sec ?
						time / interpolation_time_ : 1.0_r,
						0.0_r, math::Abs(Percent() - *interpolated_percent_)
					);

				//Forward
				if (interpolated_percent_ < Percent())
					*interpolated_percent_ += delta_percent;
				//Backward
				else if (interpolated_percent_ > Percent())
					*interpolated_percent_ -= delta_percent;

				UpdateBarInterpolated();
				break;
			}
		}
	}
}

} //ion::gui::controls