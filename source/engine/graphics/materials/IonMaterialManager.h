/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/materials
File:	IonMaterialManager.h
-------------------------------------------
*/

#ifndef ION_MATERIAL_MANAGER_H
#define ION_MATERIAL_MANAGER_H

#include <type_traits>
#include <string>
#include <string_view>

#include "IonMaterial.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::materials
{
	namespace material_manager::detail
	{
	} //material_manager::detail


	struct MaterialManager final :
		managed::ObjectManager<Material, MaterialManager>
	{
		//Default constructor
		MaterialManager() = default;

		//Deleted copy constructor
		MaterialManager(const MaterialManager&) = delete;

		//Default move constructor
		MaterialManager(MaterialManager&&) = default;

		//Destructor
		~MaterialManager() = default;


		/*
			Operators
		*/

		//Deleted copy assignment
		MaterialManager& operator=(const MaterialManager&) = delete;

		//Move assignment
		MaterialManager& operator=(MaterialManager&&) = default;


		/*
			Ranges
		*/

		//Returns a mutable range of all materials in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto Materials() noexcept
		{
			return Objects();
		}

		//Returns an immutable range of all materials in this manager
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto Materials() const noexcept
		{
			return Objects();
		}


		/*
			Materials
			Creating
		*/

		//Constructs a new material with the given name
		NonOwningPtr<Material> CreateMaterial(std::string name);

		//Constructs a new material with the given name, ambient, diffuse, specular and emissive color + shininess and receive shadows
		NonOwningPtr<Material> CreateMaterial(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess,
				bool receive_shadows = true);

		//Constructs a new material with the given name, ambient, diffuse, specular and emissive color + shininess,
		//diffuse, specular and normal map as animations + receive shadows
		NonOwningPtr<Material> CreateMaterial(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess,
				NonOwningPtr<Animation> diffuse_map, NonOwningPtr<Animation> specular_map, NonOwningPtr<Animation> normal_map,
				bool receive_shadows = true);

		//Constructs a new material with the given name, ambient, diffuse, specular and emissive color + shininess,
		//diffuse, specular and normal map as textures + receive shadows
		NonOwningPtr<Material> CreateMaterial(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess,
				NonOwningPtr<Texture> diffuse_map, NonOwningPtr<Texture> specular_map, NonOwningPtr<Texture> normal_map,
				bool receive_shadows = true);


		//Create a material as a copy of the given material
		NonOwningPtr<Material> CreateMaterial(const Material &material);

		//Create a material by moving the given material
		NonOwningPtr<Material> CreateMaterial(Material &&material);


		/*
			Materials
			Retrieving
		*/

		//Gets a pointer to a mutable material with the given name
		//Returns nullptr if material could not be found
		[[nodiscard]] NonOwningPtr<Material> GetMaterial(std::string_view name) noexcept;

		//Gets a pointer to an immutable material with the given name
		//Returns nullptr if material could not be found
		[[nodiscard]] NonOwningPtr<const Material> GetMaterial(std::string_view name) const noexcept;


		/*
			Materials
			Removing
		*/

		//Clear all removable materials from this manager
		void ClearMaterials() noexcept;

		//Remove a removable material from this manager
		bool RemoveMaterial(Material &material) noexcept;

		//Remove a removable material with the given name from this manager
		bool RemoveMaterial(std::string_view name) noexcept;
	};
} //ion::graphics::materials

#endif