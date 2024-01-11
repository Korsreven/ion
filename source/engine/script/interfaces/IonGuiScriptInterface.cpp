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

#include "IonSceneScriptInterface.h"
#include "graphics/materials/IonMaterial.h"
#include "graphics/materials/IonMaterialManager.h"
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

NonOwningPtr<graphics::materials::Material> get_material(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &material_manager : managers.ObjectsOf<graphics::materials::MaterialManager>())
	{
		if (material_manager)
		{
			if (auto material = material_manager->GetMaterial(name); material)
				return material;
		}
	}

	return nullptr;
}


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

		.AddProperty("name", ParameterType::String) //Not required
		.AddProperty("active-mouse-cursor", ParameterType::String)
		.AddProperty("active-theme", ParameterType::String)
		.AddProperty("active-tooltip", ParameterType::String)
		.AddProperty("sounds-enabled", ParameterType::Boolean);
}

ClassDefinition get_gui_component_class()
{
	return ClassDefinition::Create("component")
		.AddClass(scene_script_interface::detail::get_scene_node_class())

		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("enabled", ParameterType::Boolean)
		.AddProperty("global-position", ParameterType::Vector2)
		.AddProperty("global-z-order", ParameterType::FloatingPoint)
		.AddProperty("position", ParameterType::Vector2)
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
	auto cell = ClassDefinition::Create("cell")
		.AddRequiredProperty("column", ParameterType::Integer)
		.AddRequiredProperty("row", ParameterType::Integer)
		.AddProperty("alignment", {"left"s, "center"s, "right"s})
		.AddProperty("attach", ParameterType::String)
		.AddProperty("vertical-alignment", {"top"s, "middle"s, "bottom"s});

	auto grid = ClassDefinition::Create("grid")
		.AddClass(std::move(cell))

		.AddRequiredProperty("columns", ParameterType::Integer)
		.AddRequiredProperty("rows", ParameterType::Integer)
		.AddProperty("size", ParameterType::Vector2)
		.AddProperty("size-percentage", ParameterType::Vector2);

	return ClassDefinition::Create("panel", "panel-container")
		.AddClass(std::move(grid))

		.AddProperty("tab-order", ParameterType::Integer);
}

ClassDefinition get_gui_panel_container_class()
{
	return ClassDefinition::Create("panel-container", "component")
		.AddClass(get_gui_panel_class())

		.AddClass(get_gui_button_class())
		.AddClass(get_gui_check_box_class())
		.AddClass(get_gui_group_box_class())
		.AddClass(get_gui_image_class())
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
	return ClassDefinition::Create("button", "control")
		.AddProperty("action", {button_action_types, ParameterType::String});
}

ClassDefinition get_gui_check_box_class()
{
	return ClassDefinition::Create("check-box", "control")
		.AddProperty("checked", ParameterType::Boolean);
}

ClassDefinition get_gui_control_class()
{
	return ClassDefinition::Create("control", "component")
		.AddProperty("caption", ParameterType::String)
		.AddProperty("caption-layout", control_caption_layouts)
		.AddProperty("caption-margin", ParameterType::Vector2)
		.AddProperty("caption-overflow", {"no-wrap"s, "no-wrap-ellipsis"s, "wrap"s})
		.AddProperty("caption-padding", ParameterType::Vector2)
		.AddProperty("caption-size", ParameterType::Vector2)
		.AddProperty("enabled", ParameterType::Boolean)
		.AddProperty("focusable", ParameterType::Boolean)
		.AddProperty("focused", ParameterType::Boolean)
		.AddProperty("hit-box", {ParameterType::Vector2, ParameterType::Vector2})
		.AddProperty("size", ParameterType::Vector2)
		.AddProperty("skin", ParameterType::String)
		.AddProperty("skin-part-color", {ParameterType::Color, ParameterType::String}, 1)
		.AddProperty("skin-part-opacity", {ParameterType::FloatingPoint, ParameterType::String}, 1)
		.AddProperty("tab-order", ParameterType::Integer)
		.AddProperty("tooltip", ParameterType::String);
}

ClassDefinition get_gui_group_box_class()
{
	return ClassDefinition::Create("group-box", "control")
		.AddProperty("attach");
}

ClassDefinition get_gui_image_class()
{
	return ClassDefinition::Create("image", "control")
		.AddProperty("fill-color", ParameterType::Color)
		.AddProperty("fill-opacity", ParameterType::FloatingPoint)
		.AddProperty("mode", {"fill"s, "fit"s})
		.AddProperty("source", {ParameterType::String, ParameterType::String, ParameterType::String, ParameterType::String, ParameterType::String}, 1);
}

ClassDefinition get_gui_label_class()
{
	return ClassDefinition::Create("label", "control");
}

ClassDefinition get_gui_list_box_class()
{
	return ClassDefinition::Create("list-box", "scrollable")
		.AddProperty("icon-column-width", ParameterType::FloatingPoint)
		.AddProperty("icon-layout", {"left"s, "right"s})
		.AddProperty("icon-max-size", ParameterType::Vector2)
		.AddProperty("icon-padding", ParameterType::Vector2)
		.AddProperty("item", {ParameterType::String, ParameterType::String}, 1)
		.AddProperty("item-height-factor", ParameterType::FloatingPoint)
		.AddProperty("item-index", ParameterType::Integer)
		.AddProperty("item-layout", {"left"s, "center"s, "right"s})
		.AddProperty("item-padding", ParameterType::Vector2)
		.AddProperty("selection-padding", ParameterType::Vector2)
		.AddProperty("show-icons", ParameterType::Boolean);
}

ClassDefinition get_gui_mouse_cursor_class()
{
	return ClassDefinition::Create("mouse-cursor", "control")
		.AddProperty("hot-spot", mouse_cursor_hot_spots);
}

ClassDefinition get_gui_progress_bar_class()
{
	return ClassDefinition::Create("progress-bar", "control")
		.AddProperty("flipped", ParameterType::Boolean)
		.AddProperty("interpolation-delay", ParameterType::FloatingPoint)
		.AddProperty("interpolation-time", ParameterType::FloatingPoint)
		.AddProperty("interpolation-type", {"forward"s, "backward"s, "bidirectional"s})
		.AddProperty("percent", ParameterType::FloatingPoint)
		.AddProperty("range", {ParameterType::FloatingPoint, ParameterType::FloatingPoint})
		.AddProperty("type", {"horizontal"s, "vertical"s})
		.AddProperty("value", ParameterType::FloatingPoint);
}

ClassDefinition get_gui_radio_button_class()
{
	return ClassDefinition::Create("radio-button", "check-box")
		.AddProperty("tag", ParameterType::Integer);
}

ClassDefinition get_gui_scrollable_class()
{
	return ClassDefinition::Create("scrollable", "control")
		.AddProperty("attach", ParameterType::String)
		.AddProperty("scroll", ParameterType::Integer)
		.AddProperty("scroll-rate", ParameterType::Integer);
}

ClassDefinition get_gui_scroll_bar_class()
{
	return ClassDefinition::Create("scroll-bar", "slider")
		.AddProperty("attach", ParameterType::String)
		.AddProperty("handle-size", {ParameterType::FloatingPoint, ParameterType::FloatingPoint});
}

ClassDefinition get_gui_slider_class()
{
	return ClassDefinition::Create("slider", "control")
		.AddProperty("flipped", ParameterType::Boolean)
		.AddProperty("percent", ParameterType::FloatingPoint)
		.AddProperty("range", {ParameterType::Integer, ParameterType::Integer})
		.AddProperty("large-step", ParameterType::Integer)
		.AddProperty("small-step", ParameterType::Integer)
		.AddProperty("step", ParameterType::Integer)
		.AddProperty("type", {"horizontal"s, "vertical"s})
		.AddProperty("value", ParameterType::Integer);
}

ClassDefinition get_gui_text_box_class()
{
	return ClassDefinition::Create("text-box", "scrollable")
		.AddProperty("character-set", {"ascii"s, "extended-ascii"s})
		.AddProperty("content", ParameterType::String)
		.AddProperty("cursor-blink-rate", ParameterType::FloatingPoint)
		.AddProperty("cursor-hold-percent", ParameterType::FloatingPoint)
		.AddProperty("cursor-position", ParameterType::Integer)
		.AddProperty("key-repeat-delay", ParameterType::FloatingPoint)
		.AddProperty("key-repeat-rate", ParameterType::FloatingPoint)
		.AddProperty("mask", ParameterType::String)
		.AddProperty("max-characters", ParameterType::Integer)
		.AddProperty("placeholder-content", ParameterType::String)
		.AddProperty("read-only", ParameterType::Boolean)
		.AddProperty("reveal-count", ParameterType::Integer)
		.AddProperty("text-layout", {"left"s, "center"s, "right"s})
		.AddProperty("text-mode", {"printable"s, "alpha-numeric"s, "alpha"s, "numeric"s})
		.AddProperty("text-padding", ParameterType::Vector2);
}

ClassDefinition get_gui_tooltip_class()
{
	return ClassDefinition::Create("tooltip", "control")
		.AddProperty("auto-size", ParameterType::Boolean)
		.AddProperty("fade-in-delay", ParameterType::FloatingPoint)
		.AddProperty("fade-in-time", ParameterType::FloatingPoint)
		.AddProperty("fade-out-delay", ParameterType::FloatingPoint)
		.AddProperty("fade-out-time", ParameterType::FloatingPoint)
		.AddProperty("follow-mouse-cursor", ParameterType::Boolean)
		.AddProperty("hold-time", ParameterType::FloatingPoint)
		.AddProperty("show", ParameterType::String);
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

void set_panel_grid_properties(const script_tree::ObjectNode &object, gui_panel::PanelGrid &grid,
	GuiPanel &panel)
{
	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "cell")
		{
			auto row = obj
				.Property("row")[0]
				.Get<ScriptType::Integer>()->As<int>();
			auto column = obj
				.Property("column")[0]
				.Get<ScriptType::Integer>()->As<int>();

			set_panel_grid_cell_properties(obj, grid[{row, column}], panel);
		}
	}

	for (auto &property : object.Properties())
	{
		if (property.Name() == "size-percentage")
			grid.SizePercentage(property[0].Get<ScriptType::Vector2>()->Get());
	}
}

void set_panel_grid_cell_properties(const script_tree::ObjectNode &object, gui_panel::GridCell &cell,
	GuiPanel &panel)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "alignment")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "left")
				cell.Alignment(gui_panel::GridCellAlignment::Left);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "center")
				cell.Alignment(gui_panel::GridCellAlignment::Center);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "right")
				cell.Alignment(gui_panel::GridCellAlignment::Right);
		}
		else if (property.Name() == "attach")
		{
			if (auto control = panel.GetControl(property[0].Get<ScriptType::String>()->Get()); control)
				cell.AttachControl(control);
		}
		else if (property.Name() == "vertical-alignment")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "top")
				cell.VerticalAlignment(gui_panel::GridCellVerticalAlignment::Top);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "middle")
				cell.VerticalAlignment(gui_panel::GridCellVerticalAlignment::Middle);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "bottom")
				cell.VerticalAlignment(gui_panel::GridCellVerticalAlignment::Bottom);
		}
	}
}

void set_gui_properties(const script_tree::ObjectNode &object, GuiController &gui_controller,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "frame")
			create_gui_frame(obj, gui_controller, scene_manager, managers);
		else if (obj.Name() == "mouse-cursor")
			create_gui_mouse_cursor(obj, gui_controller, scene_manager, managers);
		else if (obj.Name() == "tooltip")
			create_gui_tooltip(obj, gui_controller, scene_manager, managers);
	}

	set_component_properties(object, gui_controller, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "active-mouse-cursor")
			gui_controller.ActiveMouseCursor(property[0].Get<ScriptType::String>()->Get());
		else if (property.Name() == "active-theme")
			gui_controller.ActiveTheme(property[0].Get<ScriptType::String>()->Get());
		else if (property.Name() == "active-tooltip")
			gui_controller.ActiveTooltip(property[0].Get<ScriptType::String>()->Get());
		else if (property.Name() == "sounds-enabled")
			gui_controller.SoundsEnabled(property[0].Get<ScriptType::Boolean>()->Get());
	}
}

void set_component_properties(const script_tree::ObjectNode &object, GuiComponent &component,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "enabled")
			component.Enabled(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "global-position")
			component.GlobalPosition(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "global-z-order")
			component.GlobalZOrder(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "position")
			component.Position(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "visible")
			component.Visible(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "z-order")
			component.ZOrder(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "scene-node")
		{
			if (auto node = component.Node(); node)
				scene_script_interface::detail::set_scene_node_properties(obj, *node, scene_manager, managers);
		}
	}
}

void set_frame_properties(const script_tree::ObjectNode &object, GuiFrame &frame,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_panel_container_properties(object, frame, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "activated")
			frame.Activated(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "active-theme")
			frame.ActiveTheme(property[0].Get<ScriptType::String>()->Get());
		else if (property.Name() == "focused")
			frame.Focused(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "show")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "modeless")
				frame.Show(gui_frame::FrameMode::Modeless);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "modal")
				frame.Show(gui_frame::FrameMode::Modal);
		}
	}
}

void set_panel_properties(const script_tree::ObjectNode &object, GuiPanel &panel,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_panel_container_properties(object, panel, scene_manager, managers);

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "grid")
		{
			auto rows = obj
				.Property("rows")[0]
				.Get<ScriptType::Integer>()->As<int>();
			auto columns = obj
				.Property("columns")[0]
				.Get<ScriptType::Integer>()->As<int>();
			auto size = obj
				.Property("size")[0]
				.Get<ScriptType::Vector2>().value_or(vector2::Zero).Get();

			if (size != vector2::Zero)
				set_panel_grid_properties(obj, panel.GridLayout(rows, columns, size), panel);
			else
				set_panel_grid_properties(obj, panel.GridLayout(rows, columns), panel);
		}
	}

	for (auto &property : object.Properties())
	{
		if (property.Name() == "tab-order")
			panel.TabOrder(property[0].Get<ScriptType::Integer>()->As<int>());
	}
}

void set_panel_container_properties(const script_tree::ObjectNode &object, GuiPanelContainer &container,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "panel")
			create_gui_panel(obj, container, scene_manager, managers);

		//Controls
		else if (obj.Name() == "button")
			create_gui_button(obj, container, scene_manager, managers);
		else if (obj.Name() == "check-box")
			create_gui_check_box(obj, container, scene_manager, managers);
		else if (obj.Name() == "group-box")
			create_gui_group_box(obj, container, scene_manager, managers);
		else if (obj.Name() == "image")
			create_gui_image(obj, container, scene_manager, managers);
		else if (obj.Name() == "label")
			create_gui_label(obj, container, scene_manager, managers);
		else if (obj.Name() == "list-box")
			create_gui_list_box(obj, container, scene_manager, managers);
		else if (obj.Name() == "progress-bar")
			create_gui_progress_bar(obj, container, scene_manager, managers);
		else if (obj.Name() == "radio-button")
			create_gui_radio_button(obj, container, scene_manager, managers);
		else if (obj.Name() == "scroll-bar")
			create_gui_scroll_bar(obj, container, scene_manager, managers);
		else if (obj.Name() == "slider")
			create_gui_slider(obj, container, scene_manager, managers);
		else if (obj.Name() == "text-box")
			create_gui_text_box(obj, container, scene_manager, managers);
	}

	set_component_properties(object, container, scene_manager, managers);
}


void set_button_properties(const script_tree::ObjectNode &object, controls::GuiButton &button,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, button, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "action")
		{
			auto action = property[0]
				.Get<ScriptType::Enumerable>()->Get();

			if (action == "show-gui")
				button.AddAction({controls::gui_button::ButtonActionType::ShowGui, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "hide-gui")
				button.AddAction({controls::gui_button::ButtonActionType::HideGui, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "enable-gui")
				button.AddAction({controls::gui_button::ButtonActionType::EnableGui, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "disable-gui")
				button.AddAction({controls::gui_button::ButtonActionType::DisableGui, property[1].Get<ScriptType::String>()->Get()});

			else if (action == "show-frame")
				button.AddAction({controls::gui_button::ButtonActionType::ShowFrame, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "show-frame-modal")
				button.AddAction({controls::gui_button::ButtonActionType::ShowFrameModal, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "hide-frame")
				button.AddAction({controls::gui_button::ButtonActionType::HideFrame, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "enable-frame")
				button.AddAction({controls::gui_button::ButtonActionType::EnableFrame, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "disable-frame")
				button.AddAction({controls::gui_button::ButtonActionType::DisableFrame, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "focus-frame")
				button.AddAction({controls::gui_button::ButtonActionType::FocusFrame, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "defocus-frame")
				button.AddAction({controls::gui_button::ButtonActionType::DefocusFrame, property[1].Get<ScriptType::String>()->Get()});

			else if (action == "show-panel")
				button.AddAction({controls::gui_button::ButtonActionType::ShowPanel, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "hide-panel")
				button.AddAction({controls::gui_button::ButtonActionType::HidePanel, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "enable-panel")
				button.AddAction({controls::gui_button::ButtonActionType::EnablePanel, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "disable-panel")
				button.AddAction({controls::gui_button::ButtonActionType::DisablePanel, property[1].Get<ScriptType::String>()->Get()});

			else if (action == "show-control")
				button.AddAction({controls::gui_button::ButtonActionType::ShowControl, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "hide-control")
				button.AddAction({controls::gui_button::ButtonActionType::HideControl, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "enable-control")
				button.AddAction({controls::gui_button::ButtonActionType::EnableControl, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "disable-control")
				button.AddAction({controls::gui_button::ButtonActionType::DisableControl, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "focus-control")
				button.AddAction({controls::gui_button::ButtonActionType::FocusControl, property[1].Get<ScriptType::String>()->Get()});
			else if (action == "defocus-control")
				button.AddAction({controls::gui_button::ButtonActionType::DefocusControl, property[1].Get<ScriptType::String>()->Get()});
		}
	}
}

void set_check_box_properties(const script_tree::ObjectNode &object, controls::GuiCheckBox &check_box,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, check_box, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "checked")
			check_box.Checked(property[0].Get<ScriptType::Boolean>()->Get());
	}
}

void set_control_properties(const script_tree::ObjectNode &object, controls::GuiControl &control,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_component_properties(object, control, scene_manager, managers);

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
		else if (property.Name() == "caption-overflow")
		{
			auto overflow = property[0]
				.Get<ScriptType::Enumerable>()->Get();

			if (overflow == "no-wrap")
				control.CaptionOverflow(controls::gui_control::ControlCaptionOverflow::NoWrap);
			else if (overflow == "no-wrap-ellipsis")
				control.CaptionOverflow(controls::gui_control::ControlCaptionOverflow::NoWrapEllipsis);
			else if (overflow == "wrap")
				control.CaptionOverflow(controls::gui_control::ControlCaptionOverflow::Wrap);
		}
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

		//Already set in GuiControl constructor
		/*else if (property.Name() == "skin")
		{
			if (auto owner = control.Owner(); owner)
			{
				if (auto skin = get_skin(*owner, property[0].Get<ScriptType::String>()->Get()); skin)
					control.Skin(*skin);
			}
		}*/

		else if (property.Name() == "skin-part-color")
		{
			if (property.NumberOfArguments() == 2)
				control.SkinPartColor(property[0].Get<ScriptType::Color>()->Get(), property[1].Get<ScriptType::String>()->Get());
			else
				control.SkinPartColor(property[0].Get<ScriptType::Color>()->Get());
		}
		else if (property.Name() == "skin-part-opacity")
		{
			if (property.NumberOfArguments() == 2)
				control.SkinPartOpacity(property[0].Get<ScriptType::FloatingPoint>()->As<real>(), property[1].Get<ScriptType::String>()->Get());
			else
				control.SkinPartOpacity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		}

		else if (property.Name() == "tab-order")
			control.TabOrder(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "tooltip")
			control.Tooltip(property[0].Get<ScriptType::String>()->Get());
	}

	if (!std::empty(hit_boxes))
		control.HitBoxes(std::move(hit_boxes));
}

void set_group_box_properties(const script_tree::ObjectNode &object, controls::GuiGroupBox &group_box,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, group_box, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "attach")
			group_box.AddControl(property[0].Get<ScriptType::String>()->Get());
	}
}

void set_image_properties(const script_tree::ObjectNode &object, controls::GuiImage &image,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, image, scene_manager, managers);
	
	for (auto &property : object.Properties())
	{
		if (property.Name() == "fill-color")
			image.FillColor(property[0].Get<ScriptType::Color>()->Get());
		else if (property.Name() == "fill-opacity")
			image.FillOpacity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "mode")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "fill")
				image.Mode(controls::gui_image::ImageMode::Fill);	
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "fit")
				image.Mode(controls::gui_image::ImageMode::Fit);
		}
		else if (property.Name() == "source")
		{
			NonOwningPtr<graphics::materials::Material> enabled;
			NonOwningPtr<graphics::materials::Material> disabled;
			NonOwningPtr<graphics::materials::Material> focused;
			NonOwningPtr<graphics::materials::Material> pressed;
			NonOwningPtr<graphics::materials::Material> hovered;

			if (property.NumberOfArguments() >= 1)
				enabled = get_material(property[0].Get<ScriptType::String>()->Get(), managers);
			if (property.NumberOfArguments() >= 2)
				disabled = get_material(property[1].Get<ScriptType::String>()->Get(), managers);
			if (property.NumberOfArguments() >= 3)
				focused = get_material(property[2].Get<ScriptType::String>()->Get(), managers);
			if (property.NumberOfArguments() >= 4)
				pressed = get_material(property[3].Get<ScriptType::String>()->Get(), managers);
			if (property.NumberOfArguments() >= 5)
				hovered = get_material(property[4].Get<ScriptType::String>()->Get(), managers);

			image.Source(enabled, disabled, focused, pressed, hovered);
		}
	}
}

void set_label_properties(const script_tree::ObjectNode &object, controls::GuiLabel &label,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, label, scene_manager, managers);
	//No label specific properties yet
}

void set_list_box_properties(const script_tree::ObjectNode &object, controls::GuiListBox &list_box,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_scrollable_properties(object, list_box, scene_manager, managers);

	controls::gui_list_box::ListBoxItems items;

	for (auto &property : object.Properties())
	{
		if (property.Name() == "icon-column-width")
			list_box.IconColumnWidth(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "icon-layout")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "left")
				list_box.IconLayout(controls::gui_list_box::ListBoxIconLayout::Left);	
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "right")
				list_box.IconLayout(controls::gui_list_box::ListBoxIconLayout::Right);
		}
		else if (property.Name() == "icon-max-size")
			list_box.IconMaxSize(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "icon-padding")
			list_box.IconPadding(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "item")
		{
			if (property.NumberOfArguments() == 2)
				items.emplace_back(property[0].Get<ScriptType::String>()->Get(),
								   get_material(property[1].Get<ScriptType::String>()->Get(), managers));
			else
				items.emplace_back(property[0].Get<ScriptType::String>()->Get());
		}
		else if (property.Name() == "item-height-factor")
			list_box.ItemHeightFactor(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "item-index")
			list_box.ItemIndex(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "item-layout")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "left")
				list_box.ItemLayout(controls::gui_list_box::ListBoxItemLayout::Left);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "center")
				list_box.ItemLayout(controls::gui_list_box::ListBoxItemLayout::Center);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "right")
				list_box.ItemLayout(controls::gui_list_box::ListBoxItemLayout::Right);
		}
		else if (property.Name() == "item-padding")
			list_box.ItemPadding(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "selection-padding")
			list_box.SelectionPadding(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "show-icons")
			list_box.ShowIcons(property[0].Get<ScriptType::Boolean>()->Get());
	}

	if (!std::empty(items))
		list_box.AddItems(std::move(items));
}

void set_mouse_cursor_properties(const script_tree::ObjectNode &object, controls::GuiMouseCursor &mouse_cursor,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, mouse_cursor, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "hot-spot")
		{
			auto hot_spot = property[0]
				.Get<ScriptType::Enumerable>()->Get();

			if (hot_spot == "top-left")
				mouse_cursor.HotSpot(controls::gui_mouse_cursor::MouseCursorHotSpot::TopLeft);
			else if (hot_spot == "top-center")
				mouse_cursor.HotSpot(controls::gui_mouse_cursor::MouseCursorHotSpot::TopCenter);
			else if (hot_spot == "top-right")
				mouse_cursor.HotSpot(controls::gui_mouse_cursor::MouseCursorHotSpot::TopRight);
			else if (hot_spot == "left")
				mouse_cursor.HotSpot(controls::gui_mouse_cursor::MouseCursorHotSpot::Left);
			else if (hot_spot == "center")
				mouse_cursor.HotSpot(controls::gui_mouse_cursor::MouseCursorHotSpot::Center);
			else if (hot_spot == "right")
				mouse_cursor.HotSpot(controls::gui_mouse_cursor::MouseCursorHotSpot::Right);
			else if (hot_spot == "bottom-left")
				mouse_cursor.HotSpot(controls::gui_mouse_cursor::MouseCursorHotSpot::BottomLeft);
			else if (hot_spot == "bottom-center")
				mouse_cursor.HotSpot(controls::gui_mouse_cursor::MouseCursorHotSpot::BottomCenter);
			else if (hot_spot == "bottom-right")
				mouse_cursor.HotSpot(controls::gui_mouse_cursor::MouseCursorHotSpot::BottomRight);
		}
	}
}

void set_progress_bar_properties(const script_tree::ObjectNode &object, controls::GuiProgressBar &progress_bar,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, progress_bar, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "flipped")
			progress_bar.Flipped(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "interpolation-delay")
			progress_bar.InterpolationDelay(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "interpolation-time")
			progress_bar.InterpolationTime(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "interpolation-type")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "forward")
				progress_bar.InterpolationType(controls::gui_progress_bar::BarInterpolationType::Forward);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "backward")
				progress_bar.InterpolationType(controls::gui_progress_bar::BarInterpolationType::Backward);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "bidirectional")
				progress_bar.InterpolationType(controls::gui_progress_bar::BarInterpolationType::Bidirectional);
		}
		else if (property.Name() == "percent")
			progress_bar.Percent(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "range")
			progress_bar.Range(property[0].Get<ScriptType::FloatingPoint>()->As<real>(),
							   property[1].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "type")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "horizontal")
				progress_bar.Type(controls::gui_progress_bar::ProgressBarType::Horizontal);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "vertical")
				progress_bar.Type(controls::gui_progress_bar::ProgressBarType::Vertical);
		}
		else if (property.Name() == "value")
			progress_bar.Value(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_radio_button_properties(const script_tree::ObjectNode &object, controls::GuiRadioButton &radio_button,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_check_box_properties(object, radio_button, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "tag")
			radio_button.Tag(property[0].Get<ScriptType::Integer>()->As<int>());
	}
}

void set_scrollable_properties(const script_tree::ObjectNode &object, controls::GuiScrollable &scrollable,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, scrollable, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "attach")
		{
			if (auto owner = scrollable.Owner(); owner)
				scrollable.AttachedScrollBar(owner->GetControlAs<controls::GuiScrollBar>(property[0].Get<ScriptType::String>()->Get()));
		}
		else if (property.Name() == "scroll")
			scrollable.Scroll(property[0].Get<ScriptType::Integer>()->As<int>());
		
		else if (property.Name() == "scroll-rate")
			scrollable.ScrollRate(property[0].Get<ScriptType::Integer>()->As<int>());
	}
}

void set_scroll_bar_properties(const script_tree::ObjectNode &object, controls::GuiScrollBar &scroll_bar,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_slider_properties(object, scroll_bar, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "attach")
		{
			if (auto owner = scroll_bar.Owner(); owner)
				scroll_bar.AttachedScrollable(owner->GetControlAs<controls::GuiScrollable>(property[0].Get<ScriptType::String>()->Get()));
		}
		else if (property.Name() == "handle-size")
			scroll_bar.HandleSize(property[0].Get<ScriptType::FloatingPoint>()->As<real>(),
								  property[1].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_slider_properties(const script_tree::ObjectNode &object, controls::GuiSlider &slider,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, slider, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "flipped")
			slider.Flipped(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "percent")
			slider.Percent(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "range")
			slider.Range(property[0].Get<ScriptType::Integer>()->As<int>(),
						 property[1].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "large-step")
			slider.LargeStep(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "small-step")
			slider.SmallStep(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "step")
			slider.Step(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "type")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "horizontal")
				slider.Type(controls::gui_slider::SliderType::Horizontal);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "vertical")
				slider.Type(controls::gui_slider::SliderType::Vertical);
		}
		else if (property.Name() == "value")
			slider.Value(property[0].Get<ScriptType::Integer>()->As<int>());
	}
}

void set_text_box_properties(const script_tree::ObjectNode &object, controls::GuiTextBox &text_box,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_scrollable_properties(object, text_box, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "character-set")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "ascii")
				text_box.CharacterSet(controls::gui_text_box::TextBoxCharacterSet::ASCII);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "extended-ascii")
				text_box.CharacterSet(controls::gui_text_box::TextBoxCharacterSet::ExtendedASCII);
		}
		else if (property.Name() == "content")
			text_box.Content(property[0].Get<ScriptType::String>()->Get());
		else if (property.Name() == "cursor-blink-rate")
			text_box.CursorBlinkRate(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "cursor-hold-percent")
			text_box.CursorHoldPercent(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "cursor-position")
			text_box.CursorPosition(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "key-repeat-delay")
			text_box.KeyRepeatDelay(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "key-repeat-rate")
			text_box.KeyRepeatRate(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "mask")
		{
			if (auto mask = property[0].Get<ScriptType::String>()->Get(); !std::empty(mask))
				text_box.Mask(mask.front());
		}
		else if (property.Name() == "max-characters")
			text_box.MaxCharacters(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "placeholder-content")
			text_box.PlaceholderContent(property[0].Get<ScriptType::String>()->Get());
		else if (property.Name() == "read-only")
			text_box.ReadOnly(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "reveal-count")
			text_box.RevealCount(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "text-layout")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "left")
				text_box.TextLayout(controls::gui_text_box::TextBoxTextLayout::Left);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "center")
				text_box.TextLayout(controls::gui_text_box::TextBoxTextLayout::Center);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "right")
				text_box.TextLayout(controls::gui_text_box::TextBoxTextLayout::Right);
		}
		else if (property.Name() == "text-mode")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "printable")
				text_box.TextMode(controls::gui_text_box::TextBoxTextMode::Printable);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "alpha-numeric")
				text_box.TextMode(controls::gui_text_box::TextBoxTextMode::AlphaNumeric);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "alpha")
				text_box.TextMode(controls::gui_text_box::TextBoxTextMode::Alpha);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "numeric")
				text_box.TextMode(controls::gui_text_box::TextBoxTextMode::Numeric);
		}
		else if (property.Name() == "text-padding")
			text_box.TextPadding(property[0].Get<ScriptType::Vector2>()->Get());
	}
}

void set_tooltip_properties(const script_tree::ObjectNode &object, controls::GuiTooltip &tooltip,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	set_control_properties(object, tooltip, scene_manager, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "auto-size")
			tooltip.AutoSize(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "fade-in-delay")
			tooltip.FadeInDelay(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "fade-in-time")
			tooltip.FadeInTime(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "fade-out-delay")
			tooltip.FadeOutDelay(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "fade-out-time")
			tooltip.FadeOutTime(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "follow-mouse-cursor")
			tooltip.FollowMouseCursor(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "hold-time")
			tooltip.HoldTime(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "show")
			tooltip.Show(property[0].Get<ScriptType::String>()->Get());
	}
}


NonOwningPtr<GuiFrame> create_gui_frame(const script_tree::ObjectNode &object,
	GuiController &gui_controller, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto frame = gui_controller.CreateFrame(std::move(name));

	if (frame)
		set_frame_properties(object, *frame, scene_manager, managers);

	return frame;
}

NonOwningPtr<GuiPanel> create_gui_panel(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto panel = container.CreatePanel(std::move(name));

	if (panel)
		set_panel_properties(object, *panel, scene_manager, managers);

	return panel;
}


NonOwningPtr<controls::GuiButton> create_gui_button(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_button_properties(object, *button, scene_manager, managers);

	return button;
}

NonOwningPtr<controls::GuiCheckBox> create_gui_check_box(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_check_box_properties(object, *check_box, scene_manager, managers);

	return check_box;
}

NonOwningPtr<controls::GuiGroupBox> create_gui_group_box(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_group_box_properties(object, *group_box, scene_manager, managers);

	return group_box;
}

NonOwningPtr<controls::GuiImage> create_gui_image(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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

	auto image =
		[&]() noexcept -> NonOwningPtr<controls::GuiImage>
		{
			//Specific skin (from active theme)
			if (auto skin = get_skin(container, skin_name); skin)
				return container.CreateImage(std::move(name), *skin, std::move(size), std::move(caption), std::move(hit_boxes));
			else //Default skin
				return container.CreateImage(std::move(name), std::move(size), std::move(caption), std::move(hit_boxes));
		}();

	if (image)
		set_image_properties(object, *image, scene_manager, managers);

	return image;
}

NonOwningPtr<controls::GuiLabel> create_gui_label(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_label_properties(object, *label, scene_manager, managers);

	return label;
}

NonOwningPtr<controls::GuiListBox> create_gui_list_box(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_list_box_properties(object, *list_box, scene_manager, managers);

	return list_box;
}

NonOwningPtr<controls::GuiMouseCursor> create_gui_mouse_cursor(const script_tree::ObjectNode &object,
	GuiController &gui_controller, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_mouse_cursor_properties(object, *mouse_cursor, scene_manager, managers);

	return mouse_cursor;
}

NonOwningPtr<controls::GuiProgressBar> create_gui_progress_bar(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_progress_bar_properties(object, *progress_bar, scene_manager, managers);

	return progress_bar;
}

NonOwningPtr<controls::GuiRadioButton> create_gui_radio_button(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_radio_button_properties(object, *radio_button, scene_manager, managers);

	return radio_button;
}

NonOwningPtr<controls::GuiScrollBar> create_gui_scroll_bar(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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

	controls::gui_slider::SliderType type = controls::gui_slider::SliderType::Vertical;

	auto hit_boxes = controls::gui_control::BoundingBoxes{};
	for (auto &property : object.Properties())
	{
		if (property.Name() == "hit-box")
			hit_boxes.push_back({
				property[0].Get<ScriptType::Vector2>()->Get(),
				property[1].Get<ScriptType::Vector2>()->Get()});
	}

	if (type_name == "horizontal")
		type = controls::gui_slider::SliderType::Horizontal;

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
		set_scroll_bar_properties(object, *scroll_bar, scene_manager, managers);

	return scroll_bar;
}

NonOwningPtr<controls::GuiSlider> create_gui_slider(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_slider_properties(object, *slider, scene_manager, managers);

	return slider;
}

NonOwningPtr<controls::GuiTextBox> create_gui_text_box(const script_tree::ObjectNode &object,
	GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_text_box_properties(object, *text_box, scene_manager, managers);

	return text_box;
}

NonOwningPtr<controls::GuiTooltip> create_gui_tooltip(const script_tree::ObjectNode &object,
	GuiController &gui_controller, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
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
		set_tooltip_properties(object, *tooltip, scene_manager, managers);

	return tooltip;
}


void create_gui(const ScriptTree &tree, GuiController &gui_controller,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "gui")
			set_gui_properties(object, gui_controller, scene_manager, managers);
	}
}

} //gui_script_interface::detail


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

void GuiScriptInterface::CreateGui(std::string_view asset_name, GuiController &gui_controller,
	graphics::scene::SceneManager &scene_manager)
{
	if (Load(asset_name))
		detail::create_gui(*tree_, gui_controller, scene_manager, Managers());
}

void GuiScriptInterface::CreateGui(std::string_view asset_name, GuiController &gui_controller,
	graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers)
{
	if (Load(asset_name))
		detail::create_gui(*tree_, gui_controller, scene_manager, managers);
}

} //ion::script::interfaces