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

namespace ion::script::interfaces
{
	namespace material_script_interface::detail
	{
		NonOwningPtr<graphics::textures::Animation> get_animation(std::string_view name, const ManagerRegister &managers) noexcept;
		NonOwningPtr<graphics::textures::Texture> get_texture(std::string_view name, const ManagerRegister &managers) noexcept;


		/**
			@name Validator classes
			@{
		*/

		script_validator::ClassDefinition get_material_class();
		ScriptValidator get_material_validator();

		///@}

		/**
			@name Tree parsing
			@{
		*/

		void set_material_properties(const script_tree::ObjectNode &object, graphics::materials::Material &material,
			const ManagerRegister &managersr);


		NonOwningPtr<graphics::materials::Material> create_material(const script_tree::ObjectNode &object,
			graphics::materials::MaterialManager &material_manager, const ManagerRegister &managers);

		void create_materials(const ScriptTree &tree,
			graphics::materials::MaterialManager &material_manager, const ManagerRegister &managers);

		///@}
	} //material_script_interface::detail


	///@brief A class representing an interface to a material script with a complete validation scheme
	///@details A material script can load materials from a script file into a material manager
	class MaterialScriptInterface final : public ScriptInterface
	{
		private:

			ScriptValidator GetValidator() const override;

		public:

			///@brief Default constructor
			MaterialScriptInterface() = default;


			/**
				@name Materials
				Creating from script
				@{
			*/

			///@brief Creates materials from a script (or object file) with the given asset name
			void CreateMaterials(std::string_view asset_name,
				graphics::materials::MaterialManager &material_manager);

			///@brief Creates materials from a script (or object file) with the given asset name
			void CreateMaterials(std::string_view asset_name,
				graphics::materials::MaterialManager &material_manager, const ManagerRegister &managers);

			///@}
	};
} //ion::script::interfaces

#endif