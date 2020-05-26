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

#ifndef ION_FILE_RESOURCE_H
#define ION_FILE_RESOURCE_H

#include <optional>
#include <string>

#include "resources/IonResource.h"

namespace ion::resources::files
{
	template <typename T>
	class FileResource : public Resource<T>
	{
		private:

			std::string name_;
			std::optional<std::string> file_data_;

		public:

			//Constructor
			FileResource(std::string name) :
				name_{std::move(name)}
			{
				//Empty
			}


			/*
				Modifiers
			*/

			//Sets the file data of the file resource to the given data
			inline void FileData(std::optional<std::string> data)
			{
				file_data_ = std::move(data);
			}


			/*
				Observers
			*/

			//Returns the name of the file resource (could be a file name or an entire path)
			[[nodiscard]] inline auto& Name() const noexcept
			{
				return name_;
			}

			//Returns the file data of the file resource
			//Returns nullopt if the file resource has not been prepared, or that it is fully loaded
			[[nodiscard]] inline auto& FileData() const noexcept
			{
				return file_data_;
			}	
	};
} //ion::resources::files

#endif