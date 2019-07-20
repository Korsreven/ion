/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources/files
File:	IonFileResource.h
-------------------------------------------
*/

#ifndef _ION_FILE_RESOURCE_
#define _ION_FILE_RESOURCE_

#include "resources/IonResource.h"
#include "repositories/IonFileRepository.h"

namespace ion::resources::files
{

template <typename T>
class FileResource : public Resource<T>
{
	protected:

		repositories::file_repository::FileEntry &file_;

	public:

		//Constructor
		FileResource(repositories::file_repository::FileEntry &file) :
			file_{file}
		{
			//Empty
		}


		/*
			Observers
		*/

		//Returns a mutable file entry for this resource
		[[nodiscard]] inline auto& File() noexcept
		{
			return file_;
		}

		//Returns an immutable file entry for this resource
		[[nodiscard]] inline const auto& File() const noexcept
		{
			return file_;
		}
};

} //ion::resources::files

#endif