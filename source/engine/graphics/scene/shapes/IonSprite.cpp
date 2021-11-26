/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonSprite.cpp
-------------------------------------------
*/

#include "IonSprite.h"

#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/textures/IonAnimation.h"
#include "graphics/textures/IonTexture.h"
#include "graphics/textures/IonTextureManager.h"
#include "graphics/materials/IonMaterial.h"

namespace ion::graphics::scene::shapes
{

using namespace sprite;

namespace sprite::detail
{

mesh::Vertices sprite_vertices(const Vector3 &position, real rotation, const Vector2 &size, const Color &color,
	const Vector2 &lower_left_tex_coord, const Vector2 &upper_right_tex_coord)
{
	auto [half_width, half_height] = (size * 0.5_r).XY();

	auto v1 = (position + Vector2{-half_width, half_height}).RotateCopy(rotation, position);
	auto v2 = (position + Vector2{-half_width, -half_height}).RotateCopy(rotation, position);
	auto v3 = (position + Vector2{half_width, -half_height}).RotateCopy(rotation, position);
	auto v4 = (position + Vector2{half_width, half_height}).RotateCopy(rotation, position);

	auto [ll_s, ll_t] = lower_left_tex_coord.XY();
	auto [ur_s, ur_t] = upper_right_tex_coord.XY();

	return {{v1, vector3::UnitZ, {ll_s, ur_t}, color},
			{v2, vector3::UnitZ, {ll_s, ll_t}, color},
			{v3, vector3::UnitZ, {ur_s, ll_t}, color},
			{v3, vector3::UnitZ, {ur_s, ll_t}, color},
			{v4, vector3::UnitZ, {ur_s, ur_t}, color},
			{v1, vector3::UnitZ, {ll_s, ur_t}, color}};
}


std::optional<Vector2> get_texture_size(materials::Material &material) noexcept
{
	auto texture =
		[&]() noexcept
		{
			if (auto [animation, texture] = material.DiffuseMap(); animation)
				return animation->UnderlyingFrameSequence() ?
					animation->UnderlyingFrameSequence()->FirstFrame() : nullptr;
			else
				return texture;
		}();

	if (texture)
	{
		if (texture->IsLoaded() || (texture->Owner() && texture->Owner()->Load(*texture)))
			return Vector2{static_cast<real>(texture->Extents()->Width), static_cast<real>(texture->Extents()->Height)};
	}
	
	return std::nullopt;
}

} //sprite::detail


//Protected

mesh::Vertices Sprite::GetVertices() const noexcept
{
	return detail::sprite_vertices(position_, rotation_, size_, color_,
		lower_left_tex_coord_, upper_right_tex_coord_);
}


std::optional<Vector2> Sprite::GetTextureSize() const noexcept
{
	if (auto material = SurfaceMaterial(); material)
	{
		if (auto texture_size = detail::get_texture_size(*material); texture_size)
		{
			//Adjust texture size from viewport to camera space
			if (auto model = Owner(); model)
			{
				if (auto scene_manager = model->Owner(); scene_manager)
				{
					if (auto viewport = scene_manager->ConnectedViewport(); viewport)
						*texture_size *= viewport->ViewportToCameraRatio();
				}
			}

			return texture_size;
		}
	}

	return std::nullopt;
}

void Sprite::RecalculateSize() noexcept
{
	if (auto texture_size = GetTextureSize(); texture_size)
	{
		Rectangle::Size(*texture_size);

		if (auto_repeat_)
			RecalculateTexCoords();
	}
}

void Sprite::RecalculateTexCoords() noexcept
{
	if (auto texture_size = GetTextureSize(); texture_size)
	{
		auto [lower_left, upper_right] =
			materials::material::detail::get_unflipped_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_);

		lower_left = vector2::Zero;
		upper_right = size_ / *texture_size;

		auto [new_lower_left, new_upper_right] =
			materials::material::detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_,
														lower_left, upper_right);
		lower_left_tex_coord_ = new_lower_left;
		upper_right_tex_coord_ = new_upper_right;

		update_vertices_ = true;
	}
}


//Public

Sprite::Sprite(const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible) :
	Sprite{vector3::Zero, size, material, visible}
{
	//Empty
}

Sprite::Sprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible) :
	Sprite{position, 0.0_r, size, material, visible}
{
	//Empty
}

Sprite::Sprite(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<materials::Material> material, bool visible) :
	Sprite{position, rotation, size, material, color::White, visible}
{
	//Empty
}


Sprite::Sprite(const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible) :
	Sprite{vector3::Zero, size, material, color, visible}
{
	//Empty
}

Sprite::Sprite(const Vector3 &position, const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible) :
	Sprite{position, 0.0_r, size, material, color, visible}
{
	//Empty
}

Sprite::Sprite(const Vector3 &position, real rotation, const Vector2 &size, NonOwningPtr<materials::Material> material, const Color &color, bool visible) :
	Rectangle{rectangle::detail::rectangle_vertices(position, rotation, size, color),
			  position, rotation, size, material, color, visible}
{
	//Empty
}


/*
	Texture coordinates
*/

void Sprite::Crop(const std::optional<Aabb> &area) noexcept
{
	//Crop by area
	if (area)
	{
		auto min = area->Min().CeilCopy(vector2::Zero).FloorCopy(vector2::UnitScale);
		auto max = area->Max().CeilCopy(vector2::Zero).FloorCopy(vector2::UnitScale);

		if (min != max)
		{
			auto [lower_left, upper_right] =
				materials::material::detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_, min, max);

			lower_left_tex_coord_ = lower_left;
			upper_right_tex_coord_ = upper_right;

			auto_repeat_ = false;
			update_vertices_ = true;
		}
	}
	//Un-crop
	else if (IsCropped())
	{
		auto [lower_left, upper_right] =
			materials::material::detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_, 0.0_r, 1.0_r);

		lower_left_tex_coord_ = lower_left;
		upper_right_tex_coord_ = upper_right;

		auto_repeat_ = false;
		update_vertices_ = true;
	}
}

void Sprite::Repeat(const std::optional<Vector2> &amount) noexcept
{
	//Repeat by amount
	if (amount)
	{
		if (auto max = amount->CeilCopy(vector2::Zero); vector2::Zero < max)
		{
			auto [lower_left, upper_right] =
				materials::material::detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_, 0.0_r, max);

			lower_left_tex_coord_ = lower_left;
			upper_right_tex_coord_ = upper_right;

			auto_repeat_ = false;
			update_vertices_ = true;
		}
	}
	//Un-repeat
	else if (IsRepeated())
	{
		auto [lower_left, upper_right] =
			materials::material::detail::get_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_, 0.0_r, 1.0_r);

		lower_left_tex_coord_ = lower_left;
		upper_right_tex_coord_ = upper_right;

		auto_repeat_ = false;
		update_vertices_ = true;
	}
}


void Sprite::FlipHorizontal() noexcept
{
	auto ll_s = lower_left_tex_coord_.X();
	auto ur_s = upper_right_tex_coord_.X();

	lower_left_tex_coord_.X(ur_s);
	upper_right_tex_coord_.X(ll_s);

	update_vertices_ = true;
}

void Sprite::FlipVertical() noexcept
{
	auto ll_t = lower_left_tex_coord_.Y();
	auto ur_t = upper_right_tex_coord_.Y();

	lower_left_tex_coord_.Y(ur_t);
	upper_right_tex_coord_.Y(ll_t);

	update_vertices_ = true;
}


bool Sprite::IsCropped() const noexcept
{
	auto [lower_left, upper_right] =
		materials::material::detail::get_unflipped_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_);
	return materials::material::detail::is_cropped(lower_left, upper_right);
}

bool Sprite::IsRepeated() const noexcept
{
	auto [lower_left, upper_right] =
		materials::material::detail::get_unflipped_tex_coords(lower_left_tex_coord_, upper_right_tex_coord_);
	return materials::material::detail::is_repeated(lower_left, upper_right);
}


bool Sprite::IsFlippedHorizontally() const noexcept
{
	return materials::material::detail::is_flipped_horizontally(lower_left_tex_coord_, upper_right_tex_coord_);
}

bool Sprite::IsFlippedVertically() const noexcept
{
	return materials::material::detail::is_flipped_vertically(lower_left_tex_coord_, upper_right_tex_coord_);
}

} //ion::graphics::scene::shapes