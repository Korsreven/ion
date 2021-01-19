/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	assets/repositories
File:	IonFileRepository.cpp
-------------------------------------------
*/

#include "IonFileRepository.h"

namespace ion::assets::repositories
{

using namespace file_repository;

namespace file_repository::detail
{

utilities::file::Paths filter_by_file_extension(utilities::file::Paths file_paths, std::string_view file_extension) noexcept
{
	//Erase all file paths that does not match file extension
	file_paths.erase(
		std::remove_if(std::begin(file_paths), std::end(file_paths),
			[&](const auto &file_path) noexcept
			{
				return file_path.extension().generic_string() != file_extension;
			}),
		std::end(file_paths));

	return file_paths;
}

std::vector<FileEntry> filter_by_file_extension(std::vector<FileEntry> files, std::string_view file_extension) noexcept
{
	//Erase all files that does not match file extension
	files.erase(
		std::remove_if(std::begin(files), std::end(files),
			[&](const auto &file) noexcept
			{
				return file.Path.extension().generic_string() != file_extension;
			}),
		std::end(files));

	return files;
}

std::string file_path_to_name(const std::filesystem::path &file_path, NamingConvention naming_convention) noexcept
{
	return naming_convention == NamingConvention::FileName ?
		file_path.filename().generic_string() :
		file_path.relative_path().generic_string();
}

} //detail


FileRepository::FileRepository(NamingConvention naming_convention) noexcept :
	naming_convention_{naming_convention}
{
	//Empty
}

FileRepository::FileRepository(Strings file_extensions) noexcept :
	file_extensions_{std::move(file_extensions)}
{
	//Empty
}

FileRepository::FileRepository(file_repository::NamingConvention naming_convention, Strings file_extensions) noexcept :
	naming_convention_{naming_convention},
	file_extensions_{std::move(file_extensions)}
{
	//Empty
}


/*
	Files
*/

std::optional<FileEntry> FileRepository::File(std::string_view name) const noexcept
{
	auto iter = files_.find(name);
	return iter != std::end(files_) ?
		std::make_optional(iter->second) :
		std::nullopt;
}

std::optional<std::string> FileRepository::FileData(std::string_view name) const noexcept
{
	auto iter = files_.find(name);

	if (iter != std::end(files_))
	{
		std::string data;

		//File is stored in a data file
		if (iter->second.DataFile)
		{
			//Load file from data file [position, count)
			if (utilities::file::LoadPartOf(iter->second.DataFile->Path, data,
				iter->second.DataFile->Position, iter->second.DataFile->Count,
				ion::utilities::file::FileLoadMode::Binary))
					return data;
		}
		//Load file from path
		else if (utilities::file::Load(iter->second.Path, data, ion::utilities::file::FileLoadMode::Binary))
			return data;
	}

	return {};
}

std::optional<std::filesystem::path> FileRepository::FilePath(std::string_view name) const noexcept
{
	auto file = File(name);
	return file ?
		std::make_optional(file->Path) :
		std::nullopt;
}


/*
	File extenions
*/

bool FileRepository::HasFileExtension(std::string_view file_extension) const noexcept
{
	return file_extensions_.contains(file_extension);
}


/*
	Adding directories
*/

bool FileRepository::AddDirectory(const std::filesystem::path &directory_path,
	utilities::file::DirectoryIteration directory_iteration)
{
	return utilities::file::IsDirectory(directory_path) ?
		AddFiles(detail::filter_by_file_extensions(
				 utilities::file::FilesIn(directory_path, directory_iteration),
				 file_extensions_)) :
		false;
}

bool FileRepository::AddDirectories(const utilities::file::Paths &directory_paths,
	utilities::file::DirectoryIteration directory_iteration)
{
	if (std::empty(directory_paths))
		return false;

	utilities::file::Paths file_paths;

	for (auto &directory_path : directory_paths)
	{
		if (utilities::file::IsDirectory(directory_path))
		{
			auto files = utilities::file::FilesIn(directory_path, directory_iteration);
			file_paths.insert(
				std::end(file_paths),
				std::make_move_iterator(std::begin(files)),
				std::make_move_iterator(std::end(files)));
		}
	}
	
	return AddFiles(detail::filter_by_file_extensions(file_paths, file_extensions_));
}

			
/*
	Adding files
*/
			
bool FileRepository::AddFile(std::filesystem::path file_path)
{
	return AddFile({std::move(file_path), std::nullopt});
}

bool FileRepository::AddFile(file_repository::FileEntry file)
{
	if (file.DataFile || utilities::file::IsFile(file.Path))
	{
		auto [iter, inserted] =
			files_.emplace(detail::file_path_to_name(file.Path, naming_convention_),
						   std::move(file));
		return inserted;
	}
	else
		return false;
}

bool FileRepository::AddFiles(utilities::file::Paths file_paths)
{
	if (std::empty(file_paths))
		return false;

	decltype(files_)::container_type file_container;
	file_container.reserve(std::size(file_paths));

	for (auto &file_path : file_paths)
	{
		if (utilities::file::IsFile(file_path))
		{
			auto file = FileEntry{std::move(file_path), std::nullopt};
			file_container.emplace_back(detail::file_path_to_name(file.Path, naming_convention_),
										std::move(file));
		}
	}

	return files_.insert(file_container);
}

bool FileRepository::AddFiles(std::vector<file_repository::FileEntry> files)
{
	if (std::empty(files))
		return false;

	decltype(files_)::container_type file_container;
	file_container.reserve(std::size(files));

	for (auto &file : files)
	{
		if (file.DataFile || utilities::file::IsFile(file.Path))
			file_container.emplace_back(detail::file_path_to_name(file.Path, naming_convention_),
										std::move(file));
	}

	return files_.insert(file_container);
}


/*
	Adding file extensions
*/

bool FileRepository::AddFileExtension(std::string file_extension)
{
	auto [iter, inserted] = file_extensions_.insert(std::move(file_extension));
	return inserted;
}

bool FileRepository::AddFileExtensions(Strings file_extensions)
{
	return file_extensions_.insert(std::move(file_extensions));
}


/*
	Clearing
*/

void FileRepository::Clear() noexcept
{
	ClearFiles();
	ClearFileExtensions();
}

void FileRepository::ClearFiles() noexcept
{
	files_.clear();
	files_.shrink_to_fit();
}

void FileRepository::ClearFileExtensions() noexcept
{
	file_extensions_.clear();
	file_extensions_.shrink_to_fit();
}


/*
	Removing directories
*/

bool FileRepository::RemoveDirectory(const std::filesystem::path &directory_path,
	utilities::file::DirectoryIteration directory_iteration) noexcept
{
	if (!utilities::file::IsDirectory(directory_path))
		return false;

	auto count = std::size(files_);

	files_.erase_if(
		[&](const auto &x) noexcept
		{
			return std::filesystem::equivalent(x.second.Path.parent_path(), directory_path);
		});

	if (directory_iteration == utilities::file::DirectoryIteration::Recursive)
		RemoveDirectories(utilities::file::DirectoriesIn(directory_path, directory_iteration),
			utilities::file::DirectoryIteration::NonRecursive);

	return count > std::size(files_);
}

bool FileRepository::RemoveDirectories(const utilities::file::Paths &directory_paths,
	utilities::file::DirectoryIteration directory_iteration) noexcept
{
	auto count = std::size(files_);

	for (auto &directory_path : directory_paths)
		RemoveDirectory(directory_path, directory_iteration);

	return count > std::size(files_);
}


/*
	Removing files
*/

bool FileRepository::RemoveFile(const std::filesystem::path &file_path) noexcept
{
	return utilities::file::IsFile(file_path) ?
		files_.erase(detail::file_path_to_name(file_path, naming_convention_)) :
		false;
}

bool FileRepository::RemoveFiles(const utilities::file::Paths &file_paths) noexcept
{
	auto count = std::size(files_);

	for (auto &file_path : file_paths)
		RemoveFile(file_path);

	return count > std::size(files_);
}


/*
	Removing extensions
*/

bool FileRepository::RemoveFileExtension(std::string_view file_extension) noexcept
{
	return file_extensions_.erase(file_extension);
}

bool FileRepository::RemoveFileExtensions(const std::vector<std::string_view> &file_extensions) noexcept
{
	auto count = std::size(file_extensions_);

	for (auto &file_extension : file_extensions)
		RemoveFileExtension(file_extension);

	return count > std::size(file_extensions_);
}

} //ion::assets::repositories