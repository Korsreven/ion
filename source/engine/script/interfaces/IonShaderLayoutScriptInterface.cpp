/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonShaderProgramScriptInterface.cpp
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
		.AddProperty("attribute-binding", {attribute_names, ParameterType::String, ParameterType::Integer}, 2)
		.AddProperty("struct-binding", {struct_names, ParameterType::String})
		.AddProperty("uniform-binding", {uniform_names, ParameterType::String, ParameterType::Integer}, 2);
}

ScriptValidator get_shader_layout_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_shader_layout_class());
}


/*
	Tree parsing
*/

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
			if (property.Name() == "attribute-binding")
			{
			}
			else if (property.Name() == "struct-binding")
			{
			}
			else if (property.Name() == "uniform-binding")
			{
			}
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