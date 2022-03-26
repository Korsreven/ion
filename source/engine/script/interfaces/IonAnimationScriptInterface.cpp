/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonAnimationScriptInterface.cpp
-------------------------------------------
*/

#include "IonAnimationScriptInterface.h"

#include <string>
#include "graphics/textures/IonFrameSequenceManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace animation_script_interface;
using namespace graphics::textures;

namespace animation_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_animation_class()
{
	return ClassDefinition::Create("animation")
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

ScriptValidator get_animation_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_animation_class());
}


/*
	Tree parsing
*/

NonOwningPtr<Animation> create_animation(const script_tree::ObjectNode &object,
	AnimationManager &animation_manager,
	FrameSequenceManager &frame_sequence_manage)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto animation = animation_manager.CreateAnimation(name, {}, {});

	if (animation)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}
	}

	return animation;
}

} //animation_script_interface::detail


//Private

ScriptValidator AnimationScriptInterface::GetValidator() const
{
	return detail::get_animation_validator();
}


/*
	Particle systems
	Creating from script
*/

void AnimationScriptInterface::CreateAnimations(std::string_view asset_name,
	AnimationManager &animation_manager,
	FrameSequenceManager &frame_sequence_manager)
{
	if (Load(asset_name))
	{
		for (auto &object : tree_->Objects())
			detail::create_animation(object, animation_manager, frame_sequence_manager);
	}
}

} //ion::script::interfaces