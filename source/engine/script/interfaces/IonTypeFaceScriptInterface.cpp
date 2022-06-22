/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonTypeFaceScriptInterface.cpp
-------------------------------------------
*/

#include "IonTypeFaceScriptInterface.h"

#include <string>
#include "graphics/fonts/IonFontManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace type_face_script_interface;
using namespace graphics::fonts;

namespace type_face_script_interface::detail
{

NonOwningPtr<Font> get_font(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &font_manager : managers.ObjectsOf<FontManager>())
	{
		if (font_manager)
		{
			if (auto font = font_manager->GetFont(name); font)
				return font;
		}
	}

	return nullptr;
}


/*
	Validator classes
*/

ClassDefinition get_type_face_class()
{
	return ClassDefinition::Create("type-face")
		.AddRequiredProperty("name", ParameterType::String)
		.AddRequiredProperty("regular", ParameterType::String)
		.AddProperty("bold", ParameterType::String)
		.AddProperty("italic", ParameterType::String)
		.AddProperty("bold-italic", ParameterType::String);
}

ScriptValidator get_type_face_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_type_face_class());
}


/*
	Tree parsing
*/

NonOwningPtr<TypeFace> create_type_face(const script_tree::ObjectNode &object,
	TypeFaceManager &type_face_manager, const ManagerRegister &managers)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto regular_name = object
		.Property("regular")[0]
		.Get<ScriptType::String>()->Get();
	auto bold_name = object
		.Property("bold")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto italic_name = object
		.Property("italic")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto bold_italic_name = object
		.Property("bold-italic")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	return type_face_manager.CreateTypeFace(std::move(name), get_font(regular_name, managers),
		get_font(bold_name, managers), get_font(italic_name, managers), get_font(bold_italic_name, managers));
}

void create_type_faces(const ScriptTree &tree,
	TypeFaceManager &type_face_manager, const ManagerRegister &managers)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "type-face")
			create_type_face(object, type_face_manager, managers);
	}
}

} //type_face_script_interface::detail


//Private

ScriptValidator TypeFaceScriptInterface::GetValidator() const
{
	return detail::get_type_face_validator();
}


/*
	Type faces
	Creating from script
*/

void TypeFaceScriptInterface::CreateTypeFaces(std::string_view asset_name,
	TypeFaceManager &type_face_manager)
{
	if (Load(asset_name))
		detail::create_type_faces(*tree_, type_face_manager, Managers());
}

void TypeFaceScriptInterface::CreateTypeFaces(std::string_view asset_name,
	TypeFaceManager &type_face_manager, const ManagerRegister &managers)
{
	if (Load(asset_name))
		detail::create_type_faces(*tree_, type_face_manager, managers);
}

} //ion::script::interfaces