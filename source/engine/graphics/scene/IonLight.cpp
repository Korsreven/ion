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

light_data::light_data(NonOwningPtr<light_texture> texture, std::optional<int> texture_layer,
	const light_texture_data &data) noexcept :

	texture{texture},
	texture_layer{texture_layer}
{
	std::copy(std::begin(data), std::end(data), std::begin(uploaded_data));
}

light_data::light_data(NonOwningPtr<light_texture> texture, std::optional<int> texture_layer,
	const emissive_light_texture_data &data) noexcept :

	texture{texture},
	texture_layer{texture_layer}
{
	std::copy(std::begin(data), std::end(data), std::begin(uploaded_data));
}


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


void upload_light_data(OwningPtr<light_texture> &texture,
	const light_pointers &lights, const Camera &camera) noexcept
{
	//Too many lights to fit inside texture, create new texture (next POT)
	if ((texture ? texture->depth : 0) < std::ssize(lights))
	{
		if (texture && texture->handle)
			textures::texture_manager::detail::unload_texture(*texture->handle);
		
		if (auto new_texture = create_light_texture(lights); new_texture)
			texture = make_owning<light_texture>(*new_texture);
	}
	
	if (texture && texture->handle)
	{
		constexpr auto type = std::is_same_v<real, float> ? GL_FLOAT : GL_DOUBLE;

		light_texture_data light_data{};
		glBindTexture(GL_TEXTURE_1D_ARRAY, texture->handle->Id);

		for (auto i = 0; auto &light : lights)
		{
			light_data[0] = static_cast<real>(light->Type());

			{
				auto [x, y, z] =
					(camera.ViewMatrix() * (light->Position() + light->ParentNode()->DerivedPosition())).XYZ(); //View adjusted
				light_data[1] = x;
				light_data[2] = y;
				light_data[3] = z;
			}

			{
				auto [x, y, z] =
					(light->Direction().Deviant(light->ParentNode()->DerivedRotation() -
					(camera.Rotation() + camera.ParentNode()->DerivedRotation()))).XYZ(); //View adjusted
				light_data[4] = x;
				light_data[5] = y;
				light_data[6] = z;
			}

			auto [sx, sy] = light->ParentNode()->DerivedScaling().XY();
			light_data[7] = light->Radius() * std::max(sx, sy);

			{
				auto [r, g, b, a] = light->AmbientColor().RGBA();
				light_data[8] = r;
				light_data[9] = g;
				light_data[10] = b;
				light_data[11] = a;
			}

			{
				auto [r, g, b, a] = light->DiffuseColor().RGBA();
				light_data[12] = r;
				light_data[13] = g;
				light_data[14] = b;
				light_data[15] = a;
			}

			{
				auto [r, g, b, a] = light->SpecularColor().RGBA();
				light_data[16] = r;
				light_data[17] = g;
				light_data[18] = b;
				light_data[19] = a;
			}

			auto [constant, linear, quadratic] = light->Attenuation();
			light_data[20] = constant;
			light_data[21] = linear;
			light_data[22] = quadratic;

			auto [cutoff_angle, outer_cutoff_angle] = light->Cutoff();
			light_data[24] = math::Cos(cutoff_angle);
			light_data[25] = math::Cos(outer_cutoff_angle);

			//Light data has changed
			if (auto &data = light->Data();
				!data.texture || data.texture_layer != i ||
				std::memcmp(std::data(data.uploaded_data), std::data(light_data), std::size(light_data) * sizeof(real)) != 0)
			{
				//Upload light data to gl
				glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0,
					0, i, texture->width, 1,
					GL_RGBA, type, std::data(light_data));
				light->Data({texture, i, light_data});
			}

			++i;
		}

		glBindTexture(GL_TEXTURE_1D_ARRAY, 0);
	}
}

void upload_emissive_light_data(OwningPtr<light_texture> &texture,
	const light_pointers &lights, const Camera &camera) noexcept
{
	//Too many lights to fit inside texture, create new texture (next POT)
	if ((texture ? texture->depth : 0) < std::ssize(lights))
	{
		if (texture && texture->handle)
			textures::texture_manager::detail::unload_texture(*texture->handle);

		if (auto new_texture = create_emissive_light_texture(lights); new_texture)
			texture = make_owning<light_texture>(*new_texture);
	}
	
	if (texture && texture->handle)
	{
		constexpr auto type = std::is_same_v<real, float> ? GL_FLOAT : GL_DOUBLE;

		emissive_light_texture_data light_data{};
		glBindTexture(GL_TEXTURE_1D_ARRAY, texture->handle->Id);

		for (auto i = 0; auto &light : lights)
		{
			auto [x, y, z] =
				(camera.ViewMatrix() * (light->Position() + light->ParentNode()->DerivedPosition())).XYZ(); //View adjusted
			light_data[0] = x;
			light_data[1] = y;
			light_data[2] = z;

			auto [sx, sy] = light->ParentNode()->DerivedScaling().XY();
			light_data[3] = light->Radius() * std::max(sx, sy);

			auto [r, g, b, a] = light->DiffuseColor().RGBA();
			light_data[4] = r;
			light_data[5] = g;
			light_data[6] = b;
			light_data[7] = a;

			//Light data has changed
			if (auto &data = light->Data();
				!data.texture || data.texture_layer != i ||
				std::memcmp(std::data(data.uploaded_data), std::data(light_data), std::size(light_data) * sizeof(real)) != 0)
			{
				//Upload light data to gl
				glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0,
					0, i, texture->width, 1,
					GL_RGBA, type, std::data(light_data));
				light->Data({texture, i, light_data});
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
	if (type_ == LightType::Point && radius_ > 0.0_r)
		aabb_ = Aabb::Size(radius_ * 2.0_r, position_);
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

	type_{type},
	position_{position},
	direction_{direction},
	radius_{radius},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},

	attenuation_constant_{attenuation_constant},
	attenuation_linear_{attenuation_linear},
	attenuation_quadratic_{attenuation_quadratic},
	
	cutoff_{detail::angle_to_cutoff(cutoff_angle)},
	outer_cutoff_{detail::angle_to_cutoff(outer_cutoff_angle)}
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