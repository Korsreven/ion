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

#include <optional>
#include <string>

#include "gui/skins/IonGuiSkin.h"
#include "gui/skins/IonGuiTheme.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace gui_script_interface;
using namespace gui;

namespace gui_script_interface::detail
{

const skins::GuiSkin* get_skin(GuiController &gui_controller, std::string_view name) noexcept
{
	auto active_theme = gui_controller.ActiveTheme();

	//Check active theme first
	if (active_theme)
	{
		if (auto skin = active_theme->GetSkin(name); skin)
			return skin.get();
	}
	
	//Check all other themes (if any)
	for (auto &theme : gui_controller.Themes())
	{
		if (&theme != active_theme)
		{
			if (auto skin = theme.GetSkin(name); skin)
				return skin.get();
		}
	}

	return nullptr;
}

const skins::GuiSkin* get_skin(GuiPanelContainer &container, std::string_view name) noexcept
{
	auto active_theme =
		[&]() -> skins::GuiTheme*
		{
			if (auto frame = container.ParentFrame(); frame)
				return frame->ActiveTheme();
			else
				return nullptr;
		}();

	//Check active theme first
	if (active_theme)
	{
		if (auto skin = active_theme->GetSkin(name); skin)
			return skin.get();
	}

	//Check gui controller themes
	if (auto frame = container.ParentFrame(); frame)
	{
		if (auto owner = frame->Owner(); owner)
			return get_skin(*owner, name);
	}
	
	return nullptr;
}


/*
	Validator classes
*/

ClassDefinition get_gui_class()
{
	return ClassDefinition::Create("gui", "component")
		.AddClass(get_gui_frame_class())
		.AddClass(get_gui_mouse_cursor_class())
		.AddClass(get_gui_tooltip_class())

		.AddProperty("active-mouse-cursor", ParameterType::String)
		.AddProperty("active-theme", ParameterType::String)
		.AddProperty("active-tooltip", ParameterType::String);
}

ClassDefinition get_gui_component_class()
{
	return ClassDefinition::Create("component")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("enabled", ParameterType::Boolean)
		.AddProperty("global-z-order", ParameterType::FloatingPoint)
		.AddProperty("visible", ParameterType::Boolean)
		.AddProperty("z-order", ParameterType::FloatingPoint);
}

ClassDefinition get_gui_frame_class()
{
	return ClassDefinition::Create("frame", "panel-container")
		.AddProperty("activated", ParameterType::Boolean)
		.AddProperty("active-theme", ParameterType::String)
		.AddProperty("focused", ParameterType::Boolean)
		.AddProperty("show", {"modeless"s, "modal"s});
}

ClassDefinition get_gui_panel_class()
{
	return ClassDefinition::Create("panel", "panel-container")
		.AddProperty("grid-layout", {ParameterType::Vector2, ParameterType::Integer, ParameterType::Integer})
		.AddProperty("tab-order", ParameterType::Integer);
}

ClassDefinition get_gui_panel_container_class()
{
	return ClassDefinition::Create("panel-container", "component")
		.AddClass(get_gui_panel_class())

		.AddClass(get_gui_button_class())
		.AddClass(get_gui_check_box_class())
		.AddClass(get_gui_group_box_class())
		.AddClass(get_gui_label_class())
		.AddClass(get_gui_list_box_class())
		.AddClass(get_gui_progress_bar_class())
		.AddClass(get_gui_radio_button_class())
		.AddClass(get_gui_scroll_bar_class())
		.AddClass(get_gui_slider_class())
		.AddClass(get_gui_text_box_class());
}


ClassDefinition get_gui_button_class()
{
	return ClassDefinition::Create("button", "control");
}

ClassDefinition get_gui_check_box_class()
{
	return ClassDefinition::Create("check-box", "control");
}

ClassDefinition get_gui_control_class()
{
	return ClassDefinition::Create("control", "component")
		.AddProperty("caption", ParameterType::String)
		.AddProperty("caption-layout", caption_layouts)
		.AddProperty("caption-margin", ParameterType::Vector2)
		.AddProperty("caption-padding", ParameterType::Vector2)
		.AddProperty("caption-size", ParameterType::Vector2)
		.AddProperty("enabled", ParameterType::Boolean)
		.AddProperty("focusable", ParameterType::Boolean)
		.AddProperty("focused", ParameterType::Boolean)
		.AddProperty("hit-box", {ParameterType::Vector2, ParameterType::Vector2})
		.AddProperty("size", ParameterType::Vector2)
		.AddProperty("skin", ParameterType::String)
		.AddProperty("tab-order", ParameterType::Integer)
		.AddProperty("tooltip", ParameterType::String);
}

ClassDefinition get_gui_group_box_class()
{
	return ClassDefinition::Create("group-box", "control");
}

ClassDefinition get_gui_label_class()
{
	return ClassDefinition::Create("label", "control");
}

ClassDefinition get_gui_list_box_class()
{
	return ClassDefinition::Create("list-box", "scrollable");
}

ClassDefinition get_gui_mouse_cursor_class()
{
	return ClassDefinition::Create("mouse-cursor", "control");
}

ClassDefinition get_gui_progress_bar_class()
{
	return ClassDefinition::Create("progress-bar", "control")
		.AddProperty("type", {"horizontal"s, "vertical"s});
}

ClassDefinition get_gui_radio_button_class()
{
	return ClassDefinition::Create("radio-button", "check-box");
}

ClassDefinition get_gui_scrollable_class()
{
	return ClassDefinition::Create("scrollable", "control")
		.AddProperty("scroll", ParameterType::Integer)
		.AddProperty("scroll-bar", ParameterType::String)
		.AddProperty("scroll-rate", ParameterType::Integer);
}

ClassDefinition get_gui_scroll_bar_class()
{
	return ClassDefinition::Create("scroll-bar", "slider");
}

ClassDefinition get_gui_slider_class()
{
	return ClassDefinition::Create("slider", "control")
		.AddProperty("flipped", ParameterType::Boolean)
		.AddProperty("percent", ParameterType::FloatingPoint)
		.AddProperty("position", ParameterType::Integer)
		.AddProperty("range", {ParameterType::Integer, ParameterType::Integer})
		.AddProperty("step-by-amount", ParameterType::Integer)
		.AddProperty("type", {"horizontal"s, "vertical"s});
}

ClassDefinition get_gui_text_box_class()
{
	return ClassDefinition::Create("text-box", "scrollable");
}

ClassDefinition get_gui_tooltip_class()
{
	return ClassDefinition::Create("tooltip", "label");
}


ScriptValidator get_gui_validator()
{
	return ScriptValidator::Create()
		.AddAbstractClass(get_gui_component_class())
		.AddAbstractClass(get_gui_control_class())
		.AddAbstractClass(get_gui_panel_container_class())
		.AddAbstractClass(get_gui_scrollable_class())

		.AddRequiredClass(get_gui_class());
}


/*
	Tree parsing
*/

void set_gui_properties(const script_tree::ObjectNode &object, GuiController &gui_controller)
{
	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "frame")
			create_gui_frame(obj, gui_controller);
		else if (obj.Name() == "mouse-cursor")
			create_gui_mouse_cursor(obj, gui_controller);
		else if (obj.Name() == "tooltip")
			create_gui_tooltip(obj, gui_controller);
	}

	set_component_properties(object, gui_controller);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "active-mouse-cursor")
			gui_controller.ActiveMouseCursor(property[0].Get<ScriptType::String>()->Get());
		else if (property.Name() == "active-theme")
			gui_controller.ActiveTheme(property[0].Get<ScriptType::String>()->Get());
		else if (property.Name() == "active-tooltip")
			gui_controller.ActiveTooltip(property[0].Get<ScriptType::String>()->Get());
	}
}

void set_component_properties(const script_tree::ObjectNode &object, GuiComponent &component)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "enabled")
			component.Enabled(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "global-z-order")
			component.GlobalZOrder(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "visible")
			component.Visible(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "z-order")
			component.ZOrder(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_panel_container_properties(const script_tree::ObjectNode &object, GuiPanelContainer &container)
{
	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "button")
			create_gui_button(obj, container);
		else if (obj.Name() == "check-box")
			create_gui_check_box(obj, container);
		else if (obj.Name() == "group-box")
			create_gui_group_box(obj, container);
		else if (obj.Name() == "label")
			create_gui_label(obj, container);
		else if (obj.Name() == "list-box")
			create_gui_list_box(obj, container);
		else if (obj.Name() == "progress-bar")
			create_gui_progress_bar(obj, container);
		else if (obj.Name() == "radio-button")
			create_gui_radio_button(obj, container);
		else if (obj.Name() == "scroll-bar")
			create_gui_scroll_bar(obj, container);
		else if (obj.Name() == "slider")
			create_gui_slider(obj, container);
		else if (obj.Name() == "text-box")
			create_gui_text_box(obj, container);
	}

	set_component_properties(object, container);
}


void set_control_properties(const script_tree::ObjectNode &object, controls::GuiControl &control)
{
	set_component_properties(object, control);

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	auto has_hit_boxes = !std::empty(control.HitBoxes());

	for (auto &property : object.Properties())
	{
		if (property.Name() == "caption")
			control.Caption(property[0].Get<ScriptType::String>()->Get());
		else if (property.Name() == "caption-layout")
		{
			auto layout = property[0]
				.Get<ScriptType::Enumerable>()->Get();

			if (layout == "outside-top-left")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideTopLeft);
			else if (layout == "outside-top-center")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideTopCenter);
			else if (layout == "outside-top-right")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideTopRight);
			else if (layout == "outside-left-top")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideLeftTop);
			else if (layout == "top-left")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::TopLeft);
			else if (layout == "top-center")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::TopCenter);
			else if (layout == "top-right")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::TopRight);
			else if (layout == "outside-right-top")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideRightTop);
			else if (layout == "outside-left-center")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideLeftCenter);
			else if (layout == "left")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::Left);
			else if (layout == "center")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::Center);
			else if (layout == "right")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::Right);
			else if (layout == "outside-right-center")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideRightCenter);
			else if (layout == "outside-left-bottom")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideLeftBottom);
			else if (layout == "bottom-left")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::BottomLeft);
			else if (layout == "bottom-center")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::BottomCenter);
			else if (layout == "bottom-right")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::BottomRight);
			else if (layout == "outside-right-bottom")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideRightBottom);
			else if (layout == "outside-bottom-left")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideBottomLeft);
			else if (layout == "outside-bottom-center")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideBottomCenter);
			else if (layout == "outside-bottom-right")
				control.CaptionLayout(controls::gui_control::ControlCaptionLayout::OutsideBottomRight);
		}
		else if (property.Name() == "caption-margin")
			control.CaptionMargin(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "caption-padding")
			control.CaptionPadding(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "caption-size")
			control.CaptionSize(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "enabled")
			control.Enabled(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "focusable")
			control.Focusable(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "focused")
			control.Focused(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "hit-box")
		{
			if (!has_hit_boxes)
				hit_boxes.push_back({
					property[0].Get<ScriptType::Vector2>()->Get(),
					property[1].Get<ScriptType::Vector2>()->Get()});
		}
		else if (property.Name() == "size")
			control.Size(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "skin")
		{
			if (auto owner = control.Owner(); owner)
			{
				if (auto skin = get_skin(*owner, property[0].Get<ScriptType::String>()->Get()); skin)
					control.Skin(*skin);
			}
		}
		else if (property.Name() == "tab-order")
			control.TabOrder(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "tooltip")
			control.Tooltip(property[0].Get<ScriptType::String>()->Get());
	}

	if (!std::empty(hit_boxes))
		control.HitBoxes(std::move(hit_boxes));
}

void set_label_properties(const script_tree::ObjectNode &object, controls::GuiLabel &label)
{
	set_control_properties(object, label);
	//No label specific properties yet
}

void set_scrollable_properties(const script_tree::ObjectNode &object, controls::GuiScrollable &scrollable)
{
	set_control_properties(object, scrollable);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "scroll")
			scrollable.Scroll(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "scroll-bar")
		{
			if (auto owner = scrollable.Owner(); owner)
				scrollable.AttachedScrollBar(owner->GetControlAs<controls::GuiScrollBar>(property[0].Get<ScriptType::String>()->Get()));
		}
		else if (property.Name() == "scroll-rate")
			scrollable.ScrollRate(property[0].Get<ScriptType::Integer>()->As<int>());
	}
}

void set_slider_properties(const script_tree::ObjectNode &object, controls::GuiSlider &slider)
{
	set_control_properties(object, slider);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "flipped")
			slider.Flipped(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "percent")
			slider.Percent(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "position")
			slider.Position(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "range")
			slider.Range(property[0].Get<ScriptType::Integer>()->As<int>(),
						 property[1].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "step-by-amount")
			slider.StepByAmount(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "type")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "horizontal")
				slider.Type(controls::gui_slider::SliderType::Horizontal);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "vertical")
				slider.Type(controls::gui_slider::SliderType::Vertical);
		}
	}
}


NonOwningPtr<GuiFrame> create_gui_frame(const script_tree::ObjectNode &object,
	GuiController &gui_controller)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto frame = gui_controller.CreateFrame(std::move(name));

	if (frame)
	{
		set_panel_container_properties(object, *frame);

		for (auto &property : object.Properties())
		{
			if (property.Name() == "activated")
				frame->Activated(property[0].Get<ScriptType::Boolean>()->Get());
			else if (property.Name() == "active-theme")
				frame->ActiveTheme(property[0].Get<ScriptType::String>()->Get());
			else if (property.Name() == "focused")
				frame->Focused(property[0].Get<ScriptType::Boolean>()->Get());
			else if (property.Name() == "show")
			{
				if (property[0].Get<ScriptType::Enumerable>()->Get() == "modeless")
					frame->Show(gui_frame::FrameMode::Modeless);
				else if (property[0].Get<ScriptType::Enumerable>()->Get() == "modal")
					frame->Show(gui_frame::FrameMode::Modal);
			}
		}
	}

	return frame;
}

NonOwningPtr<GuiPanel> create_gui_panel(const script_tree::ObjectNode &object,
	GuiFrame &frame)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto panel = frame.CreatePanel(std::move(name));

	if (panel)
	{
		set_panel_container_properties(object, *panel);

		for (auto &property : object.Properties())
		{
			if (property.Name() == "tab-order")
				panel->TabOrder(property[0].Get<ScriptType::Integer>()->As<int>());
		}
	}

	return panel;
}


NonOwningPtr<controls::GuiButton> create_gui_button(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto tooltip = std::optional<std::string>{};
	if (auto property = object
		.Property("tooltip")[0]
		.Get<ScriptType::String>(); property)
		tooltip = property->Get();

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	auto button =
		[&]() noexcept -> NonOwningPtr<controls::GuiButton>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateButton(std::move(name), *skin, std::move(size), std::move(caption), std::move(tooltip), std::move(hit_boxes));
			else //Default skin
				return container.CreateButton(std::move(name), std::move(size), std::move(caption), std::move(tooltip), std::move(hit_boxes));
		}();

	if (button)
	{
		set_control_properties(object, *button);
	}

	return button;
}

NonOwningPtr<controls::GuiCheckBox> create_gui_check_box(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto tooltip = std::optional<std::string>{};
	if (auto property = object
		.Property("tooltip")[0]
		.Get<ScriptType::String>(); property)
		tooltip = property->Get();

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	auto check_box =
		[&]() noexcept -> NonOwningPtr<controls::GuiCheckBox>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateCheckBox(std::move(name), *skin, std::move(size), std::move(caption), std::move(tooltip), std::move(hit_boxes));
			else //Default skin
				return container.CreateCheckBox(std::move(name), std::move(size), std::move(caption), std::move(tooltip), std::move(hit_boxes));
		}();

	if (check_box)
	{
		set_control_properties(object, *check_box);
	}

	return check_box;
}

NonOwningPtr<controls::GuiGroupBox> create_gui_group_box(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	auto group_box =
		[&]() noexcept -> NonOwningPtr<controls::GuiGroupBox>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateGroupBox(std::move(name), *skin, std::move(size), std::move(caption), std::move(hit_boxes));
			else //Default skin
				return container.CreateGroupBox(std::move(name), std::move(size), std::move(caption), std::move(hit_boxes));
		}();

	if (group_box)
	{
		set_control_properties(object, *group_box);
	}

	return group_box;
}

NonOwningPtr<controls::GuiLabel> create_gui_label(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	auto label =
		[&]() noexcept -> NonOwningPtr<controls::GuiLabel>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateLabel(std::move(name), *skin, std::move(size), std::move(caption), std::move(hit_boxes));
			else //Default skin
				return container.CreateLabel(std::move(name), std::move(size), std::move(caption), std::move(hit_boxes));
		}();

	if (label)
	{
		set_label_properties(object, *label);
	}

	return label;
}

NonOwningPtr<controls::GuiListBox> create_gui_list_box(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	auto list_box =
		[&]() noexcept -> NonOwningPtr<controls::GuiListBox>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateListBox(std::move(name), *skin, std::move(size), std::move(caption), std::move(hit_boxes));
			else //Default skin
				return container.CreateListBox(std::move(name), std::move(size), std::move(caption), std::move(hit_boxes));
		}();

	if (list_box)
	{
		set_scrollable_properties(object, *list_box);
	}

	return list_box;
}

NonOwningPtr<controls::GuiMouseCursor> create_gui_mouse_cursor(const script_tree::ObjectNode &object,
	GuiController &gui_controller)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto mouse_cursor =
		[&]() noexcept -> NonOwningPtr<controls::GuiMouseCursor>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(gui_controller, skin_name); skin)
				return gui_controller.CreateMouseCursor(std::move(name), *skin, std::move(size));
			else //Default skin
				return gui_controller.CreateMouseCursor(std::move(name), std::move(size));
		}();

	if (mouse_cursor)
	{
		set_control_properties(object, *mouse_cursor);
	}

	return mouse_cursor;
}

NonOwningPtr<controls::GuiProgressBar> create_gui_progress_bar(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto type_name = object
		.Property("type")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();

	controls::gui_progress_bar::ProgressBarType type = controls::gui_progress_bar::ProgressBarType::Horizontal;

	if (type_name == "vertical")
		type = controls::gui_progress_bar::ProgressBarType::Vertical;

	auto progress_bar =
		[&]() noexcept -> NonOwningPtr<controls::GuiProgressBar>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateProgressBar(std::move(name), *skin, std::move(size), std::move(caption), type);
			else //Default skin
				return container.CreateProgressBar(std::move(name), std::move(size), std::move(caption), type);
		}();

	if (progress_bar)
	{
		set_control_properties(object, *progress_bar);
	}

	return progress_bar;
}

NonOwningPtr<controls::GuiRadioButton> create_gui_radio_button(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto tooltip = std::optional<std::string>{};
	if (auto property = object
		.Property("tooltip")[0]
		.Get<ScriptType::String>(); property)
		tooltip = property->Get();

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	auto radio_button =
		[&]() noexcept -> NonOwningPtr<controls::GuiRadioButton>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateRadioButton(std::move(name), *skin, std::move(size), std::move(caption), std::move(tooltip), std::move(hit_boxes));
			else //Default skin
				return container.CreateRadioButton(std::move(name), std::move(size), std::move(caption), std::move(tooltip), std::move(hit_boxes));
		}();

	if (radio_button)
	{
		set_control_properties(object, *radio_button);
	}

	return radio_button;
}

NonOwningPtr<controls::GuiScrollBar> create_gui_scroll_bar(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto type_name = object
		.Property("type")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();

	controls::gui_slider::SliderType type = controls::gui_slider::SliderType::Horizontal;

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	if (type_name == "vertical")
		type = controls::gui_slider::SliderType::Vertical;

	auto scroll_bar =
		[&]() noexcept -> NonOwningPtr<controls::GuiScrollBar>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateScrollBar(std::move(name), *skin, std::move(size), std::move(caption), type, std::move(hit_boxes));
			else //Default skin
				return container.CreateScrollBar(std::move(name), std::move(size), std::move(caption), type, std::move(hit_boxes));
		}();

	if (scroll_bar)
	{
		set_slider_properties(object, *scroll_bar);
	}

	return scroll_bar;
}

NonOwningPtr<controls::GuiSlider> create_gui_slider(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto tooltip = std::optional<std::string>{};
	if (auto property = object
		.Property("tooltip")[0]
		.Get<ScriptType::String>(); property)
		tooltip = property->Get();

	auto type_name = object
		.Property("type")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();

	controls::gui_slider::SliderType type = controls::gui_slider::SliderType::Horizontal;

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	if (type_name == "vertical")
		type = controls::gui_slider::SliderType::Vertical;

	auto slider =
		[&]() noexcept -> NonOwningPtr<controls::GuiSlider>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateSlider(std::move(name), *skin, std::move(size), std::move(caption), std::move(tooltip), type, std::move(hit_boxes));
			else //Default skin
				return container.CreateSlider(std::move(name), std::move(size), std::move(caption), std::move(tooltip), type, std::move(hit_boxes));
		}();

	if (slider)
	{
		set_slider_properties(object, *slider);
	}

	return slider;
}

NonOwningPtr<controls::GuiTextBox> create_gui_text_box(const script_tree::ObjectNode &object,
	GuiPanelContainer &container)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	auto text_box =
		[&]() noexcept -> NonOwningPtr<controls::GuiTextBox>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateTextBox(std::move(name), *skin, std::move(size), std::move(caption), std::move(hit_boxes));
			else //Default skin
				return container.CreateTextBox(std::move(name), std::move(size), std::move(caption), std::move(hit_boxes));
		}();

	if (text_box)
	{
		set_scrollable_properties(object, *text_box);
	}

	return text_box;
}

NonOwningPtr<controls::GuiTooltip> create_gui_tooltip(const script_tree::ObjectNode &object,
	GuiController &gui_controller)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto skin_name = object
		.Property("skin")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto size = std::optional<graphics::utilities::Vector2>{};
	if (auto property = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>(); property)
		size = property->Get();

	auto caption = std::optional<std::string>{};
	if (auto property = object
		.Property("caption")[0]
		.Get<ScriptType::String>(); property)
		caption = property->Get();

	auto tooltip =
		[&]() noexcept -> NonOwningPtr<controls::GuiTooltip>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(gui_controller, skin_name); skin)
				return gui_controller.CreateTooltip(std::move(name), *skin, std::move(size));
			else //Default skin
				return gui_controller.CreateTooltip(std::move(name), std::move(size));
		}();

	if (tooltip)
	{
		set_label_properties(object, *tooltip);
	}

	return tooltip;
}


void create_gui(const ScriptTree &tree,
	GuiController &gui_controller)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "gui")
			set_gui_properties(object, gui_controller);
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