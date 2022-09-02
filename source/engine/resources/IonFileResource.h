/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources
File:	IonFileResource.h
-------------------------------------------
*/

#ifndef ION_FILE_RESOURCE_H
#define ION_FILE_RESOURCE_H

#include <filesystem>
#include <optional>
#include <string>

#include "IonResource.h"

namespace ion::resources
{
	//A class representing a file resource (usually a physical file asset)
	template <typename T>
	class FileResource : public Resource<T>
	{
		private:

			std::string asset_name_;
			std::optional<std::string> file_data_;
			std::optional<std::filesystem::path> file_path_;

		public:

			//Constructs a new file resource with the given name and asset name
			//Asset name could be a file name or an entire file path
			FileResource(std::string name, std::string asset_name) :

				Resource<T>{std::move(name)},
				asset_name_{std::move(asset_name)}
			{
				//Empty
			}


			/*
				Modifiers
			*/

			//Sets the file data of the file resource to the given data with the associated path
			inline void FileData(std::string data, std::filesystem::path path)
			{
				file_data_ = std::move(data);
				file_path_ = std::move(path);
			}

			//Resets the file data to save some memory (if not needed anymore)
			inline void ResetFileData() noexcept
			{
				file_data_.reset();
			}


			/*
				Observers
			*/

			//Returns the asset name of the file resource
			//Asset name could be a file name or an entire file path
			[[nodiscard]] inline auto& AssetName() const noexcept
			{
				return asset_name_;
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
} //ion::resources

#endif