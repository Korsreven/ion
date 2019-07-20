/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonFileUtility.cpp
-------------------------------------------
*/

#include "IonFileUtility.h"

#include <fstream>
#include <system_error>

namespace ion::utilities::file
{

namespace detail
{

std::ifstream open_file(const std::filesystem::path &file_path, FileLoadMode file_mode,
	std::streamsize *file_size)
{
	int mode = std::ios::in;

	if (file_mode == FileLoadMode::Binary)
	{
		mode |= std::ios::binary;

		if (file_size)
			mode |= std::ios::ate;
	}

	std::ifstream file(file_path, mode);

	if (file_size && file)
	{
		if (file_mode == FileLoadMode::Binary)
		{
			*file_size = file.tellg();
			file.seekg(0, std::ios::beg); //Rewind
		}
		else
		{
			file.ignore(std::numeric_limits<std::streamsize>::max());
			*file_size = file.gcount();
			file.clear(); //Since ignore will have set eof
			file.seekg(0, std::ios::beg); //Rewind
		}
	}

	return file;
}

std::ofstream open_file(const std::filesystem::path &file_path, FileSaveMode file_mode)
{
	int mode = std::ios::out;

	switch (file_mode)
	{
		case FileSaveMode::BinaryAppend:
		mode |= std::ios::app;
		[[fallthrough]];

		case FileSaveMode::Binary:
		mode |= std::ios::binary;
		break;

		case FileSaveMode::TextAppend:
		mode |= std::ios::app;
		break;
	}

	return std::ofstream(file_path, mode);
}

} //detail


/*
	Create directories
*/

bool CreateDirectory(const std::filesystem::path &directory_path) noexcept
{
	std::error_code error;
	auto result = std::filesystem::create_directory(directory_path, error);
	return !error && result;
}

bool CreateDirectories(const std::filesystem::path &directory_path) noexcept
{
	std::error_code error;
	auto result = std::filesystem::create_directories(directory_path, error);
	return !error && result;
}


/*
	Current path
*/

bool CurrentPath(const std::filesystem::path &absolute_path) noexcept
{
	std::error_code error;
	std::filesystem::current_path(absolute_path, error);
	return !error;
}

std::optional<std::filesystem::path> CurrentPath() noexcept
{
	std::error_code error;
	std::filesystem::path absolute_path = std::filesystem::current_path(error);
	return !error ?
		std::make_optional<std::filesystem::path>(std::move(absolute_path)) :
		std::nullopt;
}


/*
	Entries in directory
*/

Paths EntriesIn(const std::filesystem::path &directory_path,
	DirectoryIteration directory_iteration)
{
	Paths paths;

	if (directory_iteration == DirectoryIteration::Recursive)
	{
		for (auto &&path : std::filesystem::recursive_directory_iterator(directory_path))
			paths.push_back(path);
	}
	else
	{
		for (auto &&path : std::filesystem::directory_iterator(directory_path))
			paths.push_back(path);
	}

	return paths;
}

Paths DirectoriesIn(const std::filesystem::path &directory_path,
	DirectoryIteration directory_iteration)
{
	Paths paths;

	if (directory_iteration == DirectoryIteration::Recursive)
	{
		for (auto &&path : std::filesystem::recursive_directory_iterator(directory_path))
		{
			if (IsDirectory(path))
				paths.push_back(path);
		}
	}
	else
	{
		for (auto &&path : std::filesystem::directory_iterator(directory_path))
		{
			if (IsDirectory(path))
				paths.push_back(path);
		}
	}

	return paths;
}

Paths FilesIn(const std::filesystem::path &directory_path,
	DirectoryIteration directory_iteration)
{
	Paths paths;

	if (directory_iteration == DirectoryIteration::Recursive)
	{
		for (auto &&path : std::filesystem::recursive_directory_iterator(directory_path))
		{
			if (IsFile(path))
				paths.push_back(path);
		}
	}
	else
	{
		for (auto &&path : std::filesystem::directory_iterator(directory_path))
		{
			if (IsFile(path))
				paths.push_back(path);
		}
	}

	return paths;
}


/*
	Existing
	Directory or file
*/

bool Exists(const std::filesystem::path &path) noexcept
{
	std::error_code error;
	auto result = std::filesystem::exists(path, error);
	return !error && result;
}

bool IsDirectory(const std::filesystem::path &directory_path) noexcept
{
	std::error_code error;
	auto result = std::filesystem::is_directory(directory_path, error);
	return !error && result;
}

bool IsFile(const std::filesystem::path &file_path) noexcept
{
	std::error_code error;
	auto result = std::filesystem::is_regular_file(file_path, error);
	return !error && result;
}


/*
	Copying
	Directory or file
*/
		
bool Copy(const std::filesystem::path &path, const std::filesystem::path &new_path) noexcept
{
	std::error_code error;
	std::filesystem::copy(path, new_path, error);
	return !error;
}


/*
	Removing
	Directory or file
*/

bool Remove(const std::filesystem::path &path) noexcept
{
	std::error_code error;
	auto result = std::filesystem::remove(path, error);
	return !error && result;
}

bool RemoveAll(const std::filesystem::path &path) noexcept
{
	std::error_code error;
	auto result = std::filesystem::remove_all(path, error);
	return !error && result != static_cast<decltype(result)>(-1);
}


/*
	Renaming and moving
	Directory or file
*/

bool Rename(const std::filesystem::path &path, const std::filesystem::path &new_path) noexcept
{
	std::error_code error;
	std::filesystem::rename(path, new_path, error);
	return !error;
}

bool Move(const std::filesystem::path &path, const std::filesystem::path &new_path) noexcept
{
	return Rename(path, new_path);
}


/*
	Load from file
*/

bool Load(const std::filesystem::path &file_path, std::string &data,
	FileLoadMode file_mode)
{
	std::streamsize file_size;
	auto file = detail::open_file(file_path, file_mode, &file_size);
	
	if (file)
	{
		auto data_size = data.size();
		data.append(static_cast<size_t>(file_size), '\0');

		file.read(&data[data_size], file_size); //Read bytes
		return true;
	}

	return false;
}

bool Load(const std::filesystem::path &file_path, Strings &data,
	FileLoadMode file_mode)
{
	auto file = detail::open_file(file_path, file_mode);

	if (file)
	{
		for (std::string str; std::getline(file, str);)
			data.push_back(str);

		return true;
	}

	return false;
}


/*
	Load from part of file
*/

bool LoadPartOf(const std::filesystem::path &file_path, std::string &data,
	std::streamoff position, std::streamsize count,
	FileLoadMode file_mode)
{
	std::streamsize file_size;
	auto file = detail::open_file(file_path, file_mode, &file_size);

	if (file)
	{
		if (position < file_size)
		{
			count = std::min(file_size - position, static_cast<std::streamsize>(count));
			file.seekg(position, std::ios::beg); //Seek position

			auto data_size = data.size();
			data.append(static_cast<size_t>(count), '\0');

			file.read(&data[data_size], count); //Read bytes
		}

		return true;
	}

	return false;
}

bool LoadPartOf(const std::filesystem::path &file_path, Strings &data,
	std::streamoff position, std::streamsize count,
	FileLoadMode file_mode)
{
	std::streamsize file_size;
	auto file = detail::open_file(file_path, file_mode, &file_size);

	if (file)
	{
		if (position < file_size)
		{
			count = std::min(file_size - position, static_cast<std::streamsize>(count));
			file.seekg(position, std::ios::beg); //Seek position

			for (std::string str; count > 0 && std::getline(file, str); --count)
			{
				if (std::size(str) > count)
				{
					str.erase(std::begin(str) + static_cast<size_t>(count),
							  std::end(str));
					data.push_back(str);
					break;
				}
			
				data.push_back(str);
				count -= std::size(str);

				if (count == 1)
					data.emplace_back();
			}
		}

		return true;
	}

	return false;
}


/*
	Save to file
*/

bool Save(const std::filesystem::path &file_path, std::string_view data,
	FileSaveMode file_mode) noexcept
{
	auto file = detail::open_file(file_path, file_mode);
	
	if (file)
	{
		file << data;
		return true;
	}

	return false;
}

bool Save(const std::filesystem::path &file_path, const Strings &data,
	FileSaveMode file_mode) noexcept
{
	auto file = detail::open_file(file_path, file_mode);
	
	if (file)
	{
		if (!std::empty(data))
		{
			file << data.front();

			for (auto iter = std::cbegin(data) + 1,
				end = std::cend(data); iter != end; ++iter)
					file << '\n' << *iter;
		}

		return true;
	}

	return false;
}


/*
	Size of file
*/

std::optional<std::uintmax_t> Size(const std::filesystem::path &file_path) noexcept
{
	std::error_code error;
	auto result = std::filesystem::file_size(file_path, error);
	return !error ?
		std::make_optional(result) :
		std::nullopt;
}

} //ion::utilities::file