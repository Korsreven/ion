/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonShaderScriptInterface.cpp
-------------------------------------------
*/

#include "IonShaderScriptInterface.h"

#include <string>

namespace ion::script::interfaces
{

using namespace std::string_literals;
using namespace script_validator;
using namespace shader_script_interface;
using namespace graphics::shaders;

namespace shader_script_interface::detail
{

/*
	Validator classes
*/

ClassDefinition get_shader_class()
{
	return ClassDefinition::Create("shader")
		.AddRequiredProperty("asset-name", ParameterType::String)
		.AddRequiredProperty("name", ParameterType::String);
}

ScriptValidator get_shader_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_shader_class());
}


/*
	Tree parsing
*/

NonOwningPtr<Shader> create_shader(const script_tree::ObjectNode &object,
	ShaderManager &shader_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto asset_name = object
		.Property("asset-name")[0]
		.Get<ScriptType::String>()->Get();

	return shader_manager.CreateShader(name, asset_name);
}

void create_shaders(const ScriptTree &tree,
	ShaderManager &shader_manager)
{
	for (auto &object : tree.Objects())
	{
		if (object.Name() == "shader")
			create_shader(object, shader_manager);
	}
}

} //shader_script_interface::detail


//Private

ScriptValidator ShaderScriptInterface::GetValidator() const
{
	return detail::get_shader_validator();
}


/*
	Shaders
	Creating from script
*/

void ShaderScriptInterface::CreateShaders(std::string_view asset_name,
	ShaderManager &shader_manager)
{
	if (Load(asset_name))
		detail::create_shaders(*tree_, shader_manager);
}

} //ion::script::interfaces