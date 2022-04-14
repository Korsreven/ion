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
#include "graphics/shaders/variables/IonShaderAttribute.h"
#include "graphics/shaders/variables/IonShaderTypes.h"
#include "graphics/shaders/variables/IonShaderUniform.h"

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
		.AddProperty("attribute", {attribute_data_types, ParameterType::String, ParameterType::Integer}, 2)
		.AddProperty("fragment-shader", ParameterType::String)
		.AddProperty("shader-layout", ParameterType::String)
		.AddProperty("uniform", {uniform_data_types, ParameterType::String, ParameterType::Integer}, 2)
		.AddProperty("vertex-shader", ParameterType::String);
}

ClassDefinition get_shader_struct_class()
{
	return ClassDefinition::Create("struct")
		.AddRequiredProperty("name", ParameterType::String)
		.AddProperty("size", ParameterType::Integer)
		.AddProperty("uniform", {uniform_data_types, ParameterType::String, ParameterType::Integer}, 2);
}


ScriptValidator get_shader_program_validator()
{
	return ScriptValidator::Create()
		.AddRequiredClass(get_shader_program_class());
}


/*
	Tree parsing
*/

void create_attribute(const script_tree::PropertyNode &property,
	ShaderProgram &shader_program)
{
	using namespace variables;

	auto type = property[0]
		.Get<ScriptType::Enumerable>()->Get();
	auto name = property[1]
		.Get<ScriptType::String>()->Get();

	//Basic types
	if (type == "bool")
		shader_program.CreateAttribute<bool>(std::move(name));
	else if (type == "int")
		shader_program.CreateAttribute<int32>(std::move(name));
	else if (type == "uint")
		shader_program.CreateAttribute<uint32>(std::move(name));
	else if (type == "float")
		shader_program.CreateAttribute<float32>(std::move(name));
	else if (type == "double")
		shader_program.CreateAttribute<float64>(std::move(name));

	//Vector types
	else if (type == "bvec2")
		shader_program.CreateAttribute<glsl::bvec2>(std::move(name));
	else if (type == "ivec2")
		shader_program.CreateAttribute<glsl::ivec2>(std::move(name));
	else if (type == "uvec2")
		shader_program.CreateAttribute<glsl::uvec2>(std::move(name));
	else if (type == "vec2")
		shader_program.CreateAttribute<glsl::vec2>(std::move(name));
	else if (type == "dvec2")
		shader_program.CreateAttribute<glsl::dvec2>(std::move(name));
	else if (type == "bvec3")
		shader_program.CreateAttribute<glsl::bvec3>(std::move(name));
	else if (type == "ivec3")
		shader_program.CreateAttribute<glsl::ivec3>(std::move(name));
	else if (type == "uvec3")
		shader_program.CreateAttribute<glsl::uvec3>(std::move(name));
	else if (type == "vec3")
		shader_program.CreateAttribute<glsl::vec3>(std::move(name));
	else if (type == "dvec3")
		shader_program.CreateAttribute<glsl::dvec3>(std::move(name));
	else if (type == "bvec4")
		shader_program.CreateAttribute<glsl::bvec4>(std::move(name));
	else if (type == "ivec4")
		shader_program.CreateAttribute<glsl::ivec4>(std::move(name));
	else if (type == "uvec4")
		shader_program.CreateAttribute<glsl::uvec4>(std::move(name));
	else if (type == "vec4")
		shader_program.CreateAttribute<glsl::vec4>(std::move(name));
	else if (type == "dvec4")
		shader_program.CreateAttribute<glsl::dvec4>(std::move(name));

	//Matrix types
	else if (type == "mat2x2" || type == "mat2")
		shader_program.CreateAttribute<glsl::mat2x2>(std::move(name));
	else if (type == "dmat2x2" || type == "dmat2")
		shader_program.CreateAttribute<glsl::dmat2x2>(std::move(name));
	else if (type == "mat2x3")
		shader_program.CreateAttribute<glsl::mat2x3>(std::move(name));
	else if (type == "dmat2x3")
		shader_program.CreateAttribute<glsl::dmat2x3>(std::move(name));
	else if (type == "mat2x4")
		shader_program.CreateAttribute<glsl::mat2x4>(std::move(name));
	else if (type == "dmat2x4")
		shader_program.CreateAttribute<glsl::dmat2x4>(std::move(name));
	else if (type == "mat3x2")
		shader_program.CreateAttribute<glsl::mat3x2>(std::move(name));
	else if (type == "dmat3x2")
		shader_program.CreateAttribute<glsl::dmat3x2>(std::move(name));
	else if (type == "mat3x3" || type == "mat3")
		shader_program.CreateAttribute<glsl::mat3x3>(std::move(name));
	else if (type == "dmat3x3" || type == "dmat3")
		shader_program.CreateAttribute<glsl::dmat3x3>(std::move(name));
	else if (type == "mat3x4")
		shader_program.CreateAttribute<glsl::mat3x4>(std::move(name));
	else if (type == "dmat3x4")
		shader_program.CreateAttribute<glsl::dmat3x4>(std::move(name));
	else if (type == "mat4x2")
		shader_program.CreateAttribute<glsl::mat4x2>(std::move(name));
	else if (type == "dmat4x2")
		shader_program.CreateAttribute<glsl::dmat4x2>(std::move(name));
	else if (type == "mat4x3")
		shader_program.CreateAttribute<glsl::mat4x3>(std::move(name));
	else if (type == "dmat4x3")
		shader_program.CreateAttribute<glsl::dmat4x3>(std::move(name));
	else if (type == "mat4x4" || type == "mat4")
		shader_program.CreateAttribute<glsl::mat4x4>(std::move(name));
	else if (type == "dmat4x4" || type == "dmat4")
		shader_program.CreateAttribute<glsl::dmat4x4>(std::move(name));
}

void create_uniform(const script_tree::PropertyNode &property,
	ShaderProgram &shader_program)
{
	using namespace variables;

	auto type = property[0]
		.Get<ScriptType::Enumerable>()->Get();
	auto name = property[1]
		.Get<ScriptType::String>()->Get();
	auto size = property[2]
		.Get<ScriptType::Integer>().value_or(1).As<int>();

	//Basic types
	if (type == "bool")
		shader_program.CreateUniform<bool>(std::move(name), size);
	else if (type == "int")
		shader_program.CreateUniform<int32>(std::move(name), size);
	else if (type == "uint")
		shader_program.CreateUniform<uint32>(std::move(name), size);
	else if (type == "float")
		shader_program.CreateUniform<float>(std::move(name), size);
	else if (type == "double")
		shader_program.CreateUniform<double>(std::move(name), size);

	//Vector types
	else if (type == "bvec2")
		shader_program.CreateUniform<glsl::bvec2>(std::move(name), size);
	else if (type == "ivec2")
		shader_program.CreateUniform<glsl::ivec2>(std::move(name), size);
	else if (type == "uvec2")
		shader_program.CreateUniform<glsl::uvec2>(std::move(name), size);
	else if (type == "vec2")
		shader_program.CreateUniform<glsl::vec2>(std::move(name), size);
	else if (type == "dvec2")
		shader_program.CreateUniform<glsl::dvec2>(std::move(name), size);
	else if (type == "bvec3")
		shader_program.CreateUniform<glsl::bvec3>(std::move(name), size);
	else if (type == "ivec3")
		shader_program.CreateUniform<glsl::ivec3>(std::move(name), size);
	else if (type == "uvec3")
		shader_program.CreateUniform<glsl::uvec3>(std::move(name), size);
	else if (type == "vec3")
		shader_program.CreateUniform<glsl::vec3>(std::move(name), size);
	else if (type == "dvec3")
		shader_program.CreateUniform<glsl::dvec3>(std::move(name), size);
	else if (type == "bvec4")
		shader_program.CreateUniform<glsl::bvec4>(std::move(name), size);
	else if (type == "ivec4")
		shader_program.CreateUniform<glsl::ivec4>(std::move(name), size);
	else if (type == "uvec4")
		shader_program.CreateUniform<glsl::uvec4>(std::move(name), size);
	else if (type == "vec4")
		shader_program.CreateUniform<glsl::vec4>(std::move(name), size);
	else if (type == "dvec4")
		shader_program.CreateUniform<glsl::dvec4>(std::move(name), size);

	//Matrix types
	else if (type == "mat2x2" || type == "mat2")
		shader_program.CreateUniform<glsl::mat2x2>(std::move(name), size);
	else if (type == "dmat2x2" || type == "dmat2")
		shader_program.CreateUniform<glsl::dmat2x2>(std::move(name), size);
	else if (type == "mat2x3")
		shader_program.CreateUniform<glsl::mat2x3>(std::move(name), size);
	else if (type == "dmat2x3")
		shader_program.CreateUniform<glsl::dmat2x3>(std::move(name), size);
	else if (type == "mat2x4")
		shader_program.CreateUniform<glsl::mat2x4>(std::move(name), size);
	else if (type == "dmat2x4")
		shader_program.CreateUniform<glsl::dmat2x4>(std::move(name), size);
	else if (type == "mat3x2")
		shader_program.CreateUniform<glsl::mat3x2>(std::move(name), size);
	else if (type == "dmat3x2")
		shader_program.CreateUniform<glsl::dmat3x2>(std::move(name), size);
	else if (type == "mat3x3" || type == "mat3")
		shader_program.CreateUniform<glsl::mat3x3>(std::move(name), size);
	else if (type == "dmat3x3" || type == "dmat3")
		shader_program.CreateUniform<glsl::dmat3x3>(std::move(name), size);
	else if (type == "mat3x4")
		shader_program.CreateUniform<glsl::mat3x4>(std::move(name), size);
	else if (type == "dmat3x4")
		shader_program.CreateUniform<glsl::dmat3x4>(std::move(name), size);
	else if (type == "mat4x2")
		shader_program.CreateUniform<glsl::mat4x2>(std::move(name), size);
	else if (type == "dmat4x2")
		shader_program.CreateUniform<glsl::dmat4x2>(std::move(name), size);
	else if (type == "mat4x3")
		shader_program.CreateUniform<glsl::mat4x3>(std::move(name), size);
	else if (type == "dmat4x3")
		shader_program.CreateUniform<glsl::dmat4x3>(std::move(name), size);
	else if (type == "mat4x4" || type == "mat4")
		shader_program.CreateUniform<glsl::mat4x4>(std::move(name), size);
	else if (type == "dmat4x4" || type == "dmat4")
		shader_program.CreateUniform<glsl::dmat4x4>(std::move(name), size);

	//Sampler types
	else if (type == "isampler2D")
		shader_program.CreateUniform<glsl::isampler2D>(std::move(name), size);
	else if (type == "usampler2D")
		shader_program.CreateUniform<glsl::usampler2D>(std::move(name), size);
	else if (type == "sampler2D")
		shader_program.CreateUniform<glsl::sampler2D>(std::move(name), size);
}

void create_uniform(const script_tree::PropertyNode &property,
	ShaderStruct &shader_struct)
{
	using namespace variables;

	auto type = property[0]
		.Get<ScriptType::Enumerable>()->Get();
	auto name = property[1]
		.Get<ScriptType::String>()->Get();
	auto size = property[2]
		.Get<ScriptType::Integer>().value_or(1).As<int>();

	//Basic types
	if (type == "bool")
		shader_struct.CreateUniform<bool>(std::move(name), size);
	else if (type == "int")
		shader_struct.CreateUniform<int32>(std::move(name), size);
	else if (type == "uint")
		shader_struct.CreateUniform<uint32>(std::move(name), size);
	else if (type == "float")
		shader_struct.CreateUniform<float>(std::move(name), size);
	else if (type == "double")
		shader_struct.CreateUniform<double>(std::move(name), size);

	//Vector types
	else if (type == "bvec2")
		shader_struct.CreateUniform<glsl::bvec2>(std::move(name), size);
	else if (type == "ivec2")
		shader_struct.CreateUniform<glsl::ivec2>(std::move(name), size);
	else if (type == "uvec2")
		shader_struct.CreateUniform<glsl::uvec2>(std::move(name), size);
	else if (type == "vec2")
		shader_struct.CreateUniform<glsl::vec2>(std::move(name), size);
	else if (type == "dvec2")
		shader_struct.CreateUniform<glsl::dvec2>(std::move(name), size);
	else if (type == "bvec3")
		shader_struct.CreateUniform<glsl::bvec3>(std::move(name), size);
	else if (type == "ivec3")
		shader_struct.CreateUniform<glsl::ivec3>(std::move(name), size);
	else if (type == "uvec3")
		shader_struct.CreateUniform<glsl::uvec3>(std::move(name), size);
	else if (type == "vec3")
		shader_struct.CreateUniform<glsl::vec3>(std::move(name), size);
	else if (type == "dvec3")
		shader_struct.CreateUniform<glsl::dvec3>(std::move(name), size);
	else if (type == "bvec4")
		shader_struct.CreateUniform<glsl::bvec4>(std::move(name), size);
	else if (type == "ivec4")
		shader_struct.CreateUniform<glsl::ivec4>(std::move(name), size);
	else if (type == "uvec4")
		shader_struct.CreateUniform<glsl::uvec4>(std::move(name), size);
	else if (type == "vec4")
		shader_struct.CreateUniform<glsl::vec4>(std::move(name), size);
	else if (type == "dvec4")
		shader_struct.CreateUniform<glsl::dvec4>(std::move(name), size);

	//Matrix types
	else if (type == "mat2x2" || type == "mat2")
		shader_struct.CreateUniform<glsl::mat2x2>(std::move(name), size);
	else if (type == "dmat2x2" || type == "dmat2")
		shader_struct.CreateUniform<glsl::dmat2x2>(std::move(name), size);
	else if (type == "mat2x3")
		shader_struct.CreateUniform<glsl::mat2x3>(std::move(name), size);
	else if (type == "dmat2x3")
		shader_struct.CreateUniform<glsl::dmat2x3>(std::move(name), size);
	else if (type == "mat2x4")
		shader_struct.CreateUniform<glsl::mat2x4>(std::move(name), size);
	else if (type == "dmat2x4")
		shader_struct.CreateUniform<glsl::dmat2x4>(std::move(name), size);
	else if (type == "mat3x2")
		shader_struct.CreateUniform<glsl::mat3x2>(std::move(name), size);
	else if (type == "dmat3x2")
		shader_struct.CreateUniform<glsl::dmat3x2>(std::move(name), size);
	else if (type == "mat3x3" || type == "mat3")
		shader_struct.CreateUniform<glsl::mat3x3>(std::move(name), size);
	else if (type == "dmat3x3" || type == "dmat3")
		shader_struct.CreateUniform<glsl::dmat3x3>(std::move(name), size);
	else if (type == "mat3x4")
		shader_struct.CreateUniform<glsl::mat3x4>(std::move(name), size);
	else if (type == "dmat3x4")
		shader_struct.CreateUniform<glsl::dmat3x4>(std::move(name), size);
	else if (type == "mat4x2")
		shader_struct.CreateUniform<glsl::mat4x2>(std::move(name), size);
	else if (type == "dmat4x2")
		shader_struct.CreateUniform<glsl::dmat4x2>(std::move(name), size);
	else if (type == "mat4x3")
		shader_struct.CreateUniform<glsl::mat4x3>(std::move(name), size);
	else if (type == "dmat4x3")
		shader_struct.CreateUniform<glsl::dmat4x3>(std::move(name), size);
	else if (type == "mat4x4" || type == "mat4")
		shader_struct.CreateUniform<glsl::mat4x4>(std::move(name), size);
	else if (type == "dmat4x4" || type == "dmat4")
		shader_struct.CreateUniform<glsl::dmat4x4>(std::move(name), size);

	//Sampler types
	else if (type == "isampler2D")
		shader_struct.CreateUniform<glsl::isampler2D>(std::move(name), size);
	else if (type == "usampler2D")
		shader_struct.CreateUniform<glsl::usampler2D>(std::move(name), size);
	else if (type == "sampler2D")
		shader_struct.CreateUniform<glsl::sampler2D>(std::move(name), size);
}


NonOwningPtr<ShaderProgram> create_shader_program(const script_tree::ObjectNode &object,
	ShaderProgramManager &shader_program_manager,
	ShaderManager &shader_manager)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto vertex_shader_name = object
		.Property("vertex-shader")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto fragment_shader_name = object
		.Property("fragment-shader")[0]
		.Get<ScriptType::String>().value_or(""s).Get();
	auto shader_layout_name = object
		.Property("shader-layout")[0]
		.Get<ScriptType::String>().value_or(""s).Get();

	auto shader_program = shader_program_manager.CreateShaderProgram(std::move(name),
		shader_manager.GetShader(vertex_shader_name), shader_manager.GetShader(fragment_shader_name),
		shader_program_manager.GetShaderLayout(shader_layout_name));

	if (shader_program)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "attribute")
				create_attribute(property, *shader_program);
			else if (property.Name() == "uniform")
				create_uniform(property, *shader_program);
		}
	}

	return shader_program;
}

NonOwningPtr<ShaderStruct> create_shader_struct(const script_tree::ObjectNode &object,
	ShaderProgram &shader_program)
{
	auto name = object
		.Property("name")[0]
		.Get<ScriptType::String>()->Get();
	auto size = object
		.Property("size")[0]
		.Get<ScriptType::Integer>().value_or(1).As<int>();

	auto shader_struct = shader_program.CreateStruct(std::move(name), size);

	if (shader_struct)
	{
		for (auto &property : object.Properties())
		{
			if (property.Name() == "uniform")
				create_uniform(property, *shader_struct);
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