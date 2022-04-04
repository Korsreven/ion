/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonTypeFaceScriptInterface.h
-------------------------------------------
*/

#ifndef ION_TYPE_FACE_SCRIPT_INTERFACE_H
#define ION_TYPE_FACE_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/fonts/IonTypeFace.h"
#include "graphics/fonts/IonTypeFaceManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

//Forward declarations
namespace graphics::fonts
{
	class FontManager;
}

namespace ion::script::interfaces
{
	namespace type_face_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_type_face_class();
		ScriptValidator get_type_face_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::fonts::TypeFace> create_type_face(const script_tree::ObjectNode &object,
			graphics::fonts::TypeFaceManager &type_face_manager,
			graphics::fonts::FontManager &font_manager);

		void create_type_faces(const ScriptTree &tree,
			graphics::fonts::TypeFaceManager &type_face_manager,
			graphics::fonts::FontManager &font_manager);
	} //type_face_script_interface::detail


	class TypeFaceScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			TypeFaceScriptInterface() = default;


			/*
				Type faces
				Creating from script
			*/

			//Create type faces from a script (or object file) with the given asset name
			void CreateTypeFaces(std::string_view asset_name,
				graphics::fonts::TypeFaceManager &type_face_manager,
				graphics::fonts::FontManager &font_manager);
	};
} //ion::script::interfaces

#endif