/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiTooltip.cpp
-------------------------------------------
*/

#include "IonGuiTooltip.h"

#include "graphics/fonts/utilities/IonFontUtility.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/utilities/IonAabb.h"

namespace ion::gui::controls
{

using namespace gui_tooltip;

namespace gui_tooltip::detail
{
} //gui_tooltip::detail


//Private

void GuiTooltip::DefaultSetup() noexcept
{
	Enabled(false);
	Visible(false);
}


//Protected

/*
	Skins
*/

void GuiTooltip::UpdateCaption() noexcept
{
	if (auto_size_ && skin_.Caption)
	{
		//Get() will not reload vertex streams when called from an immutable reference
		if (const auto &c_part = *skin_.Caption.TextObject;
			c_part.Get() && c_part.Get()->Lettering())
		{
			if (auto size = graphics::fonts::utilities::MeasureTextBlocks(
				c_part.Get()->FormattedBlocks(), *c_part.Get()->Lettering()); size)
			{
				Size(*size + caption_padding_.value_or(gui_control::detail::default_caption_padding_size) * 2.0_r);
				return;
			}
		}
	}

	GuiLabel::UpdateCaption(); //Use base functionality
}


/*
	Phase
*/

void GuiTooltip::SetPhase(detail::tooltip_phase phase) noexcept
{
	phase_ = phase;
	UpdatePhaseDuration();
}

void GuiTooltip::UpdatePhaseDuration() noexcept
{
	auto limit =
		[&]() noexcept
		{
			switch (phase_)
			{
				case detail::tooltip_phase::PreFadeIn:
				return fade_in_delay_;

				case detail::tooltip_phase::FadeIn:
				return fade_in_time_;

				case detail::tooltip_phase::PreFadeOut:
				return fade_out_delay_;

				case detail::tooltip_phase::FadeOut:
				return fade_out_time_;

				case detail::tooltip_phase::Hold:
				default:
				return hold_time_;
			}
		}();

	phase_duration_.Limit(limit);
}


void GuiTooltip::SetOpacity(real percent) noexcept
{
	if (skin_.Parts)
		skin_.Parts->Opacity(percent);

	if (skin_.Caption)
		skin_.Caption->Opacity(percent);

	opacity_ = percent;
}

void GuiTooltip::UpdatePosition(const Vector2 &position) noexcept
{
	if (node_)
	{
		auto [half_width, half_height] =
			VisualArea().value_or(aabb::Zero).ToHalfSize().XY();
		auto top_left_off =
			Vector2{half_width, -half_height} * node_->DerivedScaling();

		node_->DerivedPosition(position + top_left_off);
	}
}


//Public

GuiTooltip::GuiTooltip(std::string name, gui_control::ControlSkin skin) :
	GuiLabel{std::move(name), {}, std::move(skin)}
{
	DefaultSetup();
}

GuiTooltip::GuiTooltip(std::string name, std::optional<std::string> text, gui_control::ControlSkin skin) :
	GuiLabel{std::move(name), std::move(text), std::move(skin)}
{
	DefaultSetup();
}


/*
	Tooltip
*/

void GuiTooltip::Show(std::string text) noexcept
{
	Caption(std::move(text));
	Show();
}

void GuiTooltip::Show() noexcept
{
	if (!visible_)
	{
		SetOpacity(0.0_r);
		SetPhase(detail::tooltip_phase::PreFadeIn);
		phase_duration_.Total(0.0_sec);
	}
	else
	{
		if (phase_ != detail::tooltip_phase::PreFadeIn &&
			phase_ != detail::tooltip_phase::FadeIn)
		{
			switch (phase_)
			{
				case detail::tooltip_phase::FadeOut:
				SetPhase(detail::tooltip_phase::FadeIn);
				phase_duration_.Percent(opacity_);
				break;

				default:
				SetPhase(detail::tooltip_phase::Hold);
				phase_duration_.Total(0.0_sec);
				break;
			}
		}
	}

	Visible(true);
}

void GuiTooltip::Hide() noexcept
{
	if (visible_)
	{
		if (phase_ != detail::tooltip_phase::PreFadeOut &&
			phase_ != detail::tooltip_phase::FadeOut)
		{
			switch (phase_)
			{
				case detail::tooltip_phase::FadeIn:
				SetPhase(detail::tooltip_phase::FadeOut);
				phase_duration_.Percent(1.0_r - opacity_);
				break;

				default:
				SetPhase(detail::tooltip_phase::PreFadeOut);
				phase_duration_.Total(0.0_sec);
				break;
			}
		}
	}
}


/*
	Frame events
*/

void GuiTooltip::FrameStarted(duration time) noexcept
{
	if (visible_)
	{
		if (phase_duration_ += time)
		{
			//Switch to next phase
			SetPhase(
				[&]() noexcept
				{
					switch (phase_)
					{
						case detail::tooltip_phase::PreFadeIn:
						return detail::tooltip_phase::FadeIn;

						case detail::tooltip_phase::FadeIn:
						return detail::tooltip_phase::Hold;

						case detail::tooltip_phase::Hold:
						return detail::tooltip_phase::PreFadeOut;

						case detail::tooltip_phase::PreFadeOut:
						return detail::tooltip_phase::FadeOut;

						case detail::tooltip_phase::FadeOut:
						default:
						return detail::tooltip_phase::PreFadeIn;
					}
				}());

			switch (phase_)
			{
				case detail::tooltip_phase::Hold:
				SetOpacity(1.0_r);
				break;

				case detail::tooltip_phase::PreFadeIn:
				SetOpacity(0.0_r);
				Visible(false);
				break;
			}
		}
		else
		{
			switch (phase_)
			{
				case detail::tooltip_phase::FadeIn:
				SetOpacity(phase_duration_.Percent());
				break;

				case detail::tooltip_phase::FadeOut:
				SetOpacity(1.0_r - phase_duration_.Percent());
				break;
			}
		}
	}
}


/*
	Mouse events
*/

bool GuiTooltip::MouseMoved(Vector2 position) noexcept
{
	if (follow_mouse_cursor_ ||
		phase_ == detail::tooltip_phase::PreFadeIn)
	{
		UpdatePosition(position);
		return true;
	}
	else
		return false;
}

} //ion::gui::controls