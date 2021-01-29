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

namespace ion::graphics::scene
{

using namespace light;
using namespace utilities;

Light::Light(LightType type,
	const Vector3 &position, const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle,
	bool cast_shadows) noexcept :

	type_{type},
	position_{position},
	direction_{direction},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},

	attenuation_constant_{attenuation_constant},
	attenuation_linear_{attenuation_linear},
	attenuation_quadratic_{attenuation_quadratic},

	cutoff_{detail::angle_to_cutoff(cutoff_angle)},
	outer_cutoff_{detail::angle_to_cutoff(outer_cutoff_angle)},

	cast_shadows_{cast_shadows}
{
	//Empty
}


/*
	Static light conversions
*/

Light Light::Point(const Vector3 &position,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	bool cast_shadows) noexcept
{
	return {LightType::Point,
			position, vector3::Zero,
			ambient, diffuse, specular,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			0.0_r, 0.0_r,
			cast_shadows};
}

Light Light::Directional(const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	bool cast_shadows) noexcept
{
	return {LightType::Directional, vector3::Zero, direction,
			ambient, diffuse, specular,
			1.0_r, 0.0_r, 0.0_r,
			0.0_r, 0.0_r,
			cast_shadows};
}

Light Light::Spotlight(const Vector3 &position, const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	real cutoff_angle, real outer_cutoff_angle,
	bool cast_shadows) noexcept
{
	return {LightType::Spotlight, position, direction,
			ambient, diffuse, specular,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			cutoff_angle, outer_cutoff_angle,
			cast_shadows};
}

} //ion::graphics::scene