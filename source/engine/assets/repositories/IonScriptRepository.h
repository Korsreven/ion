/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
File:	IonScriptRepository.h
-------------------------------------------
*/

#ifndef ION_SCRIPT_REPOSITORY_H
#define ION_SCRIPT_REPOSITORY_H

#include <string_view>
#include "IonFileRepository.h"

namespace ion::assets::repositories
{
	using namespace std::string_view_literals;

	namespace script_repository::detail
	{
		constexpr auto file_extensions = ".ion"sv;
	} //script_repository::detail


	///@brief A repository class for categorizing and storing all supported script files
	struct ScriptRepository : FileRepository
	{
		///@brief Default construct a script repository
		///@details Naming convention is set to file path
		ScriptRepository() noexcept;

		///@brief Constructs a script repository with the given naming convention
		ScriptRepository(file_repository::NamingConvention naming_convention) noexcept;
	};
} //ion::assets::repositories

#endif