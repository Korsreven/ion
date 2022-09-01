/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
File:	IonAudioRepository.h
-------------------------------------------
*/

#ifndef ION_AUDIO_REPOSITORY_H
#define ION_AUDIO_REPOSITORY_H

#include <string_view>
#include "IonFileRepository.h"

namespace ion::assets::repositories
{
	using namespace std::string_view_literals;

	namespace audio_repository::detail
	{
		constexpr auto file_extensions = ".aif/.aiff/.asf/.au/.dls/.flac/.fsb/.it/.mid/.midi/.mod/.mp2/.mp3/.ogg/.rmi/.s3m/.snd/.vag/.wav/.wma/.xm/.xma"sv;
	} //audio_repository::detail


	//A repository class for categorizing and storing all supported audio files
	struct AudioRepository : FileRepository
	{
		//Default construct an audio repository
		//Naming convention is set to file path
		AudioRepository() noexcept;

		//Construct an audio repository with the given naming convention
		AudioRepository(file_repository::NamingConvention naming_convention) noexcept;
	};
} //ion::assets::repositories

#endif