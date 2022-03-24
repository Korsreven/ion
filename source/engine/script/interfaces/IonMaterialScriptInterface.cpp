/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonMaterialScriptInterface.cpp
-------------------------------------------
*/

#include "IonMaterialScriptInterface.h"

#include <string>
#include "graphics/textures/IonAnimationManager.h"
#include "graphics/textures/IonTextureManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace material_script_interface;
using namespace graphics::materials;

namespace material_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_material_class()
{
	return ClassDefinition::Create("material")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("ambient-color", ParameterType::Color)
		.AddProperty("crop", {ParameterType::Vector2, ParameterType::Vector2})
		.AddProperty("diffuse-color", ParameterType::Color)
		.AddProperty("diffuse-map", ParameterType::String)
		.AddProperty("emissive-color", ParameterType::Color)
		.AddProperty("flip-horizontal", ParameterType::Boolean)
		.AddProperty("flip-vertical", ParameterType::Boolean)
		.AddProperty("lighting-enabled", ParameterType::Boolean)
		.AddProperty("normal-map", ParameterType::String)
		.AddProperty("receive-shadows", ParameterType::Boolean)
		.AddProperty("repeat", ParameterType::Vector2)
		.AddProperty("shininess", ParameterType::FloatingPoint)
		.AddProperty("specular-color", ParameterType::Color)
		.AddProperty("specular-map", ParameterType::String)
		.AddProperty("tex-coords", {ParameterType::Vector2, ParameterType::Vector2});
}

ScriptValidator get_material_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_material_class());
}


/*
	Tree parsing
*/

NonOwningPtr<Material> create_material(const script_tree::ObjectNode &object,
	MaterialManager &material_manager,
	graphics::textures::AnimationManager &animation_manager,
	graphics::textures::TextureManager &texture_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto material = material_manager.CreateMaterial(name, {}, {}, {}, {}, 0.0_r);

	if (material)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}
	}

	return material;
}

} //material_script_interface::detail


//Private

ScriptValidator MaterialScriptInterface::GetValidator() const
{
	return detail::get_material_validator();
}


/*
	Particle systems
	Creating from script
*/

void MaterialScriptInterface::CreateMaterials(std::string_view asset_name,
	MaterialManager &material_manager,
	graphics::textures::AnimationManager &animation_manager,
	graphics::textures::TextureManager &texture_manager)
{
	if (Load(asset_name))
	{
		for (auto &object : tree_->Objects())
			detail::create_material(object, material_manager, animation_manager, texture_manager);
	}
}

} //ion::script::interfaces