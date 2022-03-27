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
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("", ParameterType::FloatingPoint);
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

	auto shader_program = shader_program_manager.CreateShaderProgram(name, {}, {}, {});

	if (shader_program)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "");
		}
	}

	return shader_program;
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
	{
		for (auto &object : tree_->Objects())
			detail::create_shader_program(object, shader_program_manager, shader_manager);
	}
}

} //ion::script::interfaces