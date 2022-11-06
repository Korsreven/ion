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

//Namespace containing commonly used file system functions for manipulating files and directories
namespace ion::utilities::file
{
	/*
		Paths (multiple std::filesystem::path)
		Engine default
	*/

	using Paths = std::vector<std::filesystem::path>;


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


	/*
		Create directories
	*/

	//Creates the directory at the given path, the parent directory must already exist
	bool CreateDirectory(const std::filesystem::path &directory_path) noexcept;

	//Creates all directories at the given path
	bool CreateDirectories(const std::filesystem::path &directory_path) noexcept;


	/*
		Current path
	*/

	//Sets the current filesystem path, also known as working directory
	//The given path must be absolute
	bool CurrentPath(const std::filesystem::path &absolute_path) noexcept;

	//Returns the current filesystem path, also known as working directory
	//The returned path is absolute
	[[nodiscard]] std::optional<std::filesystem::path> CurrentPath() noexcept;


	/*
		Entries in directory
	*/

	//Returns all entries in a directory
	//If entries listing is recursive, all sub directories are included
	[[nodiscard]] Paths EntriesIn(const std::filesystem::path &directory_path,
		DirectoryIteration directory_iteration = DirectoryIteration::NonRecursive);

	//Returns all directories in a directory
	//If entries listing is recursive, all sub directories are included
	[[nodiscard]] Paths DirectoriesIn(const std::filesystem::path &directory_path,
		DirectoryIteration directory_iteration = DirectoryIteration::NonRecursive);

	//Returns all files in a directory
	//If entries listing is recursive, all sub directories are included
	[[nodiscard]] Paths FilesIn(const std::filesystem::path &directory_path,
		DirectoryIteration directory_iteration = DirectoryIteration::NonRecursive);


	/*
		Existing
		Directory or file
	*/

	//Checks if the current path exists, as either a directory or a file
	[[nodiscard]] bool Exists(const std::filesystem::path &path) noexcept;

	//Checks if the current path exists, as a directory
	[[nodiscard]] bool IsDirectory(const std::filesystem::path &directory_path) noexcept;

	//Checks if the current path exists, as a file
	[[nodiscard]] bool IsFile(const std::filesystem::path &file_path) noexcept;


	/*
		Copying
		Directory or file
	*/
		
	//Copies directory or file from one path to another
	//Destination directory or file can have different name (rename)
	bool Copy(const std::filesystem::path &path, const std::filesystem::path &new_path) noexcept;
		

	/*
		Removing
		Directory or file
	*/

	//Removes an empty directory or file
	bool Remove(const std::filesystem::path &path) noexcept;

	//Removes a directory/file, and all sub directories/files
	bool RemoveAll(const std::filesystem::path &path) noexcept;


	/*
		Renaming/moving
		Directory or file
	*/

	//Renames directory or file to another name
	//Use move instead (for clarity), if you want to move the directory or file
	bool Rename(const std::filesystem::path &path, const std::filesystem::path &new_path) noexcept;

	//Moves directory or file from one path to another
	//Destination directory or file can have different name (rename)
	bool Move(const std::filesystem::path &path, const std::filesystem::path &new_path) noexcept;


	/*
		Load from file
	*/

	//Opens a file at the given file path and load its content to a string
	//Default file mode is plain
	bool Load(const std::filesystem::path &file_path, std::string &data,
		FileLoadMode file_mode = FileLoadMode::Text);

	//Opens a file at the given file path and load each line
	//Each line from the file is stored as a new string
	//Default file mode is plain
	bool Load(const std::filesystem::path &file_path, Strings &data,
		FileLoadMode file_mode = FileLoadMode::Text);


	/*
		Load from part of file
	*/

	//Opens part of a file at the given file path and load its content to a string
	//Default file mode is plain
	bool LoadPartOf(const std::filesystem::path &file_path, std::string &data,
		std::streamoff position, std::streamsize count,
		FileLoadMode file_mode = FileLoadMode::Text);

	//Opens part of a file at the given file path and load each line
	//Each line from the file is stored as a new string
	//Default file mode is plain
	bool LoadPartOf(const std::filesystem::path &file_path, Strings &data,
		std::streamoff position, std::streamsize count,
		FileLoadMode file_mode = FileLoadMode::Text);


	/*
		Save to file
	*/
		
	//Opens a file at the given file path and save the given data
	//Default file mode is plain
	bool Save(const std::filesystem::path &file_path, std::string_view data,
		FileSaveMode file_mode = FileSaveMode::Text) noexcept;

	//Opens a file at the given file path and save each string
	//Each string from the given data is stored as a new line
	//Default file mode is plain
	bool Save(const std::filesystem::path &file_path, const Strings &data,
		FileSaveMode file_mode = FileSaveMode::Text) noexcept;


	/*
		Size of file
	*/

	//Returns the size of the file with the given file path
	std::optional<std::uintmax_t> Size(const std::filesystem::path &file_path) noexcept;


	inline namespace literals
	{
		/*
			User defined literals (UDLs)
			For file size conversions to bytes
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
	} //literals
} //ion::utilities::file

#endif