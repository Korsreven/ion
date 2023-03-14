/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
files:	IonFileRepository.h
-------------------------------------------
*/

#ifndef ION_FILE_REPOSITORY_H
#define ION_FILE_REPOSITORY_H

#include <algorithm>
#include <filesystem>
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "adaptors/IonFlatMap.h"
#include "adaptors/IonFlatSet.h"
#include "types/IonTypes.h"
#include "utilities/IonFileUtility.h"

#ifdef RemoveDirectory
#undef RemoveDirectory
#endif

namespace ion::assets::repositories
{
	using namespace std::string_view_literals;

	namespace file_repository
	{
		enum class NamingConvention : bool
		{
			FilePath,
			FileName
		};

		struct FileLocation final
		{
			std::filesystem::path Path;
			std::streamoff Position = 0;
			std::streamsize Count = 0;
		};

		struct FileEntry final
		{
			std::filesystem::path Path;
			std::optional<FileLocation> DataFile;
		};


		namespace detail
		{
			constexpr auto file_extensions_delimiter = "/"sv;


			template <typename T>
			inline auto filter_by_file_extensions(utilities::file::Paths file_paths, const T &file_extensions) noexcept
			{
				//Something to filter on
				if (!std::empty(file_extensions))
					//Erase all files paths that does not match any files extensions
					file_paths.erase(
						std::remove_if(std::begin(file_paths), std::end(file_paths),
							[&](const auto &file_path) noexcept
							{
								return !std::binary_search(
									std::begin(file_extensions), std::end(file_extensions),
									file_path.extension().generic_string());
							}),
						std::end(file_paths));

				return file_paths;
			}

			template <typename T>
			inline auto filter_by_file_extensions(std::vector<FileEntry> files, const T &file_extensions) noexcept
			{
				//Something to filter on
				if (!std::empty(file_extensions))
					//Erase all files that does not match any file extensions
					files.erase(
						std::remove_if(std::begin(files), std::end(files),
							[&](const auto &file) noexcept
							{
								return !std::binary_search(
									std::begin(file_extensions), std::end(file_extensions),
									file.Path.extension().generic_string());
							}),
						std::end(files));

				return files;
			}

			utilities::file::Paths filter_by_file_extension(utilities::file::Paths file_paths, std::string_view file_extension) noexcept;
			std::vector<FileEntry> filter_by_file_extension(std::vector<FileEntry> files, std::string_view file_extension) noexcept;

			std::string file_path_to_name(const std::filesystem::path &file_path, NamingConvention naming_convention) noexcept;
		} //detail
	} //file_repository


	///@brief A repository base class for storing physical files, files within a data file and supported file extensions
	class FileRepository
	{
		private:

			file_repository::NamingConvention naming_convention_ = file_repository::NamingConvention::FileName;

			adaptors::FlatMap<std::string, file_repository::FileEntry> files_;
			adaptors::FlatSet<std::string> file_extensions_;

		public:

			///@brief Default construct a repository
			///@details Naming convention is set to file path
			FileRepository() = default;

			///@brief Constructs a repository with the given naming convention
			FileRepository(file_repository::NamingConvention naming_convention) noexcept;

			///@brief Constructs a repository with the given file extensions
			///@details Naming convention is set to file path
			FileRepository(Strings file_extensions) noexcept;

			///@brief Constructs a repository with the given naming convention and file extensions
			FileRepository(file_repository::NamingConvention naming_convention, Strings file_extensions) noexcept;


			/**
				@name Files
				@{
			*/

			///@brief Returns the file entry that is stored in this repository with the given name
			[[nodiscard]] std::optional<file_repository::FileEntry> File(std::string_view name) const noexcept;

			///@brief Returns the file data that is stored in this repository with the given name
			[[nodiscard]] std::optional<std::string> FileData(std::string_view name) const noexcept;

			///@brief Returns the file path that is stored in this repository with the given name
			[[nodiscard]] std::optional<std::filesystem::path> FilePath(std::string_view name) const noexcept;

			///@}

			/**
				@name File extensions
				@{
			*/

			///@brief Returns whether the given file extension was found in this repository
			[[nodiscard]] bool HasFileExtension(std::string_view file_extension) const noexcept;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all files in this repository
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Files() noexcept
			{
				return files_.Elements();
			}

			///@brief Returns an immutable range of all files in this repository
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Files() const noexcept
			{
				return files_.Elements();
			}

			///@brief Returns a mutable range of all files in this repository
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FileExtensions() noexcept
			{
				return file_extensions_.Elements();
			}

			///@brief Returns an immutable range of all files in this repository
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FileExtensions() const noexcept
			{
				return file_extensions_.Elements();
			}

			///@}

			/**
				@name Adding directories
				@{
			*/

			///@brief Adds all files from the given directory path
			///@details Files are added either recursively or non-recursively from the path.
			///Files are filtered against added file extensions
			bool AddDirectory(const std::filesystem::path &directory_path,
				utilities::file::DirectoryIteration directory_iteration = utilities::file::DirectoryIteration::NonRecursive);

			///@brief Adds all files from each given directory paths
			///@details Files are added either recursively or non-recursively from each path.
			///Files are filtered against added file extensions
			bool AddDirectories(const utilities::file::Paths &directory_paths,
				utilities::file::DirectoryIteration directory_iteration = utilities::file::DirectoryIteration::NonRecursive);

			///@}
			
			/**
				@name Adding files
				@{
			*/
			
			///@brief Adds a file from the given file path
			///@details File is not filtered against added file extensions
			bool AddFile(std::filesystem::path file_path);

			///@brief Adds a file from the given file entry
			///@details File is not filtered against added file extensions
			bool AddFile(file_repository::FileEntry file);

			///@brief Adds a file from each of the given file paths
			///@details Files are not filtered against added file extensions
			bool AddFiles(utilities::file::Paths file_paths);

			///@brief Adds a file from each of the given file entries
			///@details Files are not filtered against added file extensions
			bool AddFiles(std::vector<file_repository::FileEntry> files);

			///@}

			/**
				@name Adding file extensions
				@{
			*/
			
			///@brief Adds the given file extension
			///@details Remember that all file extensions should be prefixed with a dot
			bool AddFileExtension(std::string file_extension);

			///@brief Adds all given file extensions
			///@details Remember that all file extensions should be prefixed with a dot
			bool AddFileExtensions(Strings file_extensions);

			///@}

			/**
				@name Clearing
				@{
			*/

			///@brief Removes all files and file extensions
			void Clear() noexcept;

			///@brief Removes all files
			void ClearFiles() noexcept;

			///@brief Removes all file extensions
			void ClearFileExtensions() noexcept;

			///@}

			/**
				@name Removing directories
				@{
			*/

			///@brief Removes all files that are contained inside the given directory path
			///@details Files are removed either recursively or non-recursively from the path
			bool RemoveDirectory(const std::filesystem::path &directory_path,
				utilities::file::DirectoryIteration directory_iteration = utilities::file::DirectoryIteration::NonRecursive) noexcept;

			///@brief Removes all files that are contained inside each given directory paths
			///@details Files are removed either recursively or non-recursively from the path
			bool RemoveDirectories(const utilities::file::Paths &directory_paths,
				utilities::file::DirectoryIteration directory_iteration = utilities::file::DirectoryIteration::NonRecursive) noexcept;

			///@}

			/**
				@name Removing files
				@{
			*/

			///@brief Removes a file from the given file path
			bool RemoveFile(const std::filesystem::path &file_path) noexcept;

			///@brief Removes a file from each of the given file paths
			bool RemoveFiles(const utilities::file::Paths &file_paths) noexcept;

			///@}

			/**
				@name Removing extensions
				@{
			*/

			///@brief Removes the given file extension
			bool RemoveFileExtension(std::string_view file_extension) noexcept;

			///@brief Removes all given file extensions
			bool RemoveFileExtensions(const std::vector<std::string_view> &file_extensions) noexcept;

			///@}
	};
} //ion::assets::repositories

#endif