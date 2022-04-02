/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonTextScriptInterface.h
-------------------------------------------
*/

#ifndef ION_TEXT_SCRIPT_INTERFACE_H
#define ION_TEXT_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/fonts/IonText.h"
#include "graphics/fonts/IonTextManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"

//Forward declarations
namespace ion::graphics
{
	namespace fonts
	{
		struct TypeFaceManager;
	}
}

namespace ion::script::interfaces
{
	namespace text_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_text_class();
		ScriptValidator get_text_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::fonts::Text> create_text(const script_tree::ObjectNode &object,
			graphics::fonts::TextManager &text_manager,
			graphics::fonts::TypeFaceManager &type_face_manager);

		void create_texts(const ScriptTree &tree,
			graphics::fonts::TextManager &text_manager,
			graphics::fonts::TypeFaceManager &type_face_manager);
	} //text_script_interface::detail


	class TextScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			TextScriptInterface() = default;


			/*
				Texts
				Creating from script
			*/

			//Create texts from a script (or object file) with the given asset name
			void CreateTexts(std::string_view asset_name,
				graphics::fonts::TextManager &text_manager,
				graphics::fonts::TypeFaceManager &type_face_manager);
	};
} //ion::script::interfaces

#endif