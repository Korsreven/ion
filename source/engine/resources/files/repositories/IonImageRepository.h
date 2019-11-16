/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources/files/repositories
File:	IonImageRepository.h
-------------------------------------------
*/

#ifndef ION_IMAGE_REPOSITORY_H
#define ION_IMAGE_REPOSITORY_H

#include <string_view>
#include "IonFileRepository.h"

namespace ion::resources::files::repositories
{
	using namespace std::string_view_literals;

	namespace image_repository::detail
	{
		constexpr auto file_extensions = ".bmp/.cut/.dib/.dds/.exr/.g3/.gif/.hdr/.icb/.ico/.iff/.jbig/.jng/.jpg/.jpeg/.jpe/.jif/.koala/.mng/.pcx/.pbm/"
										 ".pgm/.ppm/.pfm/.png/.pict/.psd/.pdd/.raw/.ras/.rle/.sgi/.tdi/.tga/.tif/.tiff/.vda/.vst/.wbmp/.xbm/.xpm"sv;
	} //image_repository::detail


	struct ImageRepository : FileRepository
	{
		//Default construct an image repository
		//Naming convention is set to file path
		ImageRepository() noexcept;

		//Construct an image repository with the given naming convention
		ImageRepository(file_repository::NamingConvention naming_convention) noexcept;
	};
} //ion::resources::files::repositories

#endif