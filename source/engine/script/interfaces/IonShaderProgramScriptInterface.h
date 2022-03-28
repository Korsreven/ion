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
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"

//Forward declarations
namespace ion::graphics
{
	namespace shader
	{
		class ShaderManager;
	}
}

namespace ion::script::interfaces
{
	namespace shader_program_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_shader_program_class();
		ScriptValidator get_shader_program_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::shaders::ShaderProgram> create_shader_program(const script_tree::ObjectNode &object,
			graphics::shaders::ShaderProgramManager &shader_program_manager,
			graphics::shaders::ShaderManager &shader_manager);

		void create_shader_programs(const ScriptTree &tree,
			graphics::shaders::ShaderProgramManager &shader_program_manager,
			graphics::shaders::ShaderManager &shader_manager);
	} //shader_program_script_interface::detail


	class ShaderProgramScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			ShaderProgramScriptInterface() = default;


			/*
				Shader programs
				Creating from script
			*/

			//Create shader programs from a script (or object file) with the given asset name
			void CreateShaderPrograms(std::string_view asset_name,
				graphics::shaders::ShaderProgramManager &shader_program_manager,
				graphics::shaders::ShaderManager &shader_manager);
	};
} //ion::script::interfaces

#endif