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

namespace ion::script::interfaces
{
	namespace gui_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_gui_frame_class();

		ScriptValidator get_gui_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<gui::GuiFrame> create_gui_frame(const script_tree::ObjectNode &object,
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