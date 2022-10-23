/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiTooltip.h
-------------------------------------------
*/

#ifndef ION_GUI_TOOLTIP_H
#define ION_GUI_TOOLTIP_H

#include <optional>
#include <string>

#include "IonGuiLabel.h"
#include "IonGuiMouseCursor.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"
#include "types/IonCumulative.h"
#include "types/IonTypes.h"

namespace ion::gui
{
	class GuiController; //Forward declaration
}

namespace ion::gui::controls
{
	using namespace types::type_literals;
	using namespace graphics::utilities;

	namespace gui_tooltip
	{
		struct TooltipSkin : gui_label::LabelSkin
		{
			//Empty
		};


		namespace detail
		{
			enum class tooltip_phase
			{
				PreFadeIn,
				FadeIn,
				Hold,
				PreFadeOut,
				FadeOut
			};

			constexpr auto default_hold_time = 5.0_sec;
			constexpr auto default_fade_in_delay = 0.5_sec;
			constexpr auto default_fade_out_delay = 0.1_sec;
			constexpr auto default_fade_time = 0.1_sec;


			Vector2 in_view_offset(const Aabb &tooltip_area, const Aabb &view_area) noexcept;
			Vector2 hot_spot_offset(gui_mouse_cursor::MouseCursorHotSpot hot_spot, const Vector2 &tooltip_size, const Vector2 &cursor_size) noexcept;
		} //detail
	} //gui_tooltip


	//A class representing a GUI tooltip that will automatically show the tooltip caption of an hovered GUI control
	//A tooltip can be of a fixed sized, or auto sized based on the length of the current tooltip caption shown
	class GuiTooltip : public GuiLabel
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			bool auto_size_ = true;
			bool follow_mouse_cursor_ = false;

			duration hold_time_ = gui_tooltip::detail::default_hold_time;
			duration fade_in_delay_ = gui_tooltip::detail::default_fade_in_delay;
			duration fade_out_delay_ = gui_tooltip::detail::default_fade_out_delay;
			duration fade_in_time_ = gui_tooltip::detail::default_fade_time;
			duration fade_out_time_ = gui_tooltip::detail::default_fade_time;

			gui_tooltip::detail::tooltip_phase phase_ = gui_tooltip::detail::tooltip_phase::PreFadeIn;
			types::Cumulative<duration> phase_duration_{fade_in_delay_};
			real opacity_ = 1.0_r;

			bool update_position_ = false;


			GuiController* GetController() const noexcept;


			/*
				Skins
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

			virtual void UpdateCaption() noexcept override;
			void UpdatePosition(Vector2 position) noexcept;
			void AdjustInView() noexcept;

			void SetOpacity(real percent) noexcept;


			/*
				Phase
			*/

			void SetPhase(gui_tooltip::detail::tooltip_phase phase) noexcept;
			void UpdatePhaseDuration() noexcept;

		public:

			//Constructs a tooltip with the given name, size and tooltip text
			GuiTooltip(std::string name, const std::optional<Vector2> &size, std::optional<std::string> text) noexcept;

			//Constructs a tooltip with the given name, skin, size and tooltip text
			GuiTooltip(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size, std::optional<std::string> text);


			/*
				Modifiers
			*/

			//Sets whether or not this tooltip should automatically adjust its size
			inline void AutoSize(bool auto_size) noexcept
			{
				auto_size_ = auto_size;
			}

			//Sets whether or not this tooltip should automatically follow the mouse cursor
			inline void FollowMouseCursor(bool follow_mouse_cursor) noexcept
			{
				follow_mouse_cursor_ = follow_mouse_cursor;
			}


			//Sets the hold time for this tooltip to the given time
			inline void HoldTime(duration time) noexcept
			{
				if (hold_time_ != time && time >= 0.0_sec)
				{
					hold_time_ = time;
					UpdatePhaseDuration();
				}
			}

			//Sets the fade in delay for this tooltip to the given time
			inline void FadeInDelay(duration time) noexcept
			{
				if (fade_in_delay_ != time && time >= 0.0_sec)
				{
					fade_in_delay_ = time;
					UpdatePhaseDuration();
				}
			}

			//Sets the fade out delay for this tooltip to the given time
			inline void FadeOutDelay(duration time) noexcept
			{
				if (fade_out_delay_ != time && time >= 0.0_sec)
				{
					fade_out_delay_ = time;
					UpdatePhaseDuration();
				}
			}

			//Sets the fade in time for this tooltip to the given time
			inline void FadeInTime(duration time) noexcept
			{
				if (fade_in_time_ != time && time >= 0.0_sec)
				{
					fade_in_time_ = time;
					UpdatePhaseDuration();
				}
			}

			//Sets the fade out time for this tooltip to the given time
			inline void FadeOutTime(duration time) noexcept
			{
				if (fade_out_time_ != time && time >= 0.0_sec)
				{
					fade_out_time_ = time;
					UpdatePhaseDuration();
				}
			}


			/*
				Observers
			*/

			//Returns whether or not this tooltip should automatically adjust its size
			[[nodiscard]] inline auto AutoSize() const noexcept
			{
				return auto_size_;
			}

			//Returns whether or not this tooltip should automatically follow the mouse cursor
			[[nodiscard]] inline auto FollowMouseCursor() const noexcept
			{
				return follow_mouse_cursor_;
			}


			//Returns the hold time for this tooltip
			[[nodiscard]] inline auto HoldTime() const noexcept
			{
				return hold_time_;
			}

			//Returns the fade in delay for this tooltip
			[[nodiscard]] inline auto FadeInDelay() const noexcept
			{
				return fade_in_delay_;
			}

			//Returns the fade out delay for this tooltip
			[[nodiscard]] inline auto FadeOutDelay() const noexcept
			{
				return fade_out_delay_;
			}

			//Returns the fade in time for this tooltip
			[[nodiscard]] inline auto FadeInTime() const noexcept
			{
				return fade_in_time_;
			}

			//Returns the fade out time for this tooltip
			[[nodiscard]] inline auto FadeOutTime() const noexcept
			{
				return fade_out_time_;
			}


			/*
				Tooltip
			*/

			//Shows this tooltip with the given tooltip text
			void Show(std::string text) noexcept;

			//Shows this tooltip
			void Show() noexcept;

			//Hides this tooltip
			void Hide() noexcept;


			/*
				Frame events
			*/

			//Called from gui control when a frame has started
			virtual void FrameStarted(duration time) noexcept override;


			/*
				Mouse events
			*/

			//Called from gui control when the mouse has been moved
			//Returns true if the mouse move event has been consumed by the tooltip
			virtual bool MouseMoved(Vector2 position) noexcept override;
	};

} //ion::gui::controls

#endif