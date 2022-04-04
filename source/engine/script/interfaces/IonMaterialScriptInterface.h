/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonMaterialScriptInterface.h
-------------------------------------------
*/

#ifndef ION_MATERIAL_SCRIPT_INTERFACE_H
#define ION_MATERIAL_SCRIPT_INTERFACE_H

#include <string_view>

#include "IonScriptInterface.h"
#include "graphics/materials/IonMaterial.h"
#include "graphics/materials/IonMaterialManager.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"

//Forward declarations
namespace ion::graphics::textures
{
	struct AnimationManager;
	class TextureManager;
}

namespace ion::script::interfaces
{
	namespace material_script_interface::detail
	{
		/*
			Validator classes
		*/

		script_validator::ClassDefinition get_material_class();
		ScriptValidator get_material_validator();


		/*
			Tree parsing
		*/

		NonOwningPtr<graphics::materials::Material> create_material(const script_tree::ObjectNode &object,
			graphics::materials::MaterialManager &material_manager,
			graphics::textures::AnimationManager &animation_manager,
			graphics::textures::TextureManager &texture_manager);

		void create_materials(const ScriptTree &tree,
			graphics::materials::MaterialManager &material_manager,
			graphics::textures::AnimationManager &animation_manager,
			graphics::textures::TextureManager &texture_manager);
	} //material_script_interface::detail


	class MaterialScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			//Default constructor
			MaterialScriptInterface() = default;


			/*
				Materials
				Creating from script
			*/

			//Create materials from a script (or object file) with the given asset name
			void CreateMaterials(std::string_view asset_name,
				graphics::materials::MaterialManager &material_manager,
				graphics::textures::AnimationManager &animation_manager,
				graphics::textures::TextureManager &texture_manager);
	};
} //ion::script::interfaces

#endif