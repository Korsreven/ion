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

#include "graphics/textures/IonAnimation.h"
#include "graphics/textures/IonTexture.h"
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


Vector2 get_normalized_tex_coord(const Vector2 &tex_coord, const Vector2 &min, const Vector2 &max,
	const Vector2 &new_min, const Vector2 &new_max) noexcept
{
	using namespace ion::utilities;

	auto [s, t] = tex_coord.XY();
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
	return {get_normalized_tex_coord(lower_left, vector2::Zero, vector2::UnitScale, min, max),
			get_normalized_tex_coord(upper_right, vector2::Zero, vector2::UnitScale, min, max)};
}


/*
	Texture map
*/

std::pair<NonOwningPtr<Animation>, NonOwningPtr<Texture>> get_texture_maps(const texture_map_type &texture_map) noexcept
{
	using pair_type = std::pair<NonOwningPtr<Animation>, NonOwningPtr<Texture>>;

	return std::visit(types::overloaded{
		[](std::monostate) { return pair_type{nullptr, nullptr}; },
		[](NonOwningPtr<Animation> animation) { return pair_type{animation, nullptr}; },
		[](NonOwningPtr<Texture> texture) { return pair_type{nullptr, texture}; }
	}, texture_map);
}

NonOwningPtr<Texture> get_texture_map(const texture_map_type &texture_map) noexcept
{
	if (auto [animation, texture] = get_texture_maps(texture_map); animation)
		return animation->UnderlyingFrameSequence() ?
			animation->UnderlyingFrameSequence()->FirstFrame() : nullptr;
	else
		return texture;
}

NonOwningPtr<Texture> get_first_texture_map(const texture_map_type &diffuse_map, const texture_map_type &specular_map, const texture_map_type &normal_map) noexcept
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

Material::Material(std::string name) :
	managed::ManagedObject<MaterialManager>{std::move(name)}
{
	//Empty
}

Material::Material(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess,
	bool receive_shadows) :

	managed::ManagedObject<MaterialManager>{std::move(name)},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},
	emissive_color_{emissive},
	shininess_{shininess},

	receive_shadows_{receive_shadows}
{
	//Empty
}

Material::Material(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess,
	NonOwningPtr<Animation> diffuse_map, NonOwningPtr<Animation> specular_map, NonOwningPtr<Animation> normal_map,
	bool receive_shadows) :

	managed::ManagedObject<MaterialManager>{std::move(name)},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},
	emissive_color_{emissive},
	shininess_{shininess},

	diffuse_map_{diffuse_map},
	specular_map_{specular_map},
	normal_map_{normal_map},

	receive_shadows_{receive_shadows}
{
	//Empty
}

Material::Material(std::string name,
	const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive, real shininess,
	NonOwningPtr<Texture> diffuse_map, NonOwningPtr<Texture> specular_map, NonOwningPtr<Texture> normal_map,
	bool receive_shadows) :

	managed::ManagedObject<MaterialManager>{std::move(name)},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},
	emissive_color_{emissive},
	shininess_{shininess},

	diffuse_map_{diffuse_map},
	specular_map_{specular_map},
	normal_map_{normal_map},

	receive_shadows_{receive_shadows}
{
	//Empty
}


/*
	Observers
*/

NonOwningPtr<Texture> Material::DiffuseMap(duration time) const noexcept
{
	if (auto [animation, texture] = DiffuseMap(); animation)
		return animation->FrameAt(time);
	else if (texture)
		return texture;
	else
		return nullptr;
}

NonOwningPtr<Texture> Material::SpecularMap(duration time) const noexcept
{
	if (auto [animation, texture] = SpecularMap(); animation)
		return animation->FrameAt(time);
	else if (texture)
		return texture;
	else
		return nullptr;
}

NonOwningPtr<Texture> Material::NormalMap(duration time) const noexcept
{
	if (auto [animation, texture] = NormalMap(); animation)
		return animation->FrameAt(time);
	else if (texture)
		return texture;
	else
		return nullptr;
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
				detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_, min, max);

			lower_left_tex_coord_ = lower_left;
			upper_right_tex_coord_ = upper_right;
		}
	}
	//Un-crop
	else if (IsCropped())
	{
		auto [lower_left, upper_right] =
			detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_, 0.0_r, 1.0_r);

		lower_left_tex_coord_ = lower_left;
		upper_right_tex_coord_ = upper_right;
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
				detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_, 0.0_r, max);

			lower_left_tex_coord_ = lower_left;
			upper_right_tex_coord_ = upper_right;
		}
	}
	//Un-repeat
	else if (IsRepeated())
	{
		auto [lower_left, upper_right] =
			detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_, 0.0_r, 1.0_r);

		lower_left_tex_coord_ = lower_left;
		upper_right_tex_coord_ = upper_right;
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
	auto [lower_left, upper_right] =
		detail::get_unflipped_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_);
	return detail::is_cropped(lower_left, upper_right);
}

bool Material::IsRepeated() const noexcept
{
	auto [lower_left, upper_right] =
		detail::get_unflipped_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_);
	return detail::is_repeated(lower_left, upper_right);
}

std::pair<bool, bool> Material::IsRepeatable() const noexcept
{
	if (auto texture = detail::get_first_texture_map(diffuse_map_, specular_map_, normal_map_); texture)
	{
		auto [lower_left, upper_right] =
			detail::get_unflipped_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_);
		return detail::is_texture_map_repeatable(*texture, lower_left, upper_right);
	}
	else
		return {false, false};
}


bool Material::IsFlippedHorizontally() const noexcept
{
	return detail::is_flipped_horizontally(lower_left_tex_coord_, upper_right_tex_coord_);
}

bool Material::IsFlippedVertically() const noexcept
{
	return detail::is_flipped_vertically(lower_left_tex_coord_, upper_right_tex_coord_);
}


std::pair<Vector2, Vector2> Material::WorldTexCoords() const noexcept
{
	if (auto texture = detail::get_first_texture_map(diffuse_map_, specular_map_, normal_map_); texture)
	{
		//Use local tex coords relative to world tex coords
		if (auto world_tex_coords = texture->TexCoords(); world_tex_coords)
		{
			auto [lower_left, upper_right] =
				detail::get_unflipped_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_);
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

			return detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_,
										  norm_lower_left, norm_upper_right);
		}
	}

	//Use local tex coords
	return {lower_left_tex_coord_, upper_right_tex_coord_};
}

} //ion::graphics::materials