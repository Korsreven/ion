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

namespace ion::script::interfaces
{
	namespace type_face_script_interface::detail
	{
		NonOwningPtr<graphics::fonts::Font> get_font(std::string_view name, const ManagerRegister &managers) noexcept;


		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_type_face_class();
		ScriptValidator get_type_face_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::fonts::TypeFace> create_type_face(const script_tree::ObjectNode &object,
			graphics::fonts::TypeFaceManager &type_face_manager, const ManagerRegister &managers);

		void create_type_faces(const ScriptTree &tree,
			graphics::fonts::TypeFaceManager &type_face_manager, const ManagerRegister &managers);
	} //type_face_script_interface::detail


	//A class representing an interface to a type face script with a complete validation scheme
	//A type face script can load type faces from a script file into a type face manager
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
				graphics::fonts::TypeFaceManager &type_face_manager);

			//Create type faces from a script (or object file) with the given asset name
			void CreateTypeFaces(std::string_view asset_name,
				graphics::fonts::TypeFaceManager &type_face_manager, const ManagerRegister &managers);
	};
} //ion::script::interfaces

#endif