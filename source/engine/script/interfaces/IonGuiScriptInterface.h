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

//Forward declarations
namespace ion::graphics
{
	namespace scene
	{
		class SceneManager;
	}
}

//Forward declarations
namespace gui::skins
{
	class GuiSkin;
	class GuiTheme;
}

namespace ion::script::interfaces
{
	namespace gui_script_interface::detail
	{
		static inline const Strings button_action_types
		{
			//Controller actions
			"show-gui",
			"hide-gui",
			"enable-gui",
			"disable-gui",

			//Frame actions
			"show-frame",
			"show-frame-modal",
			"hide-frame",
			"enable-frame",
			"disable-frame",
			"focus-frame",
			"defocus-frame",

			//Panel actions
			"show-panel",
			"hide-panel",
			"enable-panel",
			"disable-panel",

			//Control actions
			"show-control",
			"hide-control",
			"enable-control",
			"disable-control",
			"focus-control",
			"defocus-control"
		};

		static inline const Strings control_caption_layouts
		{
									"outside-top-left",		"outside-top-center",		"outside-top-right",
			"outside-left-top",		"top-left",				"top-center",				"top-right",			"outside-right-top",
			"outside-left-center",	"left",					"center",					"right",				"outside-right-center",
			"outside-left-bottom",	"bottom-left",			"bottom-center",			"bottom-right",			"outside-right-bottom",
									"outside-bottom-left",	"outside-bottom-center",	"outside-bottom-right"
		};

		static inline const Strings mouse_cursor_hot_spots
		{
			"top-left",		"top-center",		"top-right",
			"left",			"center",			"right",
			"bottom-left",	"bottom-center",	"bottom-right"
		};


		NonOwningPtr<graphics::materials::Material> get_material(std::string_view name, const ManagerRegister &managers) noexcept;

		const gui::skins::GuiSkin* get_skin(gui::GuiController &gui_controller, std::string_view name) noexcept;
		const gui::skins::GuiSkin* get_skin(gui::GuiPanelContainer &container, std::string_view name) noexcept;


		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_gui_class();
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

		void set_gui_properties(const script_tree::ObjectNode &object, gui::GuiController &gui_controller,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_component_properties(const script_tree::ObjectNode &object, gui::GuiComponent &component,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_frame_properties(const script_tree::ObjectNode &object, gui::GuiFrame &frame,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_panel_properties(const script_tree::ObjectNode &object, gui::GuiPanel &panel,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_panel_container_properties(const script_tree::ObjectNode &object, gui::GuiPanelContainer &container,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);

		void set_button_properties(const script_tree::ObjectNode &object, gui::controls::GuiButton &button,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_check_box_properties(const script_tree::ObjectNode &object, gui::controls::GuiCheckBox &check_box,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_control_properties(const script_tree::ObjectNode &object, gui::controls::GuiControl &control,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_group_box_properties(const script_tree::ObjectNode &object, gui::controls::GuiGroupBox &group_box,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_label_properties(const script_tree::ObjectNode &object, gui::controls::GuiLabel &label,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_list_box_properties(const script_tree::ObjectNode &object, gui::controls::GuiListBox &list_box,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_mouse_cursor_properties(const script_tree::ObjectNode &object, gui::controls::GuiMouseCursor &mouse_cursor,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_progress_bar_properties(const script_tree::ObjectNode &object, gui::controls::GuiProgressBar &progress_bar,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_radio_button_properties(const script_tree::ObjectNode &object, gui::controls::GuiRadioButton &radio_button,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_scrollable_properties(const script_tree::ObjectNode &object, gui::controls::GuiScrollable &scrollable,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_scroll_bar_properties(const script_tree::ObjectNode &object, gui::controls::GuiScrollBar &scroll_bar,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_slider_properties(const script_tree::ObjectNode &object, gui::controls::GuiSlider &slider,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_text_box_properties(const script_tree::ObjectNode &object, gui::controls::GuiTextBox &text_box,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		void set_tooltip_properties(const script_tree::ObjectNode &object, gui::controls::GuiTooltip &tooltip,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);

		
		NonOwningPtr<gui::GuiFrame> create_gui_frame(const script_tree::ObjectNode &object,
			gui::GuiController &gui_controller, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::GuiPanel> create_gui_panel(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);

		NonOwningPtr<gui::controls::GuiButton> create_gui_button(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiCheckBox> create_gui_check_box(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiGroupBox> create_gui_group_box(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiLabel> create_gui_label(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiListBox> create_gui_list_box(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiMouseCursor> create_gui_mouse_cursor(const script_tree::ObjectNode &object,
			gui::GuiController &gui_controller, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiProgressBar> create_gui_progress_bar(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiRadioButton> create_gui_radio_button(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiScrollBar> create_gui_scroll_bar(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiSlider> create_gui_slider(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiTextBox> create_gui_text_box(const script_tree::ObjectNode &object,
			gui::GuiPanelContainer &container, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
		NonOwningPtr<gui::controls::GuiTooltip> create_gui_tooltip(const script_tree::ObjectNode &object,
			gui::GuiController &gui_controller, graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);

		void create_gui(const ScriptTree &tree, gui::GuiController &gui_controller,
			graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
	} //gui_script_interface::detail


	//A class representing an interface to a GUI script with a complete validation scheme
	//A GUI script can load GUI components from a script file into a GUI controller
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
			void CreateGui(std::string_view asset_name, gui::GuiController &gui_controller,
				graphics::scene::SceneManager &scene_manager);

			//Create gui from a script (or object file) with the given asset name
			void CreateGui(std::string_view asset_name, gui::GuiController &gui_controller,
				graphics::scene::SceneManager &scene_manager, const ManagerRegister &managers);
	};
} //ion::script::interfaces

#endif