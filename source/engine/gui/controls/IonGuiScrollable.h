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


	class GuiScrollable : public GuiControl
	{
		protected:

			int scroll_rate_ = gui_scrollable::detail::default_scroll_rate;
			NonOwningPtr<GuiScrollBar> scroll_bar_;


			/*
				Events
			*/

			//Called right after this scrollable or attached scroll bar has scrolled
			//This function must be overridden by a derived class
			virtual void Scrolled(int delta) noexcept = 0;


			/*
				Scroll bar
			*/

			void UpdateScrollBar() noexcept;

		public:

			using GuiControl::GuiControl;

			//Virtual destructor
			virtual ~GuiScrollable() noexcept;


			/*
				Modifiers
			*/

			//Sets the scroll rate of this scrollable to the given rate
			inline void ScrollRate(int rate) noexcept
			{
				scroll_rate_ = rate > 0 ? rate : 1;
			}


			/*
				Observers
			*/

			//Returns the scroll rate of this scrollable
			[[nodiscard]] inline auto ScrollRate() const noexcept
			{
				return scroll_rate_;
			}


			/*
				Scrolling
			*/

			//Scroll with the given rate
			inline void Scroll(int rate) noexcept
			{
				Scrolled(rate);
			}

			//Scroll up with the current scroll rate
			inline void ScrollUp() noexcept
			{
				Scroll(-scroll_rate_);
			}

			//Scroll down with the current scroll rate
			inline void ScrollDown() noexcept
			{
				Scroll(scroll_rate_);
			}

			//Scroll to the top element
			inline void ScrollToTop() noexcept
			{
				Scroll(std::numeric_limits<int>::min());
			}

			//Scroll to the bottom element
			inline void ScrollToBottom() noexcept
			{
				Scroll(std::numeric_limits<int>::max());
			}


			/*
				Scroll bar
			*/

			//Sets the scroll bar attached to this scrollable to the given scroll bar
			void AttachedScrollBar(NonOwningPtr<GuiScrollBar> scroll_bar) noexcept;

			//Returns a pointer to the scroll bar attached to this scrollable
			//Returns nullptr if this scrollable does not have a scroll bar attached
			[[nodiscard]] inline auto AttachedScrollBar() const noexcept
			{
				return scroll_bar_;
			}


			//Returns the total number of elements
			virtual int TotalElements() noexcept;

			//Returns the number of elements that is in view
			virtual int ElementsInView() noexcept;

			//Returns the current scroll position
			virtual int ScrollPosition() noexcept;


			/*
				Key events
			*/

			//Called from gui control when a key button has been released
			//Returns true if the key release event has been consumed by the control
			virtual bool KeyReleased(KeyButton button) noexcept override;


			/*
				Mouse events
			*/

			//Called from gui control when the mouse wheel has been rolled
			//Returns true if the mouse wheel roll event has been consumed by the control
			virtual bool MouseWheelRolled(int delta, Vector2 position) noexcept override;
	};

} //ion::gui::controls

#endif