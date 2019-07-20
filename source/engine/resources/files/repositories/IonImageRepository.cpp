/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources/files/repositories
File:	IonImageRepository.cpp
-------------------------------------------
*/

#include "IonImageRepository.h"

#include "utilities/IonStringUtility.h"

namespace ion::resources::files::repositories
{

using namespace image_repository;


ImageRepository::ImageRepository() noexcept :
	FileRepository{utilities::string::Split(detail::file_extensions, file_repository::detail::file_extensions_delimiter)}
{
	//Empty
}

ImageRepository::ImageRepository(file_repository::NamingConvention naming_convention) noexcept :
	FileRepository{naming_convention, utilities::string::Split(detail::file_extensions, file_repository::detail::file_extensions_delimiter)}
{
	//Empty
}

} //ion::resources::files::repositories