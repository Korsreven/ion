/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources
File:	IonFileResourceManager.h
-------------------------------------------
*/

#ifndef ION_FILE_RESOURCE_MANAGER_H
#define ION_FILE_RESOURCE_MANAGER_H

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "IonFileResource.h"
#include "assets/repositories/IonFileRepository.h"
#include "resources/IonResourceManager.h"
#include "unmanaged/IonObjectFactory.h"

namespace ion::resources
{
	namespace file_resource_manager::detail
	{
		template <typename RepositoriesT>
		inline auto file_data_from_resource_name(const RepositoriesT &repositories, std::string_view resource_name) noexcept ->
			std::optional<std::pair<std::string, std::filesystem::path>>
		{
			for (auto &repository : repositories)
			{
				if (auto file_data = repository.FileData(resource_name); file_data)
					return std::pair{std::move(*file_data), std::move(*repository.FilePath(resource_name))};
			}

			return {};
		}
	} //file_resource_manager::detail

	template <typename ResourceT, typename OwnerT, typename RepositoryT>
	class FileResourceManager :
		public ResourceManager<ResourceT, OwnerT>,
		protected unmanaged::ObjectFactory<RepositoryT>
	{
		static_assert(std::is_base_of_v<FileResource<OwnerT>, ResourceT>);
		static_assert(std::is_base_of_v<assets::repositories::FileRepository, RepositoryT>);

		protected:

			using ResourceBase = ResourceManager<ResourceT, OwnerT>;
			using RepositoryBase = unmanaged::ObjectFactory<RepositoryT>;


			/*
				Events
			*/

			virtual bool PrepareResource(ResourceT &resource) noexcept override
			{
				if (auto file = file_resource_manager::detail::file_data_from_resource_name(Repositories(), resource.AssetName()); file)
				{
					auto &[data, path] = *file;
					resource.FileData(std::move(data), std::move(path));
					return true;
				}
				else
					return false;
			}


			//See ResourceManager::ResourcePrepared for more details
			virtual void ResourcePrepared(ResourceT&) noexcept override
			{
				//Optional to override
			}

			//See ResourceManager::ResourceLoaded for more details
			virtual void ResourceLoaded(ResourceT &resource) noexcept override
			{
				resource.ResetFileData();
					//File data not required after resource has been loaded (save memory)
			}

			//See ResourceManager::ResourceUnloaded for more details
			virtual void ResourceUnloaded(ResourceT&) noexcept override
			{
				//Optional to override
			}

			//See ResourceManager::ResourceFailed for more details
			virtual void ResourceFailed(ResourceT &resource) noexcept override
			{
				resource.ResetFileData();
					//File data not required after resource has failed (save memory)
			}

		public:

			//Default constructor
			FileResourceManager() = default;

			//Deleted copy constructor
			FileResourceManager(const FileResourceManager&) = delete;

			//Default move constructor
			FileResourceManager(FileResourceManager&&) = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			FileResourceManager& operator=(const FileResourceManager&) = delete;

			//Move assignment
			FileResourceManager& operator=(FileResourceManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all repositories in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Repositories() noexcept
			{
				return this->RepositoryBase::Objects();
			}

			//Returns an immutable range of all repositories in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Repositories() const noexcept
			{
				return this->RepositoryBase::Objects();
			}


			/*
				Repositories
				Creating
			*/

			//Create an empty repository
			auto& CreateRepository()
			{
				return this->RepositoryBase::Create();
			}

			//Create a repository by copying/moving the given repository
			template <typename T>
			auto& CreateRepository(T &&repository)
			{
				return this->RepositoryBase::Create(std::forward<T>(repository));
			}


			/*
				Repositories
				Removing
			*/

			//Clear all repositories from this manager
			void ClearRepositories() noexcept
			{
				this->RepositoryBase::Clear();
			}

			//Remove a repository from this manager
			auto RemoveRepository(RepositoryT &repository) noexcept
			{
				return this->RepositoryBase::Remove(repository);
			}
	};
} //ion::resources

#endif