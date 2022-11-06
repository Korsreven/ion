/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonFileUtility.h
-------------------------------------------
*/

#ifndef ION_FILE_UTILITY_H
#define ION_FILE_UTILITY_H

#include <filesystem>
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "types/IonTypes.h"

#ifdef CreateDirectory
#undef CreateDirectory
#endif

///@brief Namespace containing commonly used file system functions for manipulating files and directories
namespace ion::utilities::file
{
	/**
		@name Paths (multiple std::filesystem::path)
		Engine default
		@{
	*/

	using Paths = std::vector<std::filesystem::path>;

	///@}

	enum class DirectoryIteration : bool
	{
		NonRecursive,
		Recursive
	};

	enum class FileLoadMode : bool
	{
		Text,
		Binary
	};

	enum class FileSaveMode
	{
		Text,
		Binary,
		TextAppend,
		BinaryAppend
	};


	namespace detail
	{
		std::ifstream open_file(const std::filesystem::path &file_path, FileLoadMode file_mode,
			std::streamsize *file_size = nullptr);
		std::ofstream open_file(const std::filesystem::path &file_path, FileSaveMode file_mode);
	} //detail


	/**
		@name Create directories
		@{
	*/

	///@brief Creates the directory at the given path, the parent directory must already exist
	bool CreateDirectory(const std::filesystem::path &directory_path) noexcept;

	///@brief Creates all directories at the given path
	bool CreateDirectories(const std::filesystem::path &directory_path) noexcept;

	///@}

	/**
		@name Current path
		@{
	*/

	///@brief Sets the current filesystem path, also known as working directory
	///@details The given path must be absolute
	bool CurrentPath(const std::filesystem::path &absolute_path) noexcept;

	///@brief Returns the current filesystem path, also known as working directory
	///@details The returned path is absolute
	[[nodiscard]] std::optional<std::filesystem::path> CurrentPath() noexcept;

	///@}

	/**
		@name Entries in directory
		@{
	*/

	///@brief Returns all entries in a directory
	///@details If entries listing is recursive, all sub directories are included
	[[nodiscard]] Paths EntriesIn(const std::filesystem::path &directory_path,
		DirectoryIteration directory_iteration = DirectoryIteration::NonRecursive);

	///@brief Returns all directories in a directory
	///@details If entries listing is recursive, all sub directories are included
	[[nodiscard]] Paths DirectoriesIn(const std::filesystem::path &directory_path,
		DirectoryIteration directory_iteration = DirectoryIteration::NonRecursive);

	///@brief Returns all files in a directory
	///@details If entries listing is recursive, all sub directories are included
	[[nodiscard]] Paths FilesIn(const std::filesystem::path &directory_path,
		DirectoryIteration directory_iteration = DirectoryIteration::NonRecursive);

	///@}

	/**
		@name Existing
		Directory or file
		@{
	*/

	///@brief Checks if the current path exists, as either a directory or a file
	[[nodiscard]] bool Exists(const std::filesystem::path &path) noexcept;

	///@brief Checks if the current path exists, as a directory
	[[nodiscard]] bool IsDirectory(const std::filesystem::path &directory_path) noexcept;

	///@brief Checks if the current path exists, as a file
	[[nodiscard]] bool IsFile(const std::filesystem::path &file_path) noexcept;

	///@}

	/**
		@name Copying
		Directory or file
		@{
	*/
		
	///@brief Copies directory or file from one path to another
	///@details Destination directory or file can have different name (rename)
	bool Copy(const std::filesystem::path &path, const std::filesystem::path &new_path) noexcept;
	
	///@}

	/**
		@name Removing
		Directory or file
		@{
	*/

	///@brief Removes an empty directory or file
	bool Remove(const std::filesystem::path &path) noexcept;

	///@brief Removes a directory/file, and all sub directories/files
	bool RemoveAll(const std::filesystem::path &path) noexcept;

	///@}

	/**
		@name Renaming/moving
		Directory or file
		@{
	*/

	///@brief Renames directory or file to another name
	///@details Use move instead (for clarity), if you want to move the directory or file
	bool Rename(const std::filesystem::path &path, const std::filesystem::path &new_path) noexcept;

	///@brief Moves directory or file from one path to another
	///@details Destination directory or file can have different name (rename)
	bool Move(const std::filesystem::path &path, const std::filesystem::path &new_path) noexcept;

	///@}

	/**
		@name Load from file
		@{
	*/

	///@brief Opens a file at the given file path and load its content to a string
	///@details Default file mode is plain
	bool Load(const std::filesystem::path &file_path, std::string &data,
		FileLoadMode file_mode = FileLoadMode::Text);

	///@brief Opens a file at the given file path and load each line
	///@details Each line from the file is stored as a new string.
	///Default file mode is plain
	bool Load(const std::filesystem::path &file_path, Strings &data,
		FileLoadMode file_mode = FileLoadMode::Text);

	///@}

	/**
		@name Load from part of file
		@{
	*/

	///@brief Opens part of a file at the given file path and load its content to a string
	///@details Default file mode is plain
	bool LoadPartOf(const std::filesystem::path &file_path, std::string &data,
		std::streamoff position, std::streamsize count,
		FileLoadMode file_mode = FileLoadMode::Text);

	///@brief Opens part of a file at the given file path and load each line
	///@details Each line from the file is stored as a new string.
	///Default file mode is plain
	bool LoadPartOf(const std::filesystem::path &file_path, Strings &data,
		std::streamoff position, std::streamsize count,
		FileLoadMode file_mode = FileLoadMode::Text);

	///@}

	/**
		@name Save to file
		@{
	*/
		
	///@brief Opens a file at the given file path and save the given data
	///@details Default file mode is plain
	bool Save(const std::filesystem::path &file_path, std::string_view data,
		FileSaveMode file_mode = FileSaveMode::Text) noexcept;

	///@brief Opens a file at the given file path and save each string
	///@details Each string from the given data is stored as a new line.
	///Default file mode is plain
	bool Save(const std::filesystem::path &file_path, const Strings &data,
		FileSaveMode file_mode = FileSaveMode::Text) noexcept;

	///@}

	/**
		@name Size of file
		@{
	*/

	///@brief Returns the size of the file with the given file path
	std::optional<std::uintmax_t> Size(const std::filesystem::path &file_path) noexcept;

	///@}

	inline namespace literals
	{
		/**
			@name User defined literals (UDLs)
			For file size conversions to bytes
			@{
		*/

		constexpr auto operator""_kb(unsigned long long size) noexcept
		{
			return static_cast<int64>(size * 1024);
		}

		constexpr auto operator""_mb(unsigned long long size) noexcept
		{
			return static_cast<int64>(size * 1024 * 1024);
		}

		constexpr auto operator""_gb(unsigned long long size) noexcept
		{
			return static_cast<int64>(size * 1024 * 1024 * 1024);
		}

		constexpr auto operator""_tb(unsigned long long size) noexcept
		{
			return static_cast<int64>(size * 1024 * 1024 * 1024 * 1024);
		}

		///@}
	} //literals
} //ion::utilities::file

#endif