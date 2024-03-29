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
		static inline const Strings struct_names
		{
			"matrix",
			"scene",
			"camera",
			"primitive",
			"material",
			"fog",
			"light",
			"emissive_light"
		};

		static inline const Strings attribute_names
		{
			"vertex-position",
			"vertex-normal",
			"vertex-color",
			"vertex-tex-coord",
			"vertex-rotation",
			"vertex-point-size"
		};

		static inline const Strings uniform_names
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
			"scene-lights",
			"scene-emissive-lights",
			"scene-light-count",
			"scene-emissive-light-count",

			//Camera
			"camera-position",
			"camera-rotation",

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
			"material-diffuse-map",
			"material-normal-map",
			"material-specular-map",
			"material-emissive-map",
			"material-has-diffuse-map",
			"material-has-normal-map",
			"material-has-specular-map",
			"material-has-emissive-map",
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
			"light-radius",
			"light-ambient",
			"light-diffuse",
			"light-specular",
			"light-constant",
			"light-linear",
			"light-quadratic",
			"light-cutoff",
			"light-outer-cutoff",

			//Emissive light
			"emissive-light-position",
			"emissive-light-radius",
			"emissive-light-color"
		};


		/**
			@name Validator classes
			@{
		*/

		script_validator::ClassDefinition get_shader_layout_class();
		ScriptValidator get_shader_layout_validator();

		///@}

		/**
			@name Tree parsing
			@{
		*/

		void bind_struct(const script_tree::PropertyNode &property,
			graphics::shaders::ShaderLayout &shader_layout);
		void bind_attribute(const script_tree::PropertyNode &property,
			graphics::shaders::ShaderLayout &shader_layout);	
		void bind_uniform(const script_tree::PropertyNode &property,
			graphics::shaders::ShaderLayout &shader_layout);


		void set_shader_layout_properties(const script_tree::ObjectNode &object, graphics::shaders::ShaderLayout &shader_layout);


		NonOwningPtr<graphics::shaders::ShaderLayout> create_shader_layout(const script_tree::ObjectNode &object,
			graphics::shaders::ShaderProgramManager &shader_program_manager);

		void create_shader_layouts(const ScriptTree &tree,
			graphics::shaders::ShaderProgramManager &shader_program_manager);

		///@}
	} //shader_layout_script_interface::detail


	///@brief A class representing an interface to a shader layout script with a complete validation scheme
	///@details A shader layout script can load shader layouts from a script file into a shader program manager
	class ShaderLayoutScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			///@brief Default constructor
			ShaderLayoutScriptInterface() = default;


			/**
				@name Shader layouts
				Creating from script
				@{
			*/

			///@brief Creates shader layouts from a script (or object file) with the given asset name
			void CreateShaderLayouts(std::string_view asset_name,
				graphics::shaders::ShaderProgramManager &shader_program_manager);

			///@}
	};
} //ion::script::interfaces

#endif