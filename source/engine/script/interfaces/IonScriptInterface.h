/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/interfaces
File:	IonScriptInterface.h
-------------------------------------------
*/

#ifndef ION_SCRIPT_INTERFACE_H
#define ION_SCRIPT_INTERFACE_H

#include <optional>
#include <string>
#include <string_view>

#include "assets/repositories/IonFileRepository.h"
#include "assets/repositories/IonScriptRepository.h"
#include "managed/IonObjectManager.h"
#include "managed/IonObjectRegister.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptBuilder.h"
#include "script/IonScriptCompiler.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "unmanaged/IonObjectFactory.h"

namespace ion::script::interfaces
{
	using ManagerRegister = managed::ObjectRegister<managed::ObjectManagerBase>;


	namespace script_interface::detail
	{
		template <typename RepositoriesT>
		inline auto repository_from_resource_name(const RepositoriesT &repositories, std::string_view resource_name) noexcept ->
			typename RepositoriesT::value_type::pointer
		{
			for (auto &repository : repositories)
			{
				if (auto file = repository.File(resource_name); file)
					return const_cast<typename RepositoriesT::value_type::pointer>(&repository);
			}

			return nullptr;
		}

		template <typename RepositoriesT>
		inline auto file_data_from_resource_name(const RepositoriesT &repositories, std::string_view resource_name) noexcept ->
			std::optional<std::string>
		{
			for (auto &repository : repositories)
			{
				if (auto file_data = repository.FileData(resource_name); file_data)
					return std::move(*file_data);
			}

			return {};
		}
	} //script_interface::detail


	///@brief A base class representing a complete script interface for loading, building and validating a particular script setup
	///@details All script interfaces shares a register of managers needed to fully load scriptet objects into managers or other game objects
	class ScriptInterface :
		protected unmanaged::ObjectFactory<assets::repositories::ScriptRepository>,
		protected unmanaged::ObjectFactory<assets::repositories::FileRepository>
	{
		private:

			using ScriptRepositoryBase = unmanaged::ObjectFactory<assets::repositories::ScriptRepository>;
			using FileRepositoryBase = unmanaged::ObjectFactory<assets::repositories::FileRepository>;

		protected:
		
			ScriptBuilder builder_;
			std::optional<ScriptTree> tree_;

			static inline ManagerRegister manager_register_;


			virtual ScriptValidator GetValidator() const = 0;
			bool Load(std::string_view asset_name);

		public:

			///@brief Default constructor
			ScriptInterface() = default;


			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all script repositories in this script interface
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ScriptRepositories() noexcept
			{
				return ScriptRepositoryBase::Objects();
			}

			///@brief Returns an immutable range of all script repositories in this script interface
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ScriptRepositories() const noexcept
			{
				return ScriptRepositoryBase::Objects();
			}


			///@brief Returns a mutable range of all file repositories in this script interface
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FileRepositories() noexcept
			{
				return FileRepositoryBase::Objects();
			}

			///@brief Returns an immutable range of all file repositories in this script interface
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FileRepositories() const noexcept
			{
				return FileRepositoryBase::Objects();
			}

			///@}

			/**
				@name Managers
				@{
			*/

			///@brief Returns a reference to the static manager register for all script interfaces
			[[nodiscard]] static inline auto& Managers() noexcept
			{
				return manager_register_;
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the max number of build processes the compiler is allowed to use
			///@details If nullopt is passed, a default number of build processes will be used (based on your system)
			inline void MaxBuildProcesses(std::optional<int> max_build_processes) noexcept
			{
				builder_.Compiler().MaxBuildProcesses(max_build_processes);
			}


			///@brief Sets output options for the builder
			///@details Pass nullopt or {} to turn off any builder output
			inline void Output(std::optional<script_builder::OutputOptions> output_options) noexcept
			{
				builder_.Output(output_options);
			}

			///@brief Sets output options for the compiler
			///@details Pass nullopt or {} to turn off any compiler output
			inline void CompilerOutput(std::optional<script_compiler::OutputOptions> output_options) noexcept
			{
				builder_.CompilerOutput(output_options);
			}

			///@brief Sets output options for the validator
			///@details Pass nullopt or {} to turn off any validator output
			inline void ValidatorOutput(std::optional<script_validator::OutputOptions> output_options) noexcept
			{
				builder_.ValidatorOutput(output_options);
			}

			///@brief Sets output options for the tree
			///@details Pass nullopt or {} to turn off any tree output
			inline void TreeOutput(std::optional<script_tree::PrintOptions> print_options) noexcept
			{
				builder_.TreeOutput(print_options);
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns an immutable reference to the script interface builder
			[[nodiscard]] inline auto& Builder() const noexcept
			{
				return builder_;
			}

			///@}

			/**
				@name Script repositories - Creating
				@{
			*/

			///@brief Creates an empty script repository
			NonOwningPtr<assets::repositories::ScriptRepository> CreateScriptRepository();

			///@brief Creates a script repository by copying the given script repository
			NonOwningPtr<assets::repositories::ScriptRepository> CreateScriptRepository(const assets::repositories::ScriptRepository &repository);

			///@brief Creates a script repository by moving the given script repository
			NonOwningPtr<assets::repositories::ScriptRepository> CreateScriptRepository(assets::repositories::ScriptRepository &&repository);

			///@}

			/**
				@name Script repositories - Removing
				@{
			*/

			///@brief Clears all script repositories from this script interface
			void ClearScriptRepositories() noexcept;

			///@brief Removes a script repository from this script interface
			bool RemoveScriptRepository(assets::repositories::ScriptRepository &repository) noexcept;

			///@}

			/**
				@name File repositories - Creating
				@{
			*/

			///@brief Creates an empty file repository
			NonOwningPtr<assets::repositories::FileRepository> CreateFileRepository();

			///@brief Creates a file repository by copying the given file repository
			NonOwningPtr<assets::repositories::FileRepository> CreateFileRepository(const assets::repositories::FileRepository &repository);

			///@brief Creates a file repository by moving the given file repository
			NonOwningPtr<assets::repositories::FileRepository> CreateFileRepository(assets::repositories::FileRepository &&repository);

			///@}

			/**
				@name File repositories - Removing
				@{
			*/

			///@brief Clears all file repositories from this script interface
			void ClearFileRepositories() noexcept;

			///@brief Removes a file repository from this script interface
			bool RemoveFileRepository(assets::repositories::FileRepository &repository) noexcept;

			///@}
	};
} //ion::script::interfaces

#endif