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
	Materials
	Creating
*/

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess,
	bool receive_shadows)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, emissive, shininess,
				  receive_shadows);
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess,
	NonOwningPtr<Animation> diffuse_map, NonOwningPtr<Animation> specular_map, NonOwningPtr<Animation> normal_map,
	bool receive_shadows)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, emissive, shininess,
				  diffuse_map, specular_map, normal_map,
				  receive_shadows);
}

NonOwningPtr<Material> MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess,
	NonOwningPtr<Texture> diffuse_map, NonOwningPtr<Texture> specular_map, NonOwningPtr<Texture> normal_map,
	bool receive_shadows)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, emissive, shininess,
				  diffuse_map, specular_map, normal_map,
				  receive_shadows);
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
	Materials
	Retrieving
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
	Materials
	Removing
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