/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiScrollable.cpp
-------------------------------------------
*/

#include "IonGuiScrollable.h"

#include "IonGuiScrollBar.h"
#include "gui/IonGuiPanelContainer.h"

namespace ion::gui::controls
{

using namespace gui_scrollable;

namespace gui_scrollable::detail
{
} //gui_scrollable::detail


//Protected

/*
	Scroll bar
*/

void GuiScrollable::UpdateScrollBar() noexcept
{
	if (scroll_bar_)
	{
		auto element_count = TotalElements();
		auto elements_in_view = ElementsInView();
		auto max =
			elements_in_view > 0 && element_count > elements_in_view ?
			element_count - elements_in_view : 0;

		scroll_bar_->Range(0, max);
		scroll_bar_->Position(ScrollPosition());
		scroll_bar_->StepByAmount(scroll_rate_);
	}
}


//Public

GuiScrollable::~GuiScrollable() noexcept
{
	AttachedScrollBar(nullptr); //Detach (if any)
}


/*
	Scroll bar
*/

void GuiScrollable::AttachedScrollBar(NonOwningPtr<GuiScrollBar> scroll_bar) noexcept
{
	if (scroll_bar_ != scroll_bar)
	{
		//Attach
		if (scroll_bar)
		{
			AttachedScrollBar(nullptr); //Detach previous (if any)
			scroll_bar_ = scroll_bar;
			UpdateScrollBar();

			if (auto owner = Owner(); owner)
				scroll_bar->AttachedScrollable(
					owner->GetControlAs<GuiScrollable>(*Name())
				);
		}
		
		else //Detach
		{
			auto ptr = scroll_bar_.get();
			scroll_bar_ = nullptr;

			if (ptr)
				ptr->AttachedScrollable(nullptr);
		}
	}
}


int GuiScrollable::TotalElements() noexcept
{
	return 0; //Optional to override
}

int GuiScrollable::ElementsInView() noexcept
{
	return 0; //Optional to override
}

int GuiScrollable::ScrollPosition() noexcept
{
	return 0; //Optional to override
}


/*
	Key events
*/

bool GuiScrollable::KeyReleased(KeyButton button) noexcept
{
	switch (button)
	{
		case KeyButton::PageUp:
		Scrolled(-ElementsInView());
		return true;

		case KeyButton::PageDown:
		Scrolled(ElementsInView());
		return true;

		default:
		return false;
	}
}


/*
	Mouse events
*/

bool GuiScrollable::MouseWheelRolled(int delta, [[maybe_unused]] Vector2 position) noexcept
{
	//Up
	if (delta > 0)
		ScrollUp();
	//Down
	else if (delta < 0)
		ScrollDown();
	
	return false;
}

} //ion::gui::controls