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

/*
	Texure coordinates
*/

std::pair<Vector2, Vector2> get_tex_coords(const Vector2 &lower_left, const Vector2 &upper_right,
	const Vector2 &new_lower_left, const Vector2 &new_upper_right) noexcept
{
	auto [ll_s, ll_t] = lower_left.XY();
	auto [ur_s, ur_t] = upper_right.XY();
	auto [new_ll_s, new_ll_t] = new_lower_left.XY();
	auto [new_ur_s, new_ur_t] = new_upper_right.XY();

	if (is_flipped_horizontally(lower_left, upper_right))
		std::swap(new_ll_s, new_ur_s);

	if (is_flipped_vertically(lower_left, upper_right))
		std::swap(new_ll_t, new_ur_t);

	return {{new_ll_s, new_ll_t}, {new_ur_s, new_ur_t}};
}

std::pair<Vector2, Vector2> get_unflipped_tex_coords(const Vector2 &lower_left, const Vector2 &upper_right) noexcept
{
	auto [ll_s, ll_t] = lower_left.XY();
	auto [ur_s, ur_t] = upper_right.XY();

	if (is_flipped_horizontally(lower_left, upper_right))
		std::swap(ll_s, ur_s);

	if (is_flipped_vertically(lower_left, upper_right))
		std::swap(ll_t, ur_t);

	return {{ll_s, ll_t}, {ur_s, ur_t}};
}


Vector2 get_normalized_tex_coords(const Vector2 &tex_coords, const Vector2 &min, const Vector2 &max,
	const Vector2 &new_min, const Vector2 &new_max) noexcept
{
	using namespace ion::utilities;

	auto [s, t] = tex_coords.XY();
	auto [min_s, min_t] = min.XY();
	auto [max_s, max_t] = max.XY();
	auto [new_min_s, new_min_t] = new_min.XY();
	auto [new_max_s, new_max_t] = new_max.XY();

	return {math::Normalize(s, min_s, max_s, new_min_s, new_max_s),
			math::Normalize(t, min_t, max_t, new_min_t, new_max_t)};
}

std::pair<Vector2, Vector2> get_normalized_tex_coords(const Vector2 &lower_left, const Vector2 &upper_right,
	const Vector2 &min, const Vector2 &max) noexcept
{
	return {get_normalized_tex_coords(lower_left, vector2::Zero, vector2::UnitScale, min, max),
			get_normalized_tex_coords(upper_right, vector2::Zero, vector2::UnitScale, min, max)};
}


/*
	Texture map
*/

std::pair<const Animation*, const Texture*> get_texture_maps(const map_type &map) noexcept
{
	using result_type = std::pair<const Animation*, const Texture*>;

	return std::visit(types::overloaded{
		[](std::monostate) -> result_type { return {nullptr, nullptr}; },
		[](const managed::ObservedObject<Animation> &animation) -> result_type { return {animation.Object(), nullptr}; },
		[](const managed::ObservedObject<Texture> &texture) -> result_type { return {nullptr, texture.Object()}; }	
	}, map);
}

const Texture* get_texture_map(const map_type &map) noexcept
{
	if (auto [animation, texture] = get_texture_maps(map); texture || animation)
		return animation ? animation->UnderlyingFrameSequence()->FirstFrame() : texture;
	else
		return nullptr;
}

const Texture* get_first_texture_map(const map_type &diffuse_map, const map_type &specular_map, const map_type &normal_map) noexcept
{
	if (auto texture = get_texture_map(diffuse_map); texture)
		return texture;

	if (auto texture = get_texture_map(specular_map); texture)
		return texture;

	if (auto texture = get_texture_map(normal_map); texture)
		return texture;

	return nullptr;
}


std::pair<bool, bool> is_texture_map_repeatable(const Texture &texture,
	const Vector2 &lower_left, const Vector2 &upper_right) noexcept
{
	if (auto repeatable = texture.IsRepeatable(); repeatable)
	{
		auto &[s_repeatable, t_repeatable] = *repeatable;

		auto [ll_s, ll_t] = lower_left.XY();
		auto [ur_s, ur_t] = upper_right.XY();
		return {s_repeatable && ll_s <= 0.0_r && ur_s >= 1.0_r,
				t_repeatable && ll_t <= 0.0_r && ur_t >= 1.0_r};
	}
	else
		return {false, false};
}

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
	return detail::get_texture_maps(diffuse_map_);
}

std::pair<const Animation*, const Texture*> Material::SpecularMap() const noexcept
{
	return detail::get_texture_maps(specular_map_);
}

std::pair<const Animation*, const Texture*> Material::NormalMap() const noexcept
{
	return detail::get_texture_maps(normal_map_);
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
			auto [lower_left, upper_right] =
				detail::get_tex_coords(lower_left_tex_coords_, upper_right_tex_coords_, min, max);

			lower_left_tex_coords_ = lower_left;
			upper_right_tex_coords_ = upper_right;
		}
	}
	//Un-crop
	else if (IsCropped())
	{
		auto [lower_left, upper_right] =
			detail::get_tex_coords(lower_left_tex_coords_, upper_right_tex_coords_, 0.0_r, 1.0_r);

		lower_left_tex_coords_ = lower_left;
		upper_right_tex_coords_ = upper_right;
	}
}

void Material::Repeat(const std::optional<Vector2> &amount) noexcept
{
	//Repeat by amount
	if (amount)
	{
		if (auto max = amount->CeilCopy(vector2::Zero); vector2::Zero < max)
		{
			auto [lower_left, upper_right] =
				detail::get_tex_coords(lower_left_tex_coords_, upper_right_tex_coords_, 0.0_r, max);

			lower_left_tex_coords_ = lower_left;
			upper_right_tex_coords_ = upper_right;
		}
	}
	//Un-repeat
	else if (IsRepeated())
	{
		auto [lower_left, upper_right] =
			detail::get_tex_coords(lower_left_tex_coords_, upper_right_tex_coords_, 0.0_r, 1.0_r);

		lower_left_tex_coords_ = lower_left;
		upper_right_tex_coords_ = upper_right;
	}
}


void Material::FlipHorizontal() noexcept
{
	auto ll_s = lower_left_tex_coords_.X();
	auto ur_s = upper_right_tex_coords_.X();

	lower_left_tex_coords_.X(ur_s);
	upper_right_tex_coords_.X(ll_s);
}

void Material::FlipVertical() noexcept
{
	auto ll_t = lower_left_tex_coords_.Y();
	auto ur_t = upper_right_tex_coords_.Y();

	lower_left_tex_coords_.Y(ur_t);
	upper_right_tex_coords_.Y(ll_t);
}


bool Material::IsCropped() const noexcept
{
	auto [lower_left, upper_right] =
		detail::get_unflipped_tex_coords(lower_left_tex_coords_, upper_right_tex_coords_);
	return detail::is_cropped(lower_left, upper_right);
}

bool Material::IsRepeated() const noexcept
{
	auto [lower_left, upper_right] =
		detail::get_unflipped_tex_coords(lower_left_tex_coords_, upper_right_tex_coords_);
	return detail::is_repeated(lower_left, upper_right);
}

std::pair<bool, bool> Material::IsRepeatable() const noexcept
{
	if (auto texture = detail::get_first_texture_map(diffuse_map_, specular_map_, normal_map_); texture)
	{
		auto [lower_left, upper_right] =
			detail::get_unflipped_tex_coords(lower_left_tex_coords_, upper_right_tex_coords_);
		return detail::is_texture_map_repeatable(*texture, lower_left, upper_right);
	}
	else
		return {false, false};
}


bool Material::IsFlippedHorizontally() const noexcept
{
	return detail::is_flipped_horizontally(lower_left_tex_coords_, upper_right_tex_coords_);
}

bool Material::IsFlippedVertically() const noexcept
{
	return detail::is_flipped_vertically(lower_left_tex_coords_, upper_right_tex_coords_);
}


std::pair<Vector2, Vector2> Material::WorldTexCoords() const noexcept
{
	if (auto texture = detail::get_first_texture_map(diffuse_map_, specular_map_, normal_map_); texture)
	{
		//Use local tex coords relative to world tex coords
		if (auto world_tex_coords = texture->TexCoords(); world_tex_coords)
		{
			auto [lower_left, upper_right] =
				detail::get_unflipped_tex_coords(lower_left_tex_coords_, upper_right_tex_coords_);
			auto &[world_lower_left, world_upper_right] = *world_tex_coords;

			auto [s_repeatable, t_repeatable] =
				detail::is_texture_map_repeatable(*texture, lower_left, upper_right);

			//Discard any repetition on s
			if (!s_repeatable)
			{
				lower_left.X(std::max(lower_left.X(), 0.0_r));
				upper_right.X(std::min(upper_right.X(), 1.0_r));
			}

			//Discard any repetition on t
			if (!t_repeatable)
			{
				lower_left.Y(std::max(lower_left.Y(), 0.0_r));
				upper_right.Y(std::min(upper_right.Y(), 1.0_r));
			}

			auto [norm_lower_left, norm_upper_right] =
				detail::get_normalized_tex_coords(lower_left, upper_right, world_lower_left, world_upper_right);

			return detail::get_tex_coords(lower_left_tex_coords_, upper_right_tex_coords_,
										  norm_lower_left, norm_upper_right);
		}
	}

	//Use local tex coords
	return {lower_left_tex_coords_, upper_right_tex_coords_};
}

} //ion::graphics::materials