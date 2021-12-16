/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiScrollBar.cpp
-------------------------------------------
*/

#include "IonGuiScrollBar.h"

#include "graphics/scene/shapes/IonSprite.h"

namespace ion::gui::controls
{

using namespace gui_scroll_bar;

namespace gui_scroll_bar::detail
{
} //gui_scroll_bar::detail


//Private

void GuiScrollBar::DefaultSetup() noexcept
{
	Type(gui_slider::SliderType::Vertical);
	Flipped(true);
}


//Protected

/*
	Skins
*/

void GuiScrollBar::UpdateHandle() noexcept
{
	//Set handle size
	GuiSlider::UpdateHandle(); //Use base functionality
}


//Public

GuiScrollBar::GuiScrollBar(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ScrollBarSkin> skin) :
	GuiSlider{std::move(name), std::move(caption), std::move(tooltip), std::move(skin)}
{
	DefaultSetup();
}

GuiScrollBar::GuiScrollBar(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ScrollBarSkin> skin, const Vector2 &size) :
	GuiSlider{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), size}
{
	DefaultSetup();
}

GuiScrollBar::GuiScrollBar(std::string name, std::optional<std::string> caption, std::optional<std::string> tooltip,
	OwningPtr<ScrollBarSkin> skin, gui_control::Areas areas) :
	GuiSlider{std::move(name), std::move(caption), std::move(tooltip), std::move(skin), std::move(areas)}
{
	DefaultSetup();
}


/*
	Modifiers
*/



} //ion::gui::controls