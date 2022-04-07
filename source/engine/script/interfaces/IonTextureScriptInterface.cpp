/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonTextureScriptInterface.cpp
-------------------------------------------
*/

#include "IonTextureScriptInterface.h"

#include <optional>
#include <string>

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace texture_script_interface;
using namespace graphics::textures;

namespace texture_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_texture_class()
{
	return ClassDefinition::Create("texture")
		.AddRequiredProperty("asset-name", ParameterType::String)
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("filter", {"nearest-neighbor"s, "bilinear"s})
		.AddProperty("mag-filter", {"nearest-neighbor"s, "bilinear"s})
		.AddProperty("min-filter", {"nearest-neighbor"s, "bilinear"s})
		.AddProperty("mip-filter", {"closest-match"s, "weighted-average"s})
		.AddProperty("s-wrap", {"clamp"s, "repeat"s})
		.AddProperty("t-wrap", {"clamp"s, "repeat"s})
		.AddProperty("wrap", {"clamp"s, "repeat"s});
}

ScriptValidator get_texture_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_texture_class());
}


/*
	Tree parsing
*/

NonOwningPtr<Texture> create_texture(const script_tree::ObjectNode &object,
	TextureManager &texture_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto &asset_name = object
		.Property("asset-name")[0]
		.Get<ScriptType::String>()->Get();
	auto &filter_name = object
		.Property("filter")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto &min_filter_name = object
		.Property("min-filter")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto &mag_filter_name = object
		.Property("mag-filter")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto &mip_filter_name = object
		.Property("mip-filter")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto &wrap_mode_name = object
		.Property("wrap")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto &s_wrap_mode_name = object
		.Property("s-wrap")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto &t_wrap_mode_name = object
		.Property("t-wrap")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();

	texture::TextureFilter min_filter = texture::TextureFilter::Bilinear;
	texture::TextureFilter mag_filter = texture::TextureFilter::Bilinear;

	if (filter_name == "nearest-neighbor")
		min_filter = mag_filter = texture::TextureFilter::NearestNeighbor;
	if (min_filter_name == "nearest-neighbor")
		min_filter = texture::TextureFilter::NearestNeighbor;
	if (mag_filter_name == "nearest-neighbor")
		mag_filter = texture::TextureFilter::NearestNeighbor;

	std::optional<texture::MipmapFilter> mip_filter;

	if (mip_filter_name == "closest-match")
		mip_filter = texture::MipmapFilter::ClosestMatch;
	else if (mip_filter_name == "weighted-average")
		mip_filter = texture::MipmapFilter::WeightedAverage;
	
	texture::TextureWrapMode s_wrap_mode = texture::TextureWrapMode::Clamp;
	texture::TextureWrapMode t_wrap_mode = texture::TextureWrapMode::Clamp;

	if (wrap_mode_name == "repeat")
		s_wrap_mode = t_wrap_mode = texture::TextureWrapMode::Repeat;
	if (s_wrap_mode_name == "repeat")
		s_wrap_mode = texture::TextureWrapMode::Repeat;
	if (t_wrap_mode_name == "repeat")
		t_wrap_mode = texture::TextureWrapMode::Repeat;

	return texture_manager.CreateTexture(name, asset_name, min_filter, mag_filter, mip_filter, s_wrap_mode, t_wrap_mode);
}

void create_textures(const ScriptTree &tree,
	TextureManager &texture_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "texture")
			create_texture(object, texture_manager);
	}
}

} //texture_script_interface::detail


//Private

ScriptValidator TextureScriptInterface::GetValidator() const
{
	return detail::get_texture_validator();
}


/*
	Textures
	Creating from script
*/

void TextureScriptInterface::CreateTextures(std::string_view asset_name,
	TextureManager &texture_manager)
{
	if (Load(asset_name))
		detail::create_textures(*tree_, texture_manager);
}

} //ion::script::interfaces