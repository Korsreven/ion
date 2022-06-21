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
#include "graphics/fonts/IonTextManager.h"
#include "graphics/particles/IonParticleSystemManager.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/animations/IonNodeAnimationManager.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "graphics/utilities/IonAabb.h"
#include "sounds/IonSoundManager.h"
#include "utilities/IonMath.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace scene_script_interface;
using namespace graphics::scene;

namespace scene_script_interface::detail
{

NonOwningPtr<graphics::materials::Material> get_material(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &material_manager : managers.ObjectsOf<graphics::materials::MaterialManager>())
	{
		if (material_manager)
		{
			if (auto material = material_manager->GetMaterial(name); material)
				return material;
		}
	}

	return nullptr;
}

NonOwningPtr<graphics::particles::ParticleSystem> get_particle_system(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &particle_system_manager : managers.ObjectsOf<graphics::particles::ParticleSystemManager>())
	{
		if (particle_system_manager)
		{
			if (auto particle_system = particle_system_manager->GetParticleSystem(name); particle_system)
				return particle_system;
		}
	}

	return nullptr;
}

NonOwningPtr<graphics::shaders::ShaderProgram> get_shader_program(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &shader_program_manager : managers.ObjectsOf<graphics::shaders::ShaderProgramManager>())
	{
		if (shader_program_manager)
		{
			if (auto shader_program = shader_program_manager->GetShaderProgram(name); shader_program)
				return shader_program;
		}
	}

	return nullptr;
}

NonOwningPtr<sounds::Sound> get_sound(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &sound_manager : managers.ObjectsOf<sounds::SoundManager>())
	{
		if (sound_manager)
		{
			if (auto sound = sound_manager->GetSound(name); sound)
				return sound;
		}
	}

	return nullptr;
}

NonOwningPtr<sounds::SoundChannelGroup> get_sound_channel_group(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &sound_manager : managers.ObjectsOf<sounds::SoundManager>())
	{
		if (sound_manager)
		{
			if (auto sound_channel_group = sound_manager->GetSoundChannelGroup(name); sound_channel_group)
				return sound_channel_group;
		}
	}

	return nullptr;
}

NonOwningPtr<sounds::SoundListener> get_sound_listener(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &sound_manager : managers.ObjectsOf<sounds::SoundManager>())
	{
		if (sound_manager)
		{
			if (auto sound_listener = sound_manager->GetSoundListener(name); sound_listener)
				return sound_listener;
		}
	}

	return nullptr;
}

NonOwningPtr<graphics::fonts::Text> get_text(std::string_view name, const ManagerRegister &managers) noexcept
{
	for (auto &text_manager : managers.ObjectsOf<graphics::fonts::TextManager>())
	{
		if (text_manager)
		{
			if (auto text = text_manager->GetText(name); text)
				return text;
		}
	}

	return nullptr;
}


graph::animations::node_animation::MotionTechniqueType get_motion_technique_type(const script_tree::ArgumentNode &arg)
{
	auto name = arg
		.Get<ScriptType::Enumerable>()->Get();

	if (name == "cubic")
		return graph::animations::node_animation::MotionTechniqueType::Cubic;
	else if (name == "exponential")
		return graph::animations::node_animation::MotionTechniqueType::Exponential;
	else if (name == "linear")
		return graph::animations::node_animation::MotionTechniqueType::Linear;
	else if (name == "logarithmic")
		return graph::animations::node_animation::MotionTechniqueType::Logarithmic;
	else if (name == "sigmoid")
		return graph::animations::node_animation::MotionTechniqueType::Sigmoid;
	else if (name == "sinh")
		return graph::animations::node_animation::MotionTechniqueType::Sinh;
	else //if (name == "tanh")
		return graph::animations::node_animation::MotionTechniqueType::Tanh;
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


NonOwningPtr<graph::animations::NodeAnimation> get_node_animation(std::string_view name,
	graph::SceneNode &parent_node) noexcept
{
	if (auto node_animation = parent_node.GetAnimation(name); node_animation)
		return node_animation;
	else
	{
		for (auto &child_node : parent_node.BreadthFirstSearch())
		{
			if (node_animation = child_node.GetAnimation(name); node_animation)
				return node_animation;
		}

		return nullptr;
	}
}

NonOwningPtr<graph::animations::NodeAnimationGroup> get_node_animation_group(std::string_view name,
	graph::SceneNode &parent_node) noexcept
{
	if (auto node_animation_group = parent_node.GetAnimationGroup(name); node_animation_group)
		return node_animation_group;
	else
	{
		for (auto &child_node : parent_node.BreadthFirstSearch())
		{
			if (node_animation_group = child_node.GetAnimationGroup(name); node_animation_group)
				return node_animation_group;
		}

		return nullptr;
	}
}


/*
	Validator classes
*/

ClassDefinition get_action_class()
{
	return ClassDefinition::Create("action")
		.AddRequiredProperty("time", ParameterType::FloatingPoint)
		.AddRequiredProperty("type", {"flip-visibility"s, "flip-visibility-cascading"s, "show"s, "show-cascading"s, "hide"s, "hide-cascading"s,
							  "inherit-rotation"s, "inherit-scaling"s, "disinherit-rotation"s, "disinherit-scaling"s});
}

ClassDefinition get_frustum_class()
{
	return ClassDefinition::Create("frustum")
		.AddProperty("aspect-format", {"pan-and-scan"s, "letterbox"s, "windowbox"s})
		.AddProperty("aspect-ratio", {ParameterType::FloatingPoint, ParameterType::FloatingPoint}, 1)
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

ClassDefinition get_rotating_class()
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

ClassDefinition get_translating_class()
{
	return ClassDefinition::Create("translation")
		.AddRequiredProperty("total-duration", ParameterType::FloatingPoint)
		.AddRequiredProperty("unit", ParameterType::Vector3)
		.AddProperty("motion-technique", {motion_technique_types, motion_technique_types, motion_technique_types}, 1)
		.AddProperty("start-time", ParameterType::FloatingPoint);
}


ClassDefinition get_animated_sprite_class()
{
	return ClassDefinition::Create("animated-sprite", "sprite")
		.AddProperty("jump-backward", ParameterType::FloatingPoint)
		.AddProperty("jump-forward", ParameterType::FloatingPoint)
		.AddProperty("running", ParameterType::Boolean);
}

ClassDefinition get_border_class()
{
	return ClassDefinition::Create("border", "rectangle")
		.AddRequiredProperty("border-size", ParameterType::Vector2)
		.AddProperty("border-color", ParameterType::Color)
		.AddProperty("corner-color", {ParameterType::Color, ParameterType::Color, ParameterType::Color, ParameterType::Color}, 1)
		.AddProperty("corner-style", {"none"s, "square"s, "oblique"s})
		.AddProperty("side-color", {ParameterType::Color, ParameterType::Color, ParameterType::Color, ParameterType::Color}, 1);
}

ClassDefinition get_curve_class()
{
	return ClassDefinition::Create("curve", "shape")
		.AddRequiredProperty("control-point", ParameterType::Vector3)
		.AddProperty("p", {ParameterType::Integer, ParameterType::Vector3})
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
		.AddRequiredProperty("color", ParameterType::Color) //Make color required
		.AddProperty("fill-color", ParameterType::Color)
		.AddProperty("fill-opacity", ParameterType::FloatingPoint);
}

ClassDefinition get_sprite_class()
{
	return ClassDefinition::Create("sprite", "rectangle")
		.AddRequiredProperty("material", ParameterType::String)
		.AddProperty("auto-repeat", ParameterType::Boolean)
		.AddProperty("auto-size", ParameterType::Boolean)
		.AddProperty("color", ParameterType::Color) //Make color optional
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
		.AddClass(get_rotating_class())
		.AddClass(get_scaling_class())
		.AddClass(get_translating_class())
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
		.AddAbstractClass(get_drawable_object_class())
		.AddAbstractClass(get_movable_object_class())
		.AddClass(get_camera_class())
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

		.AddProperty("derived-position", ParameterType::Vector3)
		.AddProperty("derived-rotation", ParameterType::FloatingPoint)
		.AddProperty("derived-scaling", ParameterType::Vector2)
		.AddProperty("direction", ParameterType::Vector2)
		.AddProperty("flip-visibility", ParameterType::Boolean)
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
		.AddProperty("type", {"point"s, "directional"s, "spot"s});
}

ClassDefinition get_model_class()
{
	return ClassDefinition::Create("model", "drawable-object")
		.AddAbstractClass(get_shape_class())
		.AddClass(get_animated_sprite_class())
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
		.AddRequiredClass(get_scene_node_class());
}


/*
	Tree parsing
*/

void set_frustum_properties(const script_tree::ObjectNode &object, graphics::render::Frustum &frustum)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "aspect-format")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "pan-and-scan")
				frustum.AspectFormat(graphics::render::frustum::AspectRatioFormat::PanAndScan);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "letterbox")
				frustum.AspectFormat(graphics::render::frustum::AspectRatioFormat::Letterbox);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "windowbox")
				frustum.AspectFormat(graphics::render::frustum::AspectRatioFormat::Windowbox);
		}
		else if (property.Name() == "aspect-ratio")
		{
			if (property.NumberOfArguments() == 2)
				frustum.AspectRatio(property[0].Get<ScriptType::FloatingPoint>()->As<real>(),
									property[1].Get<ScriptType::FloatingPoint>()->As<real>());
			else
				frustum.AspectRatio(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		}
		else if (property.Name() == "base-viewport-height")
			frustum.BaseViewportHeight(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "clip-plane")
			frustum.ClipPlane(graphics::utilities::Aabb{property[0].Get<ScriptType::Vector2>()->Get(), property[1].Get<ScriptType::Vector2>()->Get()});
		else if (property.Name() == "far-clip-distance")
			frustum.FarClipDistance(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "field-of-view")
			frustum.FieldOfView(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "near-clip-distance")
			frustum.NearClipDistance(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "projection")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "orthographic")
				frustum.Projection(graphics::render::frustum::ProjectionType::Orthographic);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "perspective")
				frustum.Projection(graphics::render::frustum::ProjectionType::Perspective);
		}
	}
}

void set_pass_properties(const script_tree::ObjectNode &object, graphics::render::Pass &pass,
	const ManagerRegister &managers)
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
			pass.RenderProgram(get_shader_program(property[0].Get<ScriptType::String>()->Get(), managers));
	}
}


void set_animated_sprite_properties(const script_tree::ObjectNode &object, shapes::AnimatedSprite &animated_sprite,
	const ManagerRegister &managers)
{
	set_sprite_properties(object, animated_sprite, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "running")
		{
			if (property[0].Get<ScriptType::Boolean>()->Get())
				animated_sprite.Start();
			else
				animated_sprite.Stop();
		}
		else if (property.Name() == "jump-backward")
			animated_sprite.JumpBackward(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
		else if (property.Name() == "jump-forward")
			animated_sprite.JumpForward(duration{property[0].Get<ScriptType::FloatingPoint>()->As<real>()});
	}
}

void set_border_properties(const script_tree::ObjectNode &object, shapes::Border &border,
	const ManagerRegister &managers)
{
	set_rectangle_properties(object, border, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "border-color")
			border.BorderColor(property[0].Get<ScriptType::Color>()->Get());
		else if (property.Name() == "corner-color")
		{
			if (property.NumberOfArguments() == 4)
				border.CornerColor(property[0].Get<ScriptType::Color>()->Get(),
								   property[1].Get<ScriptType::Color>()->Get(),
								   property[2].Get<ScriptType::Color>()->Get(),
								   property[3].Get<ScriptType::Color>()->Get());
			else
				border.CornerColor(property[0].Get<ScriptType::Color>()->Get());
		}
		else if (property.Name() == "side-color")
		{
			if (property.NumberOfArguments() == 4)
				border.SideColor(property[0].Get<ScriptType::Color>()->Get(),
								 property[1].Get<ScriptType::Color>()->Get(),
								 property[2].Get<ScriptType::Color>()->Get(),
								 property[3].Get<ScriptType::Color>()->Get());
			else
				border.SideColor(property[0].Get<ScriptType::Color>()->Get());
		}
	}
}

void set_curve_properties(const script_tree::ObjectNode &object, shapes::Curve &curve,
	const ManagerRegister &managers)
{
	set_shape_properties(object, curve, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "p")
			curve.P(property[0].Get<ScriptType::Integer>()->As<int>(), property[1].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "thickness")
			curve.Thickness(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_ellipse_properties(const script_tree::ObjectNode &object, shapes::Ellipse &ellipse,
	const ManagerRegister &managers)
{
	set_shape_properties(object, ellipse, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "diameter")
			ellipse.Diameter(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "position")
			ellipse.Position(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "radius")
			ellipse.Radius(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "rotation")
			ellipse.Rotation(utilities::math::ToRadians(property[0].Get<ScriptType::FloatingPoint>()->As<real>()));
		else if (property.Name() == "size")
			ellipse.Size(property[0].Get<ScriptType::Vector2>()->Get());
	}
}

void set_line_properties(const script_tree::ObjectNode &object, shapes::Line &line,
	const ManagerRegister &managers)
{
	set_shape_properties(object, line, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "a")
			line.A(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "b")
			line.B(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "thickness")
			line.Thickness(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_mesh_properties(const script_tree::ObjectNode &object, shapes::Mesh &mesh,
	const ManagerRegister &managers)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "color" || property.Name() == "vertex-color")
			mesh.VertexColor(property[0].Get<ScriptType::Color>()->Get());
		else if (property.Name() == "draw-mode")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "points")
				mesh.DrawMode(graphics::render::vertex::vertex_batch::VertexDrawMode::Points);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "lines")
				mesh.DrawMode(graphics::render::vertex::vertex_batch::VertexDrawMode::Lines);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "line-loop")
				mesh.DrawMode(graphics::render::vertex::vertex_batch::VertexDrawMode::LineLoop);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "line-strip")
				mesh.DrawMode(graphics::render::vertex::vertex_batch::VertexDrawMode::LineStrip);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "triangles")
				mesh.DrawMode(graphics::render::vertex::vertex_batch::VertexDrawMode::Triangles);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "triangle-fan")
				mesh.DrawMode(graphics::render::vertex::vertex_batch::VertexDrawMode::TriangleFan);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "triangle-strip")
				mesh.DrawMode(graphics::render::vertex::vertex_batch::VertexDrawMode::TriangleStrip);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "quads")
				mesh.DrawMode(graphics::render::vertex::vertex_batch::VertexDrawMode::Quads);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "polygon")
				mesh.DrawMode(graphics::render::vertex::vertex_batch::VertexDrawMode::Polygon);
		}
		else if (property.Name() == "include-bounding-volumes")
			mesh.IncludeBoundingVolumes(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "material" || property.Name() == "surface-material")
			mesh.SurfaceMaterial(get_material(property[0].Get<ScriptType::String>()->Get(), managers));
		else if (property.Name() == "opacity" || property.Name() == "vertex-opacity")
			mesh.VertexOpacity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "show-wireframe")
			mesh.ShowWireframe(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "draw-mode")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "manual")
				mesh.TexCoordMode(shapes::mesh::MeshTexCoordMode::Manual);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "auto")
				mesh.TexCoordMode(shapes::mesh::MeshTexCoordMode::Auto);
		}
		else if (property.Name() == "visible")
			mesh.Visible(property[0].Get<ScriptType::Boolean>()->Get());
	}
}

void set_rectangle_properties(const script_tree::ObjectNode &object, shapes::Rectangle &rectangle,
	const ManagerRegister &managers)
{
	set_shape_properties(object, rectangle, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "height")
			rectangle.Height(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "position")
			rectangle.Position(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "resize-to-fill")
			rectangle.ResizeToFill(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "resize-to-fit")
			rectangle.ResizeToFit(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "rotation")
			rectangle.Rotation(utilities::math::ToRadians(property[0].Get<ScriptType::FloatingPoint>()->As<real>()));
		else if (property.Name() == "size")
			rectangle.Size(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "width")
			rectangle.Width(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_shape_properties(const script_tree::ObjectNode &object, shapes::Shape &shape,
	const ManagerRegister &managers)
{
	set_mesh_properties(object, shape, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "fill-color")
			shape.FillColor(property[0].Get<ScriptType::Color>()->Get());
		else if (property.Name() == "fill-opacity")
			shape.FillOpacity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_sprite_properties(const script_tree::ObjectNode &object, shapes::Sprite &sprite,
	const ManagerRegister &managers)
{
	set_rectangle_properties(object, sprite, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "auto-repeat")
			sprite.AutoRepeat(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "auto-size")
			sprite.AutoSize(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "crop")
			sprite.Crop(Aabb{property[0].Get<ScriptType::Vector2>()->Get(), property[1].Get<ScriptType::Vector2>()->Get()});
		else if (property.Name() == "flip-horizontal")
		{
			if (property[0].Get<ScriptType::Boolean>()->Get())
				sprite.FlipHorizontal();
		}
		else if (property.Name() == "flip-vertical")
		{
			if (property[0].Get<ScriptType::Boolean>()->Get())
				sprite.FlipVertical();
		}
		else if (property.Name() == "repeat")
			sprite.Repeat(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "tex-coords")
			sprite.TexCoords(property[0].Get<ScriptType::Vector2>()->Get(), property[1].Get<ScriptType::Vector2>()->Get());
	}
}

void set_triangle_properties(const script_tree::ObjectNode &object, shapes::Triangle &triangle,
	const ManagerRegister &managers)
{
	set_shape_properties(object, triangle, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "a")
			triangle.A(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "b")
			triangle.B(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "c")
			triangle.C(property[0].Get<ScriptType::Vector3>()->Get());
	}
}


void set_node_animation_properties(const script_tree::ObjectNode &object, graph::animations::NodeAnimation &animation)
{
	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "action")
			create_action(obj, animation);
		else if (obj.Name() == "rotation")
			create_rotating_motion(obj, animation);
		else if (obj.Name() == "scaling")
			create_scaling_motion(obj, animation);
		else if (obj.Name() == "translation")
			create_translating_motion(obj, animation);
	}
}

void set_node_animation_group_properties(const script_tree::ObjectNode &object, graph::animations::NodeAnimationGroup &animation_group,
	graph::SceneNode &parent_node)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "add")
		{
			auto node_animation =
				get_node_animation(property[0].Get<ScriptType::String>()->Get(), parent_node);

			if (property.NumberOfArguments() == 3)
				animation_group.Add(node_animation,
									duration{property[1].Get<ScriptType::FloatingPoint>()->As<real>()},
									property[2].Get<ScriptType::Boolean>()->Get());
			else
				animation_group.Add(node_animation);
		}
	}
}

void set_node_animation_timeline_properties(const script_tree::ObjectNode &object, graph::animations::NodeAnimationTimeline &timeline,
	graph::SceneNode &parent_node)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "attach-animation")
		{
			auto node_animation =
				get_node_animation(property[0].Get<ScriptType::String>()->Get(), parent_node);

			if (property.NumberOfArguments() == 3)
				timeline.Attach(node_animation,
								duration{property[1].Get<ScriptType::FloatingPoint>()->As<real>()},
								property[2].Get<ScriptType::Boolean>()->Get());
			else
				timeline.Attach(node_animation);
		}
		else if (property.Name() == "attach-animation-group")
		{
			auto node_animation_group =
				get_node_animation_group(property[0].Get<ScriptType::String>()->Get(), parent_node);

			if (property.NumberOfArguments() == 3)
				timeline.Attach(node_animation_group,
								duration{property[1].Get<ScriptType::FloatingPoint>()->As<real>()},
								property[2].Get<ScriptType::Boolean>()->Get());
			else
				timeline.Attach(node_animation_group);
		}
		else if (property.Name() == "playback-rate")
			timeline.PlaybackRate(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "repeat-count")
			timeline.RepeatCount(property[0].Get<ScriptType::Integer>()->As<int>());
		else if (property.Name() == "running")
		{
			if (property[0].Get<ScriptType::Boolean>()->Get())
				timeline.Start();
			else
				timeline.Stop();
		}
	}
}

void set_scene_node_properties(const script_tree::ObjectNode &object, graph::SceneNode &scene_node,
	SceneManager &scene_manager, const ManagerRegister &managers)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "derived-position")
			scene_node.DerivedPosition(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "derived-rotation")
			scene_node.DerivedRotation(utilities::math::ToRadians(property[0].Get<ScriptType::FloatingPoint>()->As<real>()));
		else if (property.Name() == "derived-scaling")
			scene_node.DerivedScaling(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "direction")
			scene_node.Direction(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "flip-visibility")
			scene_node.FlipVisibility(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "inherit-rotation")
			scene_node.InheritRotation(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "inherit-scaling")
			scene_node.InheritScaling(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "position")
			scene_node.Position(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "rotate")
			scene_node.Rotate(utilities::math::ToRadians(property[0].Get<ScriptType::FloatingPoint>()->As<real>()));
		else if (property.Name() == "rotation")
			scene_node.Rotation(utilities::math::ToRadians(property[0].Get<ScriptType::FloatingPoint>()->As<real>()));
		else if (property.Name() == "rotation-origin")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "parent")
				scene_node.RotationOrigin(graph::scene_node::NodeRotationOrigin::Parent);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "local")
				scene_node.RotationOrigin(graph::scene_node::NodeRotationOrigin::Local);
		}
		else if (property.Name() == "scale")
			scene_node.Scale(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "scaling")
			scene_node.Scaling(property[0].Get<ScriptType::Vector2>()->Get());
		else if (property.Name() == "translate")
			scene_node.Translate(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "visible")
		{
			if (property.NumberOfArguments() == 2)
				scene_node.Visible(property[0].Get<ScriptType::Boolean>()->Get(), property[1].Get<ScriptType::Boolean>()->Get());
			else
				scene_node.Visible(property[0].Get<ScriptType::Boolean>()->Get());
		}
	}

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "camera")
			create_camera(obj, scene_manager);
		else if (obj.Name() == "drawable-particle-system")
			create_drawable_particle_system(obj, scene_manager, managers);
		else if (obj.Name() == "drawable-text")
			create_drawable_text(obj, scene_manager, managers);
		else if (obj.Name() == "light")
			create_light(obj, scene_manager);
		else if (obj.Name() == "model")
			create_model(obj, scene_manager, managers);
		else if (obj.Name() == "movable-sound")
			create_movable_sound(obj, scene_manager, managers);
		else if (obj.Name() == "movable-sound-listener")
			create_movable_sound_listener(obj, scene_manager, managers);

		else if (obj.Name() == "node-animation")
			create_node_animation(obj, scene_node);
		else if (obj.Name() == "node-animation-group")
			create_node_animation_group(obj, scene_node);
		else if (obj.Name() == "node-animation-timeline")
			create_node_animation_timeline(obj, scene_node);
		else if (obj.Name() == "scene-node")
			create_scene_node(obj, scene_node, scene_manager, managers);
	}
}


void set_camera_properties(const script_tree::ObjectNode &object, Camera &camera)
{
	set_movable_object_properties(object, camera);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "base-viewport-height")
			camera.BaseViewportHeight(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "position")
			camera.Position(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "rotation")
			camera.Rotation(utilities::math::ToRadians(property[0].Get<ScriptType::FloatingPoint>()->As<real>()));
	}
}

void set_drawable_object_properties(const script_tree::ObjectNode &object, DrawableObject &drawable_object,
	const ManagerRegister &managers)
{
	set_movable_object_properties(object, drawable_object);

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "pass")
			drawable_object.AddPass(create_pass(obj, managers));
	}

	for (auto &property : object.Properties())
	{
		if (property.Name() == "opacity")
			drawable_object.Opacity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_drawable_particle_system_properties(const script_tree::ObjectNode &object, DrawableParticleSystem &particle_system,
	const ManagerRegister &managers)
{
	set_drawable_object_properties(object, particle_system, managers);
}

void set_drawable_text_properties(const script_tree::ObjectNode &object, DrawableText &text,
	const ManagerRegister &managers)
{
	set_drawable_object_properties(object, text, managers);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "position")
			text.Position(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "rotation")
			text.Rotation(utilities::math::ToRadians(property[0].Get<ScriptType::FloatingPoint>()->As<real>()));
	}
}

void set_light_properties(const script_tree::ObjectNode &object, Light &light)
{
	set_movable_object_properties(object, light);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "ambient-color")
			light.AmbientColor(property[0].Get<ScriptType::Color>()->Get());
		else if (property.Name() == "attenuation")
			light.Attenuation(property[0].Get<ScriptType::FloatingPoint>()->As<real>(),
							  property[1].Get<ScriptType::FloatingPoint>()->As<real>(),
							  property[2].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "cast-shadows")
			light.CastShadows(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "cutoff")
			light.Cutoff(property[0].Get<ScriptType::FloatingPoint>()->As<real>(), property[1].Get<ScriptType::FloatingPoint>()->As<real>());
		else if (property.Name() == "diffuse-color")
			light.DiffuseColor(property[0].Get<ScriptType::Color>()->Get());
		else if (property.Name() == "direction")
			light.Direction(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "position")
			light.Position(property[0].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "specular-color")
			light.SpecularColor(property[0].Get<ScriptType::Color>()->Get());
		else if (property.Name() == "type")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "point")
				light.Type(light::LightType::Point);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "directional")
				light.Type(light::LightType::Directional);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "spot")
				light.Type(light::LightType::Spot);
		}
	}
}

void set_model_properties(const script_tree::ObjectNode &object, Model &model,
	const ManagerRegister &managers)
{
	set_drawable_object_properties(object, model, managers);

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "animated-sprite")
			create_animated_sprite(obj, model, managers);
		else if (obj.Name() == "border")
			create_border(obj, model, managers);
		else if (obj.Name() == "curve")
			create_curve(obj, model, managers);
		else if (obj.Name() == "ellipse")
			create_ellipse(obj, model, managers);
		else if (obj.Name() == "line")
			create_line(obj, model, managers);
		else if (obj.Name() == "mesh")
			create_mesh(obj, model, managers);
		else if (obj.Name() == "rectangle")
			create_rectangle(obj, model, managers);
		else if (obj.Name() == "sprite")
			create_sprite(obj, model, managers);
		else if (obj.Name() == "triangle")
			create_triangle(obj, model, managers);
	}
}

void set_movable_object_properties(const script_tree::ObjectNode &object, MovableObject &movable_object)
{
	for (auto &property : object.Properties())
	{
		if (property.Name() == "bounding-volume-colors")
			movable_object.BoundingVolumeColors(property[0].Get<ScriptType::Color>()->Get(),
												property[1].Get<ScriptType::Color>()->Get(),
												property[2].Get<ScriptType::Color>()->Get());
		else if (property.Name() == "bounding-volume-extent")
			movable_object.BoundingVolumeExtent(graphics::utilities::Aabb{property[0].Get<ScriptType::Vector2>()->Get(), property[1].Get<ScriptType::Vector2>()->Get()});
		else if (property.Name() == "prefered-bounding-volume")
		{
			if (property[0].Get<ScriptType::Enumerable>()->Get() == "bounding-box")
				movable_object.PreferredBoundingVolume(movable_object::PreferredBoundingVolumeType::BoundingBox);
			else if (property[0].Get<ScriptType::Enumerable>()->Get() == "bounding-sphere")
				movable_object.PreferredBoundingVolume(movable_object::PreferredBoundingVolumeType::BoundingSphere);
		}
		else if (property.Name() == "query-flags")
			movable_object.AddQueryFlags(property[0].Get<ScriptType::Integer>()->As<uint32>());
		else if (property.Name() == "query-mask")
			movable_object.AddQueryMask(property[0].Get<ScriptType::Integer>()->As<uint32>());
		else if (property.Name() == "show-bounding-volumes")
			movable_object.ShowBoundingVolumes(property[0].Get<ScriptType::Boolean>()->Get());
		else if (property.Name() == "visible")
			movable_object.Visible(property[0].Get<ScriptType::Boolean>()->Get());
	}
}

void set_movable_sound_properties(const script_tree::ObjectNode &object, MovableSound &sound)
{
	set_movable_object_properties(object, sound);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "position")
			sound.Position(property[0].Get<ScriptType::Vector3>()->Get());
	}
}

void set_movable_sound_listener_properties(const script_tree::ObjectNode &object, MovableSoundListener &sound_listener)
{
	set_movable_object_properties(object, sound_listener);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "position")
			sound_listener.Position(property[0].Get<ScriptType::Vector3>()->Get());
	}
}


graphics::render::Frustum create_frustum(const script_tree::ObjectNode &object)
{
	graphics::render::Frustum frustum;
	set_frustum_properties(object, frustum);
	return frustum;
}

graphics::render::Pass create_pass(const script_tree::ObjectNode &object,
	const ManagerRegister &managers)
{
	graphics::render::Pass pass;
	set_pass_properties(object, pass, managers);
	return pass;
}


void create_action(const script_tree::ObjectNode &object,
	graph::animations::NodeAnimation &animation)
{
	auto type_name = object
		.Property("type")[0]
		.Get<ScriptType::Enumerable>()->Get();
	auto time = duration{object
		.Property("time")[0]
		.Get<ScriptType::FloatingPoint>()->As<real>()};

	auto type =
		[&]() noexcept
		{
			if (type_name == "flip-visibility")
				return graph::animations::node_animation::NodeActionType::FlipVisibility;
			else if (type_name == "flip-visibility-cascading")
				return graph::animations::node_animation::NodeActionType::FlipVisibilityCascading;
			else if (type_name == "show")
				return graph::animations::node_animation::NodeActionType::Show;
			else if (type_name == "show-cascading")
				return graph::animations::node_animation::NodeActionType::ShowCascading;
			else if (type_name == "hide")
				return graph::animations::node_animation::NodeActionType::Hide;
			else if (type_name == "hide-cascading")
				return graph::animations::node_animation::NodeActionType::HideCascading;

			else if (type_name == "inherit-rotation")
				return graph::animations::node_animation::NodeActionType::InheritRotation;
			else if (type_name == "inherit-scaling")
				return graph::animations::node_animation::NodeActionType::InheritScaling;
			else if (type_name == "disinherit-rotation")
				return graph::animations::node_animation::NodeActionType::DisinheritRotation;
			else //if (type_name == "disinherit-scaling")
				return graph::animations::node_animation::NodeActionType::DisinheritScaling;
		}();

	animation.AddAction(type, time);
}

void create_rotating_motion(const script_tree::ObjectNode &object,
	graph::animations::NodeAnimation &animation)
{
	auto angle = utilities::math::ToRadians(object
		.Property("angle")[0]
		.Get<ScriptType::FloatingPoint>()->As<real>());
	auto total_duration = duration{object
		.Property("total-duration")[0]
		.Get<ScriptType::FloatingPoint>()->As<real>()};
	auto start_time = duration{object
		.Property("start-time")[0]
		.Get<ScriptType::FloatingPoint>().value_or(0.0).As<real>()};
	auto technique_name = object
		.Property("motion-technique")[0];

	auto technique = technique_name ?
		get_motion_technique_type(technique_name) :
		graph::animations::node_animation::MotionTechniqueType::Linear;

	animation.AddRotation(angle, total_duration, start_time, technique);
}

void create_scaling_motion(const script_tree::ObjectNode &object,
	graph::animations::NodeAnimation &animation)
{
	auto unit = object
		.Property("unit")[0]
		.Get<ScriptType::Vector2>()->Get();
	auto total_duration = duration{object
		.Property("total-duration")[0]
		.Get<ScriptType::FloatingPoint>()->As<real>()};
	auto start_time = duration{object
		.Property("start-time")[0]
		.Get<ScriptType::FloatingPoint>().value_or(0.0).As<real>()};
	auto technique_name_x = object
		.Property("motion-technique")[0];
	auto technique_name_y = object
		.Property("motion-technique")[1];

	auto technique_x = technique_name_x ?
		get_motion_technique_type(technique_name_x) :
		graph::animations::node_animation::MotionTechniqueType::Linear;
	auto technique_y = technique_name_y ?
		get_motion_technique_type(technique_name_y) :
		technique_x;

	animation.AddScaling(unit, total_duration, start_time, technique_x, technique_y);
}

void create_translating_motion(const script_tree::ObjectNode &object,
	graph::animations::NodeAnimation &animation)
{
	auto unit = object
		.Property("unit")[0]
		.Get<ScriptType::Vector3>()->Get();
	auto total_duration = duration{object
		.Property("total-duration")[0]
		.Get<ScriptType::FloatingPoint>()->As<real>()};
	auto start_time = duration{object
		.Property("start-time")[0]
		.Get<ScriptType::FloatingPoint>().value_or(0.0).As<real>()};
	auto technique_name_x = object
		.Property("motion-technique")[0];
	auto technique_name_y = object
		.Property("motion-technique")[1];
	auto technique_name_z = object
		.Property("motion-technique")[2];

	auto technique_x = technique_name_x ?
		get_motion_technique_type(technique_name_x) :
		graph::animations::node_animation::MotionTechniqueType::Linear;
	auto technique_y = technique_name_y ?
		get_motion_technique_type(technique_name_y) :
		technique_x;
	auto technique_z = technique_name_z ?
		get_motion_technique_type(technique_name_z) :
		technique_x;

	animation.AddTranslation(unit, total_duration, start_time, technique_x, technique_y, technique_z);
}


NonOwningPtr<shapes::AnimatedSprite> create_animated_sprite(const script_tree::ObjectNode &object,
	Model &model, const ManagerRegister &managers)
{
	auto position = object
		.Property("position")[0]
		.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();
	auto rotation = object
		.Property("rotation")[0]
		.Get<ScriptType::FloatingPoint>().value_or(0.0).As<real>();
	auto size = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>()->Get();
	auto material_name = object
		.Property("material")[0]
		.Get<ScriptType::String>()->Get();
	auto color = object
		.Property("color")[0]
		.Get<ScriptType::Color>().value_or(color::White).Get();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto animated_sprite = model.CreateMesh<shapes::AnimatedSprite>(position, rotation, size,
		get_material(material_name, managers), color, visible);

	if (animated_sprite)
		set_animated_sprite_properties(object, *animated_sprite, managers);

	return animated_sprite;
}

NonOwningPtr<shapes::Border> create_border(const script_tree::ObjectNode &object,
	Model &model, const ManagerRegister &managers)
{
	auto position = object
		.Property("position")[0]
		.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();
	auto rotation = utilities::math::ToRadians(object
		.Property("rotation")[0]
		.Get<ScriptType::FloatingPoint>().value_or(0.0).As<real>());
	auto size = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>()->Get();
	auto border_size = object
		.Property("boder-size")[0]
		.Get<ScriptType::Vector2>()->Get();
	auto corner_style_name = object
		.Property("corner-style")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto color = object
		.Property("color")[0]
		.Get<ScriptType::Color>()->Get();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto corner_style =
		[&]() noexcept
		{
			if (corner_style_name == "square")
				return shapes::border::BorderCornerStyle::Square;
			else if (corner_style_name == "oblique")
				return shapes::border::BorderCornerStyle::Oblique;
			else //if (corner_style_name == "none")
				return shapes::border::BorderCornerStyle::None;
		}();

	auto border = model.CreateMesh<shapes::Border>(position, rotation, size, border_size, corner_style, color, visible);

	if (border)
		set_border_properties(object, *border, managers);

	return border;
}

NonOwningPtr<shapes::Curve> create_curve(const script_tree::ObjectNode &object,
	Model &model, const ManagerRegister &managers)
{
	auto color = object
		.Property("color")[0]
		.Get<ScriptType::Color>()->Get();
	auto thickness = object
		.Property("thickness")[0]
		.Get<ScriptType::FloatingPoint>().value_or(1.0).As<real>();
	auto smoothness = object
		.Property("smoothness")[0]
		.Get<ScriptType::Integer>().value_or(shapes::curve::detail::default_curve_smoothness).As<int>();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto control_points = shapes::curve::ControlPoints{};

	for (auto &property : object.Properties())
	{
		if (property.Name() == "control-point")
			control_points.push_back(property[0].Get<ScriptType::Vector3>()->Get());
	}

	auto curve = model.CreateMesh<shapes::Curve>(std::move(control_points), color, thickness, smoothness, visible);

	if (curve)
		set_curve_properties(object, *curve, managers);

	return curve;
}

NonOwningPtr<shapes::Ellipse> create_ellipse(const script_tree::ObjectNode &object,
	Model &model, const ManagerRegister &managers)
{
	auto position = object
		.Property("position")[0]
		.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();
	auto rotation = utilities::math::ToRadians(object
		.Property("rotation")[0]
		.Get<ScriptType::FloatingPoint>().value_or(0.0).As<real>());
	auto size = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>()->Get();
	auto color = object
		.Property("color")[0]
		.Get<ScriptType::Color>()->Get();
	auto sides = object
		.Property("sides")[0]
		.Get<ScriptType::Integer>().value_or(shapes::ellipse::detail::default_ellipse_sides).As<int>();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto ellipse = model.CreateMesh<shapes::Ellipse>(position, rotation, size, color, sides, visible);

	if (ellipse)
		set_ellipse_properties(object, *ellipse, managers);

	return ellipse;
}

NonOwningPtr<shapes::Line> create_line(const script_tree::ObjectNode &object,
	Model &model, const ManagerRegister &managers)
{
	auto a = object
		.Property("a")[0]
		.Get<ScriptType::Vector3>()->Get();
	auto b = object
		.Property("b")[0]
		.Get<ScriptType::Vector3>()->Get();
	auto color = object
		.Property("color")[0]
		.Get<ScriptType::Color>()->Get();
	auto thickness = object
		.Property("thickness")[0]
		.Get<ScriptType::FloatingPoint>().value_or(1.0).As<real>();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto line = model.CreateMesh<shapes::Line>(a, b, color, thickness, visible);

	if (line)
		set_line_properties(object, *line, managers);

	return line;
}

NonOwningPtr<shapes::Mesh> create_mesh(const script_tree::ObjectNode &object,
	Model &model, const ManagerRegister &managers)
{
	auto draw_mode_name = object
		.Property("draw-mode")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto material_name = object
		.Property("material")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto tex_coord_mode_name = object
		.Property("tex-coord-mode")[0]
		.Get<ScriptType::Enumerable>().value_or(""s).Get();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto draw_mode =
		[&]() noexcept
		{
			//"points"s, "lines"s, "line-loop"s, "line-strip"s, "triangles"s, "triangle-fan"s, "triangle-strip"s, "quads"s, "polygon"s

			if (draw_mode_name == "points")
				return graphics::render::vertex::vertex_batch::VertexDrawMode::Points;
			else if (draw_mode_name == "lines")
				return graphics::render::vertex::vertex_batch::VertexDrawMode::Lines;
			else if (draw_mode_name == "line-loop")
				return graphics::render::vertex::vertex_batch::VertexDrawMode::LineLoop;
			else if (draw_mode_name == "line-strip")
				return graphics::render::vertex::vertex_batch::VertexDrawMode::LineStrip;
			else if (draw_mode_name == "triangle-fan")
				return graphics::render::vertex::vertex_batch::VertexDrawMode::TriangleFan;
			else if (draw_mode_name == "triangle-strip")
				return graphics::render::vertex::vertex_batch::VertexDrawMode::TriangleStrip;
			else if (draw_mode_name == "quads")
				return graphics::render::vertex::vertex_batch::VertexDrawMode::Quads;
			else if (draw_mode_name == "polygon")
				return graphics::render::vertex::vertex_batch::VertexDrawMode::Polygon;
			else //if (draw_mode_name == "triangles")
				return graphics::render::vertex::vertex_batch::VertexDrawMode::Triangles;
		}();

	auto vertices = shapes::mesh::Vertices{};

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "vertices")
		{
			for (auto &o : obj.Objects())
			{
				if (o.Name() == "vertex")
				{
					auto position = o
						.Property("position")[0]
						.Get<ScriptType::Vector3>()->Get();
					auto normal = o
						.Property("normal")[0]
						.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();			
					auto tex_coord = o
						.Property("tex-coord")[0]
						.Get<ScriptType::Vector2>().value_or(vector2::Zero).Get();
					auto color = o
						.Property("color")[0]
						.Get<ScriptType::Color>().value_or(color::White).Get();	

					vertices.emplace_back(position, normal, tex_coord, color);
				}
			}
		}
	}

	auto tex_coord_mode =
		[&]() noexcept
		{
			if (tex_coord_mode_name == "manual")
				return shapes::mesh::MeshTexCoordMode::Manual;
			else //if (tex_coord_mode_name == "auto")
				return shapes::mesh::MeshTexCoordMode::Auto;
		}();

	auto mesh = model.CreateMesh(draw_mode, vertices,
		get_material(material_name, managers), tex_coord_mode, visible);

	if (mesh)
		set_mesh_properties(object, *mesh, managers);

	return mesh;
}

NonOwningPtr<shapes::Rectangle> create_rectangle(const script_tree::ObjectNode &object,
	Model &model, const ManagerRegister &managers)
{
	auto position = object
		.Property("position")[0]
		.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();
	auto rotation = utilities::math::ToRadians(object
		.Property("rotation")[0]
		.Get<ScriptType::FloatingPoint>().value_or(0.0).As<real>());
	auto size = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>()->Get();
	auto color = object
		.Property("color")[0]
		.Get<ScriptType::Color>()->Get();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto rectangle = model.CreateMesh<shapes::Rectangle>(position, rotation, size, color, visible);

	if (rectangle)
		set_rectangle_properties(object, *rectangle, managers);

	return rectangle;
}

NonOwningPtr<shapes::Sprite> create_sprite(const script_tree::ObjectNode &object,
	Model &model, const ManagerRegister &managers)
{
	auto position = object
		.Property("position")[0]
		.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();
	auto rotation = utilities::math::ToRadians(object
		.Property("rotation")[0]
		.Get<ScriptType::FloatingPoint>().value_or(0.0).As<real>());
	auto size = object
		.Property("size")[0]
		.Get<ScriptType::Vector2>()->Get();
	auto material_name = object
		.Property("material")[0]
		.Get<ScriptType::String>()->Get();
	auto color = object
		.Property("color")[0]
		.Get<ScriptType::Color>().value_or(color::White).Get();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto sprite = model.CreateMesh<shapes::Sprite>(position, rotation, size,
		get_material(material_name, managers), color, visible);

	if (sprite)
		set_sprite_properties(object, *sprite, managers);

	return sprite;
}

NonOwningPtr<shapes::Triangle> create_triangle(const script_tree::ObjectNode &object,
	Model &model, const ManagerRegister &managers)
{
	auto a = object
		.Property("a")[0]
		.Get<ScriptType::Vector3>()->Get();
	auto b = object
		.Property("b")[0]
		.Get<ScriptType::Vector3>()->Get();
	auto c = object
		.Property("c")[0]
		.Get<ScriptType::Vector3>()->Get();
	auto color = object
		.Property("color")[0]
		.Get<ScriptType::Color>()->Get();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto triangle = model.CreateMesh<shapes::Triangle>(a, b, c, color, visible);

	if (triangle)
		set_triangle_properties(object, *triangle, managers);

	return triangle;
}


NonOwningPtr<graph::animations::NodeAnimation> create_node_animation(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto node_animation = parent_node.CreateAnimation(std::move(name));

	if (node_animation)
		set_node_animation_properties(object, *node_animation);

	return node_animation;
}

NonOwningPtr<graph::animations::NodeAnimationGroup> create_node_animation_group(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto node_animation_group = parent_node.CreateAnimationGroup(std::move(name));

	if (node_animation_group)
		set_node_animation_group_properties(object, *node_animation_group, parent_node);

	return node_animation_group;
}

NonOwningPtr<graph::animations::NodeAnimationTimeline> create_node_animation_timeline(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node)
{
	auto playback_rate = object
		.Property("playback-rate")[0]
		.Get<ScriptType::FloatingPoint>().value_or(1.0).As<real>();
	auto running = object
		.Property("running")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto node_animation_timeline =
		[&]() noexcept
		{
			if (auto &property = object.Property("name"); property)
				return parent_node.CreateTimeline(property[0].Get<ScriptType::String>()->Get(), playback_rate, running);
			else
				return parent_node.CreateTimeline(playback_rate, running);
		}();

	if (node_animation_timeline)
		set_node_animation_timeline_properties(object, *node_animation_timeline, parent_node);

	return node_animation_timeline;
}

NonOwningPtr<graph::SceneNode> create_scene_node(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node, SceneManager &scene_manager, const ManagerRegister &managers)
{
	auto name =
		[&]() noexcept -> std::optional<std::string>
		{
			if (auto &property = object.Property("name"); property)
				return property[0].Get<ScriptType::String>()->Get();
			else
				return {};
		}();
	auto position = object
		.Property("position")[0]
		.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();
	auto initial_direction = object
		.Property("initial-direction")[0]
		.Get<ScriptType::Vector2>().value_or(vector2::Zero).Get();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(parent_node.Visible()).Get();

	auto node = parent_node.CreateChildNode(std::move(name), position, initial_direction, visible);

	if (node)
		set_scene_node_properties(object, *node, scene_manager, managers);

	return node;
}


NonOwningPtr<Camera> create_camera(const script_tree::ObjectNode &object,
	graphics::scene::SceneManager &scene_manager)
{
	auto name =
		[&]() noexcept -> std::optional<std::string>
		{
			if (auto &property = object.Property("name"); property)
				return property[0].Get<ScriptType::String>()->Get();
			else
				return {};
		}();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto frustum = graphics::render::Frustum{};

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "frustum")
			set_frustum_properties(obj, frustum);
	}

	auto camera = scene_manager.CreateCamera(std::move(name), frustum, visible);

	if (camera)
		set_camera_properties(object, *camera);

	return camera;
}

NonOwningPtr<DrawableParticleSystem> create_drawable_particle_system(const script_tree::ObjectNode &object,
	SceneManager &scene_manager, const ManagerRegister &managers)
{
	auto name =
		[&]() noexcept -> std::optional<std::string>
		{
			if (auto &property = object.Property("name"); property)
				return property[0].Get<ScriptType::String>()->Get();
			else
				return {};
		}();
	auto particle_system_name = object
		.Property("particle-system")[0]
		.Get<ScriptType::String>()->Get();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto drawable_particle_system = scene_manager.CreateParticleSystem(std::move(name),
		get_particle_system(particle_system_name, managers), visible);

	if (drawable_particle_system)
		set_drawable_particle_system_properties(object, *drawable_particle_system, managers);

	return drawable_particle_system;
}

NonOwningPtr<DrawableText> create_drawable_text(const script_tree::ObjectNode &object,
	SceneManager &scene_manager, const ManagerRegister &managers)
{
	auto name =
		[&]() noexcept -> std::optional<std::string>
		{
			if (auto &property = object.Property("name"); property)
				return property[0].Get<ScriptType::String>()->Get();
			else
				return {};
		}();
	auto position = object
		.Property("position")[0]
		.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();
	auto rotation = utilities::math::ToRadians(object
		.Property("rotation")[0]
		.Get<ScriptType::FloatingPoint>().value_or(0.0).As<real>());
	auto text_name = object
		.Property("text")[0]
		.Get<ScriptType::String>()->Get();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto drawable_text = scene_manager.CreateText(std::move(name), position, rotation,
		get_text(text_name, managers), visible);

	if (drawable_text)
		set_drawable_text_properties(object, *drawable_text, managers);

	return drawable_text;
}

NonOwningPtr<Light> create_light(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	auto name =
		[&]() noexcept -> std::optional<std::string>
		{
			if (auto &property = object.Property("name"); property)
				return property[0].Get<ScriptType::String>()->Get();
			else
				return {};
		}();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto light = scene_manager.CreateLight(std::move(name), visible);

	if (light)
		set_light_properties(object, *light);

	return light;
}

NonOwningPtr<Model> create_model(const script_tree::ObjectNode &object,
	SceneManager &scene_manager, const ManagerRegister &managers)
{
	auto name =
		[&]() noexcept -> std::optional<std::string>
		{
			if (auto &property = object.Property("name"); property)
				return property[0].Get<ScriptType::String>()->Get();
			else
				return {};
		}();
	auto visible = object
		.Property("visible")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto model = scene_manager.CreateModel(std::move(name), visible);

	if (model)
		set_model_properties(object, *model, managers);

	return model;
}

NonOwningPtr<MovableSound> create_movable_sound(const script_tree::ObjectNode &object,
	SceneManager &scene_manager, const ManagerRegister &managers)
{
	auto name =
		[&]() noexcept -> std::optional<std::string>
		{
			if (auto &property = object.Property("name"); property)
				return property[0].Get<ScriptType::String>()->Get();
			else
				return {};
		}();
	auto position = object
		.Property("position")[0]
		.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();
	auto sound_name = object
		.Property("sound")[0]
		.Get<ScriptType::String>()->Get();
	auto sound_channel_group_name = object
		.Property("sound-channel-group")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto paused = object
		.Property("paused")[0]
		.Get<ScriptType::Boolean>().value_or(true).Get();

	auto movable_sound = scene_manager.CreateSound(std::move(name), position,
		get_sound(sound_name, managers), get_sound_channel_group(sound_channel_group_name, managers), paused);

	if (movable_sound)
		set_movable_sound_properties(object, *movable_sound);

	return movable_sound;
}

NonOwningPtr<MovableSoundListener> create_movable_sound_listener(const script_tree::ObjectNode &object,
	SceneManager &scene_manager, const ManagerRegister &managers)
{
	auto name =
		[&]() noexcept -> std::optional<std::string>
		{
			if (auto &property = object.Property("name"); property)
				return property[0].Get<ScriptType::String>()->Get();
			else
				return {};
		}();
	auto position = object
		.Property("position")[0]
		.Get<ScriptType::Vector3>().value_or(vector3::Zero).Get();
	auto sound_listener_name = object
		.Property("sound-listener")[0]
		.Get<ScriptType::String>()->Get();

	auto movable_sound_listener = scene_manager.CreateSoundListener(std::move(name), position,
		get_sound_listener(sound_listener_name, managers));

	if (movable_sound_listener)
		set_movable_sound_listener_properties(object, *movable_sound_listener);

	return movable_sound_listener;
}


void create_scene(const ScriptTree &tree, graph::SceneNode &parent_node,
	SceneManager &scene_manager, const ManagerRegister &managers)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "scene-node")
			create_scene_node(object, parent_node, scene_manager, managers);
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

void SceneScriptInterface::CreateScene(std::string_view asset_name, graph::SceneNode &parent_node,
	SceneManager &scene_manager)
{
	if (Load(asset_name))
		detail::create_scene(*tree_, parent_node, scene_manager, Managers());
}

void SceneScriptInterface::CreateScene(std::string_view asset_name, graph::SceneNode &parent_node,
	SceneManager &scene_manager, const ManagerRegister &managers)
{
	if (Load(asset_name))
		detail::create_scene(*tree_, parent_node, scene_manager, managers);
}

} //ion::script::interfaces