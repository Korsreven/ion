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

ClassDefinition get_gui_frame_class()
{
	return ClassDefinition::Create("frame")
		.AddRequiredProperty("name", ParameterType::String);
}

ScriptValidator get_gui_validator()
{
	return ScriptValidator::Create()
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