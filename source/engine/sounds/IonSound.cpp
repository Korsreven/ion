/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSound.cpp
-------------------------------------------
*/

#include "IonSound.h"

namespace ion::sounds
{

using namespace sound;

namespace sound::detail
{
} //sound::detail


Sound::Sound(std::string name, std::string asset_name) :
	FileResource{std::move(name), std::move(asset_name)}
{
	//Empty
}

} //ion::sounds