/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptCompiler.h
-------------------------------------------
*/

#ifndef ION_SCRIPT_COMPILER_H
#define ION_SCRIPT_COMPILER_H

#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "IonScriptError.h"
#include "IonScriptTree.h"
#include "adaptors/IonFlatMap.h"
#include "adaptors/IonFlatSet.h"
#include "resources/files/repositories/IonScriptRepository.h"
#include "parallel/IonWorkerPool.h"
#include "utilities/IonConvert.h"

namespace ion::script
{
	namespace script_compiler 
	{
		enum class OutputOptions
		{
			Summary,
			Units,
			SummaryAndUnits
		};


		namespace detail
		{
			enum class token_name
			{	
				Comment,
				Function,
				Identifier,
				Operator,
				Rule,
				Selector,
				Separator,
				Unit,
				UnknownSymbol,
				WhiteSpace,

				//Literal types
				BooleanLiteral,
				HexLiteral,
				NumericLiteral,
				StringLiteral,

				//Operator types
				BinaryOperator,
				UnaryOperator
			};


			struct translation_unit
			{
				std::string file_path; //Normalized, viewed
				std::string source; //Viewed
				CompileError error; //Referenced
			};

			using translation_units = std::vector<std::unique_ptr<translation_unit>>;

			struct lexical_token
			{
				token_name name = token_name::UnknownSymbol;
				std::string_view value;
				translation_unit *unit = nullptr;
				int line_number = 0;
			};

			using lexical_tokens = std::vector<lexical_token>;
			using string_views = std::vector<std::string_view>;

			struct selector_group
			{
				string_views classes;
				std::vector<int> combinators;
			};

			using selector_groups = std::vector<selector_group>;

			struct template_rule
			{
				selector_groups selectors;
				script_tree::ObjectNode *object = nullptr;
			};

			using template_rules = std::vector<template_rule>;
			using variable_declarations = adaptors::FlatMap<std::string_view, script_tree::ArgumentNodes>;


			struct file_trace
			{
				std::vector<std::filesystem::path> stack;
					//Contains unique files (direct-line inclusion)
					//front() returns the entry file path, and back() returns the current file path

				/*
					Observers
				*/

				inline auto& current_file_path() const noexcept
				{
					return stack.back();
				}

				inline auto& entry_file_path() const noexcept
				{
					return stack.front();
				}

			
				/*
					Files
				*/

				bool push_file(std::filesystem::path file_path);
				void pop_file();
			};

			struct build_system
			{
				std::filesystem::path root_path;
				const assets::repositories::ScriptRepository *repository = nullptr;		
				
				translation_units units; //Need stable memory addressing
				std::mutex m; //Protects 'units'
				parallel::WorkerPool<std::optional<lexical_tokens>, std::string> processes;	


				build_system(std::filesystem::path root_path);
				build_system(const assets::repositories::ScriptRepository &repository);		

				void start_process(std::string str, file_trace trace);
			};


			struct scope
			{
				script_tree::ObjectNodes objects;
				script_tree::PropertyNodes properties;
				variable_declarations variables;
				std::string classes;
			};

			struct syntax_context
			{
				lexical_token *next_token = nullptr;
				lexical_token *previous_token = nullptr;

				int curly_brace_depth = 0;
				int parenthesis_depth = 0;

				bool inside_import = false;
				bool inside_object_signature = false;
				bool inside_template_signature = false;
				bool inside_property = false;
				bool inside_variable = false;
				bool inside_function = false;
				bool inside_calc_function = false;
			};

			struct parse_context
			{
				std::vector<scope> scopes;
				script_tree::ArgumentNodes property_arguments;
				script_tree::ArgumentNodes variable_arguments;
				script_tree::ArgumentNodes function_arguments;

				std::vector<lexical_token*> object_tokens;
				lexical_token *identifier_token = nullptr;
				lexical_token *property_token = nullptr;
				lexical_token *variable_token = nullptr;
				lexical_token *function_token = nullptr;

				template_rules templates;
				selector_groups selectors;	
				adaptors::FlatSet<std::string_view> selector_classes;
				std::string classes;

				int scope_depth = 0;
				bool unary_minus = false;
			};


			constexpr auto is_alpha(char c) noexcept
			{
				return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
			}

			constexpr auto is_white_space(char c) noexcept
			{
				//Unroll all true cases
				switch (c)
				{
					case ' ':
					case '\n': //Line feed
					case '\r': //Carriage return
					case '\t': //Horizontal tab
					case '\v': //Vertical tab
					return true;

					default:
					return false;
				}
			}

			constexpr auto is_valid_in_identifier(char c)
			{
				if (!is_alpha(c) &&
					!ion::utilities::convert::detail::is_digit(c))
				{
					//Unroll all true cases
					switch (c)
					{
						case '-': //Hyphen
						case '_': //Underscore
						return true;

						default:
						return false;
					}
				}

				return true;
			}

			constexpr auto is_class_selector(char c) noexcept
			{
				//Unroll all true cases
				switch (c)
				{
					case '.': //Class
					case '#': //Id
					case '*': //Both
					return true;

					default:
					return false;
				}
			}

			constexpr auto is_combinator_selector(char c) noexcept
			{
				//Unroll all true cases
				switch (c)
				{
					case ',': //Both
					case '>': //Child
					case '+': //Adjacent sibling
					case '~': //General sibling
					return true;

					default:
					return false;
				}
			}

			constexpr auto is_variable_prefix(char c) noexcept
			{
				return c == '$';
			}

			constexpr auto is_class_identifier(std::string_view str) noexcept
			{
				return is_class_selector(str.front());
			}

			constexpr auto is_variable_identifier(std::string_view str) noexcept
			{
				return is_variable_prefix(str.front());
			}


			constexpr auto is_boolean_literal(std::string_view str) noexcept
			{
				return str == "true" || str == "false";
			}

			constexpr auto is_function(std::string_view str) noexcept
			{
				return str == "rgb" ||
					   str == "rgba" ||
					   str == "hsl" ||
					   str == "hsla" ||
					   str == "hwb" ||
					   str == "hwba" ||
					   str == "cmyk" ||
					   str == "cmyka" ||
					   str == "vec2" ||
					   str == "calc";
			}

			constexpr auto is_hex_digit(char c) noexcept
			{
				if (!ion::utilities::convert::detail::is_digit(c))
				{
					//Unroll all true cases
					switch (c)
					{
						case 'A':
						case 'B':
						case 'C':
						case 'D':
						case 'E':
						case 'F':
						case 'a':
						case 'b':
						case 'c':
						case 'd':
						case 'e':
						case 'f':
						return true;

						default:
						return false;
					}
				}

				return true;
			}

			constexpr auto is_import_rule(std::string_view str) noexcept
			{
				return str == "@import";
			}

			constexpr auto is_operator(char c) noexcept
			{
				//Unroll all true cases
				switch (c)
				{
					case '-':
					case '+':
					case '*':
					case '/':
					return true;

					default:
					return false;
				}
			}

			constexpr auto is_selector(char c) noexcept
			{
				return is_class_selector(c) || is_combinator_selector(c);
			}

			constexpr auto is_separator(char c) noexcept
			{
				//Unroll all true cases
				switch (c)
				{
					case ':':
					case ';':
					case '{':
					case '}':
					case '(':
					case ')':
					case ',':
					return true;

					default:
					return false;
				}
			}

			constexpr auto is_unit(std::string_view str) noexcept
			{
				return str == "%" ||
					   str == "ch" ||
					   str == "cm" ||
					   str == "em" ||
					   str == "ex" ||
					   str == "fr" ||
					   str == "in" ||
					   str == "mm" ||
					   str == "pc" ||
					   str == "pt" ||
					   str == "px" ||
					   str == "rem" ||
					   str == "vh" ||
					   str == "vmax" ||
					   str == "vmin" ||
					   str == "vw";
			}


			constexpr auto is_start_of_comment(char c, char next_c) noexcept
			{
				if (c == '/')
				{
					//Unroll all true cases
					switch (next_c)
					{
						case '/':
						case '*':
						return true;
					}
				}

				return false;
			}

			constexpr auto is_start_of_hex_literal(char c, char next_c) noexcept
			{
				return c == '#' && is_hex_digit(next_c);
			}

			constexpr auto is_start_of_identifier(char c, char next_c = '\0') noexcept
			{
				//A digit
				if (ion::utilities::convert::detail::is_digit(c) ||
					//Two hyphens
					(c == '-' && (next_c == '-' ||
						//Hyphen followed by a non valid identifier or digit
						!is_valid_in_identifier(next_c) ||
						ion::utilities::convert::detail::is_digit(next_c))))
						return false;
				else
					return is_valid_in_identifier(c);
			}

			constexpr auto is_start_of_numeric_literal(char c, char next_c = '\0') noexcept
			{
				return ion::utilities::convert::detail::is_digit(c) ||
					(c == '.' && ion::utilities::convert::detail::is_digit(next_c));
			}

			constexpr auto is_start_of_rule(char c) noexcept
			{
				return c == '@';
			}

			constexpr auto is_start_of_string_literal(char c) noexcept
			{
				//Unroll all true cases
				switch (c)
				{
					case '"':
					case '\'':
					return true;

					default:
					return false;
				}
			}


			std::optional<std::filesystem::path> full_file_path(std::filesystem::path file_path,
				const build_system &system, const std::filesystem::path &current_path);
			
			bool open_file(const std::filesystem::path &file_path, const build_system &system,
				file_trace &trace, std::string &str, CompileError &error);
			bool load_from_repository(std::string_view name, const build_system &system,
				file_trace &trace, std::string &str, CompileError &error);
			bool import_unit(std::string str_argument, const build_system &system,
				file_trace &trace, std::string &str, CompileError &error);


			/*
				Lexing
			*/

			std::pair<std::string_view, int> get_comment_lexeme(std::string_view str) noexcept;
			std::string_view get_hex_literal_lexeme(std::string_view str) noexcept;
			std::string_view get_identifer_lexeme(std::string_view str) noexcept;
			std::string_view get_numeric_literal_lexeme(std::string_view str) noexcept;
			std::pair<std::string_view, int> get_string_literal_lexeme(std::string_view str) noexcept;
			std::pair<std::string_view, int> get_white_space_lexeme(std::string_view str) noexcept;

			std::optional<lexical_tokens> lex(translation_unit &unit, file_trace trace, build_system &system);

		
			/*
				Parsing
			*/

			//Syntax checking

			bool check_function_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
			bool check_identifier_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
			bool check_literal_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
			bool check_operator_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
			bool check_rule_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
			bool check_selector_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
			bool check_separator_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
			bool check_unit_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
			bool check_unknown_symbol_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;

			bool check_syntax(lexical_tokens &tokens, CompileError &error) noexcept;

			//Linking

			void link(lexical_tokens &tokens, const adaptors::FlatMap<std::string, std::optional<lexical_tokens>> &results, const build_system &system);

			//Calling
		
			std::optional<script_tree::ArgumentType> call_cmyk(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept;
			std::optional<script_tree::ArgumentType> call_hsl(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept;
			std::optional<script_tree::ArgumentType> call_hwb(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept;
			std::optional<script_tree::ArgumentType> call_rgb(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept;
			std::optional<script_tree::ArgumentType> call_vec2(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept;

			std::optional<script_tree::ArgumentType> call_function(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept;

			//Parsing

			bool parse_function(lexical_token &token, parse_context &context, CompileError &error) noexcept;
			bool parse_identifier(lexical_token &token, parse_context &context, CompileError &error);
			bool parse_literal(lexical_token &token, lexical_token *next_token, parse_context &context, CompileError &error);
			bool parse_unary_operator(lexical_token &token, parse_context &context, CompileError &error) noexcept;
			bool parse_selector(lexical_token &token, parse_context &context, CompileError &error) noexcept;
			bool parse_separator(lexical_token &token, parse_context &context, CompileError &error);
			bool parse_unit(lexical_token &token, parse_context &context, CompileError &error) noexcept;

			void pre_parse(lexical_tokens &tokens) noexcept;
			std::optional<ScriptTree> parse(lexical_tokens tokens, build_system &system, CompileError &error);

			//Inheriting

			adaptors::FlatSet<std::string_view> split_classes(std::string_view str);
			std::string join_classes(const adaptors::FlatSet<std::string_view> &classes);
			string_views get_classes(script_tree::ObjectNode &object);

			std::pair<bool, int> is_matching(string_views::const_iterator first_selector_class,
				string_views::const_iterator last_selector_class, const string_views &classes) noexcept;
			void append_matching_templates(const script_tree::detail::generations &descendants,
				template_rules::const_iterator first, template_rules::const_iterator last);

			void inherit(script_tree::ObjectNodes &objects, template_rules &templates);


			/*
				Compiling
			*/

			std::optional<ScriptTree> compile(file_trace trace, build_system &system, CompileError &error, std::vector<CompileError> &errors);
			std::optional<ScriptTree> compile_unit(translation_unit &unit, file_trace trace, build_system &system);
			std::optional<lexical_tokens> partial_compile_unit(translation_unit &unit, file_trace trace, build_system &system);
			

			/*
				Outputting
			*/

			std::string print_output(duration compile_time, const std::vector<CompileError> &errors, OutputOptions output_options);
		} //detail
	} //script_compiler

	class ScriptCompiler final
	{
		private:
		
			const assets::repositories::ScriptRepository *repository_ = nullptr;	
			std::optional<int> max_build_processes_;

			std::vector<CompileError> compile_errors_;
			duration compile_time_;

		public:

			//Construct a new script compiler without a repository
			ScriptCompiler() = default;

			//Construct a new script compiler with the given repository
			ScriptCompiler(const assets::repositories::ScriptRepository &repository);


			/*
				Modifiers
			*/

			//Set the max number of build processes the compiler is allowed to use
			//If nullopt is passed, a default number of build processes will be used (based on your system)
			inline void MaxBuildProcesses(std::optional<int> max_build_processes) noexcept
			{
				max_build_processes_ = max_build_processes;
			}


			/*
				Observers
			*/

			//Returns all compile errors from the previous compilation (per unit)
			//The errors returned are per compiled unit (file)
			//The compilation is successful if all errors returned indicates a success
			[[nodiscard]] inline auto& CompileErrors() const noexcept
			{
				return compile_errors_;
			}

			//Returns the compile time of the previous compilation
			[[nodiscard]] inline auto CompileTime() const noexcept
			{
				return compile_time_;
			}


			//Returns the max number of build processes the compiler is allowed to use
			//If nullopt is returned, a default number of build processes is being used (based on your system)
			[[nodiscard]] inline auto MaxBuildProcesses() const noexcept
			{
				return max_build_processes_;
			}


			/*
				Compiling
			*/

			//Compile a script entry with the given name (from a repository) by lexing, parsing and linking it.
			//Returns a ScriptTree that contains objects, object properties and property arguments
			[[nodiscard]] std::optional<ScriptTree> Compile(std::string_view name, CompileError &error);

			//Compile a script file with the given file path by lexing, parsing and linking it.
			//Returns a ScriptTree that contains objects, object properties and property arguments
			[[nodiscard]] std::optional<ScriptTree> CompileFile(std::filesystem::path file_path, CompileError &error);

			//Compile a script file with the given file path (and root path) by lexing, parsing and linking it.
			//Returns a ScriptTree that contains objects, object properties and property arguments
			[[nodiscard]] std::optional<ScriptTree> CompileFile(std::filesystem::path file_path, std::filesystem::path root_path, CompileError &error);


			/*
				Outputting
			*/

			//Prints the output from the previous compilation
			//Whats printed is based on the given compiler output options
			[[nodiscard]] std::string PrintOutput(script_compiler::OutputOptions output_options = script_compiler::OutputOptions::SummaryAndUnits) const;
	};
} //ion::script

#endif