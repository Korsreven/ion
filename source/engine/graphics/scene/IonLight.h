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
#include <bit>
#include <cmath>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "IonMovableObject.h"
#include "adaptors/IonFlatMap.h"
#include "graphics/textures/IonTexture.h"
#include "graphics/textures/IonTextureManager.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

#undef max

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
		enum class LightType : uint32
		{
			Point		= std::bit_cast<uint32>(0.0_r),
			Directional = std::bit_cast<uint32>(1.0_r),
			Spot		= std::bit_cast<uint32>(2.0_r)
		};

		namespace detail
		{
			constexpr auto light_float_components = 25 + 1;
			constexpr auto emissive_light_float_components = 8;
			constexpr auto min_texture_depth = 8;
			
			constexpr auto light_texture_width =
				std::max(static_cast<int>(textures::texture_manager::detail::upper_power_of_two(light_float_components)), 4) / 4;
			constexpr auto emissive_light_texture_width =
				std::max(static_cast<int>(textures::texture_manager::detail::upper_power_of_two(emissive_light_float_components)), 4) / 4;

			constexpr auto default_cutoff_angle = math::ToRadians(45.0_r);
			constexpr auto default_outer_cutoff_angle = math::ToRadians(55.0_r);


			inline auto angle_to_cutoff(real angle) noexcept
			{
				return math::Cos(angle);
			}

			inline auto cutoff_to_angle(real cutoff) noexcept
			{
				return std::acos(cutoff);
			}


			struct light_render_data
			{
				static_assert(sizeof(light::LightType) == sizeof(real));

				light::LightType type = light::LightType::Point;
				Vector3 position;
				Vector3 direction;
				real radius = 0.0_r;

				Color ambient_color = color::White;
				Color diffuse_color = color::White;
				Color specular_color = color::DarkGray;

				real attenuation_constant = 1.0_r;
				real attenuation_linear = 0.0_r;
				real attenuation_quadratic = 0.0_r;
				real _padding{};

				real cutoff = angle_to_cutoff(default_cutoff_angle);
				real outer_cutoff = angle_to_cutoff(default_outer_cutoff_angle);
				real _tail_padding[light_texture_width * 4 - light_float_components]{};
			};

			struct emissive_light_render_data
			{
				Vector3 position;
				real radius = 0.0_r;
				Color diffuse_color = color::White;
			};


			struct light_texture
			{
				std::optional<textures::texture::TextureHandle> handle;
				int width = 0;
				int depth = 0;
			};

			using light_texture_map = adaptors::FlatMap<int, Light*>;
			using light_pointers = std::vector<Light*>;


			std::optional<light_texture> create_texture(int width, int depth) noexcept;
			std::optional<light_texture> create_light_texture(const light_pointers &lights) noexcept;
			std::optional<light_texture> create_emissive_light_texture(const light_pointers &lights) noexcept;

			void upload_light_data(OwningPtr<light_texture> &texture, light_texture_map &texture_map,
				const light_pointers &lights, const Camera &camera) noexcept;
			void upload_emissive_light_data(OwningPtr<light_texture> &texture, light_texture_map &texture_map,
				const light_pointers &lights, const Camera &camera) noexcept;
		} //detail
	} //light


	///@brief A class representing a light that can be attached to a scene node
	///@details Each light is rendered with a fragment shader using the 'phong' lighting model
	class Light final : public MovableObject
	{
		private:

			Vector3 position_;
			Vector3 direction_;
			real radius_ = 0.0_r;

			real ambient_alpha_ = 1.0_r;
			real diffuse_alpha_ = 1.0_r;
			real specular_alpha_ = 1.0_r;
			real intensity_ = 1.0_r;
			real fade_intensity_ = 1.0_r;

			light::detail::light_render_data data_;
			light::detail::emissive_light_render_data emissive_data_;
			
			bool cast_shadows_ = true;
			bool update_bounding_volumes_ = true;
			bool dirty_ = true;


			void PrepareBoundingVolumes() noexcept;

		public:

			///@brief Constructs a new light with the given name and visibility
			explicit Light(std::optional<std::string> name = {}, bool visible = true) noexcept;

			///@brief Constructs a new light with the given name and values
			Light(std::optional<std::string> name, light::LightType type,
				const Vector3 &position, const Vector3 &direction, real radius,
				const Color &diffuse,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle, bool visible = true) noexcept;

			///@brief Constructs a new light with the given name and values
			Light(std::optional<std::string> name, light::LightType type,
				const Vector3 &position, const Vector3 &direction, real radius,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle, bool visible = true) noexcept;


			/**
				@name Static light conversions
				@{
			*/

			///@brief Returns a new point light from the given name and values
			[[nodiscard]] static Light Point(std::optional<std::string> name,
				const Vector3 &position, real radius, const Color &diffuse,
				bool visible = true) noexcept;

			///@brief Returns a new point light from the given name and values
			[[nodiscard]] static Light Point(std::optional<std::string> name,
				const Vector3 &position, real radius, const Color &diffuse,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				bool visible = true) noexcept;

			///@brief Returns a new point light from the given name and values
			[[nodiscard]] static Light Point(std::optional<std::string> name,
				const Vector3 &position, real radius,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				bool visible = true) noexcept;


			///@brief Returns a new directional light from the given name and values
			[[nodiscard]] static Light Directional(std::optional<std::string> name,
				const Vector3 &direction, const Color &diffuse, bool visible = true) noexcept;

			///@brief Returns a new directional light from the given name and values
			[[nodiscard]] static Light Directional(std::optional<std::string> name,
				const Vector3 &direction,
				const Color &ambient, const Color &diffuse, const Color &specular,
				bool visible = true) noexcept;


			///@brief Returns a new spot light from the given name and values
			[[nodiscard]] static Light Spot(std::optional<std::string> name,
				const Vector3 &position, const Vector3 &direction, const Color &diffuse,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle, bool visible = true) noexcept;

			///@brief Returns a new spot light from the given name and values
			[[nodiscard]] static Light Spot(std::optional<std::string> name,
				const Vector3 &position, const Vector3 &direction,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle, bool visible = true) noexcept;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the type of light given off by this light source to the given type
			inline void Type(light::LightType type) noexcept
			{
				if (data_.type != type)
				{
					data_.type = type;
					update_bounding_volumes_ = true;
					dirty_ = true;
				}
			}

			///@brief Sets the position of the light to the given position
			///@details This value only applies for lights of type point and spot light
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = data_.position = emissive_data_.position = position;
					dirty_ = true;
				}
			}

			///@brief Sets the position of the light to the given position
			///@details This value only applies for lights of type point and spot light
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), data_.position.Z()});
			}

			///@brief Sets the direction of the light to the given direction
			///@details This value only applies for lights of type directional light
			inline void Direction(const Vector3 &direction) noexcept
			{
				if (direction_ != direction)
				{
					direction_ = data_.direction = direction;
					dirty_ = true;
				}
			}

			///@brief Sets the radius of the light to the given value
			///@details A radius of 0 indicates an unlimited radius.
			///These values only applies for lights of type point light
			inline void Radius(real radius) noexcept
			{
				if (radius_ != radius)
				{
					radius_ = data_.radius = emissive_data_.radius = radius;
					update_bounding_volumes_ = true;
					dirty_ = true;
				}
			}


			///@brief Sets the color of the ambient light given off by this light source to the given color
			inline void AmbientColor(const Color &ambient) noexcept
			{
				if (data_.ambient_color != ambient)
				{
					data_.ambient_color = ambient;
					ambient_alpha_ = ambient.A();
					dirty_ = true;
				}
			}
			
			///@brief Sets the color of the diffuse light given off by this light source to the given color
			///@details Also sets the ambient color if equal to the diffuse color
			inline void DiffuseColor(const Color &diffuse) noexcept
			{
				if (data_.diffuse_color != diffuse)
				{
					if (data_.diffuse_color == data_.ambient_color)
						AmbientColor(diffuse);

					data_.diffuse_color = emissive_data_.diffuse_color = diffuse;
					diffuse_alpha_ = diffuse.A();
					dirty_ = true;
				}
			}
			
			///@brief Sets the color of the specular light given off by this light source to the given color
			inline void SpecularColor(const Color &specular) noexcept
			{
				if (data_.specular_color != specular)
				{
					data_.specular_color = specular;
					specular_alpha_ = specular.A();
					dirty_ = true;
				}
			}

			///@brief Sets the intensity of the light given off by this light source to the given value
			inline void Intensity(real intensity) noexcept
			{
				if (intensity_ != intensity)
				{
					intensity_ = intensity;
					data_.ambient_color.A(ambient_alpha_ * intensity * fade_intensity_);
					data_.diffuse_color.A(diffuse_alpha_ * intensity * fade_intensity_);
					data_.specular_color.A(specular_alpha_ * intensity * fade_intensity_);
					emissive_data_.diffuse_color.A(data_.diffuse_color.A());
					dirty_ = true;
				}
			}

			///@brief Sets the fade intensity of the light given off by this light source to the given value
			///@details This is in use by the built in fading system. See NodeAnimation::FadingMotionType::LightIntensity for more details
			///To set a custom user intensity, consider calling Light::Intensity instead
			inline void FadeIntensity(real intensity) noexcept
			{
				if (fade_intensity_ != intensity)
				{
					fade_intensity_ = intensity;
					data_.ambient_color.A(ambient_alpha_ * intensity_ * intensity);
					data_.diffuse_color.A(diffuse_alpha_ * intensity_ * intensity);
					data_.specular_color.A(specular_alpha_ * intensity_ * intensity);
					emissive_data_.diffuse_color.A(data_.diffuse_color.A());
					dirty_ = true;
				}
			}


			///@brief Sets the attenuation to the given values
			///@details These values only applies for lights of type point and spot light
			inline void Attenuation(real constant, real linear, real quadratic) noexcept
			{
				if (data_.attenuation_constant != constant ||
					data_.attenuation_linear != linear ||
					data_.attenuation_quadratic != quadratic)
				{
					data_.attenuation_constant = constant;
					data_.attenuation_linear = linear;
					data_.attenuation_quadratic = quadratic;
					dirty_ = true;
				}
			}

			///@brief Sets the inner and outer cutoff values of the light to the given angles (radians)
			///@details These values only applies for lights of type spot light
			inline void Cutoff(real inner_angle, real outer_angle) noexcept
			{
				if (auto cutoff = light::detail::angle_to_cutoff(inner_angle),
					outer_cutoff = light::detail::angle_to_cutoff(outer_angle);
					data_.cutoff != cutoff ||
					data_.outer_cutoff != outer_cutoff)
				{
					data_.cutoff = cutoff;
					data_.outer_cutoff = outer_cutoff;
					dirty_ = true;
				}
			}


			///@brief Sets if this light casts shadows or not
			inline void CastShadows(bool enabled) noexcept
			{
				cast_shadows_ = enabled;
			}


			///@brief View adjust this light for the given camera
			///@details This function is typically called before uploading data
			void ViewAdjust(const Camera &camera);

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the type of light given off by this light source 
			[[nodiscard]] inline auto Type() const noexcept
			{
				return data_.type;
			}

			///@brief Returns the position of the light
			///@details This value only applies for lights of type point and spot light
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			///@brief Returns the direction of the light
			///@details This value only applies for lights of type directional and spot light
			[[nodiscard]] inline auto& Direction() const noexcept
			{
				return direction_;
			}

			///@brief Returns the radius of the light
			///@details A radius of 0 indicates an unlimited radius.
			///These values only applies for lights of type point light
			[[nodiscard]] inline auto Radius() const noexcept
			{
				return radius_;
			}


			///@brief Returns the color of the ambient light given off by this light source
			[[nodiscard]] inline auto& AmbientColor() const noexcept
			{
				return data_.ambient_color;
			}
			
			///@brief Returns the color of the diffuse light given off by this light source
			[[nodiscard]] inline auto& DiffuseColor() const noexcept
			{
				return data_.diffuse_color;
			}
			
			///@brief Returns the color of the specular light given off by this light source
			[[nodiscard]] inline auto& SpecularColor() const noexcept
			{
				return data_.specular_color;
			}

			///@brief Returns the intensity of the light given off by this light source
			[[nodiscard]] inline auto Intensity() const noexcept
			{
				return intensity_;
			}

			///@brief Returns the fade intensity of the light given off by this light source
			[[nodiscard]] inline auto FadeIntensity() const noexcept
			{
				return fade_intensity_;
			}


			///@brief Returns the constant, linear and quadratic attenuation values
			///@details These values only applies for lights of type point and spot light
			[[nodiscard]] inline auto Attenuation() const noexcept
			{
				return std::tuple{data_.attenuation_constant, data_.attenuation_linear, data_.attenuation_quadratic};
			}

			///@brief Returns the inner and outer cutoff of the light
			///@details These values only applies for lights of type spot light
			[[nodiscard]] inline auto Cutoff() const noexcept
			{
				return std::pair{data_.cutoff, data_.outer_cutoff};
			}

			///@brief Returns the inner and outer cutoff angle (radians) of the light
			///@details These values only applies for lights of type spot light
			[[nodiscard]] inline auto CutoffAngle() const noexcept
			{
				return std::pair{light::detail::cutoff_to_angle(data_.cutoff),
								 light::detail::cutoff_to_angle(data_.outer_cutoff)};
			}


			///@brief Returns the data for this light
			[[nodiscard]] inline auto& Data() const noexcept
			{
				return data_;
			}

			///@brief Returns the emissive data for this light
			[[nodiscard]] inline auto& EmissiveData() const noexcept
			{
				return emissive_data_;
			}


			///@brief Returns true if this light cast shadows
			[[nodiscard]] inline auto CastShadows() const noexcept
			{
				return cast_shadows_;
			}

			///@brief Returns true if this light is dirty
			///@details Resets dirty flag when accessed
			[[nodiscard]] inline auto IsDirty() noexcept
			{
				return std::exchange(dirty_, false);
			}

			///@brief Returns true if this light is dirty
			[[nodiscard]] inline auto IsDirty() const noexcept
			{
				return dirty_;
			}

			///@}

			/**
				@name Preparing
				@{
			*/

			///@brief Prepares this light
			///@details This function is typically called each frame
			void Prepare() override;

			///@}
	};
} //ion::graphics::scene

#endif