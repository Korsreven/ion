/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources/files
File:	IonFileResourceLoader.cpp
-------------------------------------------
*/

#include "IonFileResourceLoader.h"

#include "utilities/IonConvert.h"
#include "utilities/IonStringUtility.h"

namespace ion::resources::files
{

using namespace file_resource_loader;
using namespace repositories;
using namespace utilities;

namespace file_resource_loader::detail
{

bool load_data_file(const std::filesystem::path &file_path, Strings &file_entries) noexcept
{
	std::string data;

	//Read data file header
	if (file::LoadPartOf(file_path, data,
			0, std::size(detail::data_file_header),
			file::FileLoadMode::Binary) &&
		//Control header content
		data == detail::data_file_header)
	{
		data.clear();

		//Read data file footer
		if (auto file_size = file::Size(file_path);
			file_size &&
			file::LoadPartOf(file_path, data,
				*file_size - detail::data_file_footer_size, detail::data_file_footer_size,
				file::FileLoadMode::Binary))
		{
			//Control footer content
			if (auto entries_size = convert::FirstPartTo<std::streamsize>(data);
				entries_size && *entries_size > 0)
			{
				data.clear();

				//Read file entries
				if (file::LoadPartOf(file_path, data,
						*file_size - detail::data_file_footer_size - *entries_size, *entries_size,
						file::FileLoadMode::Binary))
				{
					file_entries = string::Split(data, data_file_delimiter,
						string::StringSplitOptions::PreserveEmptyEntries);
					return true;
				}
			}
		}
	}

	return false;
}

bool save_data_file_header(const std::filesystem::path &file_path) noexcept
{
	return file::Save(file_path, data_file_header, file::FileSaveMode::Binary);
}

bool save_data_file_bytes(const std::filesystem::path &file_path, std::string_view file_bytes) noexcept
{
	return file::Save(file_path, file_bytes, file::FileSaveMode::BinaryAppend);
}

bool save_data_file_footer(const std::filesystem::path &file_path, const Strings &file_entries) noexcept
{
	auto file_entries_section =
		string::Join(data_file_delimiter, file_entries);

	//Add total file entries to the end of the file entries section
	file_entries_section +=
		string::PadRightCopy(
			convert::ToString(std::size(file_entries_section)),
			data_file_footer_size,
			data_file_delimiter);

	return file::Save(file_path, file_entries_section, file::FileSaveMode::BinaryAppend);
}

} //file_resource_loader::detail


//Private

bool FileResourceLoader::ExtractDataFile(const std::filesystem::path &data_file_path)
{
	Strings file_entries;
	
	if (detail::load_data_file(data_file_path, file_entries) &&
		//Make sure the size is modulo 3
		std::size(file_entries) % 3 == 0)
	{
		auto file_path = data_file_path.relative_path();
		std::vector<file_repository::FileEntry> files;
		files.reserve(std::size(file_entries) / 3);

		for (auto iter = std::cbegin(file_entries),
			end = std::cend(file_entries); iter != end; iter += 3)
				files.push_back({iter[0], file_repository::FileLocation{file_path,
					convert::FirstPartTo<std::streamoff>(iter[1]).value_or(0),
					convert::FirstPartTo<std::streamsize>(iter[2]).value_or(0)}});

		return LoadFiles(files);
	}

	return false;
}

bool FileResourceLoader::ExtractDataFiles(const file::Paths &data_file_paths)
{
	auto added = false;

	for (auto &data_file_path : data_file_paths)
		added |= ExtractDataFile(data_file_path);

	return added;
}


//Public


/*
	Attaching
*/

bool FileResourceLoader::Attach(FileRepository &file_repository)
{
	auto iter =
		std::find_if(std::begin(repositories_), std::end(repositories_),
			[&](auto &x) noexcept
			{
				return x == &file_repository;
			});

	//Repository not already added
	if (iter == std::end(repositories_))
	{
		repositories_.push_back(&file_repository);
		return true;
	}
	else
		return false;
}


/*
	Clearing
*/

void FileResourceLoader::Clear() noexcept
{
	repositories_.clear();
}


/*
	Compiling
*/

bool FileResourceLoader::CompileDataFile(std::filesystem::path data_file_path,
	file_resource_loader::FileSplitSize file_split_size) const noexcept
{
	//No repositories attached to loader
	if (std::empty(repositories_))
		return false;

	file::Paths file_paths;

	//Add all files that does not already come from a data file
	for (const auto &repository : repositories_)
	{
		for (const auto &file : repository->Files())
		{
			if (!file.second.DataFile)
				file_paths.push_back(file.second.Path);
		}
	}

	//No files to compile
	if (std::empty(file_paths))
		return false;

	//No file name in file path
	if (!data_file_path.has_filename())
		return false;

	//Save header
	if (!detail::save_data_file_header(data_file_path))
		return false;

	Strings file_entries;
	{
		auto file_name = data_file_path.filename().replace_extension().generic_string();
		auto extension = data_file_path.extension().generic_string();
		auto data_file_count = 1;
		std::string data;

		decltype(file_repository::FileLocation::Position) current_position =
			std::size(detail::data_file_header);
		decltype(file_repository::FileLocation::Count) total_count =
			std::size(detail::data_file_header);

		for (const auto &file_path : file_paths)
		{
			if (file::Load(file_path, data, file::FileLoadMode::Binary))
			{
				//Do not add zero byte files
				if (std::empty(data))
					continue;

				auto byte_count = std::size(data);
				auto path_part = file_path.generic_string();
				auto position_part = convert::ToString(current_position);
				auto count_part = convert::ToString(byte_count);

				auto count = byte_count;
				count += std::size(path_part);
				count += std::size(position_part);
				count += std::size(count_part);
				count += !std::empty(file_entries) + 2; //Delimiters

				//Split file, if too big
				if (total_count + count >
					detail::max_file_split_size(file_split_size) - detail::data_file_footer_size)
				{
					//Save footer for current data file
					if (std::empty(file_entries) ||
						!detail::save_data_file_footer(data_file_path, file_entries))
							return false;
					
					data_file_path.replace_filename(
						file_name + convert::ToString(++data_file_count) + extension);

					//Save header for new data file
					if (!detail::save_data_file_header(data_file_path))
						return false;

					file_entries.clear();
					current_position = std::size(detail::data_file_header);
					total_count = std::size(detail::data_file_header);

					position_part = convert::ToString(current_position);
				}

				//Save file bytes
				if (!detail::save_data_file_bytes(data_file_path, data))
					return false;

				file_entries.push_back(std::move(path_part));
				file_entries.push_back(std::move(position_part));
				file_entries.push_back(std::move(count_part));
				
				current_position += byte_count;
				total_count += count;
				data.clear();
			}
		}
	}

	return !std::empty(file_entries) ?
		detail::save_data_file_footer(data_file_path, file_entries) :
		false;
}


/*
	Detaching
*/

bool FileResourceLoader::Detach(FileRepository &file_repository) noexcept
{
	auto iter =
		std::find_if(std::begin(repositories_), std::end(repositories_),
			[&](auto &x) noexcept
			{
				return x == &file_repository;
			});

	//Repository found
	if (iter != std::end(repositories_))
	{
		repositories_.erase(iter);
		return true;
	}
	else
		return false;
}


/*
	Loading directories
*/

bool FileResourceLoader::LoadDirectory(const std::filesystem::path &directory_path,
	file::DirectoryIteration directory_iteration)
{
	if (!file::IsDirectory(directory_path))
		return false;

	auto file_paths =
		file::FilesIn(directory_path, directory_iteration);
	auto added = false;

	if (!std::empty(file_paths))
	{
		//Extract data files first (if any)
		ExtractDataFiles(
			file_repository::detail::filter_by_file_extension(file_paths, data_file_extension_));

		for (auto &repository : repositories_)
			added |= repository->AddFiles(
				file_repository::detail::filter_by_file_extensions(file_paths, repository->FileExtensions()));
	}

	return added;
}

bool FileResourceLoader::LoadDirectories(const file::Paths &directory_paths,
	file::DirectoryIteration directory_iteration)
{
	auto added = false;
	
	for (auto &directory_path : directory_paths)
		added |= LoadDirectory(directory_path, directory_iteration);

	return added;
}


/*
	Loading files
*/

bool FileResourceLoader::LoadFile(const std::filesystem::path &file_path)
{
	{
		auto data_files = file_repository::detail::filter_by_file_extension(
			{file_path}, data_file_extension_);

		//Extract data files first (if any)
		if (!std::empty(data_files))
			ExtractDataFile(data_files.front());
	}

	return LoadFile({std::move(file_path), std::nullopt});
}

bool FileResourceLoader::LoadFile(file_repository::FileEntry file)
{
	auto added = false;

	for (auto &repository : repositories_)
	{
		auto files = file_repository::detail::filter_by_file_extensions(
			file::Paths{file.Path}, repository->FileExtensions());

		if (!std::empty(files))
			added |= repository->AddFile(files.front());
	}

	return added;
}

bool FileResourceLoader::LoadFiles(const file::Paths &file_paths)
{
	auto added = false;

	if (!std::empty(file_paths))
	{
		//Extract data files first (if any)
		ExtractDataFiles(
			file_repository::detail::filter_by_file_extension(file_paths, data_file_extension_));

		for (auto &repository : repositories_)
			added |= repository->AddFiles(
				file_repository::detail::filter_by_file_extensions(file_paths, repository->FileExtensions()));
	}

	return added;
}

bool FileResourceLoader::LoadFiles(const std::vector<file_repository::FileEntry> &files)
{
	auto added = false;

	if (!std::empty(files))
	{
		for (auto &repository : repositories_)
			added |= repository->AddFiles(
				file_repository::detail::filter_by_file_extensions(files, repository->FileExtensions()));
	}

	return added;
}

} //ion::resources::files