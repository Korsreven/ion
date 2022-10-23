/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonFontScriptInterface.h
-------------------------------------------
*/

#ifndef ION_FONT_SCRIPT_INTERFACE_H
#define ION_FONT_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/fonts/IonFont.h"
#include "graphics/fonts/IonFontManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

namespace ion::script::interfaces
{
	namespace font_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_font_class();
		ScriptValidator get_font_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::fonts::Font> create_font(const script_tree::ObjectNode &object,
			graphics::fonts::FontManager &font_manager);

		void create_fonts(const ScriptTree &tree,
			graphics::fonts::FontManager &font_manager);
	} //font_script_interface::detail


	//A class representing an interface to a font script with a complete validation scheme
	//A font script can load fonts from a script file into a font manager
	class FontScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			FontScriptInterface() = default;


			/*
				Fonts
				Creating from script
			*/

			//Creates fonts from a script (or object file) with the given asset name
			void CreateFonts(std::string_view asset_name,
				graphics::fonts::FontManager &font_manager);
	};
} //ion::script::interfaces

#endif