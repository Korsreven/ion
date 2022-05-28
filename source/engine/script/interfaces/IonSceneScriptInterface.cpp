/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonSceneScriptInterface.cpp
-------------------------------------------
*/

#include "IonSceneScriptInterface.h"

#include <optional>
#include <string>

#include "graphics/materials/IonMaterialManager.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/animations/IonNodeAnimationManager.h"
#include "graphics/shaders/IonShaderProgramManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace scene_script_interface;
using namespace graphics::scene;

namespace scene_script_interface::detail
{

graphics::render::pass::BlendFactor get_pass_blend_factor(const script_tree::ArgumentNode &arg)
{
	auto name = arg
		.Get<ScriptType::Enumerable>()->Get();

	if (name == "zero")
		return graphics::render::pass::BlendFactor::Zero;
	else if (name == "one")
		return graphics::render::pass::BlendFactor::One;

	else if (name == "source-color")
		return graphics::render::pass::BlendFactor::SourceColor;
	else if (name == "one-minus-source-color")
		return graphics::render::pass::BlendFactor::OneMinus_SourceColor;
	else if (name == "destination-color")
		return graphics::render::pass::BlendFactor::DestinationColor;
	else if (name == "one-minus-destination-color")
		return graphics::render::pass::BlendFactor::OneMinus_DestinationColor;

	else if (name == "source-alpha")
		return graphics::render::pass::BlendFactor::SourceAlpha;
	else if (name == "one-minus-source-alpha")
		return graphics::render::pass::BlendFactor::OneMinus_SourceAlpha;
	else if (name == "destination-alpha")
		return graphics::render::pass::BlendFactor::DestinationAlpha;
	else if (name == "one-minus-destination-alpha")
		return graphics::render::pass::BlendFactor::OneMinus_DestinationAlpha;

	else if (name == "constant-color")
		return graphics::render::pass::BlendFactor::ConstantColor;
	else if (name == "one-minus-constant-color")
		return graphics::render::pass::BlendFactor::OneMinus_ConstantColor;
	else if (name == "constant-alpha")
		return graphics::render::pass::BlendFactor::ConstantAlpha;
	else if (name == "one-minus-constant-alpha")
		return graphics::render::pass::BlendFactor::OneMinus_ConstantAlpha;

	else if (name == "source-one-color")
		return graphics::render::pass::BlendFactor::SourceOneColor;
	else if (name == "one-minus-source-one-color")
		return graphics::render::pass::BlendFactor::OneMinus_SourceOneColor;
	else if (name == "source-one-alpha")
		return graphics::render::pass::BlendFactor::SourceOneAlpha;
	else if (name == "one-minus-source-one-alpha")
		return graphics::render::pass::BlendFactor::OneMinus_SourceOneAlpha;

	else //if (name == "source-alpha-saturate")
		return graphics::render::pass::BlendFactor::SourceAlphaSaturate;
}

graphics::render::pass::BlendEquationMode get_pass_blend_equation_mode(const script_tree::ArgumentNode &arg)
{
	auto name = arg
		.Get<ScriptType::Enumerable>()->Get();

	if (name == "add")
		return graphics::render::pass::BlendEquationMode::Add;
	else if (name == "subtract")
		return graphics::render::pass::BlendEquationMode::Subtract;
	else if (name == "reverse-subtract")
		return graphics::render::pass::BlendEquationMode::ReverseSubtract;
	else if (name == "min")
		return graphics::render::pass::BlendEquationMode::Min;
	else //if (name == "max")
		return graphics::render::pass::BlendEquationMode::Max;
}


/*
	Validator classes
*/

ClassDefinition get_pass_class()
{
	return ClassDefinition::Create("pass")	
		.AddProperty("blending-factor", {pass_blend_factors, pass_blend_factors, pass_blend_factors, pass_blend_factors}, 2)
		.AddProperty("blending-equation-mode", {pass_blend_equation_modes, pass_blend_equation_modes}, 1)
		.AddProperty("iterations", ParameterType::Integer)
		.AddProperty("shader-program", ParameterType::String);
}


ClassDefinition get_node_animation_class()
{
	return ClassDefinition::Create("node-animation");
}

ClassDefinition get_node_animation_group_class()
{
	return ClassDefinition::Create("node-animation-group");
}

ClassDefinition get_node_animation_timeline_class()
{
	return ClassDefinition::Create("node-animation-timeline");
}

ClassDefinition get_scene_node_class()
{
	return ClassDefinition::Create("scene-node");
}


ClassDefinition get_camera_class()
{
	return ClassDefinition::Create("camera", "movable-object");
}

ClassDefinition get_drawable_animation_class()
{
	return ClassDefinition::Create("animation", "drawable-object")
		.AddRequiredProperty("size", ParameterType::Vector2)
		.AddProperty("color", ParameterType::Color)
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("rotation", ParameterType::FloatingPoint)
		.AddProperty("animation", ParameterType::String);
}

ClassDefinition get_drawable_object_class()
{
	return ClassDefinition::Create("drawable-object", "movable-object")
		.AddClass(get_pass_class())
		.AddProperty("opacity", ParameterType::FloatingPoint);
}

ClassDefinition get_drawable_particle_system_class()
{
	return ClassDefinition::Create("particle-system", "drawable-object")
		.AddProperty("particle-system", ParameterType::String);
}

ClassDefinition get_drawable_text_class()
{
	return ClassDefinition::Create("text", "drawable-object")
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("rotation", ParameterType::FloatingPoint)
		.AddProperty("text", ParameterType::String);
}

ClassDefinition get_light_class()
{
	return ClassDefinition::Create("light", "movable-object");
}

ClassDefinition get_model_class()
{
	return ClassDefinition::Create("model", "drawable-object");
}

ClassDefinition get_movable_object_class()
{
	return ClassDefinition::Create("movable-object")
		.AddProperty("bounding-volume-colors", {ParameterType::Color, ParameterType::Color, ParameterType::Color})
		.AddProperty("bounding-volume-extent", {ParameterType::Vector2, ParameterType::Vector2})
		.AddProperty("name", ParameterType::String)
		.AddProperty("parent-node", ParameterType::String)
		.AddProperty("prefered-bounding-volume", {"bounding-box"s, "bounding-sphere"s})
		.AddProperty("query-flags", ParameterType::Integer)
		.AddProperty("query-mask", ParameterType::Integer)
		.AddProperty("show-bounding-volumes", ParameterType::Boolean)
		.AddProperty("visible", ParameterType::Boolean);
}

ClassDefinition get_movable_sound_class()
{
	return ClassDefinition::Create("sound", "movable-object");
}

ClassDefinition get_movable_sound_listener_class()
{
	return ClassDefinition::Create("sound-listener", "movable-object");
}


ScriptValidator get_scene_validator()
{
	return ScriptValidator::Create();
}


/*
	Tree parsing
*/

void set_pass_properties(const script_tree::ObjectNode &object, graphics::render::Pass &pass,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "blending-factor")
		{
			if (property.NumberOfArguments() == 4)
				pass.BlendingFactor(get_pass_blend_factor(property[0]), get_pass_blend_factor(property[1]),
									get_pass_blend_factor(property[2]), get_pass_blend_factor(property[3]));
			else
				pass.BlendingFactor(get_pass_blend_factor(property[0]), get_pass_blend_factor(property[1]));
		}
		else if (property.Name() == "blending-equation-mode")
		{
			if (property.NumberOfArguments() == 2)
				pass.BlendingEquationMode(get_pass_blend_equation_mode(property[0]), get_pass_blend_equation_mode(property[1]));
			else
				pass.BlendingEquationMode(get_pass_blend_equation_mode(property[0]));
		}
		else if (property.Name() == "iterations")
			pass.Iterations(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "shader-program")
			pass.RenderProgram(shader_program_manager.GetShaderProgram(property[0].Get<ScriptType::String>()->Get()));
	}
}


void set_node_animation_properties(const script_tree::ObjectNode &object, graph::animations::NodeAnimation &animation)
{

}

void set_node_animation_group_properties(const script_tree::ObjectNode &object, graph::animations::NodeAnimationGroup &animation_group)
{

}

void set_node_animation_timeline_properties(const script_tree::ObjectNode &object, graph::animations::NodeAnimationTimeline &timeline)
{

}

void set_scene_node_properties(const script_tree::ObjectNode &object, graph::SceneNode &scene_node)
{

}


void set_camera_properties(const script_tree::ObjectNode &object, Camera &camera)
{

}

void set_drawable_animation_properties(const script_tree::ObjectNode &object, DrawableAnimation &animation)
{

}

void set_drawable_object_properties(const script_tree::ObjectNode &object, DrawableObject &drawable_object)
{

}

void set_drawable_particle_system_properties(const script_tree::ObjectNode &object, DrawableParticleSystem &particle_system)
{

}

void set_drawable_text_properties(const script_tree::ObjectNode &object, DrawableText &text)
{

}

void set_light_properties(const script_tree::ObjectNode &object, Light &light)
{

}

void set_model_properties(const script_tree::ObjectNode &object, Model &model)
{

}

void set_movable_object_properties(const script_tree::ObjectNode &object, MovableObject &movable_object)
{

}

void set_movable_sound_properties(const script_tree::ObjectNode &object, MovableSound &sound)
{

}

void set_movable_sound_listener_properties(const script_tree::ObjectNode &object, MovableSoundListener &sound_listener)
{

}

		
NonOwningPtr<graph::SceneNode> create_scene_node(const script_tree::ObjectNode &object,
	graph::SceneNode &scene_node,
	graphics::materials::MaterialManager &material_manager)
{
	return nullptr;
}

NonOwningPtr<graph::animations::NodeAnimation> create_node_animation(const script_tree::ObjectNode &object,
	graph::SceneNode &scene_node)
{
	return nullptr;
}

NonOwningPtr<graph::animations::NodeAnimationGroup> create_node_animation_group(const script_tree::ObjectNode &object,
	graph::SceneNode &scene_node)
{
	return nullptr;
}

NonOwningPtr<graph::animations::NodeAnimationTimeline> create_node_animation_timeline(const script_tree::ObjectNode &object,
	graph::SceneNode &scene_node)
{
	return nullptr;
}


NonOwningPtr<Camera> create_camera(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	return nullptr;
}

NonOwningPtr<DrawableAnimation> create_drawable_animation(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	return nullptr;
}

NonOwningPtr<DrawableParticleSystem> create_drawable_particle_system(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	return nullptr;
}

NonOwningPtr<DrawableText> create_drawable_text(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	return nullptr;
}

NonOwningPtr<Light> create_light(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	return nullptr;
}

NonOwningPtr<Model> create_model(const script_tree::ObjectNode &object,
	SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager)
{
	return nullptr;
}

NonOwningPtr<MovableSound> create_movable_sound(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	return nullptr;
}

NonOwningPtr<MovableSoundListener> create_movable_sound_listener(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	return nullptr;
}


void create_scene(const ScriptTree &tree,
	graph::SceneNode &scene_node,
	SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager)
{

}

} //scene_script_interface::detail


//Private

ScriptValidator SceneScriptInterface::GetValidator() const
{
	return detail::get_scene_validator();
}


//Public

/*
	Scene
	Creating from script
*/

void SceneScriptInterface::CreateScene(std::string_view asset_name,
	graph::SceneNode &scene_node,
	SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager)
{
	if (Load(asset_name))
		detail::create_scene(*tree_, scene_node, scene_manager, material_manager);
}

} //ion::script::interfaces