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

#include <cmath>
#include <tuple>
#include <utility>

#include "IonMovableObject.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::scene
{
	using namespace ion::utilities;
	using namespace types::type_literals;

	using utilities::Color;
	using utilities::Vector3;

	namespace light
	{
		enum class LightType
		{
			Point,
			Directional,
			Spotlight
		};

		namespace detail
		{
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
		} //detail
	} //light


	class Light final : public MovableObject
	{
		private:

			light::LightType type_ = light::LightType::Point;
			Vector3 position_; //TEMP, should be located in parent node
			Vector3 direction_; //TEMP, should be located in parent node

			Color ambient_color_;
			Color diffuse_color_;
			Color specular_color_;

			real attenuation_constant_ = 1.0_r;
			real attenuation_linear_ = 0.0_r;
			real attenuation_quadratic_ = 0.0_r;

			real cutoff_ = light::detail::angle_to_cutoff(light::detail::default_cutoff_angle);
			real outer_cutoff_ = light::detail::angle_to_cutoff(light::detail::default_outer_cutoff_angle);

			bool cast_shadows_ = true;

		public:

			//Construct a new light with the given visibility
			explicit Light(bool visible = true) noexcept;

			//Construct a new light with the given values
			Light(light::LightType type,
				const Vector3 &position, const Vector3 &direction,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle,
				bool cast_shadows = true, bool visible = true) noexcept;


			/*
				Static light conversions
			*/

			//Returns a new point light from the given values
			[[nodiscard]] static Light Point(const Vector3 &position,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				bool cast_shadows = true) noexcept;

			//Returns a new directional light from the given values
			[[nodiscard]] static Light Directional(const Vector3 &direction,
				const Color &ambient, const Color &diffuse, const Color &specular,
				bool cast_shadows = true) noexcept;

			//Returns a new spotlight from the given values
			[[nodiscard]] static Light Spotlight(const Vector3 &position, const Vector3 &direction,
				const Color &ambient, const Color &diffuse, const Color &specular,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				real cutoff_angle, real outer_cutoff_angle,
				bool cast_shadows = true) noexcept;


			/*
				Modifiers
			*/

			//Sets the type of light given off by this light source to the given type
			inline void Type(light::LightType type) noexcept
			{
				type_ = type;
			}

			//Sets the position of the light to the given position
			//This value only applies for lights of type point
			inline void Position(const Vector3 &position) noexcept
			{
				position_ = position;
			}

			//Sets the direction of the light to the given direction
			//This value only applies for lights of type directional
			inline void Direction(const Vector3 &direction) noexcept
			{
				direction_ = direction;
			}


			//Sets the color of the ambient light given off by this light source to the given color
			inline void AmbientColor(const Color &ambient) noexcept
			{
				ambient_color_ = ambient;
			}
			
			//Sets the color of the diffuse light given off by this light source to the given color
			inline void DiffuseColor(const Color &diffuse) noexcept
			{
				diffuse_color_ = diffuse;
			}
			
			//Sets the color of the specular light given off by this light source to the given color
			inline void SpecularColor(const Color &specular) noexcept
			{
				specular_color_ = specular;
			}


			//Sets the attenuation to the given values
			inline void Attenuation(real constant, real linear, real quadratic) noexcept
			{
				attenuation_constant_ = constant;
				attenuation_constant_ = linear;
				attenuation_constant_ = quadratic;
			}

			//Sets the inner and outer cutoff values of the light to the given angles (radians)
			//These values only applies for lights of type spotlight
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


			/*
				Observers
			*/

			//Returns the type of light given off by this light source 
			[[nodiscard]] inline auto Type() const noexcept
			{
				return type_;
			}

			//Returns the position of the light
			//This value only applies for lights of type point
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the direction of the light
			//This value only applies for lights of type directional
			[[nodiscard]] inline auto& Direction() const noexcept
			{
				return direction_;
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
			[[nodiscard]] inline auto Attenuation() const noexcept
			{
				return std::tuple{attenuation_constant_, attenuation_linear_, attenuation_quadratic_};
			}

			//Returns the inner and outer cutoff angle (radians) of the light
			//These values only applies for lights of type spotlight
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
	};
} //ion::graphics::scene

#endif