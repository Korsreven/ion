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

#include "IonScriptTypes.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "types/IonTypes.h"
#include "timers/IonStopwatch.h"
#include "utilities/IonConvert.h"
#include "utilities/IonFileUtility.h"
#include "utilities/IonParseUtility.h"
#include "utilities/IonStringUtility.h"

namespace ion::script
{

using namespace script_compiler;
using namespace script_error;

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace types::type_literals;

namespace script_compiler::detail
{

//file_trace

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
	//Don't pop back first file (entry point)
	if (std::size(stack) > 1)
		stack.pop_back();
}

//build_system

build_system::build_system(const resources::files::repositories::ScriptRepository &repository) :
	repository{&repository}
{
	//Empty
}

build_system::build_system(std::filesystem::path root_path) :
	root_path{std::move(root_path)}
{
	//Empty
}

void build_system::start_process(std::string str, file_trace trace)
{
	auto file_path = trace.current_file_path().string();
	translation_unit *unit = nullptr;

	{
		std::lock_guard lock{m};

		units.push_back(std::make_unique<translation_unit>(
			translation_unit{std::move(file_path), std::move(str)}));
		unit = units.back().get();
	}

	auto id = ion::utilities::string::ToLowerCaseCopy(unit->file_path);
	processes.RunTask(std::move(id), partial_compile_unit, std::ref(*unit), std::move(trace), std::ref(*this));
}


std::optional<std::filesystem::path> full_file_path(std::filesystem::path file_path,
	const build_system &system, const std::filesystem::path &current_path)
{
	if (system.repository)
	{
		if (auto full_path = system.repository->FilePath(file_path.string()); full_path)
			file_path = *full_path;

		return std::make_optional(std::move(file_path));
	}

	//File path is relative to...
	if (file_path.is_relative())
	{
		//root path
 		if (std::empty(current_path))
			file_path = system.root_path / std::filesystem::relative(file_path, system.root_path);
		//root path
		else if (auto str = file_path.string(); str.front() == '/' || str.front() == '\\')
			file_path = system.root_path / file_path;
		//current path
		else
			file_path = current_path / file_path;
	}

	return ion::utilities::file::IsFile(file_path) ?
		std::make_optional(std::move(file_path)) :
		std::nullopt;
}

bool open_file(const std::filesystem::path &file_path, const build_system &system,
	file_trace &trace, std::string &str, CompileError &error)
{
	if (auto full_path = full_file_path(file_path, system,
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

bool load_from_repository(std::string_view name, const build_system &system,
	file_trace &trace, std::string &str, CompileError &error)
{
	if (auto file_path = system.repository->FilePath(name); file_path)
	{
		if (auto data = system.repository->FileData(name); data &&
			trace.push_file(file_path->lexically_normal()))
		{
			str = std::move(*data);
			return true;
		}
		else
		{
			error = {CompileErrorCode::CircularImport, trace.current_file_path()};
			return false;
		}
	}
	else
	{
		if (std::empty(trace.stack))
			error = {CompileErrorCode::InvalidResourceName, name};
		else
			error = {CompileErrorCode::InvalidFilePath, trace.current_file_path()};

		return false;
	}
}

bool import_unit(std::string str_argument, const build_system &system,
	file_trace &trace, std::string &str, CompileError &error)
{
	if (system.repository)
		return load_from_repository(str_argument, system, trace, str, error);
	else
		return open_file(std::move(str_argument), system, trace, str, error);
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
		[&, escaped = false, escaped_cr = false](auto c) mutable noexcept
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
					case '\\': //Backslash
					escaped = !escaped;
					return (escaped_cr = false);

					case '\r': //Carriage return
					escaped_cr = escaped;
					break;

					case '\n': //Line feed
					{
						if (!escaped && !escaped_cr)
							return true;
						else
							++line_breaks;

						[[fallthrough]];
					}
					
					default:
					escaped_cr = false;
					break;
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

std::optional<lexical_tokens> lex(translation_unit &unit, file_trace trace, build_system &system)
{
	std::string_view str = unit.source;
	lexical_tokens tokens;
	lexical_token token;
	auto line_number = 1;
	
	auto inside_import = false;
	auto inside_declaration = false;
	auto import_argument = ""sv;
	auto scope_depth = 0;

	for (auto iter = std::begin(str), end = std::end(str); iter != end;)
	{
		auto c = *iter;
		auto next_c = iter + 1 != end ? *(iter + 1) : '\0';
		auto off = iter - std::begin(str);

		//White space
		if (is_white_space(c))
		{
			auto [lexeme, line_breaks] = get_white_space_lexeme(str.substr(off));
			token = {token_name::WhiteSpace, lexeme, &unit, line_number += line_breaks};

			//For global scope only
			if (scope_depth == 0 && !std::empty(tokens) &&		
				tokens.back().name == token_name::Identifier &&
				is_class_identifier(tokens.back().value) &&
				is_class_selector(next_c))
					token.name = token_name::Selector; //Descendant selector
		}
		//Selector
		else if (is_selector(c) && scope_depth == 0 && !inside_declaration) //For global scope only
		{
			token = {token_name::Selector, str.substr(off, 1), &unit, line_number};

			if (token.value.front() == '*')
				token.name = token_name::Identifier; //Change to class identifier
		}
		//Separator
		else if (is_separator(c))
			token = {token_name::Separator, str.substr(off, 1), &unit, line_number};
		//String literal
		else if (is_start_of_string_literal(c))
		{
			auto [lexeme, line_breaks] = get_string_literal_lexeme(str.substr(off));
			token = {token_name::StringLiteral, lexeme, &unit, line_number += line_breaks};
		}
		//Comment
		else if (is_start_of_comment(c, next_c)) //Must be checked before operator, because of slash (/)
		{
			auto [lexeme, line_breaks] = get_comment_lexeme(str.substr(off));
			token = {token_name::Comment, lexeme, &unit, line_number += line_breaks};
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
				}(), lexeme, &unit, line_number};

			//Expand token to engulf class selector/variable prefix
			if (token.name == token_name::Identifier && !std::empty(tokens) &&
				((tokens.back().name == token_name::Selector && is_class_selector(tokens.back().value.front())) ||
				(tokens.back().name == token_name::UnknownSymbol && is_variable_prefix(tokens.back().value.front()))))
			{			
				token.value = str.substr(off - 1, std::size(lexeme) + 1); //Append class selector to identifier token
				tokens.pop_back(); //Remove class selector
				--iter;
			}
		}
		//Operator
		else if (is_operator(c)) //Must be checked after comment and identifier, because of division (/) and minus (-)
			token = {token_name::Operator, str.substr(off, 1), &unit, line_number};
		//Numeric literal
		else if (is_start_of_numeric_literal(c, next_c))
			token = {token_name::NumericLiteral, get_numeric_literal_lexeme(str.substr(off)), &unit, line_number};
		//Hex literal
		else if (is_start_of_hex_literal(c, next_c))
			token = {token_name::HexLiteral, get_hex_literal_lexeme(str.substr(off)), &unit, line_number};
		//Rule
		else if (is_start_of_rule(c))
			token = {token_name::Rule, get_identifer_lexeme(str.substr(off)), &unit, line_number};
		//Unknown symbol
		else
			token = {token_name::UnknownSymbol, str.substr(off, 1), &unit, line_number};

		iter += std::size(token.value);
		tokens.push_back(token);

		//For selectors (scope aware)
		if (token.name == token_name::Separator)
		{
			switch (token.value.front())
			{
				case '{':
				case '(':
				++scope_depth;
				break;

				case '}':
				case ')':
				{
					if (scope_depth > 0)
						--scope_depth;
					break;
				}

				case ':':
				inside_declaration = true;
				break;

				case ';':
				inside_declaration = false;
				break;
			}
		}


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
						if (std::string imported_str; import_unit(std::move(*result), system, trace, imported_str, unit.error))
						{
							system.start_process(std::move(imported_str), trace);
							trace.pop_file();
						}
						else
						{
							//Imbue generated error with line number
							unit.error.LineNumber = token.line_number;
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
	if (!context.inside_property && !context.inside_variable)
	{
		error = {CompileErrorCode::UnexpectedFunction, token.unit->file_path, token.line_number};
		return false;
	}
	else if (context.inside_function)
	{
		error = {CompileErrorCode::UnexpectedFunction, token.unit->file_path, token.line_number};
		return false;
	}
	else if (!context.next_token ||
			context.next_token->name != token_name::Separator ||
			context.next_token->value.front() != '(')
	{
		error = {CompileErrorCode::MissingOpenParenthesis, token.unit->file_path, token.line_number};
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
		error = {CompileErrorCode::UnexpectedIdentifier, token.unit->file_path, token.line_number};
		return false;
	}
	else if (is_variable_identifier(token.value)) //Variable
	{
		//Declaration
		//$var:
		if (context.next_token && context.next_token->name == token_name::Separator && context.next_token->value.front() == ':')
		{
			if (context.inside_property || context.inside_variable)
			{
				error = {CompileErrorCode::UnexpectedVariableDeclaration, token.unit->file_path, token.line_number};
				return false;
			}
		}
		//$var
		else if (!context.inside_property && !context.inside_variable)
		{
			error = {CompileErrorCode::UnexpectedIdentifier, token.unit->file_path, token.line_number};
			return false;
		}
	}
	else if (!is_class_identifier(token.value) && //Object/property
			!context.inside_property && !context.inside_variable && //Not enumerable

			//Not
			!(context.next_token &&

			//property:
			//object {	
			((context.next_token->name == token_name::Separator &&
				(context.next_token->value.front() == ':' || context.next_token->value.front() == '{')) ||
			//object "classes"
			context.next_token->name == token_name::StringLiteral)))
	{
		error = {CompileErrorCode::UnexpectedIdentifier, token.unit->file_path, token.line_number};
		return false;
	}
	else if (is_class_identifier(token.value) && //Class
			
			//Not
			!(context.next_token &&

			//class {	
			((context.next_token->name == token_name::Separator && context.next_token->value.front() == '{') ||
			//class "classes"
			context.next_token->name == token_name::StringLiteral ||		
			//class <combinator>
			context.next_token->name == token_name::Selector ||
			//class <class>
			(context.next_token->name == token_name::Identifier && is_class_identifier(context.next_token->value)))))
	{
		error = {CompileErrorCode::UnexpectedIdentifier, token.unit->file_path, token.line_number};
		return false;
	}

	context.inside_template_signature =
		is_class_identifier(token.value);

	context.inside_object_signature =
		!context.inside_template_signature &&
		!context.inside_property &&
		!context.inside_variable &&
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
			error = {CompileErrorCode::InvalidStringLiteral, token.unit->file_path, token.line_number};
			return false;
		}
		else if (!context.inside_import && !context.inside_object_signature &&
				 !context.inside_template_signature && !context.inside_property &&
				 !context.inside_variable)
		{
			error = {CompileErrorCode::UnexpectedLiteral, token.unit->file_path, token.line_number};
			return false;
		}
		//"argument";
		else if (context.inside_import &&
				(!context.next_token ||
				context.next_token->name != token_name::Separator ||
				context.next_token->value.front() != ';'))
		{
			error = {CompileErrorCode::MissingSemicolon, token.unit->file_path, token.line_number};
			return false;
		}
		//"classes" {
		else if ((context.inside_object_signature || context.inside_template_signature) &&
				(!context.next_token ||
				context.next_token->name != token_name::Separator ||
				context.next_token->value.front() != '{'))
		{
			error = {CompileErrorCode::MissingOpenCurlyBrace, token.unit->file_path, token.line_number};
			return false;
		}
	}
	else
	{
		if (!context.inside_property && !context.inside_variable)
		{
			error = {CompileErrorCode::UnexpectedLiteral, token.unit->file_path, token.line_number};
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
				error = {CompileErrorCode::InvalidHexLiteral, token.unit->file_path, token.line_number};
				return false;
			}
		}
	}

	return true;
}

bool check_operator_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept
{
	if (!context.inside_property && !context.inside_variable)
	{
		error = {CompileErrorCode::UnexpectedOperator, token.unit->file_path, token.line_number};
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
				error = {CompileErrorCode::InvalidRightOperand, token.unit->file_path, token.line_number};
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
				error = {CompileErrorCode::InvalidLeftOperand, token.unit->file_path, token.line_number};
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
			error = {CompileErrorCode::UnexpectedBinaryOperator, token.unit->file_path, token.line_number};
			return false;
		}
		else if (!context.next_token ||
				context.next_token->name != token_name::NumericLiteral)
		{
			error = {CompileErrorCode::UnexpectedUnaryOperator, token.unit->file_path, token.line_number};
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
			error = {CompileErrorCode::UnexpectedImportStatement, token.unit->file_path, token.line_number};
			return false;
		}
		//@import "argument"
		else if (!context.next_token ||
				context.next_token->name != token_name::StringLiteral)
		{
			error = {CompileErrorCode::InvalidImportStatement, token.unit->file_path, token.line_number};
			return false;
		}

		context.inside_import = true;
	}
	else
	{
		error = {CompileErrorCode::InvalidRule, token.unit->file_path, token.line_number};
		return false;
	}

	return true;
}

bool check_selector_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept
{
	//Class selectors, isolated (not appended to an identifier)
	if (is_class_selector(token.value.front()))
	{
		error = {CompileErrorCode::MissingIdentifier, token.unit->file_path, token.line_number};
		return false;
	}
	else //Combinator selectors including descendant (whitespace)
	{
		if (!( //Not
			//Left hand operand
			context.previous_token &&
			context.previous_token->name == token_name::Identifier &&
			is_class_identifier(context.previous_token->value)))
		{
			error = {CompileErrorCode::InvalidLeftOperand, token.unit->file_path, token.line_number};
			return false;
		}
		
		else if (!( //Not
				//Right hand operand
				context.next_token &&
				context.next_token->name == token_name::Identifier &&
				is_class_identifier(context.next_token->value)))
		{
			error = {CompileErrorCode::InvalidRightOperand, token.unit->file_path, token.line_number};
			return false;
		}
	}

	return true;
}

bool check_separator_syntax(lexical_token &token, syntax_context &context, CompileError &error) noexcept
{
	switch (token.value.front())
	{
		case ':':
		{
			if (context.curly_brace_depth == 0 &&
				(!context.previous_token || !is_variable_identifier(context.previous_token->value)))
			{
				error = {CompileErrorCode::UnexpectedColon, token.unit->file_path, token.line_number};
				return false;
			}
			else if (context.inside_property || context.inside_variable)
			{
				error = {CompileErrorCode::UnexpectedColon, token.unit->file_path, token.line_number};
				return false;
			}

			context.inside_variable = is_variable_identifier(context.previous_token->value);
			context.inside_property = !context.inside_variable;
			break;
		}

		case ';':
		{
			if (!context.inside_import && !context.inside_property && !context.inside_variable)
			{
				error = {CompileErrorCode::UnexpectedSemicolon, token.unit->file_path, token.line_number};
				return false;
			}
			else if (context.parenthesis_depth > 0)
			{
				error = {CompileErrorCode::MissingCloseParenthesis, token.unit->file_path, token.line_number};
				return false;
			}
			else if (context.previous_token &&
					 context.previous_token->name == token_name::Separator &&
					 context.previous_token->value.front() == ':')
			{
				error = {CompileErrorCode::UnexpectedSemicolon, token.unit->file_path, token.line_number};
				return false;
			}

			context.inside_import = false;
			context.inside_property = false;
			context.inside_variable = false;
			break;
		}

		case '{':
		{
			if (!context.inside_object_signature && !context.inside_template_signature)
			{
				error = {CompileErrorCode::UnexpectedOpenCurlyBrace, token.unit->file_path, token.line_number};
				return false;
			}

			++context.curly_brace_depth;
			context.inside_object_signature = false;
			context.inside_template_signature = false;
			break;
		}

		case '}':
		{
			if (context.inside_property || context.inside_variable)
			{
				error = {CompileErrorCode::UnexpectedCloseCurlyBrace, token.unit->file_path, token.line_number};
				return false;
			}
			else if (context.curly_brace_depth == 0)
			{
				error = {CompileErrorCode::UnmatchedCloseCurlyBrace, token.unit->file_path, token.line_number};
				return false;
			}

			--context.curly_brace_depth;
			break;
		}

		case '(':
		{
			if (!context.inside_function) 
			{
				error = {CompileErrorCode::UnexpectedOpenParenthesis, token.unit->file_path, token.line_number};
				return false;
			}
			else if (!context.inside_calc_function && context.parenthesis_depth > 0)
			{
				error = {CompileErrorCode::UnexpectedOpenParenthesis, token.unit->file_path, token.line_number};
				return false;
			}
			else if (context.next_token && context.next_token->name == token_name::Separator &&
					 context.next_token->value.front() == ')')
			{
				error = {CompileErrorCode::EmptyParentheses, token.unit->file_path, token.line_number};
				return false;
			}

			++context.parenthesis_depth;
			break;
		}

		case ')':
		{
			if (!context.inside_function) 
			{
				error = {CompileErrorCode::UnexpectedCloseParenthesis, token.unit->file_path, token.line_number};
				return false;
			}
			else if (context.parenthesis_depth == 0)
			{
				error = {CompileErrorCode::UnmatchedCloseParenthesis, token.unit->file_path, token.line_number};
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
				error = {CompileErrorCode::UnexpectedComma, token.unit->file_path, token.line_number};
				return false;
			}
			else if ((context.next_token && context.next_token->name == token_name::Separator &&
					 (context.next_token->value.front() == ',' || context.next_token->value.front() == ')')) ||
					 (context.previous_token && context.previous_token->name == token_name::Separator &&
					 context.previous_token->value.front() == '('))
			{
				error = {CompileErrorCode::EmptyFunctionArgument, token.unit->file_path, token.line_number};
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
		error = {CompileErrorCode::InvalidUnit, token.unit->file_path, token.line_number};
		return false;
	}

	return true;
}

bool check_unknown_symbol_syntax(lexical_token &token, syntax_context&, CompileError &error) noexcept
{
	//More specialized message
	if (is_selector(token.value.front()))
	{
		error = {CompileErrorCode::UnexpectedSelector, token.unit->file_path, token.line_number};
		return false;
	}
	else if (is_variable_prefix(token.value.front()))
	{
		error = {CompileErrorCode::MissingIdentifier, token.unit->file_path, token.line_number};
		return false;
	}

	//Default message
	error = {CompileErrorCode::UnknownSymbol, token.unit->file_path, token.line_number};
	return false;
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

			//Selector
			case token_name::Selector:
			{
				if (!check_selector_syntax(token, context, error))
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
				if (!check_unknown_symbol_syntax(token, context, error))
					return false;

				break;
			}

			//Literal
			default:
			{
				if (!check_literal_syntax(token, context, error))
					return false;
			}
		}
	}
	
	if (context.inside_property || context.inside_variable)
	{
		error = {CompileErrorCode::MissingSemicolon, tokens.back().unit->file_path, tokens.back().line_number};
		return false;
	}
	else if (context.curly_brace_depth > 0)
	{
		error = {CompileErrorCode::MissingCloseCurlyBrace, tokens.back().unit->file_path, tokens.back().line_number};
		return false;
	}

	return true;
}


void link(lexical_tokens &tokens, const adaptors::FlatMap<std::string, std::optional<lexical_tokens>> &results, const build_system &system)
{
	for (auto iter = std::begin(tokens); iter != std::end(tokens);)
	{
		auto &token = *iter;

		//Link in imported tokens from another file
		if (token.name == token_name::Rule &&
			is_import_rule(token.value))
		{
			auto file_path = utilities::parse::AsString((iter + 1)->value);
			auto full_path = *full_file_path(*file_path, system, std::filesystem::path{token.unit->file_path}.parent_path());
			auto id = ion::utilities::string::ToLowerCaseCopy(full_path.lexically_normal().string());

			if (auto it = results.find(id); it != std::end(results))
			{
				auto &[key, value] = *it;

				iter = tokens.erase(iter, iter + 3);
				iter = tokens.insert(iter, std::begin(*value), std::end(*value));	
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
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.unit->file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const ScriptType::Integer &value) mutable noexcept
			{
				//As percent
				argument = script_tree::ArgumentType{ScriptType::FloatingPoint{
					value.Get() * ScriptType::FloatingPoint::value_type{0.01}}};
			},
			//Floating point
			[](const ScriptType::FloatingPoint&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.unit->file_path, token.line_number};
			});

		if (error)
			return {};
	}

	auto color = graphics::utilities::Color::CMYK(
		arguments[0].Get<ScriptType::FloatingPoint>()->As<real>(),
		arguments[1].Get<ScriptType::FloatingPoint>()->As<real>(),
		arguments[2].Get<ScriptType::FloatingPoint>()->As<real>(),
		arguments[3].Get<ScriptType::FloatingPoint>()->As<real>());

	//Alpha component
	if (std::size(arguments) == 5)
		color.A(arguments[4].Get<ScriptType::FloatingPoint>()->As<real>());

	return std::optional<ScriptType::Color>(color);
}

std::optional<script_tree::ArgumentType> call_hsl(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	//Check number of arguments
	if ((token.value == "hsl" && std::size(arguments) != 3) ||
		(token.value == "hsla" && std::size(arguments) != 4))
	{
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.unit->file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const ScriptType::Integer &value) mutable noexcept
			{
				argument =
					&argument == &arguments.front() ?
					//Hue as is
					script_tree::ArgumentType{ScriptType::FloatingPoint{
						value.As<ScriptType::FloatingPoint::value_type>()}} :
					//Saturation, lightness and alpha as percent
					script_tree::ArgumentType{ScriptType::FloatingPoint{
						value.Get() * ScriptType::FloatingPoint::value_type{0.01}}};
			},
			//Floating point
			[](const ScriptType::FloatingPoint&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.unit->file_path, token.line_number};
			});

		if (error)
			return {};
	}

	auto color = graphics::utilities::Color::HSL(
		arguments[0].Get<ScriptType::FloatingPoint>()->As<real>(),
		arguments[1].Get<ScriptType::FloatingPoint>()->As<real>(),
		arguments[2].Get<ScriptType::FloatingPoint>()->As<real>());

	//Alpha component
	if (std::size(arguments) == 4)
		color.A(arguments[3].Get<ScriptType::FloatingPoint>()->As<real>());

	return std::optional<ScriptType::Color>(color);
}

std::optional<script_tree::ArgumentType> call_hwb(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	//Check number of arguments
	if ((token.value == "hwb" && std::size(arguments) != 3) ||
		(token.value == "hwba" && std::size(arguments) != 4))
	{
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.unit->file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const ScriptType::Integer &value) mutable noexcept
			{
				argument =
					&argument == &arguments.front() ?
					//Hue as is
					script_tree::ArgumentType{ScriptType::FloatingPoint{
						value.As<ScriptType::FloatingPoint::value_type>()}} :
					//Whiteness, blackness and alpha as percent
					script_tree::ArgumentType{ScriptType::FloatingPoint{
						value.Get() * ScriptType::FloatingPoint::value_type{0.01}}};
			},
			//Floating point
			[](const ScriptType::FloatingPoint&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.unit->file_path, token.line_number};
			});

		if (error)
			return {};
	}

	auto color = graphics::utilities::Color::HWB(
		arguments[0].Get<ScriptType::FloatingPoint>()->As<real>(),
		arguments[1].Get<ScriptType::FloatingPoint>()->As<real>(),
		arguments[2].Get<ScriptType::FloatingPoint>()->As<real>());

	//Alpha component
	if (std::size(arguments) == 4)
		color.A(arguments[3].Get<ScriptType::FloatingPoint>()->As<real>());

	return std::optional<ScriptType::Color>(color);
}

std::optional<script_tree::ArgumentType> call_rgb(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	//Check number of arguments
	if ((token.value == "rgb" && std::size(arguments) != 3) ||
		(token.value == "rgba" && std::size(arguments) != 4))
	{
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.unit->file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const ScriptType::Integer &value) mutable noexcept
			{
				argument =
					&argument == &arguments.back() && std::size(arguments) == 4 ?
					//Alpha as percent
					script_tree::ArgumentType{ScriptType::FloatingPoint{
						value.Get() * ScriptType::FloatingPoint::value_type{0.01}}} :
					//RGB divide by 255
					script_tree::ArgumentType{ScriptType::FloatingPoint{
						value.Get() / ScriptType::FloatingPoint::value_type{255.0}}};
			},
			//Floating point
			[](const ScriptType::FloatingPoint&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.unit->file_path, token.line_number};
			});

		if (error)
			return {};
	}

	auto color = graphics::utilities::Color{
		arguments[0].Get<ScriptType::FloatingPoint>()->As<real>(),
		arguments[1].Get<ScriptType::FloatingPoint>()->As<real>(),
		arguments[2].Get<ScriptType::FloatingPoint>()->As<real>()};

	//Alpha component
	if (std::size(arguments) == 4)
		color.A(arguments[3].Get<ScriptType::FloatingPoint>()->As<real>());

	return std::optional<ScriptType::Color>(color);
}

std::optional<script_tree::ArgumentType> call_vec2(lexical_token &token, script_tree::ArgumentNodes arguments, CompileError &error) noexcept
{
	if (std::size(arguments) != 1 &&
		std::size(arguments) != 2)
	{
		error = {CompileErrorCode::InvalidNumberOfFunctionArguments, token.unit->file_path, token.line_number};
		return {};
	}

	//Convert all arguments to floating point
	for (auto &argument : arguments)
	{		
		argument.Visit(
			//Integer
			[&](const ScriptType::Integer &value) mutable noexcept
			{
				argument = script_tree::ArgumentType{ScriptType::FloatingPoint{
					value.As<ScriptType::FloatingPoint::value_type>()}};
			},
			//Floating point
			[](const ScriptType::FloatingPoint&) noexcept
			{
				//Nothing to do
			},
			//Default
			[&](auto&&) noexcept
			{
				error = {CompileErrorCode::InvalidFunctionArgument, token.unit->file_path, token.line_number};
			});

		if (error)
			return {};
	}

	return std::size(arguments) == 1 ?
		//Scalar
		std::optional<ScriptType::Vector2>(graphics::utilities::Vector2{
			arguments[0].Get<ScriptType::FloatingPoint>()->As<real>()}) :
		//Two components
		std::optional<ScriptType::Vector2>(graphics::utilities::Vector2{
			arguments[0].Get<ScriptType::FloatingPoint>()->As<real>(),
			arguments[1].Get<ScriptType::FloatingPoint>()->As<real>()});
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
	//cmyk/cmyka
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

bool parse_identifier(lexical_token &token, parse_context &context, CompileError &error)
{
	//Function, property or variable argument
	if (context.function_token || context.property_token || context.variable_token)
	{
		//Variable
		if (is_variable_identifier(token.value))
		{
			auto found = false;

			//Find variable from inner to outermost scope
			for (auto scope_depth = std::min(context.scope_depth, std::ssize(context.scopes) - 1);
				scope_depth >= 0; --scope_depth)
			{
				if (auto iter = context.scopes[scope_depth].variables.find(token.value);
					iter != std::end(context.scopes[scope_depth].variables))
				{
					//Function arguments
					if (context.function_token)
						context.function_arguments.insert(std::end(context.function_arguments),
						std::begin(iter->second), std::end(iter->second));
					//Property arguments
					else if (context.property_token)
						context.property_arguments.insert(std::end(context.property_arguments),
						std::begin(iter->second), std::end(iter->second));
					//Variable arguments
					else if (context.variable_token)
						context.variable_arguments.insert(std::end(context.variable_arguments),
						std::begin(iter->second), std::end(iter->second));

					found = true;
					break;
				}
			}

			if (!found)
			{
				error = {CompileErrorCode::UndeclaredVariable, token.unit->file_path, token.line_number};
				return false;
			}
		}
		//Enumerable
		else
		{
			auto argument =
				[&]() noexcept -> script_tree::ArgumentType
				{
					if (auto color = utilities::parse::detail::color_name_as_color(token.value); color)
						return ScriptType::Color{*color};
					else
						return ScriptType::Enumerable{std::string{token.value}};
				}();

			//Function argument
			if (context.function_token)
				context.function_arguments.push_back(std::move(argument));
			//Property argument
			else if (context.property_token)
				context.property_arguments.push_back(std::move(argument));
			//Variable argument
			else if (context.variable_token)
				context.variable_arguments.push_back(std::move(argument));
		}
	}
	else
	{
		context.identifier_token = &token;

		if (is_class_identifier(token.value))
		{
			//First identifier in selector group
			if (std::empty(context.selectors))
				context.selectors.emplace_back();

			context.selector_classes.insert(token.value.substr(std::size(token.value) > 1));
		}
	}

	return true;
}

bool parse_literal(lexical_token &token, lexical_token *next_token, parse_context &context, CompileError &error)
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
					context.function_arguments.emplace_back(ScriptType::Boolean{*result});
				//Property argument
				else if (context.property_token)
					context.property_arguments.emplace_back(ScriptType::Boolean{*result});
				//Variable argument
				else if (context.variable_token)
					context.variable_arguments.emplace_back(ScriptType::Boolean{*result});
			}
			else
			{
				error = {CompileErrorCode::InvalidBooleanLiteral, token.unit->file_path, token.line_number};
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
					context.function_arguments.emplace_back(ScriptType::Color{*result});
				//Property argument
				else if (context.property_token)
					context.property_arguments.emplace_back(ScriptType::Color{*result});
				//Variable argument
				else if (context.variable_token)
					context.variable_arguments.emplace_back(ScriptType::Color{*result});
			}
			else
			{
				error = {CompileErrorCode::InvalidHexLiteral, token.unit->file_path, token.line_number};
				return false;
			}

			break;
		}

		//Numeric literal
		case token_name::NumericLiteral:
		{
			auto unit =
				next_token && next_token->name == token_name::Unit && next_token->value.front() != '%' ?
				std::string{next_token->value} : ""s;

			//Real
			if (utilities::parse::detail::parse_as_floating_point(token.value))
			{
				auto result = ion::utilities::convert::To<ScriptType::FloatingPoint::value_type>(token.value);

				if (result)
				{
					//Function argument
					if (context.function_token)
						context.function_arguments.emplace_back(ScriptType::FloatingPoint{context.unary_minus ? -*result : *result}, unit);
					//Property argument
					else if (context.property_token)
						context.property_arguments.emplace_back(ScriptType::FloatingPoint{context.unary_minus ? -*result : *result}, unit);
					//Variable argument
					else if (context.variable_token)
						context.variable_arguments.emplace_back(ScriptType::FloatingPoint{context.unary_minus ? -*result : *result}, unit);
				}
				else
				{
					error = {CompileErrorCode::InvalidNumericLiteral, token.unit->file_path, token.line_number};
					return false;
				}
			}
			//Integer
			else
			{
				auto result = ion::utilities::convert::To<ScriptType::Integer::value_type>(token.value);

				if (result)
				{
					//Function argument
					if (context.function_token)
						context.function_arguments.emplace_back(ScriptType::Integer{context.unary_minus ? -*result : *result}, unit);
					//Property argument
					else if (context.property_token)
						context.property_arguments.emplace_back(ScriptType::Integer{context.unary_minus ? -*result : *result}, unit);
					//Variable argument
					else if (context.variable_token)
						context.variable_arguments.emplace_back(ScriptType::Integer{context.unary_minus ? -*result : *result}, unit);
				}
				else
				{
					error = {CompileErrorCode::InvalidNumericLiteral, token.unit->file_path, token.line_number};
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
					context.function_arguments.emplace_back(ScriptType::String{*result});
				//Property argument
				else if (context.property_token)
					context.property_arguments.emplace_back(ScriptType::String{*result});
				//Variable argument
				else if (context.variable_token)
					context.variable_arguments.emplace_back(ScriptType::String{*result});
				//Objects/Templates
				else
					context.classes = std::move(*result);
			}
			else
			{
				error = {CompileErrorCode::InvalidStringLiteral, token.unit->file_path, token.line_number};
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

bool parse_selector(lexical_token &token, parse_context &context, CompileError&) noexcept
{
	for (auto &selector_class : context.selector_classes)
		context.selectors.back().classes.push_back(selector_class);
	context.selector_classes.clear();

	//New selector group
	if (token.value.front() == ',')
		context.selectors.emplace_back();
	else
	{
		context.selectors.back().combinators.push_back(
			std::ssize(context.selectors.back().classes));
		context.selectors.back().classes.push_back(token.value);
	}

	return true;
}

bool parse_separator(lexical_token &token, parse_context &context, CompileError &error)
{
	switch (token.value.front())
	{
		case ':':
		{
			//Variable
			if (is_variable_identifier(context.identifier_token->value))
				context.variable_token = context.identifier_token;
			//Property
			else
				context.property_token = context.identifier_token;

			break;
		}

		case ';':
		{
			//Property
			if (context.property_token)
			{
				context.scopes[context.scope_depth - 1].properties.emplace_back(
					std::string{context.property_token->value}, std::move(context.property_arguments));
				context.property_token = nullptr;
			}
			//Variable
			else if (context.variable_token)
			{
				if (context.scope_depth == std::ssize(context.scopes))
					context.scopes.emplace_back();

				context.scopes[context.scope_depth].variables[context.variable_token->value] =
					std::move(context.variable_arguments);
				context.variable_token = nullptr;
			}

			break;
		}

		case '{':
		{
			if (context.scope_depth == std::ssize(context.scopes))
				context.scopes.emplace_back();

			//Classes
			if (!std::empty(context.classes))
			{
				auto classes = split_classes(context.classes);

				//Erase explicit object name (if any) in classes (is implicit)
				if (!is_class_identifier(context.identifier_token->value))
					classes.erase(context.identifier_token->value);

				context.scopes[context.scope_depth].classes = join_classes(classes);
				context.classes.clear();
			}

			//Template
			if (is_class_identifier(context.identifier_token->value))
			{
				for (auto &selector_class : context.selector_classes)
					context.selectors.back().classes.push_back(selector_class);
				context.selector_classes.clear();
			}

			context.object_tokens.push_back(context.identifier_token);
			++context.scope_depth;
			break;
		}

		case '}':
		{
			//Leaf object, no childrens
			if (auto &scope = context.scopes[context.scope_depth - 1];
				&scope == &context.scopes.back())
				scope.objects.emplace_back(std::string{context.object_tokens.back()->value}, std::move(scope.classes), std::move(scope.properties));
			//Take childrens (if any) from one depth deeper
			else
				scope.objects.emplace_back(std::string{context.object_tokens.back()->value}, std::move(scope.classes), std::move(scope.properties),
										   std::move(context.scopes[context.scope_depth].objects));

			//Clear local variable stack
			if (context.scope_depth < std::ssize(context.scopes))
				context.scopes[context.scope_depth].variables.clear();

			context.object_tokens.pop_back();
			--context.scope_depth;

			//Template, store selectors
			if (context.scope_depth == 0 && !std::empty(context.selectors))
				context.templates.push_back({std::move(context.selectors)});

			break;
		}

		case ')':
		{
			//End of function
			if (context.function_token)
			{
				auto result = call_function(*context.function_token, std::move(context.function_arguments), error);

				if (result)
				{
					//Property argument
					if (context.property_token)
						context.property_arguments.emplace_back(std::move(*result));
					//Variable argument
					else if (context.variable_token)
						context.variable_arguments.emplace_back(std::move(*result));
				}

				context.function_token = nullptr;
			}
		}
	}

	return true;
}

bool parse_unit(lexical_token &token, parse_context &context, CompileError&) noexcept
{
	auto &argument =
		[&]() noexcept -> script_tree::ArgumentNode&
		{
			//Function arguments
			if (context.function_token)
				return context.function_arguments.back();
			//Variable arguments
			else if (context.variable_token)
				return context.variable_arguments.back();
			//Property arguments
			else
				return context.property_arguments.back();
		}();

	argument.Visit(
		//Integer
		[&](const ScriptType::Integer &value) mutable noexcept
		{
			if (token.value == "%")
				argument = script_tree::ArgumentType{ScriptType::FloatingPoint{
					value.Get() * ScriptType::FloatingPoint::value_type{0.01}}};
		},
		//Floating point
		[&](const ScriptType::FloatingPoint &value) mutable noexcept
		{
			if (token.value == "%")
				argument = script_tree::ArgumentType{ScriptType::FloatingPoint{
					value.Get() * ScriptType::FloatingPoint::value_type{0.01}}};
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
	auto results = system.processes.Get();

	for (const auto &unit : system.units)
	{
		if (unit->error)
			return {};
	}

	//Link together tokens (main compilation) with the external tokens (external compilations)
	link(tokens, results, system);


	//Parse and build tree
	parse_context context;
	
	auto off = 0U;
	for (auto &token : tokens)
	{
		switch (token.name)
		{
			//Function
			case token_name::Function:
			{
				parse_function(token, context, token.unit->error);
				break;
			}

			//Identifier
			case token_name::Identifier:
			{
				parse_identifier(token, context, token.unit->error);
				break;
			}

			//Selector
			case token_name::Selector:
			{
				parse_selector(token, context, token.unit->error);
				break;
			}

			//Separator
			case token_name::Separator:
			{
				parse_separator(token, context, token.unit->error);
				break;
			}
			
			//Unary operator
			case token_name::UnaryOperator:
			{
				parse_unary_operator(token, context, token.unit->error);
				break;
			}

			//Unit
			case token_name::Unit:
			{
				parse_unit(token, context, token.unit->error);
				break;
			}

			//Literals
			case token_name::BooleanLiteral:
			case token_name::HexLiteral:
			case token_name::NumericLiteral:
			case token_name::StringLiteral:
			{
				auto next_token = off + 1 < std::size(tokens) ? &tokens[off + 1] : nullptr; //For units
				parse_literal(token, next_token, context, token.unit->error);
				break;
			}
		}

		//An error has occurred
		if (token.unit->error)
			return {};

		++off;
	}

	if (std::empty(context.scopes))
		return {};
	
	
	auto &top_level_objects = context.scopes.front().objects;

	//Inherit from templates using pattern matching rules (selectors)
	inherit(top_level_objects, context.templates);

	return !std::empty(top_level_objects) ?
		std::make_optional<ScriptTree>(std::move(top_level_objects)) :
		std::nullopt;
}


adaptors::FlatSet<std::string_view> split_classes(std::string_view str)
{
	adaptors::FlatSet<std::string_view> result;

	for (auto iter = std::begin(str);
		(iter = std::find_if(iter, std::end(str), std::not_fn(is_white_space))) != std::end(str);)
	{
		auto iter2 = std::find_if(iter + 1, std::end(str), is_white_space);
		result.insert(str.substr(iter - std::begin(str), iter2 - iter));
		iter = iter2 + (iter2 != std::end(str));
	}

	return result;
}

std::string join_classes(const adaptors::FlatSet<std::string_view> &classes)
{
	std::string str;

	if (!std::empty(classes))
	{
		auto iter = std::begin(classes);
		str += *iter;

		for (++iter; iter != std::end(classes); ++iter)
		{
			str += " ";
			str += *iter;
		}
	}

	return str;
}

string_views get_classes(script_tree::ObjectNode &object)
{
	string_views result;
	auto name = std::string_view{object.Name()};
	auto classes = std::string_view{object.Classes()};

	//Split classes
	if (!std::empty(classes))
	{
		//Each class is sorted, unique and with exactly one space in between
		for (size_t from = 0, to = 0; to != std::string_view::npos; from = to + 1)
			result.push_back(classes.substr(from, (to = classes.find(' ', from)) - from));
	}

	//Add object name as class
	if (!is_class_identifier(name))
		result.insert( //Result should be small, use linear search to find insertion point
			std::find_if(std::begin(result), std::end(result),
				[&](const auto &str) noexcept
				{
					return name < str;
				}), name);

	return result;
}


std::pair<bool, int> is_matching(string_views::const_iterator first_selector_class,
	string_views::const_iterator last_selector_class, const string_views &classes) noexcept
{
	//* is always in front in a sorted range (if existing)
	auto select_all = first_selector_class->front() == '*';

	string_views result;
	std::set_intersection(std::begin(classes), std::end(classes),
						  first_selector_class + select_all, //Skip *
						  last_selector_class, std::back_inserter(result));

	auto count = std::ssize(result);
	return {count == last_selector_class - first_selector_class - select_all, count};
}

void append_matching_templates(const script_tree::detail::generations &descendants,
	template_rules::const_iterator first, template_rules::const_iterator last)
{
	auto &object = *descendants.back().back();
	auto is_template = is_class_identifier(object.Name());
	auto classes = get_classes(object);

	if (std::empty(classes))
		return;

	adaptors::FlatMap<std::pair<int,int>, script_tree::ObjectNode*, std::greater<>> matching_templates;
	
	for (auto off = 0; first != last; ++first, ++off)
	{
		auto &[selectors, template_object] = *first;	
		auto max_specificity = -1;

		for (auto &group : selectors)
		{		
			auto group_matching = true;
			auto group_specificity = 0;
			auto position = std::pair{std::size(descendants) - 1,
									  std::size(descendants.back()) - 1};
			
			auto iter = std::rbegin(group.combinators); //Right to left
			auto end = std::rend(group.combinators);
			auto combinator = end;

			struct snapshot
			{
				int group_specificity;
				decltype(position) position;
				decltype(iter) iter;
				int from;
			};

			std::optional<snapshot> restore_point;

			for (auto to = std::ssize(group.classes); to > 0 && group_matching;)
			{
				auto from = iter != end ? *iter + 1 : 0;
				auto first_selector_class = std::begin(group.classes) + from;
				auto last_selector_class = std::begin(group.classes) + to;

				auto [matching, specificity] =
					[&]() noexcept
					{
						//Has combinator
						if (combinator != end)
						{
							switch (group.classes[*combinator].front())
							{
								case '>':
								{
									//Has ancestor
									if (position.first > 0)
									{
										--position.first;
										position.second = std::size(descendants[position.first]) - 1;
										return is_matching(first_selector_class, last_selector_class,
											get_classes(*descendants[position.first][position.second]));
									}
									else
										break;
								}

								case '+':
								{
									//Has preceding sibling
									if (position.second > 0)
									{
										--position.second;

										//Break if preceding sibling is not the same identifier category
										if (is_template != is_class_identifier(descendants[position.first][position.second]->Name()))
											break;

										return is_matching(first_selector_class, last_selector_class,
											get_classes(*descendants[position.first][position.second]));
									}
									else
										break;
								}

								case '~':
								{
									//Has preceding sibling
									while (position.second > 0)
									{
										--position.second;
										
										//Break if preceding sibling is not the same identifier category
										if (is_template != is_class_identifier(descendants[position.first][position.second]->Name()))
											break;

										if (auto result = is_matching(first_selector_class, last_selector_class,
											get_classes(*descendants[position.first][position.second])); result.first)
											return result;
									}

									break;
								}

								default: //' '
								{
									//Has ancestor
									while (position.first > 0)
									{
										--position.first;
										position.second = std::size(descendants[position.first]) - 1;

										if (auto result = is_matching(first_selector_class, last_selector_class,
											get_classes(*descendants[position.first][position.second])); result.first)
										{
											//Make restore point
											restore_point = snapshot{
												group_specificity,
												position,
												iter - 1,
												to + 1
											};
											return result;
										}
									}

									break;
								}
							}

							return std::pair{false, 0};
						}
						else
							return is_matching(first_selector_class, last_selector_class, classes);
					}();

				//Accumulate specificity if matching
				if (matching && (!is_template || specificity > 0))
					group_specificity += specificity;
				else
				{
					//Has restore point - rollback
					if (restore_point)
					{
						group_specificity = restore_point->group_specificity;
						position = restore_point->position;
						iter = restore_point->iter;
						from = restore_point->from;

						restore_point.reset();
					}
					else
					{
						group_matching = false;
						break;
					}
				}

				//Next combinator
				if (from > 0)
				{
					combinator = iter++;
					--from;
				}

				to = from;
			}

			if (group_matching)
				max_specificity = std::max(group_specificity, max_specificity);
		}

		if (max_specificity >= 0)
			matching_templates.emplace(std::pair{max_specificity, off}, template_object);
	}

	//Inherit in order most to least matched pattern
	for (auto &[key, template_object] : matching_templates)
	{
		object.Append(template_object->Properties(), script_tree::AppendCondition::NoDuplicateNames);
		object.Append(template_object->Objects(), script_tree::AppendCondition::NoDuplicateClasses);
	}
}

void inherit(script_tree::ObjectNodes &objects, template_rules &templates)
{
	//Templates available
	if (!std::empty(templates))
	{
		auto available_templates = 0;

		for (auto &descendants : script_tree::detail::lineage_depth_first_search(objects))
		{
			//Global scope
			if (std::size(descendants) == 1 && //Make template previously added visible
				available_templates < std::ssize(templates) &&
				templates[available_templates].object)
				++available_templates;

			//Do pattern matching against visible templates
			if (available_templates > 0)
				append_matching_templates(descendants, std::begin(templates), std::begin(templates) + available_templates);

			//Add template (invisible for now)
			if (auto object = descendants.back().back(); is_class_identifier(object->Name()))
				templates[available_templates].object = object;
		}

		//Erase all templates
		objects.erase(
			std::remove_if(std::begin(objects), std::end(objects),
				[](auto &object) noexcept
				{
					return is_class_identifier(object.Name());
				}), std::end(objects));
	}
}


/*
	Compiling
*/

std::optional<ScriptTree> compile(file_trace trace, build_system &system, CompileError &error, std::vector<CompileError> &errors)
{
	//Start by compiling the given unit (entry point)
	auto tree = compile_unit(*system.units.front(), trace, system);

	//Inherit error from the first unit that failed to build
	for (const auto &unit : system.units)
	{
		//Add error
		if (unit->error)
		{
			if (!error)
				error = unit->error;

			errors.push_back(unit->error);
		}
		//Add success (file path)
		else
			errors.emplace_back().FilePath = unit->file_path;
	}

	//Set file path on no error (success)
	if (!error && !std::empty(errors))
		error.FilePath = errors.front().FilePath;

	return tree;
}

std::optional<ScriptTree> compile_unit(translation_unit &unit, file_trace trace, build_system &system)
{
	auto tokens = lex(unit, std::move(trace), system);
	auto result = tokens ? parse(std::move(*tokens), system, unit.error) : std::nullopt;
	system.processes.Wait(); //Make sure all external compilations are completed
	return result;
}

std::optional<lexical_tokens> partial_compile_unit(translation_unit &unit, file_trace trace, build_system &system)
{
	auto tokens = lex(unit, std::move(trace), system);

	if (tokens)
	{
		//Discard unnecessary tokens
		pre_parse(*tokens);

		//Syntax error checking
		check_syntax(*tokens, unit.error);
	}

	return {unit.error ? std::nullopt : std::move(tokens)};
}


/*
	Outputting
*/

std::string print_output(duration compile_time, const std::vector<CompileError> &errors, OutputOptions output_options)
{
	std::string output;
	{
		auto error = !std::empty(errors) ? errors.front() : CompileError{};
	
		//Find first error (if any)
		if (!error)
		{
			if (auto iter = std::find_if(std::begin(errors), std::end(errors),
				[](auto &error) noexcept
				{
					return !error;
				}); iter != std::end(errors))
				//Has error
				error = *iter;
		}

		if (output_options == OutputOptions::Summary ||
			output_options == OutputOptions::SummaryAndUnits)
		{
			if (!std::empty(output))
				output += "\n\n";

			output += ion::utilities::string::Concat(
				"[Compiler summary]\n"
				"Message - ",
					!error ?
					"Compilation succeeded!" :
					ion::utilities::string::Concat("Compilation failed. ", error.Condition.message(), " ('", error.FilePath.string(), "', line ", error.LineNumber, ")"),
				"\n"
				"Compile time - ", ion::utilities::string::Format(compile_time.count(), "0.0000"sv), " seconds\n"
				"Compiled units - ", std::size(errors));
		}
	}

	if (!std::empty(errors) &&
		(output_options == OutputOptions::Units ||
		 output_options == OutputOptions::SummaryAndUnits))
	{
		if (!std::empty(output))
			output += "\n\n";

		output += "[Compiled units]";

		for (const auto &error : errors)
			output += ion::utilities::string::Concat(
				"\n'", error.FilePath.string(), "' - ",
					!error ?
					"OK" : ion::utilities::string::Concat("Error. ", error.Condition.message(), " (line ", error.LineNumber, ")"));
	}

	return output;
}

} //script_compiler::detail


//ScriptCompiler

ScriptCompiler::ScriptCompiler(const resources::files::repositories::ScriptRepository &repository) :
	repository_{&repository}
{
	//Empty
}


/*
	Compiling
*/

std::optional<ScriptTree> ScriptCompiler::Compile(std::string_view name, CompileError &error)
{
	compile_errors_.clear();
	compile_time_ = {}; //Reset

	if (repository_)
	{
		script_compiler::detail::build_system system{*repository_};
		script_compiler::detail::file_trace trace;

		//File path needs to be a valid file
		if (std::string str; script_compiler::detail::load_from_repository(name, system, trace, str, error))
		{	
			system.units.push_back(std::make_unique<script_compiler::detail::translation_unit>(
				script_compiler::detail::translation_unit{trace.current_file_path().string(), std::move(str)}));

			if (max_build_processes_)
				system.processes.MaxWorkerThreads(*max_build_processes_);

			auto stopwatch = timers::Stopwatch::StartNew();
			auto tree = script_compiler::detail::compile(std::move(trace), system, error, compile_errors_);
			compile_time_ = stopwatch.Elapsed();
			return tree;
		}
	}

	return {};
}

std::optional<ScriptTree> ScriptCompiler::CompileFile(std::filesystem::path file_path, CompileError &error)
{
	auto root_path = file_path.parent_path();
	return CompileFile(std::move(file_path), std::move(root_path), error);
}

std::optional<ScriptTree> ScriptCompiler::CompileFile(std::filesystem::path file_path, std::filesystem::path root_path, CompileError &error)
{
	compile_errors_.clear();
	compile_time_ = {}; //Reset

	//Root path needs to be a valid directory
	if (ion::utilities::file::IsDirectory(root_path))
	{
		script_compiler::detail::build_system system{std::move(root_path)};
		script_compiler::detail::file_trace trace;
		
		//File path needs to be a valid file
		if (std::string str; script_compiler::detail::open_file(file_path, system, trace, str, error))
		{	
			system.units.push_back(std::make_unique<script_compiler::detail::translation_unit>(
				script_compiler::detail::translation_unit{trace.current_file_path().string(), std::move(str)}));

			if (max_build_processes_)
				system.processes.MaxWorkerThreads(*max_build_processes_);

			auto stopwatch = timers::Stopwatch::StartNew();
			auto tree = script_compiler::detail::compile(std::move(trace), system, error, compile_errors_);
			compile_time_ = stopwatch.Elapsed();
			return tree;
		}
	}

	return {};
}


/*
	Outputting
*/

std::string ScriptCompiler::PrintOutput(OutputOptions output_options) const
{
	return script_compiler::detail::print_output(compile_time_, compile_errors_, output_options);
}

} //ion::script