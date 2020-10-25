/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
File:	IonFontRepository.cpp
-------------------------------------------
*/

#include "IonFontRepository.h"

#include "utilities/IonStringUtility.h"

namespace ion::assets::repositories
{

using namespace font_repository;


FontRepository::FontRepository() noexcept :
	FileRepository{utilities::string::Split(detail::file_extensions, file_repository::detail::file_extensions_delimiter)}
{
	//Empty
}

FontRepository::FontRepository(file_repository::NamingConvention naming_convention) noexcept :
	FileRepository{naming_convention, utilities::string::Split(detail::file_extensions, file_repository::detail::file_extensions_delimiter)}
{
	//Empty
}

} //ion::assets::repositories