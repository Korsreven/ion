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
#include "graphics/utilities/IonColor.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::materials
{
	using namespace types::type_literals;
	using namespace utilities;

	namespace material_manager::detail
	{
	} //material_manager::detail


	///@brief A class that manages and stores materials
	class MaterialManager final :
		public managed::ObjectManager<Material, MaterialManager>
	{
		public:

			///@brief Default constructor
			MaterialManager() = default;

			///@brief Deleted copy constructor
			MaterialManager(const MaterialManager&) = delete;

			///@brief Default move constructor
			MaterialManager(MaterialManager&&) = default;

			///@brief Destructor
			~MaterialManager() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			MaterialManager& operator=(const MaterialManager&) = delete;

			///@brief Default move assignment
			MaterialManager& operator=(MaterialManager&&) = default;

			///@}

			/**
				@name Ranges
			*/

			///@brief Returns a mutable range of all materials in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Materials() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all materials in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Materials() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Materials
				Creating
				@{
			*/

			///@brief Creates a material with the given name
			NonOwningPtr<Material> CreateMaterial(std::string name);

			///@brief Creates a material with the given name and diffuse color
			NonOwningPtr<Material> CreateMaterial(std::string name, const Color &diffuse);

			///@brief Creates a material with the given name, diffuse map and diffuse color
			NonOwningPtr<Material> CreateMaterial(std::string name, NonOwningPtr<Animation> diffuse_map, const Color &diffuse = color::White);

			///@brief Creates a material with the given name, diffuse map and diffuse color
			NonOwningPtr<Material> CreateMaterial(std::string name, NonOwningPtr<Texture> diffuse_map, const Color &diffuse = color::White);


			///@brief Creates a material with the given name, ambient color, diffuse color, specular color, emissive color and shininess
			NonOwningPtr<Material> CreateMaterial(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive,
				real shininess = material::detail::default_shininess);

			///@brief Creates a material with the given name, diffuse map, normal map, specular map, emissive map,
			///diffuse color and shininess
			NonOwningPtr<Material> CreateMaterial(std::string name, NonOwningPtr<Animation> diffuse_map,
				NonOwningPtr<Animation> normal_map, NonOwningPtr<Animation> specular_map, NonOwningPtr<Animation> emissive_map,
				const Color &diffuse = color::White, real shininess = material::detail::default_shininess);

			///@brief Creates a material with the given name, diffuse map, normal map, specular map, emissive map,
			///diffuse color and shininess
			NonOwningPtr<Material> CreateMaterial(std::string name, NonOwningPtr<Texture> diffuse_map,
				NonOwningPtr<Texture> normal_map, NonOwningPtr<Texture> specular_map, NonOwningPtr<Texture> emissive_map,
				const Color &diffuse = color::White, real shininess = material::detail::default_shininess);

			///@brief Creates a material with the given name, diffuse map, normal map, specular map, emissive map,
			///ambient color, diffuse color, specular color, emissive color and shininess
			NonOwningPtr<Material> CreateMaterial(std::string name, NonOwningPtr<Animation> diffuse_map,
				NonOwningPtr<Animation> normal_map, NonOwningPtr<Animation> specular_map, NonOwningPtr<Animation> emissive_map,
				const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive,
				real shininess = material::detail::default_shininess);

			///@brief Creates a material with the given name, diffuse map, normal map, specular map, emissive map,
			///ambient color, diffuse color, specular color, emissive color and shininess
			NonOwningPtr<Material> CreateMaterial(std::string name, NonOwningPtr<Texture> diffuse_map,
				NonOwningPtr<Texture> normal_map, NonOwningPtr<Texture> specular_map, NonOwningPtr<Texture> emissive_map,
				const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive,
				real shininess = material::detail::default_shininess);


			///@brief Creates a material as a copy of the given material
			NonOwningPtr<Material> CreateMaterial(const Material &material);

			///@brief Creates a material by moving the given material
			NonOwningPtr<Material> CreateMaterial(Material &&material);

			///@}

			/**
				@name Materials
				Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable material with the given name
			///@details Returns nullptr if material could not be found
			[[nodiscard]] NonOwningPtr<Material> GetMaterial(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable material with the given name
			///@details Returns nullptr if material could not be found
			[[nodiscard]] NonOwningPtr<const Material> GetMaterial(std::string_view name) const noexcept;

			///@}

			/**
				@name Materials
				Removing
				@{
			*/

			///@brief Clears all removable materials from this manager
			void ClearMaterials() noexcept;

			///@brief Removes a removable material from this manager
			bool RemoveMaterial(Material &material) noexcept;

			///@brief Removes a removable material with the given name from this manager
			bool RemoveMaterial(std::string_view name) noexcept;

			///@}
	};
} //ion::graphics::materials

#endif