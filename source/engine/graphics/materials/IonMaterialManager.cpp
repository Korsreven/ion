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

Material& MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, shininess);
}

Material& MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	const std::optional<Color> &emissive, bool receive_shadows)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, shininess,
				  emissive, receive_shadows);
}


Material& MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	Animation *diffuse_map, Animation *specular_map, Animation *normal_map)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, shininess,
				  diffuse_map, specular_map, normal_map);
}

Material& MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	Animation *diffuse_map, Animation *specular_map, Animation *normal_map,
	const std::optional<Color> &emissive, bool receive_shadows)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, shininess,
				  diffuse_map, specular_map, normal_map,
				  emissive, receive_shadows);
}


Material& MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	Texture *diffuse_map, Texture *specular_map, Texture *normal_map)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, shininess,
				  diffuse_map, specular_map, normal_map);
}

Material& MaterialManager::CreateMaterial(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	Texture *diffuse_map, Texture *specular_map, Texture *normal_map,
	const std::optional<Color> &emissive, bool receive_shadows)
{
	return Create(std::move(name),
				  ambient, diffuse, specular, shininess,
				  diffuse_map, specular_map, normal_map,
				  emissive, receive_shadows);
}


Material& MaterialManager::CreateMaterial(const Material &material)
{
	return Create(material);
}

Material& MaterialManager::CreateMaterial(Material &&material)
{
	return Create(std::move(material));
}


/*
	Materials
	Retrieving
*/

Material* MaterialManager::GetMaterial(std::string_view name) noexcept
{
	return Get(name);
}

const Material* MaterialManager::GetMaterial(std::string_view name) const noexcept
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