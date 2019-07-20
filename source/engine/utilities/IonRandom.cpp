/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonRandom.cpp
-------------------------------------------
*/

#include "IonRandom.h"

namespace ion::utilities::random
{

namespace detail
{

void random_generator::Initialize() noexcept
{
	Engine32();
	Engine64();
}

} //detail

} //ion::utilities::random