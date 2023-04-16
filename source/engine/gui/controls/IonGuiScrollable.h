/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiScrollable.h
-------------------------------------------
*/

#ifndef ION_GUI_SCROLLABLE_H
#define ION_GUI_SCROLLABLE_H

#include <limits>

#include "IonGuiControl.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"

#undef min
#undef max

namespace ion::gui::controls
{
	using namespace events::listeners;
	using namespace graphics::utilities;

	class GuiScrollBar; //Forward declaration

	namespace gui_scrollable
	{
		namespace detail
		{
			constexpr auto default_scroll_rate = 3;
		} //detail
	} //gui_scrollable


	///@brief A class representing a GUI scrollable that can be attached to a GUI scroll bar and vice versa
	class GuiScrollable : public GuiControl
	{
		protected:

			int scroll_rate_ = gui_scrollable::detail::default_scroll_rate;
			NonOwningPtr<GuiScrollBar> scroll_bar_;


			/**
				@name Events
				@{
			*/

			///@brief Called right after this scrollable or attached scroll bar has scrolled
			///@details This function must be overridden by a derived class
			virtual void Scrolled(int delta) noexcept = 0;

			///@}

			/**
				@name Scroll bar
				@{
			*/

			void UpdateScrollBar() noexcept;

			///@}

		public:

			using GuiControl::GuiControl;

			///@brief Default move constructor
			GuiScrollable(GuiScrollable&&) = default;


			///@brief Virtual destructor
			virtual ~GuiScrollable() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Default move assignment
			GuiScrollable& operator=(GuiScrollable&&) = default;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the scroll rate of this scrollable to the given rate
			inline void ScrollRate(int rate) noexcept
			{
				scroll_rate_ = rate > 0 ? rate : 1;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the scroll rate of this scrollable
			[[nodiscard]] inline auto ScrollRate() const noexcept
			{
				return scroll_rate_;
			}

			///@}

			/**
				@name Scrolling
				@{
			*/

			///@brief Scrolls with the given rate
			inline void Scroll(int rate) noexcept
			{
				Scrolled(rate);
			}

			///@brief Scrolls up with the current scroll rate
			inline void ScrollUp() noexcept
			{
				Scroll(-scroll_rate_);
			}

			///@brief Scrolls down with the current scroll rate
			inline void ScrollDown() noexcept
			{
				Scroll(scroll_rate_);
			}

			///@brief Scrolls to the top element
			inline void ScrollToTop() noexcept
			{
				Scroll(std::numeric_limits<int>::min());
			}

			///@brief Scrolls to the bottom element
			inline void ScrollToBottom() noexcept
			{
				Scroll(std::numeric_limits<int>::max());
			}

			///@}

			/**
				@name Scroll bar
				@{
			*/

			///@brief Sets the scroll bar attached to this scrollable to the given scroll bar
			void AttachedScrollBar(NonOwningPtr<GuiScrollBar> scroll_bar) noexcept;

			///@brief Returns a pointer to the scroll bar attached to this scrollable
			///@details Returns nullptr if this scrollable does not have a scroll bar attached
			[[nodiscard]] inline auto AttachedScrollBar() const noexcept
			{
				return scroll_bar_;
			}


			///@brief Returns the total number of elements
			virtual int TotalElements() noexcept;

			///@brief Returns the number of elements that is in view
			virtual int ElementsInView() noexcept;

			///@brief Returns the current scroll value
			virtual int ScrollValue() noexcept;

			///@}

			/**
				@name Key events
				@{
			*/

			///@brief Called from gui control when a key button has been released
			///@details Returns true if the key release event has been consumed by the control
			virtual bool KeyReleased(KeyButton button) noexcept override;

			///@}

			/**
				@name Mouse events
				@{
			*/

			///@brief Called from gui control when the mouse wheel has been rolled
			///@details Returns true if the mouse wheel roll event has been consumed by the control
			virtual bool MouseWheelRolled(int delta, Vector2 position) noexcept override;

			///@}
	};

} //ion::gui::controls

#endif