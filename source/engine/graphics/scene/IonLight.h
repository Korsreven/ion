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

#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::scene
{
	class SceneManager; //Forward declaration

	using namespace ion::utilities;
	using namespace types::type_literals;

	using utilities::Color;
	using utilities::Vector2;

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
			constexpr auto default_cutoff_angle = math::Degree(45.0_r);


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


	class Light final
	{
		private:

			light::LightType type_ = light::LightType::Point;
			Vector2 position_;
			Vector2 direction_; //Todo: Should be Vector3
			real cutoff_ = light::detail::angle_to_cutoff(light::detail::default_cutoff_angle);

			Color ambient_color_;
			Color diffuse_color_;
			Color specular_color_;

			real attenuation_constant_ = 1.0_r;
			real attenuation_linear_ = 1.0_r;
			real attenuation_quadratic_ = 1.0_r;

			bool cast_shadows_ = true;

		public:

			//Default constructor
			Light() = default;

			//Constructor
			Light(light::LightType type,
				const Vector2 &position, const Vector2 &direction, real cutoff_angle,
				const Color &ambient_color, const Color &diffuse_color, const Color &specular_color,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				bool cast_shadows = true);


			/*
				Static light conversions
			*/

			//Returns a new point light from the given values
			[[nodiscard]] static Light Point(const Vector2 &position,
				const Color &ambient_color, const Color &diffuse_color, const Color &specular_color,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				bool cast_shadows = true) noexcept;

			//Returns a new directional light from the given values
			[[nodiscard]] static Light Directional(const Vector2 &direction,
				const Color &ambient_color, const Color &diffuse_color, const Color &specular_color,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
				bool cast_shadows = true) noexcept;

			//Returns a new spotlight from the given values
			[[nodiscard]] static Light Spotlight(const Vector2 &position, const Vector2 &direction, real cutoff_angle,
				const Color &ambient_color, const Color &diffuse_color, const Color &specular_color,
				real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
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
			inline void Position(const Vector2 &position) noexcept
			{
				position_ = position;
			}

			//Sets the direction of the light to the given direction
			//This value only applies for lights of type directional
			inline void Direction(const Vector2 &direction) noexcept
			{
				direction_ = direction;
			}

			//Sets the cutoff value of the light to the given angle (radians)
			//This value only applies for lights of type spotlight
			inline void Cutoff(real angle) noexcept
			{
				cutoff_ = light::detail::angle_to_cutoff(angle);
			}


			//Sets the color of the ambient light given off by this light source to the given color
			inline void AmbientColor(const Color &ambient_color) noexcept
			{
				ambient_color_ = ambient_color;
			}
			
			//Sets the color of the diffuse light given off by this light source to the given color
			inline void DiffuseColor(const Color &diffuse_color) noexcept
			{
				diffuse_color_ = diffuse_color;
			}
			
			//Sets the color of the specular light given off by this light source to the given color
			inline void SpecularColor(const Color &specular_color) noexcept
			{
				specular_color_ = specular_color;
			}


			//Sets the constant attenuation factor to the given value
			inline void AttenuationConstant(real factor) noexcept
			{
				attenuation_constant_ = factor;
			}

			//Sets the linear attenuation factor to the given value
			inline void AttenuationLinear(real factor) noexcept
			{
				attenuation_linear_ = factor;
			}

			//Sets the quadratic attenuation factor to the given value
			inline void AttenuationQuadratic(real factor) noexcept
			{
				attenuation_quadratic_ = factor;
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

			//Returns the cutoff angle (radians) of the light
			//This value only applies for lights of type spotlight
			[[nodiscard]] inline auto Cutoff() const noexcept
			{
				return light::detail::cutoff_to_angle(cutoff_);
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


			//Returns the constant attenuation factor
			[[nodiscard]] inline auto AttenuationConstant() const noexcept
			{
				return attenuation_constant_;
			}

			//Returns the linear attenuation factor
			[[nodiscard]] inline auto AttenuationLinear() const noexcept
			{
				return attenuation_linear_;
			}

			//Returns the linear quadratic factor
			[[nodiscard]] inline auto AttenuationQuadratic() const noexcept
			{
				return attenuation_quadratic_;
			}


			//Returns true if this light cast shadows
			[[nodiscard]] inline auto CastShadows() const noexcept
			{
				return cast_shadows_;
			}
	};
} //ion::graphics::scene

#endif