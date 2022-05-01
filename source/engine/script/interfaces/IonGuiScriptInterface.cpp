/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonGuiScriptInterface.cpp
-------------------------------------------
*/

#include "IonGuiScriptInterface.h"

#include <string>

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace gui_script_interface;
using namespace gui;
using namespace gui::skins;

namespace gui_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_gui_component_class()
{
	return ClassDefinition::Create("component")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_frame_class()
{
	return ClassDefinition::Create("frame", "panel-container")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_panel_class()
{
	return ClassDefinition::Create("panel", "panel-container")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_panel_container_class()
{
	return ClassDefinition::Create("panel-container", "component")
		.AddClass(get_gui_panel_class())
		.AddRequiredProperty("name", ParameterType::String);
}


ClassDefinition get_gui_button_class()
{
	return ClassDefinition::Create("button", "control")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_check_box_class()
{
	return ClassDefinition::Create("check-box", "control")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_control_class()
{
	return ClassDefinition::Create("control", "component")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_group_box_class()
{
	return ClassDefinition::Create("group-box", "control")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_label_class()
{
	return ClassDefinition::Create("label", "control")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_list_box_class()
{
	return ClassDefinition::Create("list-box", "scrollable")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_mouse_cursor_class()
{
	return ClassDefinition::Create("mouse-cursor", "control")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_progress_bar_class()
{
	return ClassDefinition::Create("progress-bar", "control")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_radio_button_class()
{
	return ClassDefinition::Create("radio-button", "check-box")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_scrollable_class()
{
	return ClassDefinition::Create("scrollable", "control")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_scroll_bar_class()
{
	return ClassDefinition::Create("scroll-bar", "slider")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_slider_class()
{
	return ClassDefinition::Create("slider", "control")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_text_box_class()
{
	return ClassDefinition::Create("text-box", "scrollable")
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_gui_tooltip_class()
{
	return ClassDefinition::Create("tooltip", "label")
		.AddRequiredProperty("name", ParameterType::String);
}


ScriptValidator get_gui_validator()
{
	return ScriptValidator::Create()
		.AddAbstractClass(get_gui_component_class())
		.AddAbstractClass(get_gui_control_class())
		.AddAbstractClass(get_gui_panel_container_class())
		.AddAbstractClass(get_gui_scrollable_class())

		.AddRequiredClass(get_gui_frame_class());
}


/*
	Tree parsing
*/

NonOwningPtr<GuiFrame> create_gui_frame(const script_tree::ObjectNode &object,
	GuiController &gui_controller)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto frame = gui_controller.CreateFrame(std::move(name));

	if (frame)
	{
		//Todo
	}

	return frame;
}

void create_gui(const ScriptTree &tree,
	GuiController &gui_controller)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "frame")
			create_gui_frame(object, gui_controller);
	}
}

} //gui_theme_script_interface::detail


//Private

ScriptValidator GuiScriptInterface::GetValidator() const
{
	return detail::get_gui_validator();
}


//Public

/*
	Gui
	Creating from script
*/

void GuiScriptInterface::CreateGui(std::string_view asset_name,
	GuiController &gui_controller)
{
	if (Load(asset_name))
		detail::create_gui(*tree_, gui_controller);
}

} //ion::script::interfaces