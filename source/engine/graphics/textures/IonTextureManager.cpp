/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTextureManager.cpp
-------------------------------------------
*/

#include "IonTextureManager.h"

namespace ion::graphics::textures
{

using namespace texture_manager;

namespace texture_manager::detail
{

} //texture_manager::detail


//Protected

/*
	Events
*/

bool TextureManger::LoadResource(Texture &texture) noexcept
{
	texture;
	return true;
}

bool TextureManger::UnloadResource(Texture &texture) noexcept
{
	texture;
	return true;
}


//Public

TextureManger::~TextureManger() noexcept
{
	this->UnloadAll();
		//Unload all resources before this manager is destroyed
		//Virtual functions cannot be called post destruction
}

} //ion::graphics::textures