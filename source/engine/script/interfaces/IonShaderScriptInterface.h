/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonShaderScriptInterface.h
-------------------------------------------
*/

#ifndef ION_SHADER_SCRIPT_INTERFACE_H
#define ION_SHADER_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/shaders/IonShader.h"
#include "graphics/shaders/IonShaderManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

namespace ion::script::interfaces
{
	namespace shader_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_shader_class();
		ScriptValidator get_shader_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::shaders::Shader> create_shader(const script_tree::ObjectNode &object,
			graphics::shaders::ShaderManager &shader_manager);

		void create_shaders(const ScriptTree &tree,
			graphics::shaders::ShaderManager &shader_manager);
	} //shader_script_interface::detail


	//A class representing an interface to a shader script with a complete validation scheme
	//A shader script can load shaders from a script file into a shader manager
	class ShaderScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			ShaderScriptInterface() = default;


			/*
				Shaders
				Creating from script
			*/

			//Creates shaders from a script (or object file) with the given asset name
			void CreateShaders(std::string_view asset_name,
				graphics::shaders::ShaderManager &shader_manager);
	};
} //ion::script::interfaces

#endif