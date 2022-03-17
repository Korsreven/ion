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

#include "assets/repositories/IonFileRepository.h"
#include "assets/repositories/IonScriptRepository.h"
#include "memory/IonNonOwningPtr.h"
#include "script/IonScriptBuilder.h"
#include "script/IonScriptCompiler.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "unmanaged/IonObjectFactory.h"

namespace ion::script::interfaces
{
	namespace script_interface::detail
	{
	} //script_interface::detail


	class ScriptInterface :
		protected unmanaged::ObjectFactory<assets::repositories::ScriptRepository>,
		protected unmanaged::ObjectFactory<assets::repositories::FileRepository>
	{
		private:

			using ScriptRepositoryBase = unmanaged::ObjectFactory<assets::repositories::ScriptRepository>;
			using FileRepositoryBase = unmanaged::ObjectFactory<assets::repositories::FileRepository>;

		protected:
		
			ScriptBuilder builder_;
			std::optional<assets::repositories::ScriptRepository> repository_;

		public:

			//Default constructor
			ScriptInterface() = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all script repositories in this script interface
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ScriptRepositories() noexcept
			{
				return ScriptRepositoryBase::Objects();
			}

			//Returns an immutable range of all script repositories in this script interface
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ScriptRepositories() const noexcept
			{
				return ScriptRepositoryBase::Objects();
			}


			//Returns a mutable range of all file repositories in this script interface
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FileRepositories() noexcept
			{
				return FileRepositoryBase::Objects();
			}

			//Returns an immutable range of all file repositories in this script interface
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto FileRepositories() const noexcept
			{
				return FileRepositoryBase::Objects();
			}


			/*
				Modifiers
			*/

			//Sets output options for the script interface
			//Pass nullopt or {} to turn off any script interface output
			inline void Output(std::optional<script_builder::OutputOptions> output_options) noexcept
			{
				builder_.Output(output_options);
			}

			//Sets output options for the script interface compiler
			//Pass nullopt or {} to turn off any script interface compiler output
			inline void CompilerOutput(std::optional<script_compiler::OutputOptions> output_options) noexcept
			{
				builder_.CompilerOutput(output_options);
			}

			//Sets output options for the script interface validator
			//Pass nullopt or {} to turn off any script interface validator output
			inline void ValidatorOutput(std::optional<script_validator::OutputOptions> output_options) noexcept
			{
				builder_.ValidatorOutput(output_options);
			}

			//Sets output options for the script interface tree
			//Pass nullopt or {} to turn off any script interface tree output
			inline void TreeOutput(std::optional<script_tree::PrintOptions> print_options) noexcept
			{
				builder_.TreeOutput(print_options);
			}


			/*
				Observers
			*/

			//Returns an immutable reference to the script interface builder
			[[nodiscard]] inline auto& Builder() const noexcept
			{
				return builder_;
			}


			/*
				Script repositories
				Creating
			*/

			//Create an empty script repository
			NonOwningPtr<assets::repositories::ScriptRepository> CreateScriptRepository();

			//Create a script repository by copying the given script repository
			NonOwningPtr<assets::repositories::ScriptRepository> CreateScriptRepository(const assets::repositories::ScriptRepository &repository);

			//Create a script repository by moving the given script repository
			NonOwningPtr<assets::repositories::ScriptRepository> CreateScriptRepository(assets::repositories::ScriptRepository &&repository);


			/*
				Script repositories
				Removing
			*/

			//Clear all script repositories from this script interface
			void ClearScriptRepositories() noexcept;

			//Remove a script repository from this script interface
			bool RemoveScriptRepository(assets::repositories::ScriptRepository &repository) noexcept;


			/*
				File repositories
				Creating
			*/

			//Create an empty file repository
			NonOwningPtr<assets::repositories::FileRepository> CreateFileRepository();

			//Create a file repository by copying the given file repository
			NonOwningPtr<assets::repositories::FileRepository> CreateFileRepository(const assets::repositories::FileRepository &repository);

			//Create a file repository by moving the given file repository
			NonOwningPtr<assets::repositories::FileRepository> CreateFileRepository(assets::repositories::FileRepository &&repository);


			/*
				File repositories
				Removing
			*/

			//Clear all file repositories from this script interface
			void ClearFileRepositories() noexcept;

			//Remove a file repository from this script interface
			bool RemoveFileRepository(assets::repositories::FileRepository &repository) noexcept;
	};
} //ion::script::interfaces

#endif