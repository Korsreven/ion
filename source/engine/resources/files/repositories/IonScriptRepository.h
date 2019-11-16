/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources/files/repositories
File:	IonScriptRepository.h
-------------------------------------------
*/

#ifndef _ION_SCRIPT_REPOSITORY_
#define _ION_SCRIPT_REPOSITORY_

#include <string_view>
#include "IonFileRepository.h"

namespace ion::resources::files::repositories
{
	using namespace std::string_view_literals;

	namespace script_repository::detail
	{
		constexpr auto file_extensions = ".ion"sv;
	} //script_repository::detail


	struct ScriptRepository : FileRepository
	{
		//Default construct a script repository
		//Naming convention is set to file path
		ScriptRepository() noexcept;

		//Construct a script repository with the given naming convention
		ScriptRepository(file_repository::NamingConvention naming_convention) noexcept;
	};
} //ion::resources::files::repositories

#endif