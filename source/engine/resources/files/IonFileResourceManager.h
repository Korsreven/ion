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

#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>

#include "IonFileResource.h"
#include "repositories/IonFileRepository.h"
#include "resources/IonResourceManager.h"
#include "unmanaged/IonObjectFactory.h"

namespace ion::resources::files
{
	namespace file_resource_manager::detail
	{
		template <typename ResourceT>
		inline auto is_resources_equivalent(const ResourceT &lhs, const ResourceT &rhs) noexcept
		{
			return lhs.Name() == rhs.Name();
		}

		template <typename RepositoriesT>
		inline auto file_entry_from_resource_name(const RepositoriesT &repositories, std::string_view resource_name) noexcept
		{
			for (auto &repository : repositories)
			{
				if (auto file_entry = repository.File(resource_name); file_entry)
					return file_entry;
			}

			return std::optional<repositories::file_repository::FileEntry>{};
		}

		template <typename RepositoriesT>
		inline auto file_data_from_resource_name(const RepositoriesT &repositories, std::string_view resource_name) noexcept
		{
			for (auto &repository : repositories)
			{
				if (auto file_data = repository.FileData(resource_name); file_data)
					return file_data;
			}

			return std::optional<std::string>{};
		}
	} //file_resource_manager::detail

	template <typename ResourceT, typename OwnerT, typename RepositoryT>
	class FileResourceManager :
		public ResourceManager<ResourceT, OwnerT>,
		protected unmanaged::ObjectFactory<RepositoryT>
	{
		static_assert(std::is_base_of_v<FileResource<OwnerT>, ResourceT>);
		static_assert(std::is_base_of_v<repositories::FileRepository, RepositoryT>);

		private:



		protected:

			using ResourceBase = ResourceManager<ResourceT, OwnerT>;
			using RepositoryBase = unmanaged::ObjectFactory<RepositoryT>;


			/*
				Events
			*/

			virtual bool IsResourcesEquivalent(const ResourceT &lhs, const ResourceT &rhs) noexcept override
			{
				return file_resource_manager::detail::is_resources_equivalent(lhs, rhs);
			}

			virtual bool PrepareResource(ResourceT &resource) noexcept override
			{
				resource.FileData(file_resource_manager::detail::file_data_from_resource_name(FileRepositories(), resource.Name()));
				return !!resource.FileData();
			}


			//See ResourceManager::ResourcePrepared for more details
			virtual void ResourcePrepared(ResourceT&) noexcept override
			{
				//Optional to override
			}

			//See ResourceManager::ResourceLoaded for more details
			virtual void ResourceLoaded(ResourceT&) noexcept override
			{
				//Optional to override
			}

			//See ResourceManager::ResourceUnloaded for more details
			virtual void ResourceUnloaded(ResourceT&) noexcept override
			{
				//Optional to override
			}

			//See ResourceManager::ResourceFailed for more details
			virtual void ResourceFailed(ResourceT&) noexcept override
			{
				//Optional to override
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
			[[nodiscard]] inline auto FileRepositories() noexcept
			{
				return this->RepositoryBase::Objects();
			}

			//Returns an immutable range of all repositories in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto FileRepositories() const noexcept
			{
				return this->RepositoryBase::Objects();
			}


			//Returns a mutable range of all resources in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FileResources() noexcept
			{
				return this->Resources();
			}

			//Returns an immutable range of all resources in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto FileResources() const noexcept
			{
				return this->Resources();
			}


			/*
				File resources
				Creating
			*/

			//Create a file resource with the given resource name
			auto& CreateFileResource(std::string resource_name)
			{
				return this->CreateResource(std::move(resource_name));
			}


			/*
				File resources
				Removing
			*/

			//Clear all file repositories from this factory
			void ClearFileResources() noexcept
			{
				this->ClearResource();
			}

			//Remove an affector from this factory
			auto RemoveFileResource(ResourceT &resource) noexcept
			{
				return this->RemoveResource(resource);
			}


			/*
				File repositories
				Creating
			*/

			//Create an empty file repository
			auto& CreateFileRepository()
			{
				return this->RepositoryBase::Create();
			}

			//Create a file repository by copying/moving the given repository
			template <typename T>
			auto& CreateFileRepository(T &&repository)
			{
				return this->RepositoryBase::Create(std::forward<T>(repository));
			}


			/*
				File repositories
				Removing
			*/

			//Clear all file repositories from this factory
			void ClearFileRepositories() noexcept
			{
				this->RepositoryBase::Clear();
			}

			//Remove an affector from this factory
			auto RemoveFileRepository(RepositoryT &repository) noexcept
			{
				return this->RepositoryBase::Remove(repository);
			}
	};
} //ion::resources::files

#endif