/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonTextScriptInterface.cpp
-------------------------------------------
*/

#include "IonTextScriptInterface.h"

#include <string>
#include "graphics/fonts/IonTypeFaceManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace text_script_interface;
using namespace graphics::fonts;

namespace text_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_text_class()
{
	return ClassDefinition::Create("text")
		.AddRequiredProperty("name", ParameterType::String)
		.AddRequiredProperty("type-face", ParameterType::String)
		.AddProperty("alignment", {"left"s, "center"s, "right"s})
		.AddProperty("area-size", ParameterType::Vector2)
		.AddProperty("content", ParameterType::String)
		.AddProperty("default-background-color", ParameterType::Color)
		.AddProperty("default-decoration", {"underline"s, "line-through"s, "overline"s})
		.AddProperty("default-decoration-color", ParameterType::Color)
		.AddProperty("default-font-style", {"bold"s, "italic"s, "bold-italic"s})
		.AddProperty("default-foreground-color", ParameterType::Color)
		.AddProperty("from-line", ParameterType::Integer)
		.AddProperty("formatting", {"none"s, "html"s})
		.AddProperty("line-height", ParameterType::FloatingPoint)
		.AddProperty("line-height-factor", ParameterType::FloatingPoint)
		.AddProperty("max-lines", ParameterType::Integer)
		.AddProperty("overflow", {"truncate"s, "truncate-ellipsis"s, "word-truncate"s, "word-wrap"s, "wrap"s})
		.AddProperty("padding", ParameterType::Vector2)
		.AddProperty("vertical-alignment", {"top"s, "middle"s, "bottom"s});
}

ScriptValidator get_text_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_text_class());
}


/*
	Tree parsing
*/

NonOwningPtr<Text> create_text(const script_tree::ObjectNode &object,
	TextManager &text_manager,
	TypeFaceManager &type_face_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto &content = object
		.Property("content")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto &type_face_name = object
		.Property("type-face")[0]
		.Get<ScriptType::String>()->Get();

	auto text = text_manager.CreateText(name, content, type_face_manager.GetTypeFace(type_face_name));

	if (text)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}
	}

	return text;
}

void create_texts(const ScriptTree &tree,
	TextManager &text_manager,
	TypeFaceManager &type_face_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "text")
			create_text(object, text_manager, type_face_manager);
	}
}

} //text_script_interface::detail


//Private

ScriptValidator TextScriptInterface::GetValidator() const
{
	return detail::get_text_validator();
}


/*
	Texts
	Creating from script
*/

void TextScriptInterface::CreateTexts(std::string_view asset_name,
	TextManager &text_manager,
	TypeFaceManager &type_face_manager)
{
	if (Load(asset_name))
		detail::create_texts(*tree_, text_manager, type_face_manager);
}

} //ion::script::interfaces