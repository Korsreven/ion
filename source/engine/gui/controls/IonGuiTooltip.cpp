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

#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/utilities/IonAabb.h"
#include "gui/IonGuiController.h"
#include "gui/IonGuiFrame.h"

namespace ion::gui::controls
{

using namespace gui_tooltip;

namespace gui_tooltip::detail
{

Vector2 in_view_offset(const Aabb &tooltip_area, const Aabb &view_area) noexcept
{
	auto [tooltip_min, tooltip_max] = tooltip_area.MinMax();
	auto [view_min, view_max] = view_area.MinMax();

	auto x = 0.0_r;
	//Outside right edge, nudge left
	if (tooltip_max.X() > view_max.X())
		x = view_max.X() - tooltip_max.X();

	//Outside left edge, nudge right
	if (tooltip_min.X() < view_min.X())
		x = view_min.X() - tooltip_min.X();

	auto y = 0.0_r;
	//Outside bottom edge, nudge up
	if (tooltip_min.Y() < view_min.Y())
		y = view_min.Y() - tooltip_min.Y();

	//Outside top edge, nudge down
	if (tooltip_max.Y() > view_max.Y())
		y = view_max.Y() - tooltip_max.Y();

	return {x, y};
}

Vector2 hot_spot_offset(gui_mouse_cursor::MouseCursorHotSpot hot_spot, const Vector2 &tooltip_size, const Vector2 &cursor_size) noexcept
{
	auto [half_width, half_height] = (tooltip_size * 0.5_r).XY();
	auto [cursor_width, cursor_height] = cursor_size.XY();

	switch (hot_spot)
	{
		case gui_mouse_cursor::MouseCursorHotSpot::TopLeft:
		return {half_width, -half_height - cursor_height};

		case gui_mouse_cursor::MouseCursorHotSpot::TopCenter:
		return {0.0_r, -half_height - cursor_height};

		case gui_mouse_cursor::MouseCursorHotSpot::TopRight:
		return {-half_width, -half_height - cursor_height};

		case gui_mouse_cursor::MouseCursorHotSpot::Left:
		return {half_width, -half_height - cursor_height * 0.5_r};

		case gui_mouse_cursor::MouseCursorHotSpot::Right:
		return {-half_width, -half_height - cursor_height * 0.5_r};

		case gui_mouse_cursor::MouseCursorHotSpot::BottomLeft:
		return {half_width, half_height + cursor_height};

		case gui_mouse_cursor::MouseCursorHotSpot::BottomCenter:
		return {0.0_r, half_height + cursor_height};

		case gui_mouse_cursor::MouseCursorHotSpot::BottomRight:
		return {-half_width, half_height + cursor_height};

		default:
		return {0.0_r, -half_height - cursor_height * 0.5_r};
	}
}

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

OwningPtr<gui_control::ControlSkin> GuiTooltip::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<TooltipSkin*>(skin.get()))
	{
		auto tooltip_skin = make_owning<TooltipSkin>();
		tooltip_skin->Assign(*skin);
		return tooltip_skin;
	}
	else
		return skin;
}


void GuiTooltip::UpdateCaption() noexcept
{
	static auto need_update = true;

	if (skin_)
	{
		if (auto &part = skin_->Caption; part && auto_size_ && need_update)
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
					text->Clear();


				if (auto size = text->MinimumAreaSize(); size != vector2::Zero)
				{
					size += 2.0_r; //Make sure there is enough space (rounding error)

					//Adjust size from viewport to ortho space
					if (auto scene_manager = part->Owner(); scene_manager)
					{
						if (auto viewport = scene_manager->ConnectedViewport(); viewport)
							size *= viewport->ViewportToOrthoRatio();
					}

					auto border_size =
						gui_control::detail::get_border_size(*skin_, false).value_or(vector2::Zero);

					need_update = false;
					Size(size + border_size);
					need_update = true;
					return;
				}
			}
		}
	}

	GuiLabel::UpdateCaption(); //Use base functionality
}

void GuiTooltip::UpdatePosition(Vector2 position) noexcept
{
	if (node_)
	{
		if (auto controller =
			[&]() noexcept -> GuiController*
			{
				if (auto owner = Owner(); owner)
				{
					if (auto frame = Owner()->ParentFrame(); frame)
						return frame->Owner();
				}
				else if (auto controller = dynamic_cast<GuiController*>(GuiComponent::Owner()); controller)
					return controller;

				return nullptr;
			}(); controller)
		{
			auto size =
				size_.value_or(vector2::Zero) * node_->DerivedScaling();

			//Custom cursor
			if (auto mouse_cursor = controller->ActiveMouseCursor(); mouse_cursor)
			{
				if (auto mouse_cursor_node = mouse_cursor->Node(); mouse_cursor_node)
				{
					auto cursor_size =
						mouse_cursor->Size().value_or(vector2::Zero) * mouse_cursor_node->DerivedScaling();
					
					//Adjust tooltip position based on cursor hot spot
					position += detail::hot_spot_offset(mouse_cursor->HotSpot(), size, cursor_size);
				}
			}
			else //OS cursor
			{
				auto cursor_size = vector2::Zero; //OS cursor size?

				//Adjust tooltip position based on cursor hot spot
				position += detail::hot_spot_offset(gui_mouse_cursor::MouseCursorHotSpot::TopLeft, size, cursor_size);
			}
		}

		node_->DerivedPosition(position);
		AdjustInView();
	}
}

void GuiTooltip::AdjustInView() noexcept
{
	if (node_ && skin_)
	{
		//Make sure tooltip stays within view area
		if (auto view_area =
			[&]() noexcept
			{
				if (auto scene_manager = skin_->Caption->Owner(); scene_manager)
				{
					if (auto viewport = scene_manager->ConnectedViewport(); viewport)
						return viewport->ConnectedCamera()->WorldAxisAlignedBoundingBox();
				}

				return aabb::Zero;
			}(); view_area != aabb::Zero)
		{
			if (auto tooltip_area =
				[&]() noexcept
				{
					if (skin_->Parts)
					{
						skin_->Parts->Prepare();
						return skin_->Parts->WorldAxisAlignedBoundingBox();
					}
					else if (skin_->Caption)
					{
						skin_->Caption->Prepare();
						return skin_->Caption->WorldAxisAlignedBoundingBox();
					}

					return aabb::Zero;
				}(); tooltip_area != aabb::Zero)

				node_->Position(node_->Position() + detail::in_view_offset(tooltip_area, view_area));
		}
	}
}


void GuiTooltip::SetOpacity(real percent) noexcept
{
	if (skin_)
	{
		if (skin_->Parts)
			skin_->Parts->Opacity(percent);

		if (skin_->Caption)
			skin_->Caption->Opacity(percent);
	}

	opacity_ = percent;
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


//Public

GuiTooltip::GuiTooltip(std::string name, const std::optional<Vector2> &size, std::optional<std::string> text) :

	GuiLabel{std::move(name), size, std::move(text)},
	auto_size_{!size}
{
	DefaultSetup();
}

GuiTooltip::GuiTooltip(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size, std::optional<std::string> text) :

	GuiLabel{std::move(name), skin, size, std::move(text)},
	auto_size_{!size}
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

	update_position_ = true;
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
		(follow_mouse_cursor_ || phase_ == detail::tooltip_phase::PreFadeIn || update_position_))
	{
		UpdatePosition(position);
		update_position_ = false;
		return true;
	}
	else
		return false;
}

} //ion::gui::controls