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

graphics::scene::graph::animations::node_animation::MotionTechniqueType get_motion_technique_type(const script_tree::ArgumentNode &arg)
{
	auto name = arg
		.Get<ScriptType::Enumerable>()->Get();

	if (name == "cubic")
		return graphics::scene::graph::animations::node_animation::MotionTechniqueType::Cubic;
	else if (name == "exponential")
		return graphics::scene::graph::animations::node_animation::MotionTechniqueType::Exponential;
	else if (name == "linear")
		return graphics::scene::graph::animations::node_animation::MotionTechniqueType::Linear;
	else if (name == "logarithmic")
		return graphics::scene::graph::animations::node_animation::MotionTechniqueType::Logarithmic;
	else if (name == "sigmoid")
		return graphics::scene::graph::animations::node_animation::MotionTechniqueType::Sigmoid;
	else if (name == "sinh")
		return graphics::scene::graph::animations::node_animation::MotionTechniqueType::Sinh;
	else //if (name == "tanh")
		return graphics::scene::graph::animations::node_animation::MotionTechniqueType::Tanh;
}

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

ClassDefinition get_action_class()
{
	return ClassDefinition::Create("action")
		.AddRequiredProperty("duration", ParameterType::FloatingPoint)
		.AddRequiredProperty("type", {"flip-visibility"s, "flip-visibility-cascading"s, "show"s, "show-cascading"s, "hide"s, "hide-cascading"s,
							  "inherit-rotation"s, "inherit-scaling"s, "disinherit-rotation"s, "disinherit-scaling"s});
}

ClassDefinition get_frustum_class()
{
	return ClassDefinition::Create("frustum")
		.AddProperty("aspect-format", {"pan-and-scan"s, "letterbox"s, "windowbox"s})
		.AddProperty("aspect-ratio", ParameterType::FloatingPoint)
		.AddProperty("base-viewport-height", ParameterType::FloatingPoint)
		.AddProperty("clip-plane", {ParameterType::Vector2, ParameterType::Vector2})
		.AddProperty("far-clip-distance", ParameterType::FloatingPoint)
		.AddProperty("field-of-view", ParameterType::FloatingPoint)
		.AddProperty("near-clip-distance", ParameterType::FloatingPoint)
		.AddProperty("projection", {"orthographic"s, "perspective"s});
}

ClassDefinition get_pass_class()
{
	return ClassDefinition::Create("pass")
		.AddProperty("blending-factor", {pass_blend_factors, pass_blend_factors, pass_blend_factors, pass_blend_factors}, 2)
		.AddProperty("blending-equation-mode", {pass_blend_equation_modes, pass_blend_equation_modes}, 1)
		.AddProperty("iterations", ParameterType::Integer)
		.AddProperty("shader-program", ParameterType::String);
}

ClassDefinition get_rotation_class()
{
	return ClassDefinition::Create("rotation")
		.AddRequiredProperty("angle", ParameterType::FloatingPoint)
		.AddRequiredProperty("total-duration", ParameterType::FloatingPoint)
		.AddProperty("motion-technique", motion_technique_types)
		.AddProperty("start-time", ParameterType::FloatingPoint);
}

ClassDefinition get_scaling_class()
{
	return ClassDefinition::Create("scaling")
		.AddRequiredProperty("total-duration", ParameterType::FloatingPoint)
		.AddRequiredProperty("unit", ParameterType::Vector2)
		.AddProperty("motion-technique", {motion_technique_types, motion_technique_types}, 1)
		.AddProperty("start-time", ParameterType::FloatingPoint);
}

ClassDefinition get_translation_class()
{
	return ClassDefinition::Create("translation")
		.AddRequiredProperty("total-duration", ParameterType::FloatingPoint)
		.AddRequiredProperty("unit", ParameterType::Vector3)
		.AddProperty("motion-technique", {motion_technique_types, motion_technique_types, motion_technique_types}, 1)
		.AddProperty("start-time", ParameterType::FloatingPoint);
}


ClassDefinition get_border_class()
{
	return ClassDefinition::Create("border", "rectangle")
		.AddRequiredProperty("border-size", ParameterType::Vector2)
		.AddRequiredProperty("size", ParameterType::Vector2)
		.AddProperty("border-color", ParameterType::Color)
		.AddProperty("corner-color", {ParameterType::Color, ParameterType::Color, ParameterType::Color, ParameterType::Color}, 1)
		.AddProperty("corner-style", {"none"s, "square"s, "oblique"s})
		.AddProperty("side-color", {ParameterType::Color, ParameterType::Color, ParameterType::Color, ParameterType::Color}, 1);
}

ClassDefinition get_curve_class()
{
	return ClassDefinition::Create("curve", "shape")
		.AddRequiredProperty("control-point", ParameterType::Vector3)
		.AddProperty("smoothness", ParameterType::Integer)
		.AddProperty("thickness", ParameterType::FloatingPoint);
}

ClassDefinition get_ellipse_class()
{
	return ClassDefinition::Create("ellipse", "shape")
		.AddRequiredProperty("size", ParameterType::Vector2)
		.AddProperty("diameter", ParameterType::FloatingPoint)
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("radius", ParameterType::FloatingPoint)
		.AddProperty("rotation", ParameterType::FloatingPoint)
		.AddProperty("sides", ParameterType::Integer);
}

ClassDefinition get_line_class()
{
	return ClassDefinition::Create("line", "shape")
		.AddRequiredProperty("a", ParameterType::Vector3)
		.AddRequiredProperty("b", ParameterType::Vector3)
		.AddProperty("thickness", ParameterType::FloatingPoint);
}

ClassDefinition get_mesh_class()
{
	auto vertex = ClassDefinition::Create("vertex")
		.AddRequiredProperty("position", ParameterType::Vector3)
		.AddProperty("color", ParameterType::Color)
		.AddProperty("normal", ParameterType::Vector3)
		.AddProperty("tex-coord", ParameterType::Vector2);

	auto vertices = ClassDefinition::Create("vertices")
		.AddRequiredClass(std::move(vertex));

	return ClassDefinition::Create("mesh")
		.AddRequiredClass(std::move(vertices))

		.AddProperty("color", ParameterType::Color)
		.AddProperty("draw-mode", {"points"s, "lines"s, "line-loop"s, "line-strip"s, "triangles"s, "triangle-fan"s, "triangle-strip"s, "quads"s, "polygon"s})
		.AddProperty("include-bounding-volumes", ParameterType::Boolean)
		.AddProperty("material", ParameterType::String)
		.AddProperty("opacity", ParameterType::FloatingPoint)
		.AddProperty("show-wireframe", ParameterType::Boolean)
		.AddProperty("surface-material", ParameterType::String)
		.AddProperty("tex-coord-mode", {"manual"s, "auto"s})
		.AddProperty("vertex-color", ParameterType::Color)
		.AddProperty("vertex-opacity", ParameterType::FloatingPoint)
		.AddProperty("visible", ParameterType::Boolean);
}

ClassDefinition get_rectangle_class()
{
	return ClassDefinition::Create("rectangle", "shape")
		.AddRequiredProperty("size", ParameterType::Vector2)
		.AddProperty("height", ParameterType::FloatingPoint)
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("resize-to-fill", ParameterType::Vector2)
		.AddProperty("resize-to-fit", ParameterType::Vector2)
		.AddProperty("rotation", ParameterType::FloatingPoint)
		.AddProperty("width", ParameterType::FloatingPoint);
}

ClassDefinition get_shape_class()
{
	return ClassDefinition::Create("shape", "mesh")
		.AddRequiredProperty("color", ParameterType::Color)
		.AddProperty("fill-color", ParameterType::Color)
		.AddProperty("fill-opacity", ParameterType::FloatingPoint);
}

ClassDefinition get_sprite_class()
{
	return ClassDefinition::Create("sprite", "rectangle")
		.AddRequiredProperty("material", ParameterType::String)
		.AddProperty("auto-repeat", ParameterType::Boolean)
		.AddProperty("auto-size", ParameterType::Boolean)
		.AddProperty("color", ParameterType::Color)
		.AddProperty("crop", {ParameterType::Vector2, ParameterType::Vector2})
		.AddProperty("flip-horizontal", ParameterType::Boolean)
		.AddProperty("flip-vertical", ParameterType::Boolean)
		.AddProperty("repeat", ParameterType::Vector2)
		.AddProperty("tex-coords", {ParameterType::Vector2, ParameterType::Vector2});
}

ClassDefinition get_triangle_class()
{
	return ClassDefinition::Create("triangle", "shape")
		.AddRequiredProperty("a", ParameterType::Vector3)
		.AddRequiredProperty("b", ParameterType::Vector3)
		.AddRequiredProperty("c", ParameterType::Vector3);
}


ClassDefinition get_node_animation_class()
{
	return ClassDefinition::Create("node-animation")
		.AddClass(get_action_class())
		.AddClass(get_rotation_class())
		.AddClass(get_scaling_class())
		.AddClass(get_translation_class())
		.AddRequiredProperty("name", ParameterType::String);
}

ClassDefinition get_node_animation_group_class()
{
	return ClassDefinition::Create("node-animation-group")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("add", {ParameterType::String, ParameterType::FloatingPoint, ParameterType::Boolean}, 1);
}

ClassDefinition get_node_animation_timeline_class()
{
	return ClassDefinition::Create("node-animation-timeline")
		.AddProperty("attach", {ParameterType::String, ParameterType::FloatingPoint, ParameterType::Boolean}, 1)
		.AddProperty("attach-animation", {ParameterType::String, ParameterType::FloatingPoint, ParameterType::Boolean}, 1)
		.AddProperty("attach-animation-group", {ParameterType::String, ParameterType::FloatingPoint, ParameterType::Boolean}, 1)
		.AddProperty("name", ParameterType::String)
		.AddProperty("playback-rate", ParameterType::FloatingPoint)
		.AddProperty("repeat-count", ParameterType::Integer)
		.AddProperty("running", ParameterType::Boolean);
}

ClassDefinition get_scene_node_class()
{
	return ClassDefinition::Create("scene-node")

		//Class definitions already added to scene script validator
		//Classes could have been forward declared, but scene node is also used from GUI script interface
		.AddAbstractClass(get_drawable_object_class())
		.AddAbstractClass(get_movable_object_class())
		.AddClass(get_camera_class())
		.AddClass(get_drawable_animation_class())
		.AddClass(get_drawable_particle_system_class())
		.AddClass(get_drawable_text_class())
		.AddClass(get_light_class())
		.AddClass(get_model_class())
		.AddClass(get_movable_sound_class())
		.AddClass(get_movable_sound_listener_class())

		.AddClass(get_node_animation_class())
		.AddClass(get_node_animation_group_class())
		.AddClass(get_node_animation_timeline_class())
		.AddClass("scene-node")
		
		.AddProperty("attach", ParameterType::String)
		.AddProperty("derived-position", ParameterType::Vector3)
		.AddProperty("derived-rotation", ParameterType::FloatingPoint)
		.AddProperty("derived-scaling", ParameterType::Vector2)
		.AddProperty("direction", ParameterType::Vector2)
		.AddProperty("flip-visibility", {ParameterType::Boolean, ParameterType::Boolean}, 1)
		.AddProperty("inherit-rotation", ParameterType::Boolean)
		.AddProperty("inherit-scaling", ParameterType::Boolean)
		.AddProperty("initial-direction", ParameterType::Vector2)
		.AddProperty("name", ParameterType::String)
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("rotate", ParameterType::FloatingPoint)
		.AddProperty("rotation", ParameterType::FloatingPoint)
		.AddProperty("rotation-origin", {"parent"s, "local"s})
		.AddProperty("scale", ParameterType::Vector2)
		.AddProperty("scaling", ParameterType::Vector2)
		.AddProperty("translate", ParameterType::Vector3)
		.AddProperty("visible", {ParameterType::Boolean, ParameterType::Boolean}, 1);
}


ClassDefinition get_camera_class()
{
	return ClassDefinition::Create("camera", "movable-object")
		.AddClass(get_frustum_class())

		.AddProperty("base-viewport-height", ParameterType::FloatingPoint)
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("rotation", ParameterType::FloatingPoint);
}

ClassDefinition get_drawable_animation_class()
{
	return ClassDefinition::Create("drawable-animation", "drawable-object")
		.AddRequiredProperty("animation", ParameterType::String)
		.AddRequiredProperty("size", ParameterType::Vector2)
		.AddProperty("color", ParameterType::Color)
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("rotation", ParameterType::FloatingPoint);
}

ClassDefinition get_drawable_object_class()
{
	return ClassDefinition::Create("drawable-object", "movable-object")
		.AddClass(get_pass_class())
		.AddProperty("opacity", ParameterType::FloatingPoint);
}

ClassDefinition get_drawable_particle_system_class()
{
	return ClassDefinition::Create("drawable-particle-system", "drawable-object")
		.AddRequiredProperty("particle-system", ParameterType::String);
}

ClassDefinition get_drawable_text_class()
{
	return ClassDefinition::Create("drawable-text", "drawable-object")
		.AddRequiredProperty("text", ParameterType::String)
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("rotation", ParameterType::FloatingPoint);
}

ClassDefinition get_light_class()
{
	return ClassDefinition::Create("light", "movable-object")
		.AddProperty("ambient-color", ParameterType::Color)
		.AddProperty("attenuation", {ParameterType::FloatingPoint, ParameterType::FloatingPoint, ParameterType::FloatingPoint})
		.AddProperty("cast-shadows", ParameterType::Boolean)
		.AddProperty("cutoff", {ParameterType::FloatingPoint, ParameterType::FloatingPoint})
		.AddProperty("diffuse-color", ParameterType::Color)
		.AddProperty("direction", ParameterType::Vector3)
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("specular-color", ParameterType::Color)
		.AddProperty("type", {"point"s, "directional"s, "spotlight"s});
}

ClassDefinition get_model_class()
{
	return ClassDefinition::Create("model", "drawable-object")
		.AddAbstractClass(get_shape_class())
		.AddClass(get_border_class())
		.AddClass(get_curve_class())
		.AddClass(get_ellipse_class())
		.AddClass(get_line_class())
		.AddClass(get_mesh_class())
		.AddClass(get_rectangle_class())
		.AddClass(get_sprite_class())
		.AddClass(get_triangle_class());
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
	return ClassDefinition::Create("movable-sound", "movable-object")
		.AddRequiredProperty("sound", ParameterType::String)
		.AddProperty("paused", ParameterType::Boolean)
		.AddProperty("position", ParameterType::Vector3)
		.AddProperty("sound-channel-group", ParameterType::String);
}

ClassDefinition get_movable_sound_listener_class()
{
	return ClassDefinition::Create("movable-sound-listener", "movable-object")
		.AddRequiredProperty("sound-listener", ParameterType::String)
		.AddProperty("position", ParameterType::Vector3);
}


ScriptValidator get_scene_validator()
{
	return ScriptValidator::Create()
		.AddAbstractClass(get_drawable_object_class())
		.AddAbstractClass(get_movable_object_class())
		.AddClass(get_camera_class())
		.AddClass(get_drawable_animation_class())
		.AddClass(get_drawable_particle_system_class())
		.AddClass(get_drawable_text_class())
		.AddClass(get_light_class())
		.AddClass(get_model_class())
		.AddClass(get_movable_sound_class())
		.AddClass(get_movable_sound_listener_class())

		.AddClass(get_scene_node_class());
}


/*
	Tree parsing
*/

void set_frustum_properties(const script_tree::ObjectNode &object, graphics::render::Frustum &frustum)
{
	for (auto &property : object.Properties())
	{
	}
}

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
	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_node_animation_group_properties(const script_tree::ObjectNode &object, graph::animations::NodeAnimationGroup &animation_group)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_node_animation_timeline_properties(const script_tree::ObjectNode &object, graph::animations::NodeAnimationTimeline &timeline)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_scene_node_properties(const script_tree::ObjectNode &object, graph::SceneNode &parent_node)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}


void set_camera_properties(const script_tree::ObjectNode &object, Camera &camera)
{
	set_movable_object_properties(object, camera);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_drawable_animation_properties(const script_tree::ObjectNode &object, DrawableAnimation &animation)
{
	set_drawable_object_properties(object, animation);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_drawable_object_properties(const script_tree::ObjectNode &object, DrawableObject &drawable_object)
{
	set_movable_object_properties(object, drawable_object);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_drawable_particle_system_properties(const script_tree::ObjectNode &object, DrawableParticleSystem &particle_system)
{
	set_drawable_object_properties(object, particle_system);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_drawable_text_properties(const script_tree::ObjectNode &object, DrawableText &text)
{
	set_drawable_object_properties(object, text);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_light_properties(const script_tree::ObjectNode &object, Light &light)
{
	set_movable_object_properties(object, light);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_model_properties(const script_tree::ObjectNode &object, Model &model)
{
	set_drawable_object_properties(object, model);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_movable_object_properties(const script_tree::ObjectNode &object, MovableObject &movable_object)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_movable_sound_properties(const script_tree::ObjectNode &object, MovableSound &sound)
{
	set_movable_object_properties(object, sound);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_movable_sound_listener_properties(const script_tree::ObjectNode &object, MovableSoundListener &sound_listener)
{
	set_movable_object_properties(object, sound_listener);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}


graphics::render::Frustum create_frustum(const script_tree::ObjectNode &object)
{
	graphics::render::Frustum frustum;
	set_frustum_properties(object, frustum);
	return frustum;
}

graphics::render::Pass create_pass(const script_tree::ObjectNode &object,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	graphics::render::Pass pass;
	set_pass_properties(object, pass, shader_program_manager);
	return pass;
}

		
NonOwningPtr<graph::SceneNode> create_scene_node(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node,
	graphics::scene::SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager)
{
	auto node = parent_node.CreateChildNode();

	if (node)
	{
		for (auto &obj : object.Objects())
		{
			if (obj.Name() == "camera")
				create_camera(obj, scene_manager);
			else if (obj.Name() == "drawable-animation")
				create_drawable_animation(obj, scene_manager);
			else if (obj.Name() == "drawable-particle-system")
				create_drawable_particle_system(obj, scene_manager);
			else if (obj.Name() == "drawable-text")
				create_drawable_text(obj, scene_manager);
			else if (obj.Name() == "light")
				create_light(obj, scene_manager);
			else if (obj.Name() == "model")
				create_model(obj, scene_manager, material_manager);
			else if (obj.Name() == "movable-sound")
				create_movable_sound(obj, scene_manager);
			else if (obj.Name() == "movable-sound-listener")
				create_movable_sound_listener(obj, scene_manager);

			else if (obj.Name() == "node-animation")
				create_node_animation(obj, *node);
			else if (obj.Name() == "node-animation-group")
				create_node_animation_group(obj, *node);
			else if (obj.Name() == "node-animation-timeline")
				create_node_animation_timeline(obj, *node);
			else if (obj.Name() == "scene-node")
				create_scene_node(obj, *node, scene_manager, material_manager);
		}

		set_scene_node_properties(object, *node);
	}

	return node;
}

NonOwningPtr<graph::animations::NodeAnimation> create_node_animation(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node)
{
	return nullptr;
}

NonOwningPtr<graph::animations::NodeAnimationGroup> create_node_animation_group(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node)
{
	return nullptr;
}

NonOwningPtr<graph::animations::NodeAnimationTimeline> create_node_animation_timeline(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node)
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
	graph::SceneNode &parent_node,
	SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "camera")
			create_camera(object, scene_manager);
		else if (object.Name() == "drawable-animation")
			create_drawable_animation(object, scene_manager);
		else if (object.Name() == "drawable-particle-system")
			create_drawable_particle_system(object, scene_manager);
		else if (object.Name() == "drawable-text")
			create_drawable_text(object, scene_manager);
		else if (object.Name() == "light")
			create_light(object, scene_manager);
		else if (object.Name() == "model")
			create_model(object, scene_manager, material_manager);
		else if (object.Name() == "movable-sound")
			create_movable_sound(object, scene_manager);
		else if (object.Name() == "movable-sound-listener")
			create_movable_sound_listener(object, scene_manager);

		else if (object.Name() == "scene-node")
			create_scene_node(object, parent_node, scene_manager, material_manager);
	}
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
	graph::SceneNode &parent_node,
	SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager)
{
	if (Load(asset_name))
		detail::create_scene(*tree_, parent_node, scene_manager, material_manager);
}

} //ion::script::interfaces