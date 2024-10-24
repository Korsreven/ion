/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonShaderProgramScriptInterface.h
-------------------------------------------
*/

#ifndef ION_SHADER_PROGRAM_SCRIPT_INTERFACE_H
#define ION_SHADER_PROGRAM_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/shaders/IonShaderStruct.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

namespace ion::script::interfaces
{
	namespace shader_program_script_interface::detail
	{
		static inline const Strings attribute_data_types
		{
			//Basic types
			"bool", "int", "uint", "float", "double",

			//Vector types
			"bvec2", "ivec2", "uvec2", "vec2", "dvec2",
			"bvec3", "ivec3", "uvec3", "vec3", "dvec3",
			"bvec4", "ivec4", "uvec4", "vec4", "dvec4",

			//Matrix types
			"mat2x2", "dmat2x2", "mat2", "dmat2",
			"mat2x3", "dmat2x3",
			"mat2x4", "dmat2x4",
			"mat3x2", "dmat3x2",
			"mat3x3", "dmat3x3", "mat3", "dmat3",
			"mat3x4", "dmat3x4",
			"mat4x2", "dmat4x2",
			"mat4x3", "dmat4x3",
			"mat4x4", "dmat4x4", "mat4", "dmat4"
		};

		static inline const Strings uniform_data_types
		{
			//Basic types
			"bool", "int", "uint", "float", "double",

			//Vector types
			"bvec2", "ivec2", "uvec2", "vec2", "dvec2",
			"bvec3", "ivec3", "uvec3", "vec3", "dvec3",
			"bvec4", "ivec4", "uvec4", "vec4", "dvec4",

			//Matrix types
			"mat2x2", "dmat2x2", "mat2", "dmat2",
			"mat2x3", "dmat2x3",
			"mat2x4", "dmat2x4",
			"mat3x2", "dmat3x2",
			"mat3x3", "dmat3x3", "mat3", "dmat3",
			"mat3x4", "dmat3x4",
			"mat4x2", "dmat4x2",
			"mat4x3", "dmat4x3",
			"mat4x4", "dmat4x4", "mat4", "dmat4",
			
			//Sampler types
			"isampler1D", "usampler1D", "sampler1D",
			"isampler1DArray", "usampler1DArray", "sampler1DArray",
			"isampler2D", "usampler2D", "sampler2D",
			"isampler2DArray", "usampler2DArray", "sampler2DArray"
		};


		NonOwningPtr<graphics::shaders::Shader> get_shader(std::string_view name, const ManagerRegister &managers) noexcept;
		NonOwningPtr<graphics::shaders::ShaderProgram> get_shader_program(std::string_view name, const ManagerRegister &managers) noexcept;


		/**
			@name Validator classes
			@{
		*/

		script_validator::ClassDefinition get_shader_program_class();
		script_validator::ClassDefinition get_shader_struct_class();

		ScriptValidator get_shader_program_validator();

		///@}

		/**
			@name Tree parsing
			@{
		*/

		void create_attribute(const script_tree::PropertyNode &property,
			graphics::shaders::ShaderProgram &shader_program);
		void create_uniform(const script_tree::PropertyNode &property,
			graphics::shaders::ShaderProgram &shader_program);
		void create_uniform(const script_tree::PropertyNode &property,
			graphics::shaders::ShaderStruct &shader_struct);


		void set_shader_program_properties(const script_tree::ObjectNode &object,
			graphics::shaders::ShaderProgram &shader_program, const ManagerRegister &managers);
		void set_shader_struct_properties(const script_tree::ObjectNode &object, graphics::shaders::ShaderStruct &shader_struct);


		NonOwningPtr<graphics::shaders::ShaderProgram> create_shader_program(const script_tree::ObjectNode &object,
			graphics::shaders::ShaderProgramManager &shader_program_manager, const ManagerRegister &managers);
		NonOwningPtr<graphics::shaders::ShaderStruct> create_shader_struct(const script_tree::ObjectNode &object,
			graphics::shaders::ShaderProgram &shader_program);

		void create_shader_programs(const ScriptTree &tree,
			graphics::shaders::ShaderProgramManager &shader_program_manager, const ManagerRegister &managers);

		///@}
	} //shader_program_script_interface::detail


	///@brief A class representing an interface to a shader program script with a complete validation scheme
	///@details A shader program script can load shader programs from a script file into a shader program manager
	class ShaderProgramScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			///@brief Default constructor
			ShaderProgramScriptInterface() = default;


			/**
				@name Shader programs
				Creating from script
				@{
			*/

			///@brief Creates shader programs from a script (or object file) with the given asset name
			void CreateShaderPrograms(std::string_view asset_name,
				graphics::shaders::ShaderProgramManager &shader_program_manager);

			///@brief Creates shader programs from a script (or object file) with the given asset name
			void CreateShaderPrograms(std::string_view asset_name,
				graphics::shaders::ShaderProgramManager &shader_program_manager, const ManagerRegister &managers);

			///@}
	};
} //ion::script::interfaces

#endif