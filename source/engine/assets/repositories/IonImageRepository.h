/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
File:	IonImageRepository.h
-------------------------------------------
*/

#ifndef ION_IMAGE_REPOSITORY_H
#define ION_IMAGE_REPOSITORY_H

#include <string_view>
#include "IonFileRepository.h"

namespace ion::assets::repositories
{
	using namespace std::string_view_literals;

	namespace image_repository::detail
	{
		constexpr auto file_extensions = ".bmp/.cut/.dib/.dds/.exr/.g3/.gif/.hdr/.icb/.ico/.iff/.jbig/.jng/.jpg/.jpeg/.jpe/.jif/.koala/.mng/.pcx/.pbm/"
										 ".pgm/.ppm/.pfm/.png/.pict/.psd/.pdd/.raw/.ras/.rle/.sgi/.tdi/.tga/.tif/.tiff/.vda/.vst/.wbmp/.xbm/.xpm"sv;
	} //image_repository::detail


	///@brief A repository class for categorizing and storing all supported image files
	struct ImageRepository : FileRepository
	{
		///@brief Default construct an image repository
		///@details Naming convention is set to file path
		ImageRepository() noexcept;

		///@brief Constructs an image repository with the given naming convention
		ImageRepository(file_repository::NamingConvention naming_convention) noexcept;
	};
} //ion::assets::repositories

#endif