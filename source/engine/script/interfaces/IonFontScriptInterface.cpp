/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonFontScriptInterface.cpp
-------------------------------------------
*/

#include "IonFontScriptInterface.h"

#include <string>
#include "graphics/fonts/IonFontManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace font_script_interface;
using namespace graphics::fonts;

namespace font_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_font_class()
{
	return ClassDefinition::Create("font")
		.AddRequiredProperty("asset-name", ParameterType::String)
		.AddRequiredProperty("name", ParameterType::String)
		.AddRequiredProperty("size", ParameterType::Integer)
		.AddProperty("character-set", {"ascii"s, "extended-ascii"s})
		.AddProperty("character-spacing", ParameterType::Integer)
		.AddProperty("face-index", ParameterType::Integer)
		.AddProperty("filter", {"nearest-neighbor"s, "bilinear"s})
		.AddProperty("mag-filter", {"nearest-neighbor"s, "bilinear"s})
		.AddProperty("min-filter", {"nearest-neighbor"s, "bilinear"s});
}

ScriptValidator get_font_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_font_class());
}


/*
	Tree parsing
*/

NonOwningPtr<Font> create_font(const script_tree::ObjectNode &object,
	FontManager &font_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto &asset_name = object
		.Property("asset-name")[0]
		.Get<ScriptType::String>()->Get();
	auto size = object
		.Property("size")[0]
		.Get<ScriptType::Integer>()->As<int>();
	auto face_index = object
		.Property("face-index")[0]
		.Get<ScriptType::Integer>().value_or(0).As<int>();
	auto character_spacing = object
		.Property("character-spacing")[0]
		.Get<ScriptType::Integer>().value_or(0).As<int>();
	auto &character_set_type = object
		.Property("character-set")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto &filter_type = object
		.Property("filter")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto &min_filter_type = object
		.Property("min-filter")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto &mag_filter_type = object
		.Property("mag-filter")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();

	font::FontCharacterSet character_set = font::FontCharacterSet::ASCII;	
	
	if (character_set_type == "extended-ascii")
		character_set = font::FontCharacterSet::ExtendedASCII;

	font::FontGlyphFilter min_filter = font::FontGlyphFilter::Bilinear;
	font::FontGlyphFilter mag_filter = font::FontGlyphFilter::Bilinear;

	if (filter_type == "nearest-neighbor")
		min_filter = mag_filter = font::FontGlyphFilter::NearestNeighbor;
	if (min_filter_type == "nearest-neighbor")
		min_filter = font::FontGlyphFilter::NearestNeighbor;
	if (mag_filter_type == "nearest-neighbor")
		mag_filter = font::FontGlyphFilter::NearestNeighbor;

	return font_manager.CreateFont(name, asset_name, size, face_index, character_spacing, character_set, min_filter, mag_filter);
}

void create_fonts(const ScriptTree &tree,
	FontManager &font_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "font")
			create_font(object, font_manager);
	}
}

} //font_script_interface::detail


//Private

ScriptValidator FontScriptInterface::GetValidator() const
{
	return detail::get_font_validator();
}


/*
	Fonts
	Creating from script
*/

void FontScriptInterface::CreateFonts(std::string_view asset_name,
	FontManager &font_manager)
{
	if (Load(asset_name))
		detail::create_fonts(*tree_, font_manager);
}

} //ion::script::interfaces