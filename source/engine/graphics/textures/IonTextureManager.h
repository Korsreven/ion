/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTextureManager.h
-------------------------------------------
*/

#ifndef ION_TEXTURE_MANAGER_H
#define ION_TEXTURE_MANAGER_H

#include "IonTexture.h"
#include "resources/IonResourceManager.h"

namespace ion::graphics::textures
{
	namespace texture_manager::detail
	{

	} //texture_manager::detail


	class TextureManger : public resources::ResourceManager<TextureManger, Texture>
	{
	};
} //ion::graphics::textures

#endif