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

#ifndef _ION_SCRIPT_COMPILER_
#define _ION_SCRIPT_COMPILER_

#include <condition_variable>
#include <filesystem>
#include <future>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "IonScriptError.h"
#include "IonScriptTree.h"
#include "adaptors/IonFlatMap.h"
#include "utilities/IonConvert.h"

namespace ion::script
{
	using ScriptTrees = std::vector<ScriptTree>;

	namespace script_compiler::detail
	{
		enum class token_name
		{	
			Comment,
			Identifier,
			Function,
			Operator,
			Rule,
			Separator,
			Unit,
			UnknownSymbol,
			WhiteSpace,

			//Operator types
			BinaryOperator,
			UnaryOperator,

			//Literal types
			BooleanLiteral,
			HexLiteral,
			NumericLiteral,
			StringLiteral
		};

		struct lexical_token
		{
			token_name name = token_name::UnknownSymbol;
			std::string_view value;
			int line_number = 0;
		};

		using lexical_tokens = std::vector<lexical_token>;


		struct build_context;
		struct compile_worker_pool;

		struct compile_worker
		{
			std::future<std::optional<lexical_tokens>> process;
			compile_worker(std::string str, build_context context, compile_worker_pool &worker_pool);
		};

		struct compile_worker_pool
		{
			adaptors::FlatMap<std::filesystem::path, compile_worker> workers;
			int processes = 0;
			std::condition_variable cv;
			std::mutex m;

			void start(std::string str, build_context context, compile_worker_pool &worker_pool);
		};


		struct build_context
		{
			std::filesystem::path root_path;
			std::vector<std::filesystem::path> file_hierarchy;
				//File hierarchy is an unique stack of files (direct-line inclusion)
				//front() returns the root file path and back returns the current file path


			/*
				Observers
			*/

			inline auto& current_file_path() const noexcept
			{
				return file_hierarchy.back();
			}

			inline auto& root_file_path() const noexcept
			{
				return file_hierarchy.front();
			}

			
			/*
				Stack operations
			*/

			bool push_file(std::filesystem::path file_path);
			void pop_file();
		};

		struct syntax_context
		{
			lexical_token *next_token = nullptr;
			lexical_token *previous_token = nullptr;

			int curly_brace_depth = 0;
			int parenthesis_depth = 0;

			bool inside_import = false;
			bool inside_object_signature = false;
			bool inside_property = false;
			bool inside_function = false;
			bool inside_calc_function = false;
		};

		struct parse_context
		{
			struct scope
			{
				script_tree::ObjectNodes objects;
				script_tree::PropertyNodes properties;
			};

			std::vector<scope> scopes;	
			script_tree::ArgumentNodes property_arguments;
			script_tree::ArgumentNodes function_arguments;
			
			std::vector<lexical_token*> object_tokens;
			lexical_token *identifier_token = nullptr;	
			lexical_token *property_token = nullptr;
			lexical_token *function_token = nullptr;

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

		std::optional<std::filesystem::path> full_import_path(std::filesystem::path file_path, const build_context &context);
		bool import_file(std::filesystem::path file_path, build_context &context, std::string &str, CompileError &error);

		/*
			Lexing
		*/

		std::pair<std::string_view, int> get_comment_lexeme(std::string_view str) noexcept;
		std::string_view get_hex_literal_lexeme(std::string_view str) noexcept;
		std::string_view get_identifer_lexeme(std::string_view str) noexcept;
		std::string_view get_numeric_literal_lexeme(std::string_view str) noexcept;
		std::pair<std::string_view, int> get_string_literal_lexeme(std::string_view str) noexcept;
		std::pair<std::string_view, int> get_white_space_lexeme(std::string_view str) noexcept;

		std::optional<lexical_tokens> lex(std::string_view str, build_context context, compile_worker_pool &worker_pool, CompileError &error);

		
		/*
			Parsing
		*/

		//Syntax checking

		bool check_function_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
		bool check_identifier_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
		bool check_literal_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
		bool check_operator_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
		bool check_rule_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
		bool check_separator_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;
		bool check_unit_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept;

		bool check_syntax(lexical_tokens &tokens, CompileError &error) noexcept;

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
		bool parse_literal(lexical_token &token, parse_context &context, CompileError &error);
		bool parse_unary_operator(lexical_token &token, parse_context &context, CompileError &error) noexcept;
		bool parse_separator(lexical_token &token, parse_context &context, CompileError &error);
		bool parse_unit(lexical_token &token, parse_context &context, CompileError &error) noexcept;

		void pre_parse(lexical_tokens &tokens) noexcept;
		std::optional<ScriptTree> parse(lexical_tokens tokens, compile_worker_pool &worker_pool, CompileError &error);


		/*
			Compiling
		*/
	
		std::optional<ScriptTree> compile(std::string str, build_context context, CompileError &error);
		std::optional<lexical_tokens> partial_compile(std::string str, build_context context, compile_worker_pool &worker_pool);
	} //script_compiler::detail


	class ScriptCompiler final
	{
		private:

			bool use_multi_core_ = true;
			bool stop_on_cyclic_import_ = true;

		public:

			ScriptCompiler() = default;


			/*
				Compiling
			*/

			//Compile the given script file by lexing, parsing and linking it.
			//Returns a ScriptTree that contains objects and object properties
			[[nodiscard]] std::optional<ScriptTree> Compile(std::filesystem::path file_path, CompileError &error);	

			//Compile the given script file and root path by lexing, parsing and linking it.
			//Returns a ScriptTree that contains objects and object properties
			[[nodiscard]] std::optional<ScriptTree> Compile(std::filesystem::path file_path, std::filesystem::path root_path, CompileError &error);			
	};
} //ion::script

#endif