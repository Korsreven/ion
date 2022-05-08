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
#include "graphics/shaders/IonShaderProgramManager.h"
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

graphics::render::pass::BlendFactor get_pass_blend_factor(const script_tree::ArgumentNode &arg)
{
	auto name = arg
		.Get<ScriptType::Enumerable>()->Get();

	if (name == "zero")
		return graphics::render::pass::BlendFactor::Zero;
	else if (name == "one")
		return graphics::render::pass::BlendFactor::One;

	else if (name == "source-color")
		return graphics::render::pass::BlendFactor::SourceColor;
	else if (name == "one-minus-source-color")
		return graphics::render::pass::BlendFactor::OneMinus_SourceColor;
	else if (name == "destination-color")
		return graphics::render::pass::BlendFactor::DestinationColor;
	else if (name == "one-minus-destination-color")
		return graphics::render::pass::BlendFactor::OneMinus_DestinationColor;

	else if (name == "source-alpha")
		return graphics::render::pass::BlendFactor::SourceAlpha;
	else if (name == "one-minus-source-alpha")
		return graphics::render::pass::BlendFactor::OneMinus_SourceAlpha;
	else if (name == "destination-alpha")
		return graphics::render::pass::BlendFactor::DestinationAlpha;
	else if (name == "one-minus-destination-alpha")
		return graphics::render::pass::BlendFactor::OneMinus_DestinationAlpha;

	else if (name == "constant-color")
		return graphics::render::pass::BlendFactor::ConstantColor;
	else if (name == "one-minus-constant-color")
		return graphics::render::pass::BlendFactor::OneMinus_ConstantColor;
	else if (name == "constant-alpha")
		return graphics::render::pass::BlendFactor::ConstantAlpha;
	else if (name == "one-minus-constant-alpha")
		return graphics::render::pass::BlendFactor::OneMinus_ConstantAlpha;

	else if (name == "source-one-color")
		return graphics::render::pass::BlendFactor::SourceOneColor;
	else if (name == "one-minus-source-one-color")
		return graphics::render::pass::BlendFactor::OneMinus_SourceOneColor;
	else if (name == "source-one-alpha")
		return graphics::render::pass::BlendFactor::SourceOneAlpha;
	else if (name == "one-minus-source-one-alpha")
		return graphics::render::pass::BlendFactor::OneMinus_SourceOneAlpha;

	else //if (name == "source-alpha-saturate")
		return graphics::render::pass::BlendFactor::SourceAlphaSaturate;
}

graphics::render::pass::BlendEquationMode get_pass_blend_equation_mode(const script_tree::ArgumentNode &arg)
{
	auto name = arg
		.Get<ScriptType::Enumerable>()->Get();

	if (name == "add")
		return graphics::render::pass::BlendEquationMode::Add;
	else if (name == "subtract")
		return graphics::render::pass::BlendEquationMode::Subtract;
	else if (name == "reverse-subtract")
		return graphics::render::pass::BlendEquationMode::ReverseSubtract;
	else if (name == "min")
		return graphics::render::pass::BlendEquationMode::Min;
	else //if (name == "max")
		return graphics::render::pass::BlendEquationMode::Max;
}


/*
	Validator classes
*/

script_validator::ClassDefinition get_text_style_class()
{
	return ClassDefinition::Create("text-style")
		.AddProperty("background-color", ParameterType::Color)
		.AddProperty("decoration", {"underline"s, "line-through"s, "overline"s})
		.AddProperty("decoration-color", ParameterType::Color)
		.AddProperty("foreground-color", ParameterType::Color)
		.AddProperty("font-size", {"smaller"s, "larger"s})
		.AddProperty("font-style", {"bold"s, "italic"s, "bold-italic"s})
		.AddProperty("vertical-align", {"subscript"s, "superscript"s});
}

script_validator::ClassDefinition get_pass_class()
{
	return ClassDefinition::Create("pass")	
		.AddProperty("blending-factor", {pass_blend_factors, pass_blend_factors, pass_blend_factors, pass_blend_factors}, 2)
		.AddProperty("blending-equation-mode", {pass_blend_equation_modes, pass_blend_equation_modes}, 1)
		.AddProperty("iterations", ParameterType::Integer)
		.AddProperty("shader-program", ParameterType::String);
}


ClassDefinition get_gui_skin_class()
{
	auto disabled_style = ClassDefinition::Create("disabled", "text-style");
	auto enabled_style = ClassDefinition::Create("enabled", "text-style");
	auto focused_style = ClassDefinition::Create("focused", "text-style");
	auto hovered_style = ClassDefinition::Create("hovered", "text-style");
	auto pressed_style = ClassDefinition::Create("pressed", "text-style");

	auto text_pass = ClassDefinition::Create("text-pass", "pass");


	auto part = ClassDefinition::Create("part")
		.AddRequiredProperty("name", ParameterType::String)	
		.AddProperty("disabled", ParameterType::String)
		.AddProperty("enabled", ParameterType::String)
		.AddProperty("fill-color", ParameterType::Color)
		.AddProperty("focused", ParameterType::String)
		.AddProperty("hovered", ParameterType::String)
		.AddProperty("pressed", ParameterType::String)
		.AddProperty("scaling", ParameterType::Vector2);

	auto sound_part = ClassDefinition::Create("sound-part")
		.AddRequiredProperty("name", ParameterType::String)
		.AddRequiredProperty("sound", ParameterType::String);

	auto text_part = ClassDefinition::Create("text-part")
		.AddAbstractClass(std::move(get_text_style_class()))
		.AddClass(std::move(disabled_style))
		.AddClass(std::move(enabled_style))
		.AddClass(std::move(focused_style))
		.AddClass(std::move(hovered_style))
		.AddClass(std::move(pressed_style))

		.AddRequiredProperty("name", ParameterType::String)
		.AddRequiredProperty("text", ParameterType::String);

	return ClassDefinition::Create("skin")
		.AddClass(std::move(part))
		.AddClass(std::move(get_pass_class()))
		.AddClass(std::move(sound_part))
		.AddClass(std::move(text_part))
		.AddClass(std::move(text_pass))

		.AddRequiredProperty("type",
			{"button"s, "check-box"s, "group-box"s, "label"s,
			 "list-box"s, "mouse-cursor"s, "progress-bar"s, "radio-button"s,
			 "scroll-bar"s, "slider"s, "text-box"s, "tooltip"s})
		.AddProperty("name", ParameterType::String);
}

ClassDefinition get_gui_theme_class()
{
	return ClassDefinition::Create("theme")
		.AddClass(get_gui_skin_class())
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

graphics::fonts::text::TextBlockStyle create_text_style(const script_tree::ObjectNode &object)
{
	graphics::fonts::text::TextBlockStyle style;

	for (auto &property : object.Properties())
	{
		if (property.Name() == "background-color")
			style.BackgroundColor = property[0].Get<ScriptType::Color>()->Get();
		else if (property.Name() == "decoration")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "underline")
				style.Decoration = graphics::fonts::text::TextDecoration::Underline;
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "line-through")
				style.Decoration = graphics::fonts::text::TextDecoration::LineThrough;
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "overline")
				style.Decoration = graphics::fonts::text::TextDecoration::Overline;
		}
		else if (property.Name() == "decoration-color")
			style.DecorationColor = property[0].Get<ScriptType::Color>()->Get();
		else if (property.Name() == "foreground-color")
			style.ForegroundColor = property[0].Get<ScriptType::Color>()->Get();
		else if (property.Name() == "font-size")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "smaller")
				style.FontSize = graphics::fonts::text::TextBlockFontSize::Smaller;
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "larger")
				style.FontSize = graphics::fonts::text::TextBlockFontSize::Larger;
		}
		else if (property.Name() == "font-style")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "bold")
				style.FontStyle = graphics::fonts::text::TextFontStyle::Bold;
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "italic")
				style.FontStyle = graphics::fonts::text::TextFontStyle::Italic;
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "bold-italic")
				style.FontStyle = graphics::fonts::text::TextFontStyle::BoldItalic;
		}
		else if (property.Name() == "vertical-align")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "subscript")
				style.VerticalAlign = graphics::fonts::text::TextBlockVerticalAlign::Subscript;
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "superscript")
				style.VerticalAlign = graphics::fonts::text::TextBlockVerticalAlign::Superscript;
		}
	}

	return style;
}

graphics::render::Pass create_pass(const script_tree::ObjectNode &object,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	graphics::render::Pass pass;

	for (auto &property : object.Properties())
	{
		if (property.Name() == "blending-factor")
		{
			if (property.NumberOfArguments() == 4)
				pass.BlendingFactor(get_pass_blend_factor(property[0]), get_pass_blend_factor(property[1]),
									get_pass_blend_factor(property[2]), get_pass_blend_factor(property[3]));
			else
				pass.BlendingFactor(get_pass_blend_factor(property[0]), get_pass_blend_factor(property[1]));
		}
		else if (property.Name() == "blending-equation-mode")
		{
			if (property.NumberOfArguments() == 2)
				pass.BlendingEquationMode(get_pass_blend_equation_mode(property[0]), get_pass_blend_equation_mode(property[1]));
			else
				pass.BlendingEquationMode(get_pass_blend_equation_mode(property[0]));
		}
		else if (property.Name() == "iterations")
			pass.Iterations(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "shader-program")
			pass.RenderProgram(shader_program_manager.GetShaderProgram(property[0].Get<ScriptType::String>()->Get()));
	}

	return pass;
}


NonOwningPtr<GuiSkin> create_gui_skin(const script_tree::ObjectNode &object,
	GuiTheme &theme,
	graphics::materials::MaterialManager &material_manager,
	graphics::fonts::TextManager &text_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager,
	sounds::SoundManager &sound_manager)
{
	auto type_name = object
		.Property("type")[0]
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
				if (type_name == "button")
					return theme.CreateSkin<controls::GuiButton>();
				else if (type_name == "check-box")
					return theme.CreateSkin<controls::GuiCheckBox>();
				else if (type_name == "group-box")
					return theme.CreateSkin<controls::GuiGroupBox>();
				else if (type_name == "label")
					return theme.CreateSkin<controls::GuiLabel>();
				else if (type_name == "list-box")
					return theme.CreateSkin<controls::GuiListBox>();
				else if (type_name == "mouse-cursor")
					return theme.CreateSkin<controls::GuiMouseCursor>();
				else if (type_name == "progress-bar")
					return theme.CreateSkin<controls::GuiProgressBar>();
				else if (type_name == "radio-button")
					return theme.CreateSkin<controls::GuiRadioButton>();
				else if (type_name == "scroll-bar")
					return theme.CreateSkin<controls::GuiScrollBar>();
				else if (type_name == "slider")
					return theme.CreateSkin<controls::GuiSlider>();
				else if (type_name == "text-box")
					return theme.CreateSkin<controls::GuiTextBox>();
				else if (type_name == "tooltip")
					return theme.CreateSkin<controls::GuiTooltip>();
			}
			else //Named skin
			{
				if (type_name == "button")
					return theme.CreateSkin<controls::GuiButton>(std::move(name));
				else if (type_name == "check-box")
					return theme.CreateSkin<controls::GuiCheckBox>(std::move(name));
				else if (type_name == "group-box")
					return theme.CreateSkin<controls::GuiGroupBox>(std::move(name));
				else if (type_name == "label")
					return theme.CreateSkin<controls::GuiLabel>(std::move(name));
				else if (type_name == "list-box")
					return theme.CreateSkin<controls::GuiListBox>(std::move(name));
				else if (type_name == "mouse-cursor")
					return theme.CreateSkin<controls::GuiMouseCursor>(std::move(name));
				else if (type_name == "progress-bar")
					return theme.CreateSkin<controls::GuiProgressBar>(std::move(name));
				else if (type_name == "radio-button")
					return theme.CreateSkin<controls::GuiRadioButton>(std::move(name));
				else if (type_name == "scroll-bar")
					return theme.CreateSkin<controls::GuiScrollBar>(std::move(name));
				else if (type_name == "slider")
					return theme.CreateSkin<controls::GuiSlider>(std::move(name));
				else if (type_name == "text-box")
					return theme.CreateSkin<controls::GuiTextBox>(std::move(name));
				else if (type_name == "tooltip")
					return theme.CreateSkin<controls::GuiTooltip>(std::move(name));
			}

			return nullptr;
		}();

	if (skin)
	{
		for (auto &obj : object.Objects())
		{
			if (obj.Name() == "part")
			{
				auto part_name = obj
					.Property("name")[0]
					.Get<ScriptType::String>()->Get();

				skins::gui_skin::SkinPart part;

				for (auto &property : obj.Properties())
				{
					if (property.Name() == "disabled")
						part.Disabled = material_manager.GetMaterial(property[0].Get<ScriptType::String>()->Get());
					else if (property.Name() == "enabled")
						part.Enabled = material_manager.GetMaterial(property[0].Get<ScriptType::String>()->Get());
					else if (property.Name() == "fill-color")
						part.FillColor = property[0].Get<ScriptType::Color>()->Get();
					else if (property.Name() == "focused")
						part.Focused = material_manager.GetMaterial(property[0].Get<ScriptType::String>()->Get());
					else if (property.Name() == "hovered")
						part.Hovered = material_manager.GetMaterial(property[0].Get<ScriptType::String>()->Get());
					else if (property.Name() == "pressed")
						part.Pressed = material_manager.GetMaterial(property[0].Get<ScriptType::String>()->Get());
					else if (property.Name() == "scaling")
						part.Scaling = property[0].Get<ScriptType::Vector2>()->Get();
				}

				skin->AddPart(std::move(part_name), part);
			}
			else if (obj.Name() == "pass")
				skin->AddPass(create_pass(obj, shader_program_manager));
			else if (obj.Name() == "sound-part")
			{
				auto part_name = obj
					.Property("name")[0]
					.Get<ScriptType::String>()->Get();
				auto sound_name = obj
					.Property("sound")[0]
					.Get<ScriptType::String>()->Get();

				skins::gui_skin::SkinSoundPart sound_part;
				sound_part.Base = sound_manager.GetSound(sound_name);

				skin->AddSoundPart(std::move(part_name), sound_part);
			}
			else if (obj.Name() == "text-part")
			{
				auto part_name = obj
					.Property("name")[0]
					.Get<ScriptType::String>()->Get();
				auto text_name = obj
					.Property("text")[0]
					.Get<ScriptType::String>()->Get();

				skins::gui_skin::SkinTextPart text_part;
				text_part.Base = text_manager.GetText(text_name);

				for (auto &obj2 : obj.Objects())
				{
					if (obj2.Name() == "disabled")
						text_part.Disabled = create_text_style(obj2);
					else if (obj2.Name() == "enabled")
						text_part.Enabled = create_text_style(obj2);
					else if (obj2.Name() == "focused")
						text_part.Focused = create_text_style(obj2);
					else if (obj2.Name() == "hovered")
						text_part.Hovered = create_text_style(obj2);
					else if (obj2.Name() == "pressed")
						text_part.Pressed = create_text_style(obj2);
				}

				skin->AddTextPart(std::move(part_name), text_part);
			}
			else if (obj.Name() == "text-pass")
				skin->AddTextPass(create_pass(obj, shader_program_manager));
		}
	}

	return skin;
}

NonOwningPtr<GuiTheme> create_gui_theme(const script_tree::ObjectNode &object,
	GuiController &gui_controller, NonOwningPtr<graphics::scene::SceneManager> scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::fonts::TextManager &text_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager,
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
				create_gui_skin(obj, *theme,
					material_manager, text_manager, shader_program_manager, sound_manager);
		}
	}

	return theme;
}

void create_gui_themes(const ScriptTree &tree,
	GuiController &gui_controller, NonOwningPtr<graphics::scene::SceneManager> scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::fonts::TextManager &text_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager,
	sounds::SoundManager &sound_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "theme")
			create_gui_theme(object, gui_controller, scene_manager,
				material_manager, text_manager, shader_program_manager, sound_manager);
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
	Gui themes
	Creating from script
*/

void GuiThemeScriptInterface::CreateGuiThemes(std::string_view asset_name,
	GuiController &gui_controller, NonOwningPtr<graphics::scene::SceneManager> scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::fonts::TextManager &text_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager,
	sounds::SoundManager &sound_manager)
{
	if (Load(asset_name))
		detail::create_gui_themes(*tree_, gui_controller, scene_manager,
			material_manager, text_manager, shader_program_manager, sound_manager);
}

} //ion::script::interfaces