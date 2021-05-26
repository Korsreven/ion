/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonFog.cpp
-------------------------------------------
*/

#include "IonFog.h"

namespace ion::graphics::render
{

using namespace fog;

namespace fog::detail
{
} //fog::detail


Fog::Fog(FogMode mode, real density, real near_distance, real far_distance, const Color &color) noexcept :

	mode_{mode},
	density_{density},
	near_distance_{near_distance},
	far_distance_{far_distance},
	color_{color}
{
	//Empty
}


/*
	Static light conversions
*/

Fog Fog::Exp(real density, real near_distance, real far_distance, const Color &color) noexcept
{
	return {FogMode::Exp, density, near_distance, far_distance, color};
}

Fog Fog::Exp2(real density, real near_distance, real far_distance, const Color &color) noexcept
{
	return {FogMode::Exp2, density, near_distance, far_distance, color};
}

Fog Fog::Linear(real near_distance, real far_distance, const Color &color) noexcept
{
	return {FogMode::Linear, 1.0_r, near_distance, far_distance, color};
}

} //ion::graphics::render