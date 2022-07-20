/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
File:	IonShaderRepository.h
-------------------------------------------
*/

#ifndef ION_SHADER_REPOSITORY_H
#define ION_SHADER_REPOSITORY_H

#include <string_view>
#include "IonFileRepository.h"

namespace ion::assets::repositories
{
	using namespace std::string_view_literals;

	namespace shader_repository::detail
	{
		constexpr auto file_extensions = ".frag/.fs/.glsl/.glslf/.glslv/.vert/.vs"sv;
	} //shader_repository::detail


	struct ShaderRepository : FileRepository
	{
		//Default construct a shader repository
		//Naming convention is set to file path
		ShaderRepository() noexcept;

		//Construct a shader repository with the given naming convention
		ShaderRepository(file_repository::NamingConvention naming_convention) noexcept;
	};
} //ion::assets::repositories

#endif