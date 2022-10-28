/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonLight.h
-------------------------------------------
*/

#ifndef ION_LIGHT_H
#define ION_LIGHT_H

#include <array>
#include <cmath>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "IonMovableObject.h"
#include "graphics/textures/IonTexture.h"
#include "graphics/textures/IonTextureManager.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::scene
{
	using namespace ion::utilities;
	using namespace types::type_literals;

	using utilities::Color;
	using utilities::Vector2;
	using utilities::Vector3;

	//Forward declarations
	class Camera;
	class Light;

	namespace light
	{
		enum class LightType
		{
			Point,
			Directional,
			Spot
		};

		namespace detail
		{
			using light_pointers = std::vector<Light*>;	

			constexpr auto light_float_components = 25;
			constexpr auto emissive_light_float_components = 8;
			constexpr auto min_texture_depth = 8;

			constexpr auto default_cutoff_angle = math::ToRadians(45.0_r);
			constexpr auto default_outer_cutoff_angle = math::ToRadians(55.0_r);

			constexpr auto light_texture_width =
				std::max(static_cast<int>(textures::texture_manager::detail::upper_power_of_two(light_float_components)), 4) / 4;
			constexpr auto emissive_light_texture_width =
				std::max(static_cast<int>(textures::texture_manager::detail::upper_power_of_two(emissive_light_float_components)), 4) / 4;

			
			struct light_texture
			{
				std::optional<textures::texture::TextureHandle> handle;
				int width = 0;
				int depth = 0;
			};

			using light_texture_data = std::array<real, light_texture_width * 4>; //RGBA
			using emissive_light_texture_data = std::array<real, emissive_light_texture_width * 4>; //RGBA

			struct light_data
			{
				NonOwningPtr<light_texture> texture;
				std::optional<int> texture_layer;
				light_texture_data uploaded_data{};
					//Enough space to store uploaded light or emissive light data

				light_data() = default;
				light_data(NonOwningPtr<light_texture> texture, std::optional<int> texture_layer,
					const light_texture_data &data) noexcept;
				light_data(NonOwningPtr<light_texture> texture, std::optional<int> texture_layer,
					const emissive_light_texture_data &data) noexcept;
			};


			inline auto angle_to_cutoff(real angle) noexcept
			{
				return math::Cos(angle);
			}

			inline auto cutoff_to_angle(real cutoff) noexcept
			{
				return std::acos(cutoff);
			}


			std::optional<light_texture> create_texture(int width, int depth) noexcept;
			std::optional<light_texture> create_light_texture(const light_pointers &lights) noexcept;
			std::optional<light_texture> create_emissive_light_texture(const light_pointers &lights) noexcept;

			void upload_light_data(OwningPtr<light_texture> &texture,
				const light_pointers &lights, const Camera &camera) noexcept;
			void upload_emissive_light_data(OwningPtr<light_texture> &texture,
				const light_pointers &lights, const Camera &camera) noexcept;
		} //detail
	} //light


	//A class representing a light that can be attached to a scene node
	//Each light is rendered with a fragment shader using the 'phong' lighting model
	class Light final : public MovableObject
	{
		private:

			light::LightType type_ = light::LightType::Point;
			Vector3 position_;
			Vector3 direction_;
			real radius_ = 0.0_r;

			Color ambient_color_ = color::White;
			Color diffuse_color_ = color::White;
			Color specular_color_ = color::DarkGray;

			real attenuation_constant_ = 1.0_r;
			real attenuation_linear_ = 0.0_r;
			real attenuation_quadratic_ = 0.0_r;
			
			real cutoff_ = light::detail::angle_to_cutoff(light::detail::default_cutoff_angle);
			real outer_cutoff_ = light::detail::angle_to_cutoff(light::detail::default_outer_cutoff_angle);

			bool cast_shadows_ = true;
			bool update_bounding_volumes_ = true;
			light::detail::light_data data_;


			void PrepareBoundingVolumes() noexcept;

		public:

			//Constructs a new light with the given name and visibility
			explicit Light(std::optional<std::string> name = {}, bool visible = true) noexcept;

			//Constructs a new light with the given name and values
			Light(std::optional<std::string> name, light::LightType type,
				const Vector3 &position, const Vector3 &direction, real radius,
				const Color &diffuse,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle, bool visible = true) noexcept;

			//Constructs a new light with the given name and values
			Light(std::optional<std::string> name, light::LightType type,
				const Vector3 &position, const Vector3 &direction, real radius,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle, bool visible = true) noexcept;


			/*
				Static light conversions
			*/

			//Returns a new point light from the given name and values
			[[nodiscard]] static Light Point(std::optional<std::string> name,
				const Vector3 &position, real radius, const Color &diffuse,
				bool visible = true) noexcept;

			//Returns a new point light from the given name and values
			[[nodiscard]] static Light Point(std::optional<std::string> name,
				const Vector3 &position, real radius, const Color &diffuse,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				bool visible = true) noexcept;

			//Returns a new point light from the given name and values
			[[nodiscard]] static Light Point(std::optional<std::string> name,
				const Vector3 &position, real radius,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				bool visible = true) noexcept;


			//Returns a new directional light from the given name and values
			[[nodiscard]] static Light Directional(std::optional<std::string> name,
				const Vector3 &direction, const Color &diffuse, bool visible = true) noexcept;

			//Returns a new directional light from the given name and values
			[[nodiscard]] static Light Directional(std::optional<std::string> name,
				const Vector3 &direction,
				const Color &ambient, const Color &diffuse, const Color &specular,
				bool visible = true) noexcept;


			//Returns a new spot light from the given name and values
			[[nodiscard]] static Light Spot(std::optional<std::string> name,
				const Vector3 &position, const Vector3 &direction, const Color &diffuse,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle, bool visible = true) noexcept;

			//Returns a new spot light from the given name and values
			[[nodiscard]] static Light Spot(std::optional<std::string> name,
				const Vector3 &position, const Vector3 &direction,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle, bool visible = true) noexcept;


			/*
				Modifiers
			*/

			//Sets the type of light given off by this light source to the given type
			inline void Type(light::LightType type) noexcept
			{
				if (type_ != type)
				{
					type_ = type;
					update_bounding_volumes_ = true;
				}
			}

			//Sets the position of the light to the given position
			//This value only applies for lights of type point and spot light
			inline void Position(const Vector3 &position) noexcept
			{
				position_ = position;
			}

			//Sets the position of the light to the given position
			//This value only applies for lights of type point and spot light
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}

			//Sets the direction of the light to the given direction
			//This value only applies for lights of type directional light
			inline void Direction(const Vector3 &direction) noexcept
			{
				direction_ = direction;
			}

			//Sets the radius of the light to the given value
			//A radius of 0 indicates an unlimited radius
			//These values only applies for lights of type point light
			inline void Radius(real radius) noexcept
			{
				if (radius_ != radius)
				{
					radius_ = radius;
					update_bounding_volumes_ = true;
				}
			}


			//Sets the color of the ambient light given off by this light source to the given color
			inline void AmbientColor(const Color &ambient) noexcept
			{
				ambient_color_ = ambient;
			}
			
			//Sets the color of the diffuse light given off by this light source to the given color
			//Also sets the ambient color if equal to the diffuse color
			inline void DiffuseColor(const Color &diffuse) noexcept
			{
				if (diffuse_color_ == ambient_color_)
					ambient_color_ = diffuse;

				diffuse_color_ = diffuse;
			}
			
			//Sets the color of the specular light given off by this light source to the given color
			inline void SpecularColor(const Color &specular) noexcept
			{
				specular_color_ = specular;
			}


			//Sets the attenuation to the given values
			//These values only applies for lights of type point and spot light
			inline void Attenuation(real constant, real linear, real quadratic) noexcept
			{
				attenuation_constant_ = constant;
				attenuation_linear_ = linear;
				attenuation_quadratic_ = quadratic;
			}

			//Sets the inner and outer cutoff values of the light to the given angles (radians)
			//These values only applies for lights of type spot light
			inline void Cutoff(real inner_angle, real outer_angle) noexcept
			{
				cutoff_ = light::detail::angle_to_cutoff(inner_angle);
				outer_cutoff_ = light::detail::angle_to_cutoff(outer_angle);
			}


			//Sets if this light casts shadows or not
			inline void CastShadows(bool enabled) noexcept
			{
				cast_shadows_ = enabled;
			}

			//Sets the data of this light to the given data
			inline void Data(const light::detail::light_data &data) noexcept
			{
				data_ = data;
			}


			/*
				Observers
			*/

			//Returns the type of light given off by this light source 
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			//Returns the position of the light
			//This value only applies for lights of type point and spot light
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the direction of the light
			//This value only applies for lights of type directional light
			[[nodiscard]] inline auto& Direction() const noexcept
			{
				return direction_;
			}

			//Returns the radius of the light
			//A radius of 0 indicates an unlimited radius
			//These values only applies for lights of type point light
			[[nodiscard]] inline auto Radius() const noexcept
			{
				return radius_;
			}


			//Returns the color of the ambient light given off by this light source
			[[nodiscard]] inline auto& AmbientColor() const noexcept
			{
				return ambient_color_;
			}
			
			//Returns the color of the diffuse light given off by this light source
			[[nodiscard]] inline auto& DiffuseColor() const noexcept
			{
				return diffuse_color_;
			}
			
			//Returns the color of the specular light given off by this light source
			[[nodiscard]] inline auto& SpecularColor() const noexcept
			{
				return specular_color_;
			}


			//Returns the constant, linear and quadratic attenuation values
			//These values only applies for lights of type point and spot light
			[[nodiscard]] inline auto Attenuation() const noexcept
			{
				return std::tuple{attenuation_constant_, attenuation_linear_, attenuation_quadratic_};
			}

			//Returns the inner and outer cutoff angle (radians) of the light
			//These values only applies for lights of type spot light
			[[nodiscard]] inline auto Cutoff() const noexcept
			{
				return std::pair{light::detail::cutoff_to_angle(cutoff_),
								 light::detail::cutoff_to_angle(outer_cutoff_)};
			}


			//Returns true if this light cast shadows
			[[nodiscard]] inline auto CastShadows() const noexcept
			{
				return cast_shadows_;
			}

			//Returns the data of this light
			[[nodiscard]] inline auto& Data() const noexcept
			{
				return data_;
			}


			/*
				Preparing
			*/

			//Prepares this light
			//This function is typically called each frame
			void Prepare() override;
	};
} //ion::graphics::scene

#endif