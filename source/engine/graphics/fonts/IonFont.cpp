/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonFont.cpp
-------------------------------------------
*/

#include "IonFont.h"

namespace ion::graphics::fonts
{

using namespace font;

namespace font::detail
{
} //font::detail


//Public

Font::Font(std::string name, std::string asset_name, int size, int face_index,
	int character_spacing, FontCharacterSet character_set, FontGlyphFilter min_filter, FontGlyphFilter mag_filter) noexcept :

	FileResource{std::move(name), std::move(asset_name)},

	size_{size},
	face_index_{face_index},
	character_spacing_{character_spacing},
	character_set_{character_set},

	glyph_min_filter_{min_filter},
	glyph_mag_filter_{mag_filter}
{
	//Empty
}

Font::Font(std::string name, std::string asset_name, int size,
	int character_spacing, FontCharacterSet character_set, FontGlyphFilter min_filter, FontGlyphFilter mag_filter) noexcept :

	Font{std::move(name), std::move(asset_name), size, 0, character_spacing, character_set, min_filter, mag_filter}
{
	//Empty
}

Font::Font(std::string name, std::string asset_name, int size,
	int character_spacing, FontCharacterSet character_set, FontGlyphFilter filter) noexcept :

	Font{std::move(name), std::move(asset_name), size, character_spacing, character_set, filter, filter}
{
	//Empty
}

Font::Font(std::string name, std::string asset_name, int size,
	int character_spacing, FontCharacterSet character_set) noexcept :

	FileResource{std::move(name), std::move(asset_name)},

	size_{size},
	character_spacing_{character_spacing},
	character_set_{character_set}
{
	//Empty
}

Font::Font(std::string name, std::string asset_name, int size,
	int character_spacing) noexcept :

	FileResource{std::move(name), std::move(asset_name)},

	size_{size},
	character_spacing_{character_spacing}
{
	//Empty
}

} //ion::graphics::fonts