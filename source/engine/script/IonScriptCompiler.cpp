/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptCompiler.cpp
-------------------------------------------
*/

#include "IonScriptCompiler.h"

#include <algorithm>

#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "types/IonTypes.h"
#include "utilities/IonConvert.h"
#include "utilities/IonFileUtility.h"
#include "utilities/IonParseUtility.h"
#include "utilities/IonStringUtility.h"

namespace ion::script
{

using namespace script_compiler;
using namespace script_error;
using namespace types::type_literals;

namespace script_compiler::detail
{

using namespace std::string_view_literals;


void build_system::start_process(std::string str, file_trace trace)
{
	auto file_path = trace.current_file_path().string();
	auto file_path_size = std::size(file_path);
	heapify(file_path);

	auto str_size = std::size(str);
	heapify(str);

	std::string_view file_path_view;	
	std::string_view str_view;	
	{
		std::lock_guard lock{m};
		translation_units.push_back({std::move(file_path), std::move(str)});
		file_path_view = {std::data(translation_units.back().file_path), file_path_size};
		str_view = {std::data(translation_units.back().source), str_size};	
	}

	auto id = ion::utilities::string::ToLowerCaseCopy(std::string{file_path_view});
	compilations.RunTask(std::move(id), partial_compile, str_view, file_path_view, std::move(trace), std::ref(*this));
}

bool file_trace::push_file(std::filesystem::path file_path)
{
	auto iter = std::find_if(std::cbegin(stack), std::cend(stack),
		[&](const auto &x) noexcept
		{
			std::error_code error;
			return std::filesystem::equivalent(file_path, x, error);
		});
	
	if (iter == std::cend(stack))
	{
		stack.push_back(std::move(file_path));
		return true;
	}
	else
		return false; //Cyclic import
}

void file_trace::pop_file()
{
	//Don't pop back first file (entry)
	if (std::size(stack) > 1)
		stack.pop_back();
}


void heapify(std::string &str, char padding_character)
{
	auto first = reinterpret_cast<size_t>(&str);
	auto last = first + sizeof(str);

	//Make sure the internal data of 'str' is allocated on the heap (non-SSO)
	while (reinterpret_cast<size_t>(std::data(str)) <= last &&
		   reinterpret_cast<size_t>(std::data(str)) >= first)
		str.push_back(padding_character); //Pad
}

std::optional<std::filesystem::path> full_file_path(std::filesystem::path file_path,
	const std::filesystem::path &root_path, const std::filesystem::path &current_path)
{
	std::error_code error;

	//File path is relative to...
	if (file_path.is_relative())
	{
		//root path
 		if (std::empty(current_path))
			file_path = root_path / std::filesystem::relative(file_path, root_path);
		//root path
		else if (auto str = file_path.string(); str.front() == '/' || str.front() == '\\')
			file_path = root_path / file_path;
		//current path
		else
			file_path = current_path / file_path;
	}

	return !error && ion::utilities::file::IsFile(file_path) ?
		std::make_optional(std::move(file_path)) :
		std::nullopt;
}

bool open_file(const std::filesystem::path &file_path, const std::filesystem::path &root_path,
	file_trace &trace, std::string &str, CompileError &error)
{
	if (auto full_path = full_file_path(file_path, root_path,
		!std::empty(trace.stack) ? trace.current_file_path().parent_path() : ""); full_path)
	{
		if (ion::utilities::file::Load(*full_path, str, ion::utilities::file::FileLoadMode::Binary) &&
			trace.push_file(full_path->lexically_normal()))
			return true;
		else
		{
			error = {CompileErrorCode::CircularImport, trace.current_file_path()};						
			return false;
		}		
	}
	else
	{
		error = {CompileErrorCode::InvalidFilePath,
			!std::empty(trace.stack) ? trace.current_file_path() : file_path.lexically_normal()};
		return false;
	}
}


/*
	Lexing
*/

std::pair<std::string_view, int> get_comment_lexeme(std::string_view str) noexcept
{
	//Line comment
	if (str[1] == '/')
	{
		if (auto off = str.find('\n', 2); off != std::string_view::npos)
			str.remove_suffix(std::size(str) - off);
		
		return {str, 0};
	}
	//Block comment
	else
	{
		if (auto off = str.find("*/", 2); off != std::string_view::npos)
			str.remove_suffix(std::size(str) - (off + 2));
		
		return {str, std::count(std::begin(str), std::end(str), '\n')};
	}
}

std::string_view get_hex_literal_lexeme(std::string_view str) noexcept
{
	//#<Hexadecimal>
	if (auto iter = std::find_if(std::begin(str) + 1, std::end(str), std::not_fn(is_hex_digit));
		iter != std::end(str))
		str.remove_suffix(std::size(str) - (iter - std::begin(str)));

	return str;
}

std::string_view get_identifer_lexeme(std::string_view str) noexcept
{
	if (auto iter = std::find_if(std::begin(str) + 1, std::end(str), std::not_fn(is_valid_in_identifier));
		iter != std::end(str))
		str.remove_suffix(std::size(str) - (iter - std::begin(str)));
	
	return str;
}

std::string_view get_numeric_literal_lexeme(std::string_view str) noexcept
{
	auto has_decimal_separator = str.front() == '.';
	auto has_exponent_part = false;

	//Whole and decimal numbers
	//Coefficient when scientific notation
	if (auto iter = std::find_if(std::begin(str) + static_cast<int>(has_decimal_separator) + 1, std::end(str),
		[&](auto c) noexcept
		{
			if (!ion::utilities::convert::detail::is_digit(c))
			{
				switch (c)
				{
					case '.':
					{
						if (has_decimal_separator)
							return true;

						has_decimal_separator = true;
						break;
					}

					case 'E':
					case 'e':
					has_exponent_part = true;
					[[fallthrough]];

					default:
					return true;
				}	
			}

			return false;
		}); iter != std::end(str))
	{
		//Scientific notation (base 10)
		if (has_exponent_part && std::end(str) - iter > 1)
		{
			auto exp_iter = iter + 1;

			//E-, E+
			switch (*exp_iter)
			{
				case '-':
				case '+':
				++exp_iter;
				break;
			}

			//Exponent part
			if (auto iter2 = std::find_if(exp_iter, std::end(str),
				std::not_fn(ion::utilities::convert::detail::is_digit));
				iter2 != exp_iter)

				iter = iter2;
		}

		str.remove_suffix(std::size(str) - (iter - std::begin(str)));
	}

	return str;
}

std::pair<std::string_view, int> get_string_literal_lexeme(std::string_view str) noexcept
{
	auto line_breaks = 0;

	if (auto iter = std::find_if(std::begin(str) + 1, std::end(str),
		[&, escaped = false](auto c) mutable noexcept
		{
			//Double or single quote
			if (c == str.front())
			{
				if (!escaped)
					return true;
			}
			else
			{
				switch (c)
				{
					case '\\':
					escaped = !escaped;
					return false;

					case '\n':
					{
						if (!escaped)
							return true;
						else
							++line_breaks;

						break;
					}	
				}
			}

			return (escaped = false);
		}); iter != std::end(str))

		str.remove_suffix(std::size(str) - (iter - std::begin(str) + (*iter != '\n')));

	return {str, line_breaks};
}

std::pair<std::string_view, int> get_white_space_lexeme(std::string_view str) noexcept
{
	//White space
	if (auto iter = std::find_if(std::begin(str) + 1, std::end(str), std::not_fn(is_white_space));
		iter != std::end(str))
		str.remove_suffix(std::size(str) - (iter - std::begin(str)));

	return {str, std::count(std::begin(str), std::end(str), '\n')};
}

std::optional<lexical_tokens> lex(std::string_view str, std::string_view file_path, file_trace trace, build_system &system, CompileError &error)
{
	lexical_tokens tokens;
	lexical_token token;
	auto line_number = 1;
	
	auto inside_import = false;
	auto import_argument = ""sv;

	for (auto iter = std::begin(str), end = std::end(str); iter != end;)
	{
		auto c = *iter;
		auto next_c = iter + 1 != end ? *(iter + 1) : '\0';
		auto off = iter - std::begin(str);

		//White space
		if (is_white_space(c))
		{
			auto [lexeme, line_breaks] = get_white_space_lexeme(str.substr(off));
			token = {token_name::WhiteSpace, lexeme, file_path, line_number += line_breaks};
		}
		//Separator
		else if (is_separator(c))
			token = {token_name::Separator, str.substr(off, 1), file_path, line_number};
		//String literal
		else if (is_start_of_string_literal(c))
		{
			auto [lexeme, line_breaks] = get_string_literal_lexeme(str.substr(off));
			token = {token_name::StringLiteral, lexeme, file_path, line_number += line_breaks};
		}
		//Comment
		else if (is_start_of_comment(c, next_c)) //Must be checked before operator, because of slash (/)
		{
			auto [lexeme, line_breaks] = get_comment_lexeme(str.substr(off));
			token = {token_name::Comment, lexeme, file_path, line_number += line_breaks};
		}
		//Identifier
		else if (is_start_of_identifier(c, next_c) || //Must be checked before operator, because of hyphen (-)
				(c == '%' && !std::empty(tokens) && tokens.back().name == token_name::NumericLiteral)) //Allow percent (%) if unit
		{
			auto lexeme = get_identifer_lexeme(str.substr(off));
			token = {
				//Check for reserved identifiers
				[&]() noexcept
				{
					//Unit, identifier right after a numeric literal
					if (!std::empty(tokens) && tokens.back().name == token_name::NumericLiteral) //Must be checked first
						return token_name::Unit;
					//Boolean literal
					else if (is_boolean_literal(lexeme))
						return token_name::BooleanLiteral;
					//Function
					else if (is_function(lexeme))
						return token_name::Function;
					//Identifier
					else
						return token_name::Identifier;
				}(), lexeme, file_path, line_number};
		}
		//Operator
		else if (is_operator(c)) //Must be checked after comment and identifier, because of division (/) and minus (-)
			token = {token_name::Operator, str.substr(off, 1), file_path, line_number};
		//Numeric literal
		else if (is_start_of_numeric_literal(c, next_c))
			token = {token_name::NumericLiteral, get_numeric_literal_lexeme(str.substr(off)), file_path, line_number};
		//Hex literal
		else if (is_start_of_hex_literal(c, next_c))
			token = {token_name::HexLiteral, get_hex_literal_lexeme(str.substr(off)), file_path, line_number};
		//Rule
		else if (is_start_of_rule(c))
			token = {token_name::Rule, get_identifer_lexeme(str.substr(off)), file_path, line_number};
		//Unknown symbol
		else
			token = {token_name::UnknownSymbol, str.substr(off, 1), file_path, line_number};
		
		iter += std::size(token.value);
		tokens.push_back(token);


		//@import "argument";
		if (!inside_import)
		{
			if (token.name == token_name::Rule)
				inside_import = is_import_rule(token.value);
		}
		else if (token.name != token_name::WhiteSpace &&
				 token.name != token_name::Comment)
		{
			if (std::empty(import_argument))
			{
				if (token.name == token_name::StringLiteral)
					import_argument = token.value;
				else
					inside_import = false;
			}
			else
			{
				if (token.name == token_name::Separator &&
					token.value.front() == ';')
				{
					if (auto result = utilities::parse::AsString(import_argument); result)
					{
						if (std::string imported_str; open_file(std::move(*result), system.root_path, trace, imported_str, error))
						{
							system.start_process(std::move(imported_str), trace);
							trace.pop_file();
						}
						else
						{
							//Imbue generated error with line number
							error.LineNumber = token.line_number;
							return {};
						}
					}
				}
				
				inside_import = false;
				import_argument = "";
			}
		}
	}

	return tokens;
}


/*
	Parsing
*/

bool check_function_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept
{
	if (!context.inside_property)
	{
		error = {CompileErrorCode::UnexpectedFunction, token.file_path, token.line_number};
		return false;
	}
	else if (context.inside_function)
	{
		error = {CompileErrorCode::UnexpectedFunction, token.file_path, token.line_number};
		return false;
	}
	else if (!context.next_token ||
			context.next_token->name != token_name::Separator ||
			context.next_token->value.front() != '(')
	{
		error = {CompileErrorCode::MissingOpenParenthesis, token.file_path, token.line_number};
		return false;
	}

	context.inside_function = true;
	context.inside_calc_function = token.value == "calc";
	return true;
}

bool check_identifier_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept
{
	if (context.inside_object_signature)
	{
		error = {CompileErrorCode::UnexpectedIdentifier, token.file_path, token.line_number};
		return false;
	}
	else if (!context.inside_property &&

			(!context.next_token ||
			//object "argument"
			(context.next_token->name != token_name::StringLiteral &&

			//object {
			//property :
			(context.next_token->name != token_name::Separator ||
				(context.next_token->value.front() != '{' && context.next_token->value.front() != ':')))))
	{
		error = {CompileErrorCode::UnexpectedIdentifier, token.file_path, token.line_number};
		return false;
	}

	context.inside_object_signature =
		!context.inside_property &&
		!(context.next_token &&
		context.next_token->name == token_name::Separator &&
		context.next_token->value.front() == ':');
	return true;
}

bool check_literal_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept
{
	if (token.name == token_name::StringLiteral)
	{
		//"" or ''
		//A full string correctness check is done later in parse_literal
		if (token.value.back() != token.value.front())
		{
			error = {CompileErrorCode::InvalidStringLiteral, token.file_path, token.line_number};
			return false;
		}
		else if (!context.inside_import && !context.inside_object_signature && !context.inside_property)
		{
			error = {CompileErrorCode::UnexpectedLiteral, token.file_path, token.line_number};
			return false;
		}
		//"argument";
		else if (context.inside_import &&
				(!context.next_token ||
				context.next_token->name != token_name::Separator ||
				context.next_token->value.front() != ';'))
		{
			error = {CompileErrorCode::InvalidImportStatement, token.file_path, token.line_number};
			return false;
		}
		//"classes/selectors" {
		else if (context.inside_object_signature &&
				(!context.next_token ||
				context.next_token->name != token_name::Separator ||
				context.next_token->value.front() != '{'))
		{
			error = {CompileErrorCode::MissingOpenCurlyBrace, token.file_path, token.line_number};
			return false;
		}
	}
	else
	{
		if (!context.inside_property)
		{
			error = {CompileErrorCode::UnexpectedLiteral, token.file_path, token.line_number};
			return false;
		}

		if (token.name == token_name::HexLiteral)
		{
			if (auto length = std::size(token.value);
				//Hex: #rrggbb #rrggbbaa
				length != 7 && length != 9 &&
				//Shorthand: #rgb #rgba
				length != 4 && length != 5)
			{
				error = {CompileErrorCode::InvalidHexLiteral, token.file_path, token.line_number};
				return false;
			}
		}
	}

	return true;
}

bool check_operator_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept
{
	if (!context.inside_property)
	{
		error = {CompileErrorCode::UnexpectedOperator, token.file_path, token.line_number};
		return false;
	}

	token.name =
		[&]() noexcept
		{
			switch (token.value.front())
			{
				//Could be unary
				case '-':
				case '+':
				{
					//Previous token is always available here
					if (context.previous_token &&
						context.previous_token->name == token_name::BinaryOperator ||
						(context.previous_token->name == token_name::Separator &&
							(context.previous_token->value.front() == ':' ||
							context.previous_token->value.front() == '(' ||
							context.previous_token->value.front() == ',')))
							
								return token_name::UnaryOperator;
			
					[[fallthrough]];
				}

				default:
				return token_name::BinaryOperator;
			}
		}();

	//Inside calc()
	//Expressions allowed
	if (context.inside_calc_function)
	{
		//Check right operand
		if (token.name == token_name::UnaryOperator ||
			token.name == token_name::BinaryOperator)
		{
			if (!( //Not
					//Right hand operand
					context.next_token &&
					(context.next_token->name == token_name::NumericLiteral ||
					context.next_token->name == token_name::Operator ||
					(context.next_token->name == token_name::Separator &&
						context.next_token->value.front() == '('))
				))
			{
				error = {CompileErrorCode::InvalidRightOperand, token.file_path, token.line_number};
				return false;
			}
		}
		//Check left operand
		if (token.name == token_name::BinaryOperator)
		{
			if (!( //Not
					//Left hand operand
					context.previous_token &&
					(context.previous_token->name == token_name::NumericLiteral ||
					context.previous_token->name == token_name::Unit ||
					(context.previous_token->name == token_name::Separator &&
						context.previous_token->value.front() == ')'))
				))
			{
				error = {CompileErrorCode::InvalidLeftOperand, token.file_path, token.line_number};
				return false;
			}
		}
	}
	//Not inside calc()
	//Expressions not allowed (only simple unary operators)
	else
	{
		if (token.name == token_name::BinaryOperator)
		{
			error = {CompileErrorCode::UnexpectedBinaryOperator, token.file_path, token.line_number};
			return false;
		}
		else if (!context.next_token ||
				context.next_token->name != token_name::NumericLiteral)
		{
			error = {CompileErrorCode::UnexpectedUnaryOperator, token.file_path, token.line_number};
			return false;
		}
	}

	return true;
}

bool check_rule_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept
{
	if (is_import_rule(token.value))
	{
		if (context.curly_brace_depth > 0)
		{
			error = {CompileErrorCode::UnexpectedImportStatement, token.file_path, token.line_number};
			return false;
		}
		//@import "argument"
		else if (context.next_token->name != token_name::StringLiteral)
		{
			error = {CompileErrorCode::InvalidImportStatement, token.file_path, token.line_number};
			return false;
		}

		context.inside_import = true;
	}

	return true;
}

bool check_separator_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept
{
	switch (token.value.front())
	{
		case ':':
		{
			if (context.curly_brace_depth == 0)
			{
				error = {CompileErrorCode::UnexpectedColon, token.file_path, token.line_number};
				return false;
			}
			else if (context.inside_property)
			{
				error = {CompileErrorCode::UnexpectedColon, token.file_path, token.line_number};
				return false;
			}

			context.inside_property = true;
			break;
		}

		case ';':
		{
			if (!context.inside_property && !context.inside_import)
			{
				error = {CompileErrorCode::UnexpectedSemicolon, token.file_path, token.line_number};
				return false;
			}
			else if (context.parenthesis_depth > 0)
			{
				error = {CompileErrorCode::MissingCloseParenthesis, token.file_path, token.line_number};
				return false;
			}

			context.inside_import = false;
			context.inside_property = false;
			break;
		}

		case '{':
		{
			if (!context.inside_object_signature)
			{
				error = {CompileErrorCode::UnexpectedOpenCurlyBrace, token.file_path, token.line_number};
				return false;
			}

			++context.curly_brace_depth;
			context.inside_object_signature = false;
			break;
		}

		case '}':
		{
			if (context.inside_property)
			{
				error = {CompileErrorCode::UnexpectedCloseCurlyBrace, token.file_path, token.line_number};
				return false;
			}
			else if (context.curly_brace_depth == 0)
			{
				error = {CompileErrorCode::UnmatchedCloseCurlyBrace, token.file_path, token.line_number};
				return false;
			}

			--context.curly_brace_depth;
			break;
		}

		case '(':
		{
			if (!context.inside_function) 
			{
				error = {CompileErrorCode::UnexpectedOpenParenthesis, token.file_path, token.line_number};
				return false;
			}
			else if (!context.inside_calc_function && context.parenthesis_depth > 0)
			{
				error = {CompileErrorCode::UnexpectedOpenParenthesis, token.file_path, token.line_number};
				return false;
			}
			else if (context.next_token && context.next_token->name == token_name::Separator &&
					 context.next_token->value.front() == ')')
			{
				error = {CompileErrorCode::EmptyParentheses, token.file_path, token.line_number};
				return false;
			}

			++context.parenthesis_depth;
			break;
		}

		case ')':
		{
			if (!context.inside_function) 
			{
				error = {CompileErrorCode::UnexpectedCloseParenthesis, token.file_path, token.line_number};
				return false;
			}
			else if (context.parenthesis_depth == 0)
			{
				error = {CompileErrorCode::UnmatchedCloseParenthesis, token.file_path, token.line_number};
				return false;
			}

			if (--context.parenthesis_depth == 0)
			{
				context.inside_function = false;
				context.inside_calc_function = false;
			}

			break;
		}

		case ',':
		{
			if (!context.inside_function)
			{
				error = {CompileErrorCode::UnexpectedComma, token.file_path, token.line_number};
				return false;
			}
			else if (context.next_token && context.next_token->name == token_name::Separator &&
					 context.next_token->value.front() == ',')
			{
				error = {CompileErrorCode::EmptyFunctionArgument, token.file_path, token.line_number};
				return false;
			}

			break;
		}
	}

	return true;
}

bool check_unit_syntax(lexical_token &token, syntax_context&, CompileError &error) noexcept
{
	if (!is_unit(token.value))
	{
		error = {CompileErrorCode::InvalidUnit, token.file_path, token.line_number};
		return false;
	}

	return true;
}

bool check_syntax(lexical_tokens &tokens, CompileError &error) noexcept
{
	syntax_context context;

	for (auto iter = std::begin(tokens), end = std::end(tokens);
		iter != end; ++iter)
	{
		auto &token = *iter;

		context.next_token = iter + 1 != end ? &*(iter + 1) : nullptr;
		context.previous_token = iter != std::begin(tokens) ? &*(iter - 1) : nullptr;

		switch (token.name)
		{
			//Function
			case token_name::Function:
			{
				if (!check_function_syntax(token, context, error))
					return false;

				break;
			}

			//Identifier
			case token_name::Identifier:
			{
				if (!check_identifier_syntax(token, context, error))
					return false;

				break;
			}

			//Operator
			case token_name::Operator:
			{
				if (!check_operator_syntax(token, context, error))
					return false;
				
				break;
			}

			//Rule
			case token_name::Rule:
			{
				if (!check_rule_syntax(token, context, error))
					return false;

				break;
			}

			//Separator
			case token_name::Separator:
			{
				if (!check_separator_syntax(token, context, error))
					return false;

				break;
			}
			
			//Unit
			case token_name::Unit:
			{
				if (!check_unit_syntax(token, context, error))
					return false;

				break;
			}

			//Unknown symbol
			case token_name::UnknownSymbol:
			{
				error = {CompileErrorCode::UnknownSymbol, token.file_path, token.line_number};
				return false;
			}

			//Literal
			default:
			{
				if (!check_literal_syntax(token, context, error))
					return false;
			}
		}
	}

	if (context.curly_brace_depth > 0)
	{
		error = {CompileErrorCode::MissingCloseCurlyBrace, tokens.back().file_path, tokens.back().line_number};
		return false;
	}

	return true;
}


void link(lexical_tokens &tokens, adaptors::FlatMap<std::string, compile_result> results, build_system &system)
{
	for (auto iter = std::begin(tokens); iter != std::end(tokens);)
	{
		auto &token = *iter;

		//Link in imported tokens from another file
		if (token.name == token_name::Rule &&
			is_import_rule(token.value))
		{
			auto file_path = utilities::parse::AsString((iter + 1)->value);
			auto full_path = *full_file_path(*file_path, system.root_path, std::filesystem::path{token.file_path}.parent_path());
			auto id = ion::utilities::string::ToLowerCaseCopy(full_path.string());

			if (auto it = results.find(id); it != std::end(results))
			{
				auto &[key, value] = *it;

				iter = tokens.erase(iter, iter + 3);
				iter = tokens.insert(iter, std::begin(*value.tokens), std::end(*value.tokens));	
				continue;
			}
		}

		++iter;
	}
}


std::optional<script_tree::ArgumentType> call_cmyk(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	//Check number of arguments
	if ((token.value == "cmyk" && std::size(arguments) != 4) ||
		(token.value == "cmyka" && std::size(arguments) != 5))
	{
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const script_tree::IntegerArgument &arg) mutable noexcept
			{
				//As percent
				argument = script_tree::ArgumentType{script_tree::FloatingPointArgument{
					arg.Value() * script_tree::FloatingPointArgument::value_type{0.01}}};
			},
			//Floating point
			[](const script_tree::FloatingPointArgument&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.file_path, token.line_number};
			});

		if (error)
			return {};
	}

	auto color = graphics::utilities::Color::CMYK(
		arguments[0].Get<script_tree::FloatingPointArgument>()->As<real>(),
		arguments[1].Get<script_tree::FloatingPointArgument>()->As<real>(),
		arguments[2].Get<script_tree::FloatingPointArgument>()->As<real>(),
		arguments[3].Get<script_tree::FloatingPointArgument>()->As<real>());

	//Alpha component
	if (std::size(arguments) == 5)
		color.A(arguments[4].Get<script_tree::FloatingPointArgument>()->As<real>());

	return std::optional<script_tree::ColorArgument>(color);
}

std::optional<script_tree::ArgumentType> call_hsl(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	//Check number of arguments
	if ((token.value == "hsl" && std::size(arguments) != 3) ||
		(token.value == "hsla" && std::size(arguments) != 4))
	{
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const script_tree::IntegerArgument &arg) mutable noexcept
			{
				argument =
					&argument == &arguments.front() ?
					//Hue as is
					script_tree::ArgumentType{script_tree::FloatingPointArgument{
						arg.As<script_tree::FloatingPointArgument::value_type>()}} :
					//Saturation, lightness and alpha as percent
					script_tree::ArgumentType{script_tree::FloatingPointArgument{
						arg.Value() * script_tree::FloatingPointArgument::value_type{0.01}}};
			},
			//Floating point
			[](const script_tree::FloatingPointArgument&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.file_path, token.line_number};
			});

		if (error)
			return {};
	}

	auto color = graphics::utilities::Color::HSL(
		arguments[0].Get<script_tree::FloatingPointArgument>()->As<real>(),
		arguments[1].Get<script_tree::FloatingPointArgument>()->As<real>(),
		arguments[2].Get<script_tree::FloatingPointArgument>()->As<real>());

	//Alpha component
	if (std::size(arguments) == 4)
		color.A(arguments[3].Get<script_tree::FloatingPointArgument>()->As<real>());

	return std::optional<script_tree::ColorArgument>(color);
}

std::optional<script_tree::ArgumentType> call_hwb(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	//Check number of arguments
	if ((token.value == "hwb" && std::size(arguments) != 3) ||
		(token.value == "hwba" && std::size(arguments) != 4))
	{
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const script_tree::IntegerArgument &arg) mutable noexcept
			{
				argument =
					&argument == &arguments.front() ?
					//Hue as is
					script_tree::ArgumentType{script_tree::FloatingPointArgument{
						arg.As<script_tree::FloatingPointArgument::value_type>()}} :
					//Whiteness, blackness and alpha as percent
					script_tree::ArgumentType{script_tree::FloatingPointArgument{
						arg.Value() * script_tree::FloatingPointArgument::value_type{0.01}}};
			},
			//Floating point
			[](const script_tree::FloatingPointArgument&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.file_path, token.line_number};
			});

		if (error)
			return {};
	}

	auto color = graphics::utilities::Color::HWB(
		arguments[0].Get<script_tree::FloatingPointArgument>()->As<real>(),
		arguments[1].Get<script_tree::FloatingPointArgument>()->As<real>(),
		arguments[2].Get<script_tree::FloatingPointArgument>()->As<real>());

	//Alpha component
	if (std::size(arguments) == 4)
		color.A(arguments[3].Get<script_tree::FloatingPointArgument>()->As<real>());

	return std::optional<script_tree::ColorArgument>(color);
}

std::optional<script_tree::ArgumentType> call_rgb(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	//Check number of arguments
	if ((token.value == "rgb" && std::size(arguments) != 3) ||
		(token.value == "rgba" && std::size(arguments) != 4))
	{
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const script_tree::IntegerArgument &arg) mutable noexcept
			{
				argument =
					&argument == &arguments.back() && std::size(arguments) == 4 ?
					//Alpha as percent
					script_tree::ArgumentType{script_tree::FloatingPointArgument{
						arg.Value() * script_tree::FloatingPointArgument::value_type{0.01}}} :
					//RGB divide by 255
					script_tree::ArgumentType{script_tree::FloatingPointArgument{
						arg.Value() / script_tree::FloatingPointArgument::value_type{255.0}}};
			},
			//Floating point
			[](const script_tree::FloatingPointArgument&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.file_path, token.line_number};
			});

		if (error)
			return {};
	}

	auto color = graphics::utilities::Color{
		arguments[0].Get<script_tree::FloatingPointArgument>()->As<real>(),
		arguments[1].Get<script_tree::FloatingPointArgument>()->As<real>(),
		arguments[2].Get<script_tree::FloatingPointArgument>()->As<real>()};

	//Alpha component
	if (std::size(arguments) == 4)
		color.A(arguments[3].Get<script_tree::FloatingPointArgument>()->As<real>());

	return std::optional<script_tree::ColorArgument>(color);
}

std::optional<script_tree::ArgumentType> call_vec2(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	if (std::size(arguments) != 1 &&
		std::size(arguments) != 2)
	{
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const script_tree::IntegerArgument &arg) mutable noexcept
			{
				argument = script_tree::ArgumentType{script_tree::FloatingPointArgument{
					arg.As<script_tree::FloatingPointArgument::value_type>()}};
			},
			//Floating point
			[](const script_tree::FloatingPointArgument&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.file_path, token.line_number};
			});

		if (error)
			return {};
	}

	return std::size(arguments) == 1 ?
		//Scalar
		std::optional<script_tree::Vector2Argument>(graphics::utilities::Vector2{
			arguments[0].Get<script_tree::FloatingPointArgument>()->As<real>()}) :
		//Two components
		std::optional<script_tree::Vector2Argument>(graphics::utilities::Vector2{
			arguments[0].Get<script_tree::FloatingPointArgument>()->As<real>(),
			arguments[1].Get<script_tree::FloatingPointArgument>()->As<real>()});
}

std::optional<script_tree::ArgumentType> call_function(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	//rgb/rgba
	if (token.value == "rgb" || token.value == "rgba")
		return call_rgb(token, arguments, error);
	//hsl/hsla
	else if (token.value == "hsl" || token.value == "hsla")
		return call_hsl(token, arguments, error);
	//hwb/hwba
	else if (token.value == "hwb" || token.value == "hwba")
		return call_hwb(token, arguments, error);
	//hsl/hsla
	else if (token.value == "cmyk" || token.value == "cmyka")
		return call_cmyk(token, arguments, error);
	//vec2
	else if (token.value == "vec2")
		return call_vec2(token, arguments, error);

	return {};
}


bool parse_function(lexical_token &token, parse_context &context, CompileError&) noexcept
{
	context.function_token = &token;
	return true;
}

bool parse_identifier(lexical_token &token, parse_context &context, CompileError&)
{
	//Function or property argument
	if (context.function_token || context.property_token)
	{
		auto argument =
			[&]() noexcept -> script_tree::ArgumentType
			{
				if (auto color = utilities::parse::detail::color_name_as_color(token.value); color)
					return script_tree::ColorArgument{*color};
				else
					return script_tree::EnumerableArgument{std::string{token.value}};
			}();

		//Function argument
		if (context.function_token)
			context.function_arguments.push_back(std::move(argument));
		//Property argument
		else
			context.property_arguments.push_back(std::move(argument));
	}
	else
		context.identifier_token = &token;

	return true;
}

bool parse_literal(lexical_token &token, parse_context &context, CompileError &error)
{
	switch (token.name)
	{
		//Boolean literal
		case token_name::BooleanLiteral:
		{
			auto result = utilities::parse::AsBoolean(token.value);

			if (result)
			{
				//Function argument
				if (context.function_token)
					context.function_arguments.emplace_back(script_tree::BooleanArgument{*result});
				//Property argument
				else if (context.property_token)
					context.property_arguments.emplace_back(script_tree::BooleanArgument{*result});
			}
			else
			{
				error = {CompileErrorCode::InvalidBooleanLiteral, token.file_path, token.line_number};
				return false;
			}

			break;
		}

		//Hex literal
		case token_name::HexLiteral:
		{
			auto result = utilities::parse::detail::hex_as_color(token.value);

			if (result)
			{
				//Function argument
				if (context.function_token)
					context.function_arguments.emplace_back(script_tree::ColorArgument{*result});
				//Property argument
				else if (context.property_token)
					context.property_arguments.emplace_back(script_tree::ColorArgument{*result});
			}
			else
			{
				error = {CompileErrorCode::InvalidHexLiteral, token.file_path, token.line_number};
				return false;
			}

			break;
		}

		//Numeric literal
		case token_name::NumericLiteral:
		{
			//Real
			if (utilities::parse::detail::parse_as_floating_point(token.value))
			{
				auto result = ion::utilities::convert::To<script_tree::FloatingPointArgument::value_type>(std::string{token.value});

				if (result)
				{
					//Function argument
					if (context.function_token)
						context.function_arguments.emplace_back(script_tree::FloatingPointArgument{context.unary_minus ? -*result : *result});
					//Property argument
					else if (context.property_token)
						context.property_arguments.emplace_back(script_tree::FloatingPointArgument{context.unary_minus ? -*result : *result});
				}
				else
				{
					error = {CompileErrorCode::InvalidNumericLiteral, token.file_path, token.line_number};
					return false;
				}
			}
			//Integer
			else
			{
				auto result = ion::utilities::convert::To<script_tree::IntegerArgument::value_type>(std::string{token.value});

				if (result)
				{
					//Function argument
					if (context.function_token)
						context.function_arguments.emplace_back(script_tree::IntegerArgument{context.unary_minus ? -*result : *result});
					//Property argument
					else if (context.property_token)
						context.property_arguments.emplace_back(script_tree::IntegerArgument{context.unary_minus ? -*result : *result});
				}
				else
				{
					error = {CompileErrorCode::InvalidNumericLiteral, token.file_path, token.line_number};
					return false;
				}
			}

			context.unary_minus = false;
			break;
		}

		//String literal
		case token_name::StringLiteral:
		{
			auto result = utilities::parse::AsString(token.value);

			if (result)
			{
				//Function argument
				if (context.function_token)
					context.function_arguments.emplace_back(script_tree::StringArgument{*result});
				//Property argument
				else if (context.property_token)
					context.property_arguments.emplace_back(script_tree::StringArgument{*result});
			}
			else
			{
				error = {CompileErrorCode::InvalidStringLiteral, token.file_path, token.line_number};
				return false;
			}

			break;
		}
	}

	return true;
}

bool parse_unary_operator(lexical_token &token, parse_context &context, CompileError&) noexcept
{
	context.unary_minus = token.value.front() == '-';
	return true;
}

bool parse_separator(lexical_token &token, parse_context &context, CompileError &error)
{
	switch (token.value.front())
	{
		case ':':
		context.property_token = context.identifier_token;
		break;

		case ';':
		{
			//End of property
			if (context.property_token)
			{
				context.scopes[context.scope_depth - 1].properties.emplace_back(
					std::string{context.property_token->value}, std::move(context.property_arguments));
				context.property_token = nullptr;
			}

			break;
		}

		case '{':
		{
			if (context.scope_depth == static_cast<int>(std::size(context.scopes)))
				context.scopes.emplace_back();

			context.object_tokens.push_back(context.identifier_token);
			++context.scope_depth;
			break;
		}

		case '}':
		{
			//Leaf object, no childrens
			if (auto &scope = context.scopes[context.scope_depth - 1];
				&scope == &context.scopes.back())
				scope.objects.emplace_back(std::string{context.object_tokens.back()->value}, std::move(scope.properties));
			//Take childrens (if any) from one depth deeper
			else
				scope.objects.emplace_back(std::string{context.object_tokens.back()->value}, std::move(scope.properties),
										   std::move(context.scopes[context.scope_depth].objects));

			context.object_tokens.pop_back();
			--context.scope_depth;
			break;
		}

		case ')':
		{
			//End of function
			if (context.function_token)
			{
				auto result = call_function(*context.function_token, std::move(context.function_arguments), error);

				if (result)
					context.property_arguments.emplace_back(std::move(*result));

				context.function_token = nullptr;
			}
		}
	}

	return true;
}

bool parse_unit(lexical_token &token, parse_context &context, CompileError&) noexcept
{
	auto &arguments = context.function_token ?
		//Function arguments
		context.function_arguments :
		//Property arguments
		context.property_arguments;

	auto &argument = arguments.back();
	argument.Visit(
		//Integer
		[&](const script_tree::IntegerArgument &arg) mutable noexcept
		{
			if (token.value == "%")
				argument = script_tree::ArgumentType{script_tree::FloatingPointArgument{
					arg.Value() * script_tree::FloatingPointArgument::value_type{0.01}}};
		},
		//Floating point
		[&](const script_tree::FloatingPointArgument &arg) mutable noexcept
		{
			if (token.value == "%")
				argument = script_tree::ArgumentType{script_tree::FloatingPointArgument{
					arg.Value() * script_tree::FloatingPointArgument::value_type{0.01}}};
		},
		//Default
		[](auto&&) noexcept {});
	return true;
}

void pre_parse(lexical_tokens &tokens) noexcept
{
	//Remove all white spaces and comments
	tokens.erase(
		std::remove_if(std::begin(tokens), std::end(tokens),
			[](auto &token) noexcept
			{
				return token.name == token_name::WhiteSpace ||
					   token.name == token_name::Comment;
			}), std::end(tokens));
}

std::optional<ScriptTree> parse(lexical_tokens tokens, build_system &system, CompileError &error)
{
	//Discard unnecessary tokens
	pre_parse(tokens);

	//Syntax error checking
	check_syntax(tokens, error);

	if (error)
		return {};


	//Wait for all external compilations to complete
	auto results = system.compilations.Get();

	for (auto &[key, value] : results)
	{
		if (value.error)
		{
			//Inherit error from the failed external compilation
			error = value.error;
			return {};
		}
	}

	//Link together tokens (main compilation) with the external tokens (external compilations)
	link(tokens, std::move(results), system);

	//Parse and build tree
	parse_context context;
	
	for (auto &token : tokens)
	{
		switch (token.name)
		{
			//Function
			case token_name::Function:
			{
				parse_function(token, context, error);
				break;
			}

			//Identifier
			case token_name::Identifier:
			{
				parse_identifier(token, context, error);
				break;
			}

			//Separator
			case token_name::Separator:
			{
				parse_separator(token, context, error);
				break;
			}
			
			//Unary operator
			case token_name::UnaryOperator:
			{
				parse_unary_operator(token, context, error);
				break;
			}

			//Unit
			case token_name::Unit:
			{
				parse_unit(token, context, error);
				break;
			}

			//Literals
			case token_name::BooleanLiteral:
			case token_name::HexLiteral:
			case token_name::NumericLiteral:
			case token_name::StringLiteral:
			{
				parse_literal(token, context, error);
				break;
			}
		}

		//An error has occurred
		if (error)
			return {};
	}

	return !std::empty(context.scopes) ?
		//Top-level objects
		std::make_optional<ScriptTree>(std::move(context.scopes.front().objects)) :
		std::nullopt;
}


/*
	Compiling
*/

std::optional<ScriptTree> compile(std::string_view str, std::string_view file_path, file_trace trace, build_system &system, CompileError &error)
{
	auto tokens = script_compiler::detail::lex(str, file_path, std::move(trace), system, error);
	return tokens ? script_compiler::detail::parse(std::move(*tokens), system, error) : std::nullopt;
}

compile_result partial_compile(std::string_view str, std::string_view file_path, file_trace trace, build_system &system)
{
	CompileError error;
	auto tokens = script_compiler::detail::lex(str, file_path, std::move(trace), system, error);

	if (tokens)
	{
		//Discard unnecessary tokens
		pre_parse(*tokens);

		//Syntax error checking
		check_syntax(*tokens, error);
	}

	return {error ? std::nullopt : std::move(tokens), error};
}

} //script_compiler::detail


//ScriptCompiler


/*
	Compiling
*/

std::optional<ScriptTree> ScriptCompiler::Compile(std::filesystem::path file_path, CompileError &error)
{
	auto root_path = file_path.parent_path();
	return Compile(std::move(file_path), std::move(root_path), error);
}

std::optional<ScriptTree> ScriptCompiler::Compile(std::filesystem::path file_path, std::filesystem::path root_path, CompileError &error)
{
	if (ion::utilities::file::IsDirectory(root_path))
	{
		script_compiler::detail::file_trace trace;
		
		if (std::string str; script_compiler::detail::open_file(file_path, root_path, trace, str, error))
		{
			script_compiler::detail::build_system system{std::move(root_path)};

			if (max_build_processes_)
				system.compilations.MaxWorkerThreads(*max_build_processes_);

			return script_compiler::detail::compile(str, trace.current_file_path().string(), trace, system, error);
		}
	}

	return {};
}

} //ion::script