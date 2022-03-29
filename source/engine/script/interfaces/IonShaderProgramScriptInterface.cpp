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

#include "IonShaderProgramScriptInterface.h"

#include <string>
#include "graphics/shaders/IonShaderManager.h"

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace shader_program_script_interface;
using namespace graphics::shaders;

namespace shader_program_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_shader_program_class()
{
	return ClassDefinition::Create("shader-program")
		.AddClass(get_shader_struct_class())

		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("attribute", {glsl_data_types, ParameterType::String, ParameterType::Integer}, 2)
		.AddProperty("fragment-shader", ParameterType::String)
		.AddProperty("shader-layout", ParameterType::String)
		.AddProperty("uniform", {glsl_data_types, ParameterType::String, ParameterType::Integer}, 2)
		.AddProperty("vertex-shader", ParameterType::String);
}

ClassDefinition get_shader_struct_class()
{
	return ClassDefinition::Create("struct")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("size", ParameterType::Integer)
		.AddProperty("uniform", {glsl_data_types, ParameterType::String, ParameterType::Integer}, 2);
}


ScriptValidator get_shader_program_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_shader_program_class());
}


/*
	Tree parsing
*/

NonOwningPtr<ShaderProgram> create_shader_program(const script_tree::ObjectNode &object,
	ShaderProgramManager &shader_program_manager,
	ShaderManager &shader_manager)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto &vertex_shader_name = object
		.Property("vertex-shader")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto &fragment_shader_name = object
		.Property("fragment-shader")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto &shader_layout_name = object
		.Property("shader-layout")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto shader_program = shader_program_manager.CreateShaderProgram(name,
		shader_manager.GetShader(vertex_shader_name), shader_manager.GetShader(fragment_shader_name),
		shader_program_manager.GetShaderLayout(shader_layout_name));

	if (shader_program)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "attribute")
			{
				//Todo
			}
			else if (property.Name() == "uniform")
			{
				//Todo
			}
		}
	}

	return shader_program;
}

NonOwningPtr<graphics::shaders::ShaderStruct> create_shader_struct(const script_tree::ObjectNode &object,
	ShaderProgram &shader_program)
{
	auto &name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto size = object
		.Property("size")[0]
		.Get<ScriptType::Integer>().value_or(1).As<int>();

	auto shader_struct = shader_program.CreateStruct(name, size);

	if (shader_struct)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "uniform")
			{
				//Todo
			}
		}
	}

	return shader_struct;
}


void create_shader_programs(const ScriptTree &tree,
	ShaderProgramManager &shader_program_manager,
	ShaderManager &shader_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "shader-program")
			create_shader_program(object, shader_program_manager, shader_manager);
	}
}

} //shader_program_script_interface::detail


//Private

ScriptValidator ShaderProgramScriptInterface::GetValidator() const
{
	return detail::get_shader_program_validator();
}


/*
	Shader programs
	Creating from script
*/

void ShaderProgramScriptInterface::CreateShaderPrograms(std::string_view asset_name,
	ShaderProgramManager &shader_program_manager,
	ShaderManager &shader_manager)
{
	if (Load(asset_name))
		detail::create_shader_programs(*tree_, shader_program_manager, shader_manager);
}

} //ion::script::interfaces