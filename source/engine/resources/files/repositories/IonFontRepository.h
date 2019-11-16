/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources/files/repositories
File:	IonFontRepository.h
-------------------------------------------
*/

#ifndef ION_FONT_REPOSITORY_H
#define ION_FONT_REPOSITORY_H

#include <string_view>
#include "IonFileRepository.h"

namespace ion::resources::files::repositories
{
	using namespace std::string_view_literals;

	namespace font_repository::detail
	{
		constexpr auto file_extensions = ".bdf/.cff/.fnt/.pcf/.pfr/.ttf"sv;
	} //font_repository::detail


	struct FontRepository : FileRepository
	{
		//Default construct a font repository
		//Naming convention is set to file path
		FontRepository() noexcept;

		//Construct a font repository with the given naming convention
		FontRepository(file_repository::NamingConvention naming_convention) noexcept;
	};
} //ion::resources::files::repositories

#endif