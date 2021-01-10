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
	const Vector3 &position, const Vector3 &direction, real cutoff_angle,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	bool cast_shadows) noexcept :

	type_{type},
	position_{position},
	direction_{direction},
	cutoff_{detail::angle_to_cutoff(cutoff_angle)},

	ambient_color_{ambient},
	diffuse_color_{diffuse},
	specular_color_{specular},

	attenuation_constant_{attenuation_constant},
	attenuation_linear_{attenuation_linear},
	attenuation_quadratic_{attenuation_quadratic},

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
			position, vector3::Zero, detail::default_cutoff_angle,
			ambient, diffuse, specular,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			cast_shadows};
}

Light Light::Directional(const Vector3 &direction,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	bool cast_shadows) noexcept
{
	return {LightType::Directional,
			vector3::Zero, direction, detail::default_cutoff_angle,
			ambient, diffuse, specular,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			cast_shadows};
}

Light Light::Spotlight(const Vector3 &position, const Vector3 &direction, real cutoff_angle,
	const Color &ambient, const Color &diffuse, const Color &specular,
	real attenuation_constant, real attenuation_linear, real attenuation_quadratic,
	bool cast_shadows) noexcept
{
	return {LightType::Spotlight,
			position, direction, cutoff_angle,
			ambient, diffuse, specular,
			attenuation_constant, attenuation_linear, attenuation_quadratic,
			cast_shadows};
}

} //ion::graphics::scene