/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiLabel.cpp
-------------------------------------------
*/

#include "IonGuiLabel.h"

namespace ion::gui::controls
{

using namespace gui_label;

namespace gui_label::detail
{
} //gui_label::detail


//Private

void GuiLabel::DefaultSetup() noexcept
{
	Focusable(false);
}


//Protected

/*
	Skins
*/

OwningPtr<gui_control::ControlSkin> GuiLabel::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<LabelSkin*>(skin.get()))
	{
		auto label_skin = make_owning<LabelSkin>();
		label_skin->Assign(*skin);
		return label_skin;
	}
	else
		return skin;
}


//Public

GuiLabel::GuiLabel(std::string name, std::optional<std::string> caption,
	OwningPtr<LabelSkin> skin, gui_control::BoundingBoxes hit_boxes) :
	GuiControl{std::move(name), std::move(caption), {}, std::move(skin), std::move(hit_boxes)}
{
	DefaultSetup();
}

GuiLabel::GuiLabel(std::string name, std::optional<std::string> caption,
	OwningPtr<LabelSkin> skin, const Vector2 &size, gui_control::BoundingBoxes hit_boxes) :
	GuiControl{std::move(name), std::move(caption), {}, std::move(skin), size, std::move(hit_boxes)}
{
	DefaultSetup();
}

} //ion::gui::controls