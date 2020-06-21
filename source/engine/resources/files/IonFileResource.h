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

#include <filesystem>
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
			std::optional<std::filesystem::path> file_path_;

		public:

			//Constructor
			explicit FileResource(std::string name) :
				name_{std::move(name)}
			{
				//Empty
			}


			/*
				Modifiers
			*/

			//Sets the file data of the file resource to the given data with the associated path
			inline void FileData(std::string data, std::filesystem::path path)
			{
				file_data_.emplace(std::move(data));
				file_path_.emplace(std::move(path));
			}

			//Resets the file data to save some memory (if not needed anymore)
			inline void ResetFileData() noexcept
			{
				file_data_.reset();
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
			//Returns nullopt if the file resource has not been prepared yet, or is no longer needed (fully loaded or has failed)
			[[nodiscard]] inline auto& FileData() const noexcept
			{
				return file_data_;
			}

			//Returns the file path of the file resource
			//Returns nullopt if the file resource has not been prepared yet
			[[nodiscard]] inline auto& FilePath() const noexcept
			{
				return file_path_;
			}
	};
} //ion::resources::files

#endif