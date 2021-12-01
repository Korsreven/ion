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

#include "IonGuiLabel.h"
#include "types/IonTypes.h"

namespace ion::gui::controls
{
	using namespace types::type_literals;

	namespace gui_tooltip
	{
		enum class TooltipFadeMode : bool
		{
			Normally,
			Immediately
		};


		namespace detail
		{
			constexpr auto default_hold_time = 5.0_sec;
			constexpr auto default_fade_delay = 0.5_sec;
			constexpr auto default_fade_time = 0.1_sec;
		} //detail
	} //gui_tooltip::detail

	class GuiTooltip : public GuiLabel
	{
		protected:

			bool auto_size_ = true;
			bool follow_mouse_cursor_ = false;

			duration hold_time_ = gui_tooltip::detail::default_hold_time;
			duration fade_in_delay_ = gui_tooltip::detail::default_fade_delay;
			duration fade_out_delay_ = gui_tooltip::detail::default_fade_delay;
			duration fade_in_time_ = gui_tooltip::detail::default_fade_time;
			duration fade_out_time_ = gui_tooltip::detail::default_fade_time;

		public:

			using GuiLabel::GuiLabel;


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
				hold_time_ = time;
			}

			//Sets the fade in delay for this tooltip to the given time
			inline void FadeInDelay(duration time) noexcept
			{
				fade_in_delay_ = time;
			}

			//Sets the fade out delay for this tooltip to the given time
			inline void FadeOutDelay(duration time) noexcept
			{
				fade_out_delay_ = time;
			}

			//Sets the fade in time for this tooltip to the given time
			inline void FadeInTime(duration time) noexcept
			{
				fade_in_time_ = time;
			}

			//Sets the fade out time for this tooltip to the given time
			inline void FadeOutTime(duration time) noexcept
			{
				fade_out_time_ = time;
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


			//Returns the hold time for this tooltip to the given time
			[[nodiscard]] inline auto HoldTime() const noexcept
			{
				return hold_time_;
			}

			//Returns the fade in delay for this tooltip to the given time
			[[nodiscard]] inline auto FadeInDelay() const noexcept
			{
				return fade_in_delay_;
			}

			//Returns the fade out delay for this tooltip to the given time
			[[nodiscard]] inline auto FadeOutDelay() const noexcept
			{
				return fade_out_delay_;
			}

			//Returns the fade in time for this tooltip to the given time
			[[nodiscard]] inline auto FadeInTime() const noexcept
			{
				return fade_in_time_;
			}

			//Returns the fade out time for this tooltip to the given time
			[[nodiscard]] inline auto FadeOutTime() const noexcept
			{
				return fade_out_time_;
			}


			/*
				Tooltip
			*/

			//Shows the given tooltip with the given fade mode
			void Show(std::string tooltip, gui_tooltip::TooltipFadeMode fade_mode = gui_tooltip::TooltipFadeMode::Normally) noexcept;

			//Hides the tooltip with the given fade mode
			void Hide(gui_tooltip::TooltipFadeMode fade_mode = gui_tooltip::TooltipFadeMode::Normally) noexcept;
	};

} //ion::gui::controls

#endif