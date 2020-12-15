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
#include "utilities/IonMath.h"

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


/*
	Texture coordinates
*/

void Material::Crop(const std::optional<Aabb> &area) noexcept
{
	//Crop by area
	if (area)
	{
		auto min = area->Min().CeilCopy(vector2::Zero).FloorCopy(vector2::UnitScale);
		auto max = area->Max().CeilCopy(vector2::Zero).FloorCopy(vector2::UnitScale);

		if (min != max)
		{
			auto [ll_s, ll_t] = lower_left_tex_coord_.XY();
			auto [ur_s, ur_t] = upper_right_tex_coord_.XY();
			auto [new_ll_s, new_ur_s] = detail::get_tex_coords(ll_s, ur_s, min.X(), max.X());
			auto [new_ll_t, new_ur_t] = detail::get_tex_coords(ll_t, ur_t, min.Y(), max.Y());

			lower_left_tex_coord_ = {new_ll_s, new_ll_t};
			upper_right_tex_coord_ = {new_ur_s, new_ur_t};
		}
	}
	//Un-crop
	else if (IsCropped())
	{
		auto [ll_s, ll_t] = lower_left_tex_coord_.XY();
		auto [ur_s, ur_t] = upper_right_tex_coord_.XY();
		auto [new_ll_s, new_ur_s] = detail::get_tex_coords(ll_s, ur_s, 0.0_r, 1.0_r);
		auto [new_ll_t, new_ur_t] = detail::get_tex_coords(ll_t, ur_t, 0.0_r, 1.0_r);

		lower_left_tex_coord_ = {new_ll_s, new_ll_t};
		upper_right_tex_coord_ = {new_ur_s, new_ur_t};
	}
}

void Material::Repeat(const std::optional<Vector2> &amount) noexcept
{
	//Repeat by amount
	if (amount)
	{
		if (auto max = amount->CeilCopy(vector2::Zero); max > vector2::Zero)
		{
			auto [ll_s, ll_t] = lower_left_tex_coord_.XY();
			auto [ur_s, ur_t] = upper_right_tex_coord_.XY();
			auto [new_ll_s, new_ur_s] = detail::get_tex_coords(ll_s, ur_s, 0.0_r, max.X());
			auto [new_ll_t, new_ur_t] = detail::get_tex_coords(ll_t, ur_t, 0.0_r, max.Y());

			lower_left_tex_coord_ = {new_ll_s, new_ll_t};
			upper_right_tex_coord_ = {new_ur_s, new_ur_t};
		}
	}
	//Un-repeat
	else if (IsRepeated())
	{
		auto [ll_s, ll_t] = lower_left_tex_coord_.XY();
		auto [ur_s, ur_t] = upper_right_tex_coord_.XY();
		auto [new_ll_s, new_ur_s] = detail::get_tex_coords(ll_s, ur_s, 0.0_r, 1.0_r);
		auto [new_ll_t, new_ur_t] = detail::get_tex_coords(ll_t, ur_t, 0.0_r, 1.0_r);

		lower_left_tex_coord_ = {new_ll_s, new_ll_t};
		upper_right_tex_coord_ = {new_ur_s, new_ur_t};
	}
}


void Material::FlipHorizontal() noexcept
{
	auto ll_s = lower_left_tex_coord_.X();
	auto ur_s = upper_right_tex_coord_.X();

	lower_left_tex_coord_.X(ur_s);
	upper_right_tex_coord_.X(ll_s);
}

void Material::FlipVertical() noexcept
{
	auto ll_t = lower_left_tex_coord_.Y();
	auto ur_t = upper_right_tex_coord_.Y();

	lower_left_tex_coord_.Y(ur_t);
	upper_right_tex_coord_.Y(ll_t);
}


bool Material::IsCropped() const noexcept
{
	auto [ll_s, ll_t] = lower_left_tex_coord_.XY();
	auto [ur_s, ur_t] = upper_right_tex_coord_.XY();

	if (IsFlippedHorizontally())
		std::swap(ll_s, ur_s);
	
	if (IsFlippedVertically())
		std::swap(ll_t, ur_t);
	
	return ll_s > 0.0_r || ll_t > 0.0_r ||
		   ur_s < 1.0_r || ur_t < 1.0_r;
}

bool Material::IsRepeated() const noexcept
{
	auto [ll_s, ll_t] = lower_left_tex_coord_.XY();
	auto [ur_s, ur_t] = upper_right_tex_coord_.XY();

	if (IsFlippedHorizontally())
		std::swap(ll_s, ur_s);
	
	if (IsFlippedVertically())
		std::swap(ll_t, ur_t);
	
	return ll_s < 0.0_r || ll_t < 0.0_r ||
		   ur_s > 1.0_r || ur_t > 1.0_r;
}


bool Material::IsFlippedHorizontally() const noexcept
{
	return upper_right_tex_coord_.X() < lower_left_tex_coord_.X();
}

bool Material::IsFlippedVertically() const noexcept
{
	return upper_right_tex_coord_.Y() < lower_left_tex_coord_.Y();
}


std::pair<Vector2, Vector2> Material::WorldTexCoords() const noexcept
{
	using namespace ion::utilities;

	//Get texture
	auto texture =
		[&]() noexcept -> const Texture*
		{
			if (auto [animation, texture] = this->DiffuseMap();
				texture || (animation && !animation->UnderlyingFrameSequence()->IsEmpty()))
				return animation ? &*animation->UnderlyingFrameSequence()->Frames().begin() : texture;

			if (auto [animation, texture] = this->SpecularMap();
				texture || (animation && !animation->UnderlyingFrameSequence()->IsEmpty()))
				return animation ? &*animation->UnderlyingFrameSequence()->Frames().begin() : texture;

			if (auto [animation, texture] = this->NormalMap();
				texture || (animation && !animation->UnderlyingFrameSequence()->IsEmpty()))
				return animation ? &*animation->UnderlyingFrameSequence()->Frames().begin() : texture;

			return nullptr;
		}();

	//Has texture
	if (texture)
	{
		//Use local tex coords relative to world tex coords
		if (auto world_tex_coords = texture->TexCoords(); world_tex_coords)
		{
			auto [ll_s, ll_t] = lower_left_tex_coord_.XY();
			auto [ur_s, ur_t] = upper_right_tex_coord_.XY();

			if (IsFlippedHorizontally())
				std::swap(ll_s, ur_s);

			if (IsFlippedVertically())
				std::swap(ll_t, ur_t);


			//Calculate world tex coords
			auto &[world_lower_left, world_upper_right] = *world_tex_coords;
			auto [world_ll_s, world_ll_t] = world_lower_left.XY();
			auto [world_ur_s, world_ur_t] = world_upper_right.XY();

			ll_s = ll_s < 0.0_r && world_ll_s != 0.0_r ?
				world_ll_s : math::Normalize(ll_s, 0.0_r, 1.0_r, world_ll_s, world_ur_s);
			ll_t = ll_t < 0.0_r && world_ll_t != 0.0_r ?
				world_ll_t : math::Normalize(ll_t, 0.0_r, 1.0_r, world_ll_t, world_ur_t);

			ur_s = ur_s > 1.0_r && world_ur_s != 1.0_r ?
				world_ur_s : math::Normalize(ur_s, 0.0_r, 1.0_r, world_ll_s, world_ur_s);
			ur_t = ur_t > 1.0_r && world_ur_t != 1.0_r ?
				world_ur_t : math::Normalize(ur_t, 0.0_r, 1.0_r, world_ll_t, world_ur_t);


			if (IsFlippedHorizontally())
				std::swap(ll_s, ur_s);

			if (IsFlippedVertically())
				std::swap(ll_t, ur_t);

			return {Vector2{ll_s, ll_t}, Vector2{ur_s, ur_t}};
		}
	}

	//Use local tex coords
	return {lower_left_tex_coord_, upper_right_tex_coord_};
}

} //ion::graphics::materials