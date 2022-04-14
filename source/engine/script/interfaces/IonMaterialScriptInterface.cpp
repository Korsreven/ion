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
#include "graphics/utilities/IonAabb.h"

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
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto material = material_manager.CreateMaterial(std::move(name));

	if (material)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "ambient-color")
				material->AmbientColor(property[0].Get<ScriptType::Color>()->Get());
			else if (property.Name() == "crop")
				material->Crop(graphics::utilities::Aabb{property[0].Get<ScriptType::Vector2>()->Get(), property[1].Get<ScriptType::Vector2>()->Get()});
			else if (property.Name() == "diffuse-color")
				material->DiffuseColor(property[0].Get<ScriptType::Color>()->Get());
			else if (property.Name() == "diffuse-map")
			{
				if (auto texture = texture_manager.GetTexture(property[0].Get<ScriptType::String>()->Get()); texture)
					material->DiffuseMap(texture);
				else if (auto animation = animation_manager.GetAnimation(property[0].Get<ScriptType::String>()->Get()); animation)
					material->DiffuseMap(animation);
			}
			else if (property.Name() == "emissive-color")
				material->EmissiveColor(property[0].Get<ScriptType::Color>()->Get());
			else if (property.Name() == "flip-horizontal")
			{
				if (property[0].Get<ScriptType::Boolean>()->Get())
					material->FlipHorizontal();
			}
			else if (property.Name() == "flip-vertical")
			{
				if (property[0].Get<ScriptType::Boolean>()->Get())
					material->FlipVertical();
			}
			else if (property.Name() == "lighting-enabled")
				material->LightingEnabled(property[0].Get<ScriptType::Boolean>()->Get());
			else if (property.Name() == "normal-map")
			{
				if (auto texture = texture_manager.GetTexture(property[0].Get<ScriptType::String>()->Get()); texture)
					material->NormalMap(texture);
				else if (auto animation = animation_manager.GetAnimation(property[0].Get<ScriptType::String>()->Get()); animation)
					material->NormalMap(animation);
			}
			else if (property.Name() == "receive-shadows")
				material->ReceiveShadows(property[0].Get<ScriptType::Boolean>()->Get());
			else if (property.Name() == "repeat")
				material->Repeat(property[0].Get<ScriptType::Vector2>()->Get());
			else if (property.Name() == "shininess")
				material->Shininess(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
			else if (property.Name() == "specular-color")
				material->SpecularColor(property[0].Get<ScriptType::Color>()->Get());
			else if (property.Name() == "specular-map")
			{
				if (auto texture = texture_manager.GetTexture(property[0].Get<ScriptType::String>()->Get()); texture)
					material->SpecularMap(texture);
				else if (auto animation = animation_manager.GetAnimation(property[0].Get<ScriptType::String>()->Get()); animation)
					material->SpecularMap(animation);
			}
			else if (property.Name() == "tex-coords")
				material->TexCoords(property[0].Get<ScriptType::Vector2>()->Get(), property[1].Get<ScriptType::Vector2>()->Get());
		}
	}

	return material;
}

void create_materials(const ScriptTree &tree,
	MaterialManager &material_manager,
	graphics::textures::AnimationManager &animation_manager,
	graphics::textures::TextureManager &texture_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "material")
			create_material(object, material_manager, animation_manager, texture_manager);
	}
}

} //material_script_interface::detail


//Private

ScriptValidator MaterialScriptInterface::GetValidator() const
{
	return detail::get_material_validator();
}


/*
	Materials
	Creating from script
*/

void MaterialScriptInterface::CreateMaterials(std::string_view asset_name,
	MaterialManager &material_manager,
	graphics::textures::AnimationManager &animation_manager,
	graphics::textures::TextureManager &texture_manager)
{
	if (Load(asset_name))
		detail::create_materials(*tree_, material_manager, animation_manager, texture_manager);
}

} //ion::script::interfaces