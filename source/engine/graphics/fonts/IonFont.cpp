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

Font::Font(std::string name, std::string asset_name, int size, int face_index, FontCharacterSet character_set,
	FontGlyphFilter min_filter, FontGlyphFilter mag_filter) :

	FileResource{std::move(name), std::move(asset_name)},

	size_{size},
	face_index_{face_index},
	character_set_{character_set},

	glyph_min_filter_{min_filter},
	glyph_mag_filter_{mag_filter}
{
	//Empty
}

Font::Font(std::string name, std::string asset_name, int size, FontCharacterSet character_set,
	FontGlyphFilter min_filter, FontGlyphFilter mag_filter) :

	Font{std::move(name), std::move(asset_name), size, 0, character_set, min_filter, mag_filter}
{
	//Empty
}

Font::Font(std::string name, std::string asset_name, int size, FontCharacterSet character_set,
	FontGlyphFilter filter) :

	Font{std::move(name), std::move(asset_name), size, character_set, filter, filter}
{
	//Empty
}

Font::Font(std::string name, std::string asset_name, int size, FontCharacterSet character_set) :

	FileResource{std::move(name), std::move(asset_name)},

	size_{size},
	character_set_{character_set}
{
	//Empty
}

Font::Font(std::string name, std::string asset_name, int size) :

	FileResource{std::move(name), std::move(asset_name)},
	size_{size}
{
	//Empty
}

} //ion::graphics::fonts