/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptBuilder.h
-------------------------------------------
*/

#ifndef ION_SCRIPT_BUILDER_H
#define ION_SCRIPT_BUILDER_H

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "IonScriptCompiler.h"
#include "IonScriptError.h"
#include "IonScriptTree.h"
#include "IonScriptValidator.h"

namespace ion::script
{
	namespace script_builder
	{
		enum class OutputOptions : bool
		{
			Header,
			HeaderAndSummary
		};

		enum class OutputSaveOptions : bool
		{
			Always,
			ErrorsOnly
		};


		namespace detail
		{
			std::string current_date_time();
			std::string print_output(std::filesystem::path file_path, duration built_time, ScriptError error, OutputOptions output_options);
		} //detail
	} //script_builder


	///@brief A class that builds (lexes, parses, links and compiles) and validates the compiled tree structure
	class ScriptBuilder final
	{
		private:

			ScriptCompiler compiler_;
			std::optional<ScriptTree> tree_;
			std::optional<ScriptValidator> validator_;

			CompileError compile_error_;
			ValidateError validate_error_;
			
			std::optional<script_builder::OutputOptions> output_options_;
			std::optional<script_compiler::OutputOptions> compiler_output_options_;
			std::optional<script_validator::OutputOptions> validator_output_options_;
			std::optional<script_tree::PrintOptions> tree_print_options_;

			script_builder::OutputSaveOptions output_save_options_ = script_builder::OutputSaveOptions::ErrorsOnly;


			/**
				@name Helper functions
				@{
			*/

			bool ValidateTree();
			void SaveOutput() noexcept;

			///@}

		public:

			///@brief Default constructor
			ScriptBuilder() = default;


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the compiler used by the builder to the given compiler
			inline void Compiler(ScriptCompiler compiler) noexcept
			{
				compiler_ = std::move(compiler);
			}

			///@brief Sets the validator used by the builder to the given validator
			///@details Pass nullopt or {} if no validator should be used
			inline void Validator(std::optional<ScriptValidator> validator) noexcept
			{
				validator_ = std::move(validator);
			}


			///@brief Sets output options for the builder
			///@details Pass nullopt or {} to turn off any builder output
			inline void Output(std::optional<script_builder::OutputOptions> output_options) noexcept
			{
				output_options_ = output_options;
			}

			///@brief Sets output options for the compiler
			///@details Pass nullopt or {} to turn off any compiler output
			inline void CompilerOutput(std::optional<script_compiler::OutputOptions> output_options) noexcept
			{
				compiler_output_options_ = output_options;
			}

			///@brief Sets output options for the validator
			///@details Pass nullopt or {} to turn off any validator output
			inline void ValidatorOutput(std::optional<script_validator::OutputOptions> output_options) noexcept
			{
				validator_output_options_ = output_options;
			}

			///@brief Sets output options for the tree
			///@details Pass nullopt or {} to turn off any tree output
			inline void TreeOutput(std::optional<script_tree::PrintOptions> print_options) noexcept
			{
				tree_print_options_ = print_options;
			}


			///@brief Sets output save options for the builder
			inline void SaveOutput(script_builder::OutputSaveOptions output_save_options) noexcept
			{
				output_save_options_ = output_save_options;
			}

			///@}

			/**
				@name Observers
				@{
			*/
			
			///@brief Returns a mutable reference to the compiler
			[[nodiscard]] inline auto& Compiler() noexcept
			{
				return compiler_;
			}

			///@brief Returns an immutable reference to the compiler
			[[nodiscard]] inline auto& Compiler() const noexcept
			{
				return compiler_;
			}

			///@brief Returns a mutable reference to the tree
			[[nodiscard]] inline auto& Tree() noexcept
			{
				return tree_;
			}

			///@brief Returns an immutable reference to the tree
			[[nodiscard]] inline auto& Tree() const noexcept
			{
				return tree_;
			}

			///@brief Returns a mutable reference to the validator
			///@details Returns nullopt if no validator is in use by the builder
			[[nodiscard]] inline auto& Validator() noexcept
			{
				return validator_;
			}

			///@brief Returns an immutable reference to the validator
			///@details Returns nullopt if no validator is in use by the builder
			[[nodiscard]] inline auto& Validator() const noexcept
			{
				return validator_;
			}


			///@brief Returns the compile error from the previous build
			///@details The error returned is the overall error of the entire compilation.
			///The compilation is successful if the error returned indicates a success
			[[nodiscard]] inline auto CompileError() const noexcept
			{
				return compile_error_;
			}

			///@brief Returns the validate error from the previous build
			///@details The error returned is the overall error of the entire validation.
			///The validation is okay if the error returned indicates a success
			[[nodiscard]] inline auto ValidateError() const noexcept
			{
				return validate_error_;
			}


			///@brief Returns output options for the builder
			///@details Returns nullopt if builder output is turned off
			[[nodiscard]] inline auto Output() const noexcept
			{
				return output_options_;
			}

			///@brief Returns output options for the compiler
			///@details Returns nullopt if compiler output is turned off
			[[nodiscard]] inline auto CompilerOutput() const noexcept
			{
				return compiler_output_options_;
			}

			///@brief Returns output options for the validator
			///@details Returns nullopt if validator output is turned off
			[[nodiscard]] inline auto ValidatorOutput() const noexcept
			{
				return validator_output_options_;
			}

			///@brief Returns output options for the tree
			///@details Returns nullopt if tree output is turned off
			[[nodiscard]] inline auto TreeOutput() const noexcept
			{
				return tree_print_options_;
			}


			///@brief Returns output save options for the builder
			[[nodiscard]] inline auto SaveOutput() const noexcept
			{
				return output_save_options_;
			}

			///@}

			/**
				@name Building
				@{
			*/

			///@brief Builds a script entry with the given name (from a repository) by compiling and validating it
			///@details Returns true if the script is built successfully
			bool Build(std::string_view name);

			///@brief Builds a script file with the given file path by compiling and validating it
			///@details Returns true if the script is built successfully
			bool BuildFile(std::filesystem::path file_path);

			///@brief Builds a script file with the given file path (and root path) by compiling and validating it
			///@details Returns true if the script is built successfully
			bool BuildFile(std::filesystem::path file_path, std::filesystem::path root_path);

			///@brief Builds the given script string by compiling and validating it
			///@details Returns true if the script is built successfully
			bool BuildString(std::string str);

			///@brief Builds the given script string (and root path) by compiling and validating it
			///@details Returns true if the script is built successfully
			bool BuildString(std::string str, std::filesystem::path root_path);

			///@}

			/**
				@name Outputting
				@{
			*/

			///@brief Prints the output from the previous build
			///@details Whats printed is based on the compiler, validator and tree output options
			[[nodiscard]] std::string PrintOutput(script_builder::OutputOptions output_options = script_builder::OutputOptions::HeaderAndSummary) const;

			///@}
	};
} //ion::script

#endif