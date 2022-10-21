/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTextureAtlas.cpp
-------------------------------------------
*/

#include "IonTextureAtlas.h"

#include <algorithm>

namespace ion::graphics::textures
{

using namespace texture_atlas;

namespace texture_atlas::detail
{
} //texture_atlas::detail


//Public

TextureAtlas::TextureAtlas(std::string name, std::string asset_name,
	int rows, int columns, int sub_textures, AtlasSubTextureOrder sub_texture_order) noexcept :

	Texture{std::move(name), std::move(asset_name)},

	rows_{std::clamp(rows, 1, rows)},
	columns_{std::clamp(columns, 1, columns)},
	sub_textures_{std::clamp(sub_textures, 1, rows * columns)},
	sub_texture_order_{sub_texture_order}
{
	//Empty
}

TextureAtlas::TextureAtlas(std::string name, std::string asset_name,
	texture::TextureFilter min_filter, texture::TextureFilter mag_filter,
	int rows, int columns, int sub_textures, AtlasSubTextureOrder sub_texture_order) noexcept :

	Texture{std::move(name), std::move(asset_name), min_filter, mag_filter, {},
			texture::TextureWrapMode::Clamp, texture::TextureWrapMode::Clamp},

	rows_{std::clamp(rows, 1, rows)},
	columns_{std::clamp(columns, 1, columns)},
	sub_textures_{std::clamp(sub_textures, 1, rows * columns)},
	sub_texture_order_{sub_texture_order}
{
	//Empty
}

TextureAtlas::TextureAtlas(std::string name, std::string asset_name, texture::TextureFilter filter,
	int rows, int columns, int sub_textures, AtlasSubTextureOrder sub_texture_order) noexcept :

	TextureAtlas{std::move(name), std::move(asset_name), filter, filter, rows, columns, sub_textures, sub_texture_order}
{
	//Empty
}

} //ion::graphics::textures