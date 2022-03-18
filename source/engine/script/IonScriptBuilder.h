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


		namespace detail
		{
			std::string current_date_time();
			std::string print_output(std::filesystem::path file_path, duration built_time, ScriptError error, OutputOptions output_options);
		} //detail
	} //script_builder


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


			/*
				Helper functions
			*/

			bool ValidateTree();
			void SaveOutput() noexcept;


		public:

			//Default constructor
			ScriptBuilder() = default;


			/*
				Modifiers
			*/

			//Sets the compiler used by the builder to the given compiler
			inline void Compiler(ScriptCompiler compiler) noexcept
			{
				compiler_ = std::move(compiler);
			}

			//Sets the validator used by the builder to the given validator
			//Pass nullopt or {} if no validator should be used
			inline void Validator(std::optional<ScriptValidator> validator) noexcept
			{
				validator_ = std::move(validator);
			}


			//Sets output options for the builder
			//Pass nullopt or {} to turn off any builder output
			inline void Output(std::optional<script_builder::OutputOptions> output_options) noexcept
			{
				output_options_ = output_options;
			}

			//Sets output options for the compiler
			//Pass nullopt or {} to turn off any compiler output
			inline void CompilerOutput(std::optional<script_compiler::OutputOptions> output_options) noexcept
			{
				compiler_output_options_ = output_options;
			}

			//Sets output options for the validator
			//Pass nullopt or {} to turn off any validator output
			inline void ValidatorOutput(std::optional<script_validator::OutputOptions> output_options) noexcept
			{
				validator_output_options_ = output_options;
			}

			//Sets output options for the tree
			//Pass nullopt or {} to turn off any tree output
			inline void TreeOutput(std::optional<script_tree::PrintOptions> print_options) noexcept
			{
				tree_print_options_ = print_options;
			}


			/*
				Observers
			*/
			
			//Returns a mutable reference to the compiler
			[[nodiscard]] inline auto& Compiler() noexcept
			{
				return compiler_;
			}

			//Returns an immutable reference to the compiler
			[[nodiscard]] inline auto& Compiler() const noexcept
			{
				return compiler_;
			}

			//Returns a mutable reference to the tree
			[[nodiscard]] inline auto& Tree() noexcept
			{
				return tree_;
			}

			//Returns an immutable reference to the tree
			[[nodiscard]] inline auto& Tree() const noexcept
			{
				return tree_;
			}

			//Returns a mutable reference to the validator
			//Returns nullopt if no validator is in use by the builder
			[[nodiscard]] inline auto& Validator() noexcept
			{
				return validator_;
			}

			//Returns an immutable reference to the validator
			//Returns nullopt if no validator is in use by the builder
			[[nodiscard]] inline auto& Validator() const noexcept
			{
				return validator_;
			}


			//Returns the compile error from the previous build
			//The error returned is the overall error of the entire compilation
			//The compilation is successful if the error returned indicates a success
			[[nodiscard]] inline auto CompileError() const noexcept
			{
				return compile_error_;
			}

			//Returns the validate error from the previous build
			//The error returned is the overall error of the entire validation
			//The validation is okay if the error returned indicates a success
			[[nodiscard]] inline auto ValidateError() const noexcept
			{
				return validate_error_;
			}


			//Returns output options for the builder
			//Returns nullopt if builder output is turned off
			[[nodiscard]] inline auto Output() const noexcept
			{
				return output_options_;
			}

			//Returns output options for the compiler
			//Returns nullopt if compiler output is turned off
			[[nodiscard]] inline auto CompilerOutput() const noexcept
			{
				return compiler_output_options_;
			}

			//Returns output options for the validator
			//Returns nullopt if validator output is turned off
			[[nodiscard]] inline auto ValidatorOutput() const noexcept
			{
				return validator_output_options_;
			}

			//Returns output options for the tree
			//Returns nullopt if tree output is turned off
			[[nodiscard]] inline auto TreeOutput() const noexcept
			{
				return tree_print_options_;
			}


			/*
				Building
			*/

			//Build a script entry with the given name (from a repository) by compiling and validating it
			//Returns true if the script is built successfully
			bool Build(std::string_view name);

			//Build a script file with the given file path by compiling and validating it
			//Returns true if the script is built successfully
			bool BuildFile(std::filesystem::path file_path);

			//Build a script file with the given file path (and root path) by compiling and validating it
			//Returns true if the script is built successfully
			bool BuildFile(std::filesystem::path file_path, std::filesystem::path root_path);

			//Build the given script string by compiling and validating it
			//Returns true if the script is built successfully
			bool BuildString(std::string str);

			//Build the given script string (and root path) by compiling and validating it
			//Returns true if the script is built successfully
			bool BuildString(std::string str, std::filesystem::path root_path);


			/*
				Outputting
			*/

			//Prints the output from the previous build
			//Whats printed is based on the compiler, validator and tree output options
			[[nodiscard]] std::string PrintOutput(script_builder::OutputOptions output_options = script_builder::OutputOptions::HeaderAndSummary) const;
	};
} //ion::script

#endif