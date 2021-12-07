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
#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
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
	static auto need_update = true;

	if (auto &part = skin_.Caption; auto_size_ && part && need_update)
	{
		//Caption text
		if (auto &text = part->Get(); text)
		{
			//Area size
			text->AreaSize({});

			//Padding
			text->Padding(caption_padding_.value_or(gui_control::detail::default_caption_padding_size));

			//Content
			if (caption_)
				text->Content(*caption_);
			else
				text->Content({});

			if (auto size = text->MinimumAreaSize(); size != vector2::Zero)
			{
				//Adjust size from viewport to ortho space
				if (auto scene_manager = skin_.Caption->Owner(); scene_manager)
				{
					if (auto viewport = scene_manager->ConnectedViewport(); viewport)
						size *= viewport->ViewportToOrthoRatio();
				}

				auto visual_area = VisualArea();
				auto center_area = gui_control::detail::get_center_area(skin_);

				//Calculate border size
				auto top_right_size = visual_area && center_area ?
					visual_area->Max() - center_area->Max() :
					vector2::Zero;
				auto bottom_left_size = visual_area && center_area ?
					center_area->Min() - visual_area->Min() :
					vector2::Zero;
				auto border_size = top_right_size.CeilCopy(bottom_left_size);

				need_update = false;
				Size(size + border_size * 2.0_r);
				need_update = true;
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

GuiTooltip::GuiTooltip(std::string name, std::optional<std::string> text, gui_control::ControlSkin skin) :
	GuiLabel{std::move(name), std::move(text), std::move(skin)}
{
	DefaultSetup();
}

GuiTooltip::GuiTooltip(std::string name, std::optional<std::string> text, gui_control::ControlSkin skin, const Vector2 &size) :

	GuiLabel{std::move(name), std::move(text), std::move(skin), size},
	auto_size_{false}
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

	GuiControl::Show();
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
				case detail::tooltip_phase::PreFadeIn:
				phase_duration_.Total(0.0_sec);
				GuiControl::Hide();
				break;

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
			phase_duration_.ResetWithCarry();

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
				GuiControl::Hide();
				break;
			}
		}

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


/*
	Mouse events
*/

bool GuiTooltip::MouseMoved(Vector2 position) noexcept
{
	if (visible_ &&
		(follow_mouse_cursor_ || phase_ == detail::tooltip_phase::PreFadeIn))
	{
		UpdatePosition(position);
		return true;
	}
	else
		return false;
}

} //ion::gui::controls