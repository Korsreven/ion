/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
File:	IonVideoRepository.cpp
-------------------------------------------
*/

#include "IonVideoRepository.h"

#include "utilities/IonStringUtility.h"

namespace ion::assets::repositories
{

using namespace video_repository;


VideoRepository::VideoRepository() noexcept :
	FileRepository{utilities::string::Split(detail::file_extensions, file_repository::detail::file_extensions_delimiter)}
{
	//Empty
}

VideoRepository::VideoRepository(file_repository::NamingConvention naming_convention) noexcept :
	FileRepository{naming_convention, utilities::string::Split(detail::file_extensions, file_repository::detail::file_extensions_delimiter)}
{
	//Empty
}

} //ion::assets::repositories