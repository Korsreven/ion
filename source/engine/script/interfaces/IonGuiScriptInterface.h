/*
-------------------------------------------
This source file is part of Ion Engine
- A fast and lightweight 2D game engine
- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonGuiScriptInterface.h
-------------------------------------------
*/

#ifndef ION_GUI_SCRIPT_INTERFACE_H
#define ION_GUI_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "gui/IonGuiController.h"
#include "gui/IonGuiFrame.h"
#include "gui/IonGuiPanel.h"
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
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

namespace gui::skins
{
	class GuiSkin;
	class GuiTheme;
}

namespace ion::script::interfaces
{
	namespace gui_script_interface::detail
	{
		inline static const Strings caption_layouts
		{
									"outside-top-left",		"outside-top-center",		"outside-top-right",
			"outside-left-top",		"top-left",				"top-center",				"top-right",			"outside-right-top",
			"outside-left-center",	"left",					"center",					"right",				"outside-right-center",
			"outside-left-bottom",	"bottom-left",			"bottom-center",			"bottom-right",			"outside-right-bottom",
									"outside-bottom-left",	"outside-bottom-center",	"outside-bottom-right"
		};

		const gui::skins::GuiSkin* get_skin(gui::GuiController &gui_controller, std::string_view name) noexcept;	
		const gui::skins::GuiSkin* get_skin(gui::GuiPanelContainer &container, std::string_view name) noexcept;


		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_gui_component_class();
		script_validator::ClassDefinition get_gui_frame_class();
		script_validator::ClassDefinition get_gui_panel_class();
		script_validator::ClassDefinition get_gui_panel_container_class();

		script_validator::ClassDefinition get_gui_button_class();
		script_validator::ClassDefinition get_gui_check_box_class();
		script_validator::ClassDefinition get_gui_control_class();
		script_validator::ClassDefinition get_gui_group_box_class();
		script_validator::ClassDefinition get_gui_label_class();
		script_validator::ClassDefinition get_gui_list_box_class();
		script_validator::ClassDefinition get_gui_mouse_cursor_class();
		script_validator::ClassDefinition get_gui_progress_bar_class();
		script_validator::ClassDefinition get_gui_radio_button_class();
		script_validator::ClassDefinition get_gui_scrollable_class();
		script_validator::ClassDefinition get_gui_scroll_bar_class();
		script_validator::ClassDefinition get_gui_slider_class();
		script_validator::ClassDefinition get_gui_text_box_class();
		script_validator::ClassDefinition get_gui_tooltip_class();

		ScriptValidator get_gui_validator();


		/*
			Tree parsing
		*/

		void set_component_properties(const script_tree::ObjectNode &object, gui::GuiComponent &component);
		void set_control_properties(const script_tree::ObjectNode &object, gui::controls::GuiControl &control);


		NonOwningPtr<gui::GuiFrame> create_gui_frame(const script_tree::ObjectNode &object,
			gui::GuiController &gui_controller);
		NonOwningPtr<gui::GuiPanel> create_gui_panel(const script_tree::ObjectNode &object,
			gui::GuiFrame &frame);

		NonOwningPtr<gui::controls::GuiButton> create_gui_button(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiCheckBox> create_gui_check_box(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiGroupBox> create_gui_group_box(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiLabel> create_gui_label(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiListBox> create_gui_list_box(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiMouseCursor> create_gui_mouse_cursor(const script_tree::ObjectNode &object,
			gui::GuiController &gui_controller);
		NonOwningPtr<gui::controls::GuiProgressBar> create_gui_progress_bar(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiRadioButton> create_gui_radio_button(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiScrollBar> create_scroll_bar(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiSlider> create_gui_slider(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiTextBox> create_gui_text_box(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container);
		NonOwningPtr<gui::controls::GuiTooltip> create_gui_tooltip(const script_tree::ObjectNode &object,
			gui::GuiController &gui_controller);

		void create_gui(const ScriptTree &tree,
			gui::GuiController &gui_controller);
	} //gui_theme_script_interface::detail


	class GuiScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			GuiScriptInterface() = default;


			/*
				Gui
				Creating from script
			*/

			//Create gui from a script (or object file) with the given asset name
			void CreateGui(std::string_view asset_name,
				gui::GuiController &gui_controller);
	};
} //ion::script::interfaces

#endif