/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTexture.h
-------------------------------------------
*/

#ifndef ION_TEXTURE_H
#define ION_TEXTURE_H

#include "resources/files/IonFileResource.h"

namespace ion::graphics::textures
{
	class TextureManger;

	namespace texture::detail
	{

	} //texture::detail


	class Texture : public resources::files::FileResource<TextureManger>
	{
	};
} //ion::graphics::textures

#endif