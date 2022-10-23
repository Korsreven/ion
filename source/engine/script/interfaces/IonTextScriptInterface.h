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

namespace ion::script::interfaces
{
	namespace text_script_interface::detail
	{
		NonOwningPtr<graphics::fonts::TypeFace> get_type_face(std::string_view name, const ManagerRegister &managers) noexcept;


		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_text_class();
		ScriptValidator get_text_validator();


		/*
			Tree parsing
		*/

		void set_text_properties(const script_tree::ObjectNode &object, graphics::fonts::Text &text);


		NonOwningPtr<graphics::fonts::Text> create_text(const script_tree::ObjectNode &object,
			graphics::fonts::TextManager &text_manager, const ManagerRegister &managers);

		void create_texts(const ScriptTree &tree,
			graphics::fonts::TextManager &text_manager, const ManagerRegister &managers);
	} //text_script_interface::detail


	//A class representing an interface to a text script with a complete validation scheme
	//A text script can load texts from a script file into a text manager
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

			//Creates texts from a script (or object file) with the given asset name
			void CreateTexts(std::string_view asset_name,
				graphics::fonts::TextManager &text_manager);

			//Creates texts from a script (or object file) with the given asset name
			void CreateTexts(std::string_view asset_name,
				graphics::fonts::TextManager &text_manager, const ManagerRegister &managers);
	};
} //ion::script::interfaces

#endif