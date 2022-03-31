/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonShaderLayoutScriptInterface.h
-------------------------------------------
*/

#ifndef ION_SHADER_LAYOUT_SCRIPT_INTERFACE_H
#define ION_SHADER_LAYOUT_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "types/IonTypes.h"

namespace ion::script::interfaces
{
	namespace shader_layout_script_interface::detail
	{
		inline static const Strings struct_names
		{
			"matrix",
			"scene",
			"camera",
			"node",
			"primitive",
			"material",
			"fog",
			"light"
		};

		inline static const Strings attribute_names
		{
			"vertex-position",
			"vertex-normal",
			"vertex-color",
			"vertex-tex-coord",
			"vertex-rotation",
			"vertex-point-size"
		};

		inline static const Strings uniform_names
		{
			//Matrix
			"matrix-model-view",
			"matrix-projection",
			"matrix-model-view-projection",
			"matrix-normal",

			//Scene
			"scene-ambient",
			"scene-gamma",
			"scene-has-fog",
			"scene-light-count",

			//Camera
			"camera-position",
			"camera-rotation",

			//Node
			"node-position",
			"node-direction",
			"node-rotation",
			"node-scaling",

			//Primitive
			"primitive-texture",
			"primitive-has-texture",
			"primitive-has-material",

			//Material
			"material-ambient",
			"material-diffuse",
			"material-specular",
			"material-emissive",
			"material-shininess",
			"material-diffuseMap",
			"material-specularMap",
			"material-normalMap",
			"material-has-diffuse-map",
			"material-has-specular-map",
			"material-has-normal-map",
			"material-lighting-enabled",

			//Fog
			"fog-mode",
			"fog-density",
			"fog-near",
			"fog-far",
			"fog-color",

			//Light
			"light-type",
			"light-position",
			"light-direction",
			"light-ambient",
			"light-diffuse",
			"light-specular",
			"light-constant",
			"light-linear",
			"light-quadratic",
			"light-cutoff",
			"light-outer-cutoff"
		};


		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_shader_layout_class();
		ScriptValidator get_shader_layout_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::shaders::ShaderLayout> create_shader_layout(const script_tree::ObjectNode &object,
			graphics::shaders::ShaderProgramManager &shader_program_manager);

		void create_shader_layouts(const ScriptTree &tree,
			graphics::shaders::ShaderProgramManager &shader_program_manager);
	} //shader_layout_script_interface::detail


	class ShaderLayoutScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			ShaderLayoutScriptInterface() = default;


			/*
				Shader layouts
				Creating from script
			*/

			//Create shader layouts from a script (or object file) with the given asset name
			void CreateShaderLayouts(std::string_view asset_name,
				graphics::shaders::ShaderProgramManager &shader_program_manager);
	};
} //ion::script::interfaces

#endif