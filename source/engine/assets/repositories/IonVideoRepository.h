/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
File:	IonVideoRepository.h
-------------------------------------------
*/

#ifndef ION_VIDEO_REPOSITORY_H
#define ION_VIDEO_REPOSITORY_H

#include <string_view>
#include "IonFileRepository.h"

namespace ion::assets::repositories
{
	using namespace std::string_view_literals;

	namespace video_repository::detail
	{
		constexpr auto file_extensions = ".3gp/.amc/.avi/.dv/.f4v/.flc/.flv/.m2v/.m4v/.mkv/.mp4/.mpg/.mpeg/.mov/.rm/.rmvb/.tg2/.wmv"sv;
	} //video_repository::detail


	//A repository class for categorizing and storing all supported video files
	struct VideoRepository : FileRepository
	{
		//Default construct a video repository
		//Naming convention is set to file path
		VideoRepository() noexcept;

		//Constructs a video repository with the given naming convention
		VideoRepository(file_repository::NamingConvention naming_convention) noexcept;
	};
} //ion::assets::repositories

#endif