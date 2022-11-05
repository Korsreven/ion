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
#include "IonResourceManager.h"
#include "assets/repositories/IonFileRepository.h"
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


	///@brief A class that manages and prepares file resources from its own repositories
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


			/**
				@name Events
				@{
			*/

			virtual bool PrepareResource(ResourceT &resource) override
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


			///@brief See ResourceManager::ResourcePrepared for more details
			virtual void ResourcePrepared(ResourceT&) noexcept override
			{
				//Optional to override
			}

			///@brief See ResourceManager::ResourceLoaded for more details
			virtual void ResourceLoaded(ResourceT &resource) noexcept override
			{
				resource.ResetFileData();
					//File data not required after resource has been loaded (save memory)
			}

			///@brief See ResourceManager::ResourceUnloaded for more details
			virtual void ResourceUnloaded(ResourceT&) noexcept override
			{
				//Optional to override
			}

			///@brief See ResourceManager::ResourceFailed for more details
			virtual void ResourceFailed(ResourceT &resource) noexcept override
			{
				resource.ResetFileData();
					//File data not required after resource has failed (save memory)
			}

			///@}

		public:

			///@brief Default constructor
			FileResourceManager() = default;

			///@brief Deleted copy constructor
			FileResourceManager(const FileResourceManager&) = delete;

			///@brief Default move constructor
			FileResourceManager(FileResourceManager&&) = default;


			/*
				Operators
			*/

			///@brief Deleted copy assignment
			FileResourceManager& operator=(const FileResourceManager&) = delete;

			///@brief Default move assignment
			FileResourceManager& operator=(FileResourceManager&&) = default;


			/**
				Ranges
			*/

			///@brief Returns a mutable range of all repositories in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Repositories() noexcept
			{
				return RepositoryBase::Objects();
			}

			///@brief Returns an immutable range of all repositories in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Repositories() const noexcept
			{
				return RepositoryBase::Objects();
			}


			/**
				@name Repositories - Creating
				@{
			*/

			///@brief Creates an empty repository
			auto CreateRepository()
			{
				return RepositoryBase::Create();
			}

			///@brief Creates a repository by copying/moving the given repository
			template <typename T, typename = std::enable_if_t<std::is_same_v<std::remove_cvref_t<T>, RepositoryT>>>
			auto CreateRepository(T &&repository)
			{
				return RepositoryBase::Create(std::forward<T>(repository));
			}

			///@}

			/**
				@name Repositories - Removing
				@{
			*/

			///@brief Clears all repositories from this factory
			void ClearRepositories() noexcept
			{
				RepositoryBase::Clear();
			}

			///@brief Removes a repository from this factory
			auto RemoveRepository(RepositoryT &repository) noexcept
			{
				return RepositoryBase::Remove(repository);
			}

			///@}
	};
} //ion::resources

#endif