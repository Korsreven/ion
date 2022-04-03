/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonShaderLayoutScriptInterface.cpp
-------------------------------------------
*/

#include "IonShaderLayoutScriptInterface.h"

#include <string>

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace shader_layout_script_interface;
using namespace graphics::shaders;

namespace shader_layout_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_shader_layout_class()
{
	return ClassDefinition::Create("shader-layout")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("bind-attribute", {attribute_names, ParameterType::String, ParameterType::Integer}, 2)
		.AddProperty("bind-struct", {struct_names, ParameterType::String})
		.AddProperty("bind-uniform", {uniform_names, ParameterType::String, ParameterType::Integer}, 2);
}

ScriptValidator get_shader_layout_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_shader_layout_class());
}


/*
	Tree parsing
*/

void bind_struct(const script_tree::PropertyNode &property,
	ShaderLayout &shader_layout)
{
	auto &name = property[0]
		.Get<ScriptType::Enumerable>()->Get();
	auto &struct_name = property[1]
		.Get<ScriptType::String>()->Get();

	if (name == "matrix")
		shader_layout.BindStruct(shader_layout::StructName::Matrix, struct_name);
	else if (name == "scene")
		shader_layout.BindStruct(shader_layout::StructName::Scene, struct_name);
	else if (name == "camera")
		shader_layout.BindStruct(shader_layout::StructName::Camera, struct_name);
	else if (name == "node")
		shader_layout.BindStruct(shader_layout::StructName::Node, struct_name);
	else if (name == "primitive")
		shader_layout.BindStruct(shader_layout::StructName::Primitive, struct_name);
	else if (name == "material")
		shader_layout.BindStruct(shader_layout::StructName::Material, struct_name);
	else if (name == "fog")
		shader_layout.BindStruct(shader_layout::StructName::Fog, struct_name);
	else if (name == "light")
		shader_layout.BindStruct(shader_layout::StructName::Light, struct_name);
}

void bind_attribute(const script_tree::PropertyNode &property,
	ShaderLayout &shader_layout)
{
	auto &name = property[0]
		.Get<ScriptType::Enumerable>()->Get();
	auto &decl_name = property[1]
		.Get<ScriptType::String>()->Get();

	auto declaration =
		[&]() noexcept -> shader_layout::VariableDeclaration
		{
			if (auto location = property[2].Get<ScriptType::Integer>(); location)
				return {decl_name, location->As<int>()};
			else
				return {decl_name};
		}();

	if (name == "vertex-position")
		shader_layout.BindAttribute(shader_layout::AttributeName::Vertex_Position, std::move(declaration));
	else if (name == "vertex-normal")
		shader_layout.BindAttribute(shader_layout::AttributeName::Vertex_Normal, std::move(declaration));
	else if (name == "vertex-color")
		shader_layout.BindAttribute(shader_layout::AttributeName::Vertex_Color, std::move(declaration));
	else if (name == "vertex-tex-coord")
		shader_layout.BindAttribute(shader_layout::AttributeName::Vertex_TexCoord, std::move(declaration));
	else if (name == "vertex-rotation")
		shader_layout.BindAttribute(shader_layout::AttributeName::Vertex_Rotation, std::move(declaration));
	else if (name == "vertex-point-size")
		shader_layout.BindAttribute(shader_layout::AttributeName::Vertex_PointSize, std::move(declaration));
}

void bind_uniform(const script_tree::PropertyNode &property,
	ShaderLayout &shader_layout)
{
	auto &name = property[0]
		.Get<ScriptType::Enumerable>()->Get();
	auto &decl_name = property[1]
		.Get<ScriptType::String>()->Get();

	auto declaration =
		[&]() noexcept -> shader_layout::VariableDeclaration
		{
			if (auto location = property[2].Get<ScriptType::Integer>(); location)
				return {decl_name, location->As<int>()};
			else
				return {decl_name};
		}();

	//Matrix
	if (name == "matrix-model-view")
		shader_layout.BindUniform(shader_layout::UniformName::Matrix_ModelView, std::move(declaration));
	else if (name == "matrix-projection")
		shader_layout.BindUniform(shader_layout::UniformName::Matrix_Projection, std::move(declaration));
	else if (name == "matrix-model-view-projection")
		shader_layout.BindUniform(shader_layout::UniformName::Matrix_ModelViewProjection, std::move(declaration));
	else if (name == "matrix-normal")
		shader_layout.BindUniform(shader_layout::UniformName::Matrix_Normal, std::move(declaration));

	//Scene
	else if (name == "scene-ambient")
		shader_layout.BindUniform(shader_layout::UniformName::Scene_Ambient, std::move(declaration));
	else if (name == "scene-gamma")
		shader_layout.BindUniform(shader_layout::UniformName::Scene_Gamma, std::move(declaration));
	else if (name == "scene-has-fog")
		shader_layout.BindUniform(shader_layout::UniformName::Scene_HasFog, std::move(declaration));
	else if (name == "scene-light-count")
		shader_layout.BindUniform(shader_layout::UniformName::Scene_LightCount, std::move(declaration));

	//Camera
	else if (name == "camera-position")
		shader_layout.BindUniform(shader_layout::UniformName::Camera_Position, std::move(declaration));
	else if (name == "camera-rotation")
		shader_layout.BindUniform(shader_layout::UniformName::Camera_Rotation, std::move(declaration));

	//Node
	else if (name == "node-position")
		shader_layout.BindUniform(shader_layout::UniformName::Node_Position, std::move(declaration));
	else if (name == "node-direction")
		shader_layout.BindUniform(shader_layout::UniformName::Node_Direction, std::move(declaration));
	else if (name == "node-rotation")
		shader_layout.BindUniform(shader_layout::UniformName::Node_Rotation, std::move(declaration));
	else if (name == "node-scaling")
		shader_layout.BindUniform(shader_layout::UniformName::Node_Scaling, std::move(declaration));

	//Primitive
	else if (name == "primitive-texture")
		shader_layout.BindUniform(shader_layout::UniformName::Primitive_Texture, std::move(declaration));
	else if (name == "primitive-has-texture")
		shader_layout.BindUniform(shader_layout::UniformName::Primitive_HasTexture, std::move(declaration));
	else if (name == "primitive-has-material")
		shader_layout.BindUniform(shader_layout::UniformName::Primitive_HasMaterial, std::move(declaration));

	//Material
	else if (name == "material-ambient")
		shader_layout.BindUniform(shader_layout::UniformName::Material_Ambient, std::move(declaration));
	else if (name == "material-diffuse")
		shader_layout.BindUniform(shader_layout::UniformName::Material_Diffuse, std::move(declaration));
	else if (name == "material-specular")
		shader_layout.BindUniform(shader_layout::UniformName::Material_Specular, std::move(declaration));
	else if (name == "material-emissive")
		shader_layout.BindUniform(shader_layout::UniformName::Material_Emissive, std::move(declaration));
	else if (name == "material-shininess")
		shader_layout.BindUniform(shader_layout::UniformName::Material_Shininess, std::move(declaration));
	else if (name == "material-diffuse-map")
		shader_layout.BindUniform(shader_layout::UniformName::Material_DiffuseMap, std::move(declaration));
	else if (name == "material-specular-map")
		shader_layout.BindUniform(shader_layout::UniformName::Material_SpecularMap, std::move(declaration));
	else if (name == "material-normal-map")
		shader_layout.BindUniform(shader_layout::UniformName::Material_NormalMap, std::move(declaration));
	else if (name == "material-has-diffuse-map")
		shader_layout.BindUniform(shader_layout::UniformName::Material_HasDiffuseMap, std::move(declaration));
	else if (name == "material-has-specular-map")
		shader_layout.BindUniform(shader_layout::UniformName::Material_HasSpecularMap, std::move(declaration));
	else if (name == "material-has-normal-map")
		shader_layout.BindUniform(shader_layout::UniformName::Material_HasNormalMap, std::move(declaration));
	else if (name == "material-lighting-enabled")
		shader_layout.BindUniform(shader_layout::UniformName::Material_LightingEnabled, std::move(declaration));

	//Fog
	else if (name == "fog-mode")
		shader_layout.BindUniform(shader_layout::UniformName::Fog_Mode, std::move(declaration));
	else if (name == "fog-density")
		shader_layout.BindUniform(shader_layout::UniformName::Fog_Density, std::move(declaration));
	else if (name == "fog-near")
		shader_layout.BindUniform(shader_layout::UniformName::Fog_Near, std::move(declaration));
	else if (name == "fog-far")
		shader_layout.BindUniform(shader_layout::UniformName::Fog_Far, std::move(declaration));
	else if (name == "fog-color")
		shader_layout.BindUniform(shader_layout::UniformName::Fog_Color, std::move(declaration));

	//Light
	else if (name == "light-type")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Type, std::move(declaration));
	else if (name == "light-position")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Position, std::move(declaration));
	else if (name == "light-direction")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Direction, std::move(declaration));
	else if (name == "light-ambient")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Ambient, std::move(declaration));
	else if (name == "light-diffuse")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Diffuse, std::move(declaration));
	else if (name == "light-specular")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Specular, std::move(declaration));
	else if (name == "light-constant")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Constant, std::move(declaration));
	else if (name == "light-linear")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Linear, std::move(declaration));
	else if (name == "light-quadratic")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Quadratic, std::move(declaration));
	else if (name == "light-cutoff")
		shader_layout.BindUniform(shader_layout::UniformName::Light_Cutoff, std::move(declaration));
	else if (name == "light-outer-cutoff")
		shader_layout.BindUniform(shader_layout::UniformName::Light_OuterCutoff, std::move(declaration));
}


NonOwningPtr<ShaderLayout> create_shader_layout(const script_tree::ObjectNode &object,
	ShaderProgramManager &shader_program_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();

	auto shader_layout = shader_program_manager.CreateShaderLayout(name);

	if (shader_layout)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "bind-attribute")
				bind_attribute(property, *shader_layout);
			else if (property.Name() == "bind-struct")
				bind_struct(property, *shader_layout);
			else if (property.Name() == "bind-uniform")
				bind_uniform(property, *shader_layout);
		}
	}

	return shader_layout;
}

void create_shader_layouts(const ScriptTree &tree,
	ShaderProgramManager &shader_program_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "shader-layout")
			create_shader_layout(object, shader_program_manager);
	}
}

} //shader_layout_script_interface::detail


//Private

ScriptValidator ShaderLayoutScriptInterface::GetValidator() const
{
	return detail::get_shader_layout_validator();
}


/*
	Shader layouts
	Creating from script
*/

void ShaderLayoutScriptInterface::CreateShaderLayouts(std::string_view asset_name,
	ShaderProgramManager &shader_program_manager)
{
	if (Load(asset_name))
		detail::create_shader_layouts(*tree_, shader_program_manager);
}

} //ion::script::interfaces