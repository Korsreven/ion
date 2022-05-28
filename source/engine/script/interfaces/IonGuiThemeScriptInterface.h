/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonGuiThemeScriptInterface.h
-------------------------------------------
*/

#ifndef ION_GUI_THEME_SCRIPT_INTERFACE_H
#define ION_GUI_THEME_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/fonts/IonText.h"
#include "graphics/render/IonPass.h"
#include "gui/IonGuiController.h"
#include "gui/skins/IonGuiSkin.h"
#include "gui/skins/IonGuiTheme.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace fonts
	{
		struct TextManager;
	}

	namespace materials
	{
		struct MaterialManager;
	}

	namespace scene
	{
		class SceneManager;
	}

	namespace shaders
	{
		class ShaderProgramManager;
	}
}

//Forward declarations
namespace ion::sounds
{
	class SoundManager;
}

namespace ion::script::interfaces
{
	namespace gui_theme_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_text_style_class();

		script_validator::ClassDefinition get_gui_skin_class();
		script_validator::ClassDefinition get_gui_theme_class();		

		ScriptValidator get_gui_theme_validator();


		/*
			Tree parsing
		*/

		graphics::fonts::text::TextBlockStyle create_text_style(const script_tree::ObjectNode &object);
		graphics::render::Pass create_pass(const script_tree::ObjectNode &object,
			graphics::shaders::ShaderProgramManager &shader_program_manager);
		

		NonOwningPtr<gui::skins::GuiSkin> create_gui_skin(const script_tree::ObjectNode &object,
			gui::skins::GuiTheme &theme,
			graphics::materials::MaterialManager &material_manager,
			graphics::fonts::TextManager &text_manager,
			graphics::shaders::ShaderProgramManager &shader_program_manager,
			sounds::SoundManager &sound_manager);
		NonOwningPtr<gui::skins::GuiTheme> create_gui_theme(const script_tree::ObjectNode &object,
			gui::GuiController &gui_controller, NonOwningPtr<graphics::scene::SceneManager> scene_manager,
			graphics::materials::MaterialManager &material_manager,
			graphics::fonts::TextManager &text_manager,
			graphics::shaders::ShaderProgramManager &shader_program_manager,
			sounds::SoundManager &sound_manager);

		void create_gui_themes(const ScriptTree &tree,
			gui::GuiController &gui_controller, NonOwningPtr<graphics::scene::SceneManager> scene_manager,
			graphics::materials::MaterialManager &material_manager,
			graphics::fonts::TextManager &text_manager,
			graphics::shaders::ShaderProgramManager &shader_program_manager,
			sounds::SoundManager &sound_manager);
	} //gui_theme_script_interface::detail


	class GuiThemeScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			GuiThemeScriptInterface() = default;


			/*
				Gui themes
				Creating from script
			*/

			//Create gui themes from a script (or object file) with the given asset name
			void CreateGuiThemes(std::string_view asset_name,
				gui::GuiController &gui_controller, NonOwningPtr<graphics::scene::SceneManager> scene_manager,
				graphics::materials::MaterialManager &material_manager,
				graphics::fonts::TextManager &text_manager,
				graphics::shaders::ShaderProgramManager &shader_program_manager,
				sounds::SoundManager &sound_manager);
	};
} //ion::script::interfaces

#endif