/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/materials
File:	IonMaterialManager.cpp
-------------------------------------------
*/

#include "IonMaterialManager.h"

namespace ion::graphics::materials
{

using namespace material_manager;

namespace material_manager::detail
{

} //material_manager::detail


//Public

/*
	Materials - Creating
*/

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name)
{
	return Create(std::move(name));
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name, const Color &diffuse)
{
	return Create(std::move(name), diffuse);
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name, NonOwningPtr<Animation> diffuse_map, const Color &diffuse)
{
	return Create(std::move(name), diffuse_map, diffuse);
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name, NonOwningPtr<Texture> diffuse_map, const Color &diffuse)
{
	return Create(std::move(name), diffuse_map, diffuse);
}


NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, emissive, shininess);
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name, NonOwningPtr<Animation> diffuse_map,
	NonOwningPtr<Animation> normal_map, NonOwningPtr<Animation> specular_map, NonOwningPtr<Animation> emissive_map,
	const Color &diffuse, real shininess)
{
	return Create(std::move(name), diffuse_map,
				  normal_map, specular_map, emissive_map,
				  diffuse, shininess);
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name, NonOwningPtr<Texture> diffuse_map,
	NonOwningPtr<Texture> normal_map, NonOwningPtr<Texture> specular_map, NonOwningPtr<Texture> emissive_map,
	const Color &diffuse, real shininess)
{
	return Create(std::move(name), diffuse_map,
				  normal_map, specular_map, emissive_map,
				  diffuse, shininess);
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name, NonOwningPtr<Animation> diffuse_map,
	NonOwningPtr<Animation> normal_map, NonOwningPtr<Animation> specular_map, NonOwningPtr<Animation> emissive_map,
	const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess)
{
	return Create(std::move(name), diffuse_map,
				  normal_map, specular_map, emissive_map,
				  ambient, diffuse, specular, emissive, shininess);
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name, NonOwningPtr<Texture> diffuse_map,
	NonOwningPtr<Texture> normal_map, NonOwningPtr<Texture> specular_map, NonOwningPtr<Texture> emissive_map,
	const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess)
{
	return Create(std::move(name), diffuse_map,
				  normal_map, specular_map, emissive_map,
				  ambient, diffuse, specular, emissive, shininess);
}


NonOwningPtr<Material> MaterialManager::CreateMaterial(const Material &material)
{
	return Create(material);
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(Material &&material)
{
	return Create(std::move(material));
}


/*
	Materials - Retrieving
*/

NonOwningPtr<Material> MaterialManager::GetMaterial(std::string_view name) noexcept
{
	return Get(name);
}

NonOwningPtr<const Material> MaterialManager::GetMaterial(std::string_view name) const noexcept
{
	return Get(name);
}


/*
	Materials - Removing
*/

void MaterialManager::ClearMaterials() noexcept
{
	Clear();
}

bool MaterialManager::RemoveMaterial(Material &material) noexcept
{
	return Remove(material);
}

bool MaterialManager::RemoveMaterial(std::string_view name) noexcept
{
	return Remove(name);
}

} //ion::graphics::materials