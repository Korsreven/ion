/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets
File:	IonAssetLoader.h
-------------------------------------------
*/

#ifndef ION_ASSET_LOADER_H
#define ION_ASSET_LOADER_H

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "adaptors/ranges/IonDereferenceIterable.h"
#include "repositories/IonFileRepository.h"
#include "types/IonTypes.h"
#include "utilities/IonFileUtility.h"

#undef max

namespace ion::assets
{
	using namespace std::string_view_literals;

	namespace asset_loader
	{
		enum class FileSplitSize
		{
			Max2GB,
			Max4GB,
			Max256GB,
			Max16TB,
			Max256TB
		};


		namespace detail
		{
			using namespace utilities::file::literals;

			using container_type = std::vector<repositories::FileRepository*>; //Non-owning


			constexpr auto max_file_size_2_gb = 2_gb - 1;
			constexpr auto max_file_size_4_gb = 4_gb - 1;
			constexpr auto max_file_size_256_gb = 256_gb - 1;
			constexpr auto max_file_size_16_tb = 16_tb - 64_kb;
			constexpr auto max_file_size_256_tb = 256_tb - 64_kb;

			constexpr auto data_file_header = "ion data file"sv;
			constexpr auto data_file_delimiter = "\0"sv;
			constexpr auto data_file_footer_size = 15;


			constexpr auto max_file_split_size(FileSplitSize file_split_size) noexcept
			{
				switch (file_split_size)
				{
					case FileSplitSize::Max2GB:
					return max_file_size_2_gb;

					case FileSplitSize::Max4GB:
					return max_file_size_4_gb;

					case FileSplitSize::Max256GB:
					return max_file_size_256_gb;

					case FileSplitSize::Max16TB:
					return max_file_size_16_tb;

					default:
					return max_file_size_256_tb;
				}
			}


			bool load_data_file(const std::filesystem::path &file_path, Strings &file_entries) noexcept;

			bool save_data_file_header(const std::filesystem::path &file_path) noexcept;
			bool save_data_file_bytes(const std::filesystem::path &file_path, std::string_view file_bytes) noexcept;
			bool save_data_file_footer(const std::filesystem::path &file_path, const Strings &file_entries) noexcept;
		} //detail
	} //asset_loader


	class AssetLoader final
	{
		private:

			asset_loader::detail::container_type repositories_;
			std::string data_file_extension_ = ".dat";


			bool ExtractDataFile(const std::filesystem::path &data_file_path);
			bool ExtractDataFiles(const utilities::file::Paths &data_file_paths);

		public:

			//Default constructor
			AssetLoader() = default;

			//Deleted copy constructor
			AssetLoader(const AssetLoader&) = delete;

			//Default move constructor
			AssetLoader(AssetLoader&&) = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			AssetLoader& operator=(const AssetLoader&) = delete;

			//Default move assignment
			AssetLoader& operator=(AssetLoader&&) = default;


			/*
				Modifiers
			*/

			//Sets the data file extension to the given extension
			inline void DataFileExtension(std::string data_file_extension) noexcept
			{
				data_file_extension_ = std::move(data_file_extension);
			}


			/*
				Observers
			*/

			//Returns the data file extension for this loader
			[[nodiscard]] inline auto& DataFileExtension() const noexcept
			{
				return data_file_extension_;
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all repositories attached to this loader
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Repositories() noexcept
			{
				return adaptors::ranges::DereferenceIterable<asset_loader::detail::container_type&>{repositories_};
			}

			//Returns an immutable range of all repositories attached to this loader
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Repositories() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const asset_loader::detail::container_type&>{repositories_};
			}


			/*
				Attaching
			*/

			//Attach a repository to this loader
			bool Attach(repositories::FileRepository &file_repository);


			/*
				Clearing
			*/

			//Detach all repositories from this loader
			void Clear() noexcept;


			/*
				Compiling
			*/

			//Pull files from each attached repository and compile them together as a data file
			//If the total size of the compilation exceeds the file split size, the compilation is split into multiple data files
			//When multiple data files are created, a numerical suffix is added to the file name
			bool CompileDataFile(std::filesystem::path data_file_path,
				asset_loader::FileSplitSize file_split_size = asset_loader::FileSplitSize::Max4GB) const noexcept;


			/*
				Detaching
			*/

			//Detach a repository from this loader
			bool Detach(repositories::FileRepository &file_repository) noexcept;


			/*
				Loading directories
			*/

			//Load all files from the given directory path into the attached repositories
			//Files are added either recursively or non-recursively from the path
			//Files are added to one or more repositories with the supported file extension
			bool LoadDirectory(const std::filesystem::path &directory_path,
				utilities::file::DirectoryIteration directory_iteration = utilities::file::DirectoryIteration::NonRecursive);

			//Load all files from each given directory paths into the attached repositories
			//Files are added either recursively or non-recursively from each path
			//Files are added to one or more repositories with the supported file extension
			bool LoadDirectories(const utilities::file::Paths &directory_paths,
				utilities::file::DirectoryIteration directory_iteration = utilities::file::DirectoryIteration::NonRecursive);


			/*
				Loading files
			*/

			//Load a file with the given file path into the attached repositories
			//File is added to one or more repositories with the supported file extension
			bool LoadFile(const std::filesystem::path &file_path);

			//Load a file with the given file entry into the attached repositories
			//File is added to one or more repositories with the supported file extension
			bool LoadFile(repositories::file_repository::FileEntry file);

			//Load a file from each of the given file paths into the attached repositories
			//Each file is added to one or more repositories with the supported file extension
			bool LoadFiles(const utilities::file::Paths &file_paths);

			//Load a file from each of the given file entries into the attached repositories
			//Each file is added to one or more repositories with the supported file extension
			bool LoadFiles(const std::vector<repositories::file_repository::FileEntry> &files);
	};
} //ion::assets

#endif