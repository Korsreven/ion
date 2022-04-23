/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonGuiThemeScriptInterface.cpp
-------------------------------------------
*/

#include "IonGuiThemeScriptInterface.h"

#include <string>

#include "graphics/fonts/IonTextManager.h"
#include "graphics/materials/IonMaterialManager.h"
#include "gui/controls/IonGuiButton.h"
#include "gui/controls/IonGuiCheckBox.h"
#include "gui/controls/IonGuiGroupBox.h"
#include "gui/controls/IonGuiLabel.h"
#include "gui/controls/IonGuiListBox.h"
#include "gui/controls/IonGuiMouseCursor.h"
#include "gui/controls/IonGuiProgressBar.h"
#include "gui/controls/IonGuiRadioButton.h"
#include "gui/controls/IonGuiScrollBar.h"
#include "gui/controls/IonGuiSlider.h"
#include "gui/controls/IonGuiTextBox.h"
#include "gui/controls/IonGuiTooltip.h"
#include "sounds/IonSoundManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace gui_theme_script_interface;
using namespace gui;
using namespace gui::skins;

namespace gui_theme_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_gui_skin_class()
{
	auto text_state = ClassDefinition::Create("style")
		.AddRequiredProperty("state", {"enabled"s, "disabled"s, "focused"s, "pressed"s, "hovered"s})
		.AddProperty("foreground-color", ParameterType::Color)
		.AddProperty("background-color", ParameterType::Color)
		.AddProperty("font-style", {"bold"s, "italic"s, "bold-italic"s})
		.AddProperty("decoration", {"underline"s, "line-through"s, "overline"s})
		.AddProperty("decoration-color", ParameterType::Color)
		.AddProperty("font-size", {"smaller"s, "larger"s})
		.AddProperty("vertical-align", {"subscript"s, "superscript"s});

	auto part = ClassDefinition::Create("part")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("enabled", ParameterType::String)
		.AddProperty("disabled", ParameterType::String)
		.AddProperty("focused", ParameterType::String)
		.AddProperty("pressed", ParameterType::String)
		.AddProperty("hovered", ParameterType::String)
		.AddProperty("scaling", ParameterType::Vector2)
		.AddProperty("fill-color", ParameterType::Color);

	auto text_part = ClassDefinition::Create("text-part")
		.AddRequiredProperty("name", ParameterType::String)
		.AddRequiredProperty("text", ParameterType::String)
		.AddProperty("foreground-color", ParameterType::Color)
		.AddProperty("background-color", ParameterType::Color)
		.AddProperty("font-style", {"bold"s, "italic"s, "bold-italic"s})
		.AddProperty("decoration", {"underline"s, "line-through"s, "overline"s})
		.AddProperty("decoration-color", ParameterType::Color)
		.AddProperty("font-size", {"smaller"s, "larger"s})
		.AddProperty("vertical-align", {"subscript"s, "superscript"s});

	auto sound_part = ClassDefinition::Create("sound-part")
		.AddRequiredProperty("name", ParameterType::String)
		.AddRequiredProperty("focused", ParameterType::String)
		.AddRequiredProperty("defocused", ParameterType::String)
		.AddRequiredProperty("pressed", ParameterType::String)
		.AddRequiredProperty("released", ParameterType::String)
		.AddRequiredProperty("clicked", ParameterType::String)
		.AddRequiredProperty("entered", ParameterType::String)
		.AddRequiredProperty("exited", ParameterType::String)
		.AddRequiredProperty("changed", ParameterType::String);

	return ClassDefinition::Create("skin")
		.AddClass(std::move(part))
		.AddClass(std::move(text_part))
		.AddClass(std::move(sound_part))

		.AddRequiredProperty("control",
			{"button"s, "check-box"s, "group-box"s, "label"s,
			 "list-box"s, "mouse-cursor"s, "progress-bar"s, "radio-button"s,
			 "scroll-bar"s, "slider"s, "text-box"s, "tooltip"s})
		.AddProperty("name", ParameterType::String);
}

ClassDefinition get_gui_theme_class()
{
	return ClassDefinition::Create("theme")
		.AddRequiredProperty("name", ParameterType::String);
}

ScriptValidator get_gui_theme_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_gui_theme_class());
}


/*
	Tree parsing
*/

NonOwningPtr<GuiSkin> create_gui_skin(const script_tree::ObjectNode &object,
	GuiTheme &theme,
	graphics::materials::MaterialManager &material_manager,
	graphics::fonts::TextManager &text_manager,
	sounds::SoundManager &sound_manager)
{
	auto control_name = object
		.Property("control")[0]
		.Get<ScriptType::Enumerable>()->Get();
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto skin =
		[&]() noexcept -> NonOwningPtr<GuiSkin>
		{	
			//Default skin
			if (std::empty(name))
			{
				if (control_name == "button")
					return theme.CreateSkin<controls::GuiButton>();
				else if (control_name == "check-box")
					return theme.CreateSkin<controls::GuiCheckBox>();
				else if (control_name == "group-box")
					return theme.CreateSkin<controls::GuiGroupBox>();
				else if (control_name == "label")
					return theme.CreateSkin<controls::GuiLabel>();
				else if (control_name == "list-box")
					return theme.CreateSkin<controls::GuiListBox>();
				else if (control_name == "mouse-cursor")
					return theme.CreateSkin<controls::GuiMouseCursor>();
				else if (control_name == "progress-bar")
					return theme.CreateSkin<controls::GuiProgressBar>();
				else if (control_name == "radio-button")
					return theme.CreateSkin<controls::GuiRadioButton>();
				else if (control_name == "scroll-bar")
					return theme.CreateSkin<controls::GuiScrollBar>();
				else if (control_name == "slider")
					return theme.CreateSkin<controls::GuiSlider>();
				else if (control_name == "text-box")
					return theme.CreateSkin<controls::GuiTextBox>();
				else if (control_name == "tooltip")
					return theme.CreateSkin<controls::GuiTooltip>();
			}
			else //Named skin
			{
				if (control_name == "button")
					return theme.CreateSkin<controls::GuiButton>(std::move(name));
				else if (control_name == "check-box")
					return theme.CreateSkin<controls::GuiCheckBox>(std::move(name));
				else if (control_name == "group-box")
					return theme.CreateSkin<controls::GuiGroupBox>(std::move(name));
				else if (control_name == "label")
					return theme.CreateSkin<controls::GuiLabel>(std::move(name));
				else if (control_name == "list-box")
					return theme.CreateSkin<controls::GuiListBox>(std::move(name));
				else if (control_name == "mouse-cursor")
					return theme.CreateSkin<controls::GuiMouseCursor>(std::move(name));
				else if (control_name == "progress-bar")
					return theme.CreateSkin<controls::GuiProgressBar>(std::move(name));
				else if (control_name == "radio-button")
					return theme.CreateSkin<controls::GuiRadioButton>(std::move(name));
				else if (control_name == "scroll-bar")
					return theme.CreateSkin<controls::GuiScrollBar>(std::move(name));
				else if (control_name == "slider")
					return theme.CreateSkin<controls::GuiSlider>(std::move(name));
				else if (control_name == "text-box")
					return theme.CreateSkin<controls::GuiTextBox>(std::move(name));
				else if (control_name == "tooltip")
					return theme.CreateSkin<controls::GuiTooltip>(std::move(name));
			}

			return nullptr;
		}();

	if (skin)
	{
		for (auto &property : object.Properties())
		{
		}
	}

	return skin;
}

NonOwningPtr<GuiTheme> create_gui_theme(const script_tree::ObjectNode &object,
	GuiController &gui_controller, NonOwningPtr<graphics::scene::SceneManager> scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::fonts::TextManager &text_manager,
	sounds::SoundManager &sound_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto theme = gui_controller.CreateTheme(std::move(name), scene_manager);

	if (theme)
	{
		for (auto &obj : object.Objects())
		{
			if (obj.Name() == "skin")
				detail::create_gui_skin(obj, *theme,
					material_manager, text_manager, sound_manager);
		}
	}

	return theme;
}

void create_gui_themes(const ScriptTree &tree,
	GuiController &gui_controller, NonOwningPtr<graphics::scene::SceneManager> scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::fonts::TextManager &text_manager,
	sounds::SoundManager &sound_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "gui-theme")
			create_gui_theme(object, gui_controller, scene_manager,
				material_manager, text_manager, sound_manager);
	}
}

} //gui_theme_script_interface::detail


//Private

ScriptValidator GuiThemeScriptInterface::GetValidator() const
{
	return detail::get_gui_theme_validator();
}


//Public

/*
	Frame sequences
	Creating from script
*/

void GuiThemeScriptInterface::CreateGuiThemes(std::string_view asset_name,
	GuiController &gui_controller, NonOwningPtr<graphics::scene::SceneManager> scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::fonts::TextManager &text_manager,
	sounds::SoundManager &sound_manager)
{
	if (Load(asset_name))
		detail::create_gui_themes(*tree_, gui_controller, scene_manager,
			material_manager, text_manager, sound_manager);
}

} //ion::script::interfaces