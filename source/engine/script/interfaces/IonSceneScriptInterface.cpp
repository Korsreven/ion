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
#include "graphics/utilities/IonAabb.h"
#include "utilities/IonMath.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace scene_script_interface;
using namespace graphics::scene;

namespace scene_script_interface::detail
{

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
		.AddProperty("type", {"point"s, "directional"s, "spotlight"s});
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

void set_rotating_properties(const script_tree::ObjectNode &object, graph::animations::node_animation::detail::rotating_motion &rotating)
{
	//Empty
}

void set_scaling_properties(const script_tree::ObjectNode &object, graph::animations::node_animation::detail::scaling_motion &scaling)
{
	//Empty
}

void set_translating_properties(const script_tree::ObjectNode &object, graph::animations::node_animation::detail::translating_motion &translating)
{
	//Empty
}


void set_animated_sprite_properties(const script_tree::ObjectNode &object, shapes::AnimatedSprite &animated_sprite,
	graphics::materials::MaterialManager &material_manager)
{
	set_sprite_properties(object, animated_sprite, material_manager);

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
	graphics::materials::MaterialManager &material_manager)
{
	set_rectangle_properties(object, border, material_manager);

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
	graphics::materials::MaterialManager &material_manager)
{
	set_shape_properties(object, curve, material_manager);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "p")
			curve.P(property[0].Get<ScriptType::Integer>()->As<int>(), property[1].Get<ScriptType::Vector3>()->Get());
		else if (property.Name() == "thickness")
			curve.Thickness(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_ellipse_properties(const script_tree::ObjectNode &object, shapes::Ellipse &ellipse,
	graphics::materials::MaterialManager &material_manager)
{
	set_shape_properties(object, ellipse, material_manager);

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
	graphics::materials::MaterialManager &material_manager)
{
	set_shape_properties(object, line, material_manager);

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
	graphics::materials::MaterialManager &material_manager)
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
			mesh.SurfaceMaterial(material_manager.GetMaterial(property[0].Get<ScriptType::String>()->Get()));
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
	graphics::materials::MaterialManager &material_manager)
{
	set_shape_properties(object, rectangle, material_manager);

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
	graphics::materials::MaterialManager &material_manager)
{
	set_mesh_properties(object, shape, material_manager);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "fill-color")
			shape.FillColor(property[0].Get<ScriptType::Color>()->Get());
		else if (property.Name() == "fill-opacity")
			shape.FillOpacity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_sprite_properties(const script_tree::ObjectNode &object, shapes::Sprite &sprite,
	graphics::materials::MaterialManager &material_manager)
{
	set_rectangle_properties(object, sprite, material_manager);

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
	graphics::materials::MaterialManager &material_manager)
{
	set_shape_properties(object, triangle, material_manager);

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

void set_scene_node_properties(const script_tree::ObjectNode &object, graph::SceneNode &scene_node,
	SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
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
			create_drawable_particle_system(obj, scene_manager, shader_program_manager);
		else if (obj.Name() == "drawable-text")
			create_drawable_text(obj, scene_manager, shader_program_manager);
		else if (obj.Name() == "light")
			create_light(obj, scene_manager);
		else if (obj.Name() == "model")
			create_model(obj, scene_manager, material_manager, shader_program_manager);
		else if (obj.Name() == "movable-sound")
			create_movable_sound(obj, scene_manager);
		else if (obj.Name() == "movable-sound-listener")
			create_movable_sound_listener(obj, scene_manager);

		else if (obj.Name() == "node-animation")
			create_node_animation(obj, scene_node);
		else if (obj.Name() == "node-animation-group")
			create_node_animation_group(obj, scene_node);
		else if (obj.Name() == "node-animation-timeline")
			create_node_animation_timeline(obj, scene_node);
		else if (obj.Name() == "scene-node")
			create_scene_node(obj, scene_node, scene_manager, material_manager, shader_program_manager);
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

void set_drawable_object_properties(const script_tree::ObjectNode &object, DrawableObject &drawable_object,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	set_movable_object_properties(object, drawable_object);

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "pass")
			drawable_object.AddPass(create_pass(obj, shader_program_manager));
	}

	for (auto &property : object.Properties())
	{
		if (property.Name() == "opacity")
			drawable_object.Opacity(property[0].Get<ScriptType::FloatingPoint>()->As<real>());
	}
}

void set_drawable_particle_system_properties(const script_tree::ObjectNode &object, DrawableParticleSystem &particle_system,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	set_drawable_object_properties(object, particle_system, shader_program_manager);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}
}

void set_drawable_text_properties(const script_tree::ObjectNode &object, DrawableText &text,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	set_drawable_object_properties(object, text, shader_program_manager);

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

void set_model_properties(const script_tree::ObjectNode &object, Model &model,
	graphics::materials::MaterialManager &material_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	set_drawable_object_properties(object, model, shader_program_manager);

	for (auto &property : object.Properties())
	{
		if (property.Name() == "");
	}

	for (auto &obj : object.Objects())
	{
		if (obj.Name() == "animated-sprite")
			create_animated_sprite(obj, model, material_manager);
		else if (obj.Name() == "border")
			create_border(obj, model, material_manager);
		else if (obj.Name() == "curve")
			create_curve(obj, model, material_manager);
		else if (obj.Name() == "ellipse")
			create_ellipse(obj, model, material_manager);
		else if (obj.Name() == "line")
			create_line(obj, model, material_manager);
		else if (obj.Name() == "mesh")
			create_mesh(obj, model, material_manager);
		else if (obj.Name() == "rectangle")
			create_rectangle(obj, model, material_manager);
		else if (obj.Name() == "sprite")
			create_sprite(obj, model, material_manager);
		else if (obj.Name() == "triangle")
			create_triangle(obj, model, material_manager);
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


NonOwningPtr<shapes::AnimatedSprite> create_animated_sprite(const script_tree::ObjectNode &object,
	Model &model,
	graphics::materials::MaterialManager &material_manager)
{
	auto animated_sprite = model.CreateMesh<shapes::AnimatedSprite>(graphics::utilities::Vector2{}, nullptr);

	if (animated_sprite)
		set_animated_sprite_properties(object, *animated_sprite, material_manager);

	return animated_sprite;
}

NonOwningPtr<shapes::Border> create_border(const script_tree::ObjectNode &object,
	Model &model,
	graphics::materials::MaterialManager &material_manager)
{
	auto border = model.CreateMesh<shapes::Border>(graphics::utilities::Vector2{}, graphics::utilities::Vector2{}, graphics::utilities::Color{});

	if (border)
		set_border_properties(object, *border, material_manager);

	return border;
}

NonOwningPtr<shapes::Curve> create_curve(const script_tree::ObjectNode &object,
	Model &model,
	graphics::materials::MaterialManager &material_manager)
{
	auto curve = model.CreateMesh<shapes::Curve>(shapes::curve::ControlPoints{}, graphics::utilities::Color{});

	if (curve)
		set_curve_properties(object, *curve, material_manager);

	return curve;
}

NonOwningPtr<shapes::Ellipse> create_ellipse(const script_tree::ObjectNode &object,
	Model &model,
	graphics::materials::MaterialManager &material_manager)
{
	auto ellipse = model.CreateMesh<shapes::Ellipse>(graphics::utilities::Vector2{}, graphics::utilities::Color{});

	if (ellipse)
		set_ellipse_properties(object, *ellipse, material_manager);

	return ellipse;
}

NonOwningPtr<shapes::Line> create_line(const script_tree::ObjectNode &object,
	Model &model,
	graphics::materials::MaterialManager &material_manager)
{
	auto line = model.CreateMesh<shapes::Line>(graphics::utilities::Vector3{}, graphics::utilities::Vector3{}, graphics::utilities::Color{});

	if (line)
		set_line_properties(object, *line, material_manager);

	return line;
}

NonOwningPtr<shapes::Mesh> create_mesh(const script_tree::ObjectNode &object,
	Model &model,
	graphics::materials::MaterialManager &material_manager)
{
	auto mesh = model.CreateMesh(shapes::mesh::Vertices{});

	if (mesh)
		set_mesh_properties(object, *mesh, material_manager);

	return mesh;
}

NonOwningPtr<shapes::Rectangle> create_rectangle(const script_tree::ObjectNode &object,
	Model &model,
	graphics::materials::MaterialManager &material_manager)
{
	auto rectangle = model.CreateMesh<shapes::Rectangle>(graphics::utilities::Vector2{}, graphics::utilities::Color{});

	if (rectangle)
		set_rectangle_properties(object, *rectangle, material_manager);

	return rectangle;
}

NonOwningPtr<shapes::Sprite> create_sprite(const script_tree::ObjectNode &object,
	Model &model,
	graphics::materials::MaterialManager &material_manager)
{
	auto sprite = model.CreateMesh<shapes::Sprite>(graphics::utilities::Vector2{}, nullptr);

	if (sprite)
		set_sprite_properties(object, *sprite, material_manager);

	return sprite;
}

NonOwningPtr<shapes::Triangle> create_triangle(const script_tree::ObjectNode &object,
	Model &model,
	graphics::materials::MaterialManager &material_manager)
{
	auto triangle = model.CreateMesh<shapes::Triangle>(graphics::utilities::Vector3{}, graphics::utilities::Vector3{}, graphics::utilities::Vector3{}, graphics::utilities::Color{});

	if (triangle)
		set_triangle_properties(object, *triangle, material_manager);

	return triangle;
}


NonOwningPtr<graph::SceneNode> create_scene_node(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node,
	SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	auto node = parent_node.CreateChildNode();

	if (node)
		set_scene_node_properties(object, *node, scene_manager, material_manager, shader_program_manager);

	return node;
}

NonOwningPtr<graph::animations::NodeAnimation> create_node_animation(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node)
{
	auto node_animation = parent_node.CreateAnimation("");

	if (node_animation)
		set_node_animation_properties(object, *node_animation);

	return node_animation;
}

NonOwningPtr<graph::animations::NodeAnimationGroup> create_node_animation_group(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node)
{
	auto node_animation_group = parent_node.CreateAnimationGroup("");

	if (node_animation_group)
		set_node_animation_group_properties(object, *node_animation_group);

	return node_animation_group;
}

NonOwningPtr<graph::animations::NodeAnimationTimeline> create_node_animation_timeline(const script_tree::ObjectNode &object,
	graph::SceneNode &parent_node)
{
	auto node_animation_timeline = parent_node.CreateTimeline();

	if (node_animation_timeline)
		set_node_animation_timeline_properties(object, *node_animation_timeline);

	return node_animation_timeline;
}


NonOwningPtr<Camera> create_camera(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	auto camera = scene_manager.CreateCamera();

	if (camera)
		set_camera_properties(object, *camera);

	return camera;
}

NonOwningPtr<DrawableParticleSystem> create_drawable_particle_system(const script_tree::ObjectNode &object,
	SceneManager &scene_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	return nullptr;
}

NonOwningPtr<DrawableText> create_drawable_text(const script_tree::ObjectNode &object,
	SceneManager &scene_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	return nullptr;
}

NonOwningPtr<Light> create_light(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	auto light = scene_manager.CreateLight();

	if (light)
		set_light_properties(object, *light);

	return light;
}

NonOwningPtr<Model> create_model(const script_tree::ObjectNode &object,
	SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	auto model = scene_manager.CreateModel();

	if (model)
		set_model_properties(object, *model, material_manager, shader_program_manager);

	return model;
}

NonOwningPtr<MovableSound> create_movable_sound(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	auto movable_sound = scene_manager.CreateSound("", nullptr);

	if (movable_sound)
		set_movable_sound_properties(object, *movable_sound);

	return movable_sound;
}

NonOwningPtr<MovableSoundListener> create_movable_sound_listener(const script_tree::ObjectNode &object,
	SceneManager &scene_manager)
{
	auto movable_sound_listener = scene_manager.CreateSoundListener("", nullptr);

	if (movable_sound_listener)
		set_movable_sound_listener_properties(object, *movable_sound_listener);

	return movable_sound_listener;
}


void create_scene(const ScriptTree &tree,
	graph::SceneNode &parent_node,
	SceneManager &scene_manager,
	graphics::materials::MaterialManager &material_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "scene-node")
			create_scene_node(object, parent_node, scene_manager, material_manager, shader_program_manager);
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
	graphics::materials::MaterialManager &material_manager,
	graphics::shaders::ShaderProgramManager &shader_program_manager)
{
	if (Load(asset_name))
		detail::create_scene(*tree_, parent_node, scene_manager, material_manager, shader_program_manager);
}

} //ion::script::interfaces