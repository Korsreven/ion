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

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace scene_script_interface;
using namespace graphics::scene;

namespace scene_script_interface::detail
{

/*
	Validator classes
*/

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
	return ClassDefinition::Create("animation", "drawable-object");
}

ClassDefinition get_drawable_object_class()
{
	return ClassDefinition::Create("drawable-object", "movable-object");
}

ClassDefinition get_drawable_particle_system_class()
{
	return ClassDefinition::Create("particle-system", "drawable-object");
}

ClassDefinition get_drawable_text_class()
{
	return ClassDefinition::Create("text", "drawable-object");
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
	return ClassDefinition::Create("movable-object");
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