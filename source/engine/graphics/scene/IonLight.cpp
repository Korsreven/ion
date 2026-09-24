/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonLight.cpp
-------------------------------------------
*/

#include "IonLight.h"

#include <cstring>
#include <type_traits>

#include "IonCamera.h"
#include "graph/IonSceneNode.h"
#include "graphics/IonGraphicsAPI.h"
#include "query/IonSceneQuery.h"

#undef max

namespace ion::graphics::scene
{

using namespace light;
using namespace utilities;

namespace light::detail
{

std::optional<light_texture> create_texture(int width, int depth) noexcept
{
	if (!textures::texture_manager::detail::has_support_for_array_texture())
		return {};

	constexpr auto type = std::is_same_v<real, float> ? GL_FLOAT : GL_DOUBLE;

	if (auto max_lights = textures::texture_manager::detail::max_array_texture_layers(); depth > max_lights)
		depth = max_lights;

	auto texture_handle =
		textures::texture::TextureHandle{0, textures::texture::TextureType::ArrayTexture1D};

	glGenTextures(1, reinterpret_cast<unsigned int*>(&texture_handle.Id));
	glBindTexture(GL_TEXTURE_1D_ARRAY, texture_handle.Id);

	glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Create gl texture (POT)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_1D_ARRAY, 0,
		GL_RGBA32F, width, depth,
		0, GL_RGBA, type, nullptr);

	glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
	return light_texture{texture_handle, width, depth};
}

std::optional<light_texture> create_light_texture(const light_pointers &lights) noexcept
{
	auto depth = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(std::size(lights)));
	return create_texture(light_texture_width, std::max(depth, min_texture_depth));
}

std::optional<light_texture> create_emissive_light_texture(const light_pointers &lights) noexcept
{
	auto depth = static_cast<int>(textures::texture_manager::detail::upper_power_of_two(std::size(lights)));
	return create_texture(emissive_light_texture_width, std::max(depth, min_texture_depth));
}


void upload_light_data(OwningPtr<light_texture> &texture, light_texture_map &texture_map,
	const light_pointers &lights, const Camera &camera) noexcept
{
	//Too many lights to fit inside texture, create new texture (next POT)
	if ((texture ? texture->depth : 0) < std::ssize(lights))
	{
		if (texture && texture->handle)
			textures::texture_manager::detail::unload_texture(*texture->handle);
		
		if (auto new_texture = create_light_texture(lights); new_texture)
		{
			texture = make_owning<light_texture>(*new_texture);
			texture_map.clear();
		}
	}
	
	if (texture && texture->handle)
	{
		constexpr auto type = std::is_same_v<real, float> ? GL_FLOAT : GL_DOUBLE;
		glBindTexture(GL_TEXTURE_1D_ARRAY, texture->handle->Id);

		for (auto i = 0; auto &light : lights)
		{
			light->ViewAdjust(camera);

			//Light data has changed
			if (auto iter = texture_map.find(i); iter == std::end(texture_map) ||
				iter->second != light || light->IsDirty())
			{
				//Upload light data to gl texture
				glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0,
					0, i, texture->width, 1,
					GL_RGBA, type, &light->Data());

				texture_map[i] = light;
			}

			++i;
		}

		glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
	}
}

void upload_emissive_light_data(OwningPtr<light_texture> &texture, light_texture_map &texture_map,
	const light_pointers &lights, const Camera &camera) noexcept
{
	//Too many lights to fit inside texture, create new texture (next POT)
	if ((texture ? texture->depth : 0) < std::ssize(lights))
	{
		if (texture && texture->handle)
			textures::texture_manager::detail::unload_texture(*texture->handle);

		if (auto new_texture = create_emissive_light_texture(lights); new_texture)
		{
			texture = make_owning<light_texture>(*new_texture);
			texture_map.clear();
		}
	}
	
	if (texture && texture->handle)
	{
		constexpr auto type = std::is_same_v<real, float> ? GL_FLOAT : GL_DOUBLE;
		glBindTexture(GL_TEXTURE_1D_ARRAY, texture->handle->Id);

		for (auto i = 0; auto &light : lights)
		{
			light->ViewAdjust(camera);

			//Light data has changed
			if (auto iter = texture_map.find(i); iter == std::end(texture_map) ||
				iter->second != light || light->IsDirty())
			{
				//Upload light data to gl texture
				glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0,
					0, i, texture->width, 1,
					GL_RGBA, type, &light->EmissiveData());

				texture_map[i] = light;
			}

			++i;
		}

		glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
	}
}

} //light::detail


//Private

void Light::PrepareBoundingVolumes() noexcept
{
	if (data_.type == LightType::Point && data_.radius > 0.0_r)
		aabb_ = Aabb::Size(data_.radius * 2.0_r, data_.position);
	else
		aabb_ = {};

	obb_ = aabb_;
	sphere_ = {aabb_.ToHalfSize().Max(), aabb_.Center()};
}


//Public

Light::Light(std::optional<std::string> name, bool visible) noexcept :
	MovableObject{std::move(name), visible}
{
	query_type_flags_ |= query::scene_query::QueryType::Light;
}

Light::Light(std::optional<std::string> name, LightType type,
	const Vector3 &position, const Vector3 &direction, real radius,
	const Color &diffuse,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible) noexcept :

	Light{std::move(name), type,
		  position, direction, radius,
		  diffuse, diffuse, color::DarkGray,
		  attenuation_constant, attenuation_linear, attenuation_quadratic,
		  cutoff_angle, outer_cutoff_angle, visible}
{
	//Empty
}

Light::Light(std::optional<std::string> name, LightType type,
	const Vector3 &position, const Vector3 &direction, real radius,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible) noexcept :

	MovableObject{std::move(name), visible},

	position_{position},
	direction_{direction},
	radius_{radius},

	ambient_alpha_{ambient.A()},
	diffuse_alpha_{diffuse.A()},
	specular_alpha_{specular.A()},

	data_
	{
		type,
		position,
		direction,
		radius,

		ambient,
		diffuse,
		specular,

		attenuation_constant,
		attenuation_linear,
		attenuation_quadratic,
		{},

		detail::angle_to_cutoff(cutoff_angle),
		detail::angle_to_cutoff(outer_cutoff_angle)
	},

	emissive_data_
	{
		position,
		radius,
		diffuse
	}
{
	query_type_flags_ |= query::scene_query::QueryType::Light;
}


/*
	Static light conversions
*/

Light Light::Point(std::optional<std::string> name,
	const Vector3 &position, real radius, const Color &diffuse,
	bool visible) noexcept
{
	return {std::move(name), LightType::Point,
			position, vector3::Zero, radius,
			diffuse,
			1.0_r, 0.0_r, 0.0_r,
			0.0_r, 0.0_r, visible};
}

Light Light::Point(std::optional<std::string> name,
	const Vector3 &position, real radius, const Color &diffuse,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	bool visible) noexcept
{
	return {std::move(name), LightType::Point,
			position, vector3::Zero, radius,
			diffuse,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			0.0_r, 0.0_r, visible};
}

Light Light::Point(std::optional<std::string> name,
	const Vector3 &position, real radius,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	bool visible) noexcept
{
	return {std::move(name), LightType::Point,
			position, vector3::Zero, radius,
			ambient, diffuse, specular,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			0.0_r, 0.0_r, visible};
}


Light Light::Directional(std::optional<std::string> name,
	const Vector3 &direction, const Color &diffuse, bool visible) noexcept
{
	return {std::move(name), LightType::Directional,
			vector3::Zero, direction, 0.0,
			diffuse,
			1.0_r, 0.0_r, 0.0_r,
			0.0_r, 0.0_r, visible};
}

Light Light::Directional(std::optional<std::string> name,
	const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	bool visible) noexcept
{
	return {std::move(name), LightType::Directional,
			vector3::Zero, direction, 0.0,
			ambient, diffuse, specular,
			1.0_r, 0.0_r, 0.0_r,
			0.0_r, 0.0_r, visible};
}


Light Light::Spot(std::optional<std::string> name,
	const Vector3 &position, const Vector3 &direction, const Color &diffuse,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible) noexcept
{
	return {std::move(name), LightType::Spot,
			position, direction, 0.0,
			diffuse,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			cutoff_angle, outer_cutoff_angle, visible};
}

Light Light::Spot(std::optional<std::string> name,
	const Vector3 &position, const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle, bool visible) noexcept
{
	return {std::move(name), LightType::Spot,
			position, direction, 0.0,
			ambient, diffuse, specular,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			cutoff_angle, outer_cutoff_angle, visible};
}


/*
	Modifiers
*/

void Light::ViewAdjust(const Camera &camera)
{
	if (auto parent_node = ParentNode(), camera_node = camera.ParentNode();
		parent_node && camera_node)
	{
		//Position
		if (data_.type != LightType::Directional)
		{
			auto adjusted_position = camera.ViewMatrix().
				TransformPoint(position_ + parent_node->DerivedPosition());

			if (data_.position != adjusted_position)
			{
				data_.position = emissive_data_.position = adjusted_position;
				dirty_ = true;
			}
		}

		//Direction
		if (data_.type != LightType::Point)
		{
			auto adjusted_direction = direction_.Deviant(parent_node->DerivedRotation() -
				(camera.Rotation() + camera_node->DerivedRotation()));

			if (data_.direction != adjusted_direction)
			{
				data_.direction = adjusted_direction;
				dirty_ = true;
			}
		}

		//Radius
		if (data_.type == LightType::Point)
		{
			auto adjusted_radius = radius_ * parent_node->DerivedScaling().Max();

			if (data_.radius != adjusted_radius)
			{
				data_.radius = emissive_data_.radius = adjusted_radius;
				dirty_ = true;
			}
		}
	}
}


/*
	Preparing
*/

void Light::Prepare()
{
	if (update_bounding_volumes_)
	{
		PrepareBoundingVolumes();
		update_bounding_volumes_ = false;
	}
}

} //ion::graphics::scene