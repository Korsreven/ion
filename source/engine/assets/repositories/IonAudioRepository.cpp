/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
File:	IonAudioRepository.cpp
-------------------------------------------
*/

#include "IonAudioRepository.h"

#include "utilities/IonStringUtility.h"

namespace ion::assets::repositories
{

using namespace audio_repository;


AudioRepository::AudioRepository() noexcept :
	FileRepository{utilities::string::Split(detail::file_extensions, file_repository::detail::file_extensions_delimiter)}
{
	//Empty
}

AudioRepository::AudioRepository(file_repository::NamingConvention naming_convention) noexcept :
	FileRepository{naming_convention, utilities::string::Split(detail::file_extensions, file_repository::detail::file_extensions_delimiter)}
{
	//Empty
}

} //ion::assets::repositories