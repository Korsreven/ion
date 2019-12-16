/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/particles/affectors
File:	IonAffectorInterface.cpp
-------------------------------------------
*/

#include "IonAffectorInterface.h"

namespace ion::graphics::particles::affectors
{

namespace affector_interface::detail
{

} //affector_interface::detail


/*
	Removing
*/

void AffectorInterface::ClearAffectors() noexcept
{
	affectors_.clear();
	affectors_.shrink_to_fit();
}

bool AffectorInterface::RemoveAffector(Affector &affector) noexcept
{
	auto iter =
		std::find_if(std::begin(affectors_), std::end(affectors_),
			[&](auto &x) noexcept
			{
				return x.get() == &affector;
			});

	//Affector found
	if (iter != std::end(affectors_))
	{
		affectors_.erase(iter);
		return true;
	}
	else
		return false;
}

} //ion::graphics::particles::affectors