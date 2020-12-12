/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/materials
File:	IonMaterial.cpp
-------------------------------------------
*/

#include "IonMaterial.h"

#include "types/IonTypeTraits.h"

namespace ion::graphics::materials
{

using namespace material;

namespace material::detail
{

} //material::detail


//Public

Material::Material(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess) :

	Material{std::move(name), ambient, diffuse, specular, shininess, {}}
{
	//Empty
}

Material::Material(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	const std::optional<Color> &emissive, bool receive_shadows) :

	managed::ManagedObject<MaterialManager>{std::move(name)},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},
	shininess_{shininess},

	emissive_color_{emissive},
	receive_shadows_{receive_shadows}
{
	//Empty
}


Material::Material(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	Animation *diffuse_map, Animation *specular_map, Animation *normal_map) :

	Material{std::move(name), ambient, diffuse, specular, shininess, diffuse_map, specular_map, normal_map, {}}
{
	//Empty
}

Material::Material(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	Animation *diffuse_map, Animation *specular_map, Animation *normal_map,
	const std::optional<Color> &emissive, bool receive_shadows) :

	managed::ManagedObject<MaterialManager>{std::move(name)},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},
	shininess_{shininess},

	emissive_color_{emissive},
	receive_shadows_{receive_shadows}
{
	if (diffuse_map)
		DiffuseMap(*diffuse_map);

	if (specular_map)
		SpecularMap(*specular_map);

	if (normal_map)
		NormalMap(*normal_map);
}


Material::Material(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	Texture *diffuse_map, Texture *specular_map, Texture *normal_map) :

	Material{std::move(name), ambient, diffuse, specular, shininess, diffuse_map, specular_map, normal_map, {}}
{
	//Empty
}

Material::Material(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
	Texture *diffuse_map, Texture *specular_map, Texture *normal_map,
	const std::optional<Color> &emissive, bool receive_shadows) :

	managed::ManagedObject<MaterialManager>{std::move(name)},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},
	shininess_{shininess},

	emissive_color_{emissive},
	receive_shadows_{receive_shadows}
{
	if (diffuse_map)
		DiffuseMap(*diffuse_map);

	if (specular_map)
		SpecularMap(*specular_map);

	if (normal_map)
		NormalMap(*normal_map);
}


/*
	Modifiers
*/

void Material::DiffuseMap(Animation &animation)
{
	DiffuseMap(nullptr);

	std::visit(types::overloaded{
		[&](std::monostate) { diffuse_map_ = managed::ObservedObject<Animation>{animation}; },
		[](auto&&) {} //Something still attached
	}, diffuse_map_);
}

void Material::DiffuseMap(Texture &texture)
{
	DiffuseMap(nullptr);

	std::visit(types::overloaded{
		[&](std::monostate) { diffuse_map_ = managed::ObservedObject<Texture>{texture}; },
		[](auto&&) {} //Something still attached
	}, diffuse_map_);
}

void Material::DiffuseMap(std::nullptr_t) noexcept
{
	diffuse_map_ = std::monostate{};
}


void Material::SpecularMap(Animation &animation)
{
	SpecularMap(nullptr);

	std::visit(types::overloaded{
		[&](std::monostate) { specular_map_ = managed::ObservedObject<Animation>{animation}; },
		[](auto&&) {} //Something still attached
	}, specular_map_);
}

void Material::SpecularMap(Texture &texture)
{
	SpecularMap(nullptr);

	std::visit(types::overloaded{
		[&](std::monostate) { specular_map_ = managed::ObservedObject<Texture>{texture}; },
		[](auto&&) {} //Something still attached
	}, specular_map_);
}

void Material::SpecularMap(std::nullptr_t) noexcept
{
	specular_map_ = std::monostate{};
}


void Material::NormalMap(Animation &animation)
{
	NormalMap(nullptr);

	std::visit(types::overloaded{
		[&](std::monostate) { normal_map_ = managed::ObservedObject<Animation>{animation}; },
		[](auto&&) {} //Something still attached
	}, normal_map_);
}

void Material::NormalMap(Texture &texture)
{
	NormalMap(nullptr);

	std::visit(types::overloaded{
		[&](std::monostate) { normal_map_ = managed::ObservedObject<Texture>{texture}; },
		[](auto&&) {} //Something still attached
	}, normal_map_);
}

void Material::NormalMap(std::nullptr_t) noexcept
{
	normal_map_ = std::monostate{};
}


/*
	Observers
*/

std::pair<const Animation*, const Texture*> Material::DiffuseMap() const noexcept
{
	using result_type = std::pair<const Animation*, const Texture*>;

	return std::visit(types::overloaded{
		[](std::monostate) -> result_type { return {nullptr, nullptr}; },
		[](const managed::ObservedObject<Animation> &animation) -> result_type { return {animation.Object(), nullptr}; },
		[](const managed::ObservedObject<Texture> &texture) -> result_type { return {nullptr, texture.Object()}; }	
	}, diffuse_map_);
}

std::pair<const Animation*, const Texture*> Material::SpecularMap() const noexcept
{
	using result_type = std::pair<const Animation*, const Texture*>;

	return std::visit(types::overloaded{
		[](std::monostate) -> result_type { return {nullptr, nullptr}; },
		[](const managed::ObservedObject<Animation> &animation) -> result_type { return {animation.Object(), nullptr}; },
		[](const managed::ObservedObject<Texture> &texture) -> result_type { return {nullptr, texture.Object()}; }
	}, specular_map_);
}

std::pair<const Animation*, const Texture*> Material::NormalMap() const noexcept
{
	using result_type = std::pair<const Animation*, const Texture*>;

	return std::visit(types::overloaded{
		[](std::monostate) -> result_type { return {nullptr, nullptr}; },
		[](const managed::ObservedObject<Animation> &animation) -> result_type { return {animation.Object(), nullptr}; },
		[](const managed::ObservedObject<Texture> &texture) -> result_type { return {nullptr, texture.Object()}; }
	}, normal_map_);
}

} //ion::graphics::materials