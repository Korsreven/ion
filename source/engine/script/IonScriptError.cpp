/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptError.cpp
-------------------------------------------
*/

#include "IonScriptError.h"

namespace ion::script::script_error
{

namespace detail
{

/*
	Compile errors
*/

const char* compile_error_category_impl::name() const noexcept
{
	return "ION script - compile error";
}

std::string compile_error_category_impl::message(int error_value) const
{
	switch (static_cast<CompileErrorCode>(error_value))
	{
		//File
		case CompileErrorCode::CircularImport:
		return "A circular import detected";

		case CompileErrorCode::MissingImportFile:
		return "A missing import file detected";

		
		//Function
		case CompileErrorCode::EmptyFunctionArgument:
		return "An empty function argument found";

		case CompileErrorCode::InvalidFunctionArgument:
		return "An invalid function argument found";

		case CompileErrorCode::InvalidNumberOfFunctionArguments:
		return "An invalid number of function arguments found";

		case CompileErrorCode::UnexpectedFunction:
		return "An unexpected function found";


		//Identifier
		case CompileErrorCode::UnexpectedIdentifier:
		return "An unexpected identifier found";


		//Literal
		case CompileErrorCode::InvalidBooleanLiteral:
		return "An invalid boolean literal found";

		case CompileErrorCode::InvalidHexLiteral:
		return "An invalid hex literal found";

		case CompileErrorCode::InvalidNumericLiteral:
		return "An invalid numeric literal found";

		case CompileErrorCode::InvalidStringLiteral:
		return "An invalid string literal found";

		case CompileErrorCode::UnexpectedLiteral:
		return "An unexpected literal found";


		//Operator
		case CompileErrorCode::InvalidLeftOperand:
		return "An invalid left operand found";

		case CompileErrorCode::InvalidRightOperand:
		return "An invalid right operand found";

		case CompileErrorCode::UnexpectedOperator:
		return "An unexpected operator found";

		case CompileErrorCode::UnexpectedBinaryOperator:
		return "An unexpected binary operator found";

		case CompileErrorCode::UnexpectedUnaryOperator:
		return "An unexpected unary operator found";


		//Rule
		case CompileErrorCode::InvalidImportStatement:
		return "An invalid import statement found";

		case CompileErrorCode::UnexpectedImportStatement:
		return "An unexpected import statement found";


		//Separator
		case CompileErrorCode::EmptyParentheses:
		return "An empty parentheses found";

		case CompileErrorCode::MissingCloseCurlyBrace:
		return "A missing close curly brace detected";

		case CompileErrorCode::MissingOpenCurlyBrace:
		return "A missing open curly brace detected";

		case CompileErrorCode::MissingCloseParenthesis:
		return "A missing close parenthesis detected";

		case CompileErrorCode::MissingOpenParenthesis:
		return "A missing open parenthesis detected";

		case CompileErrorCode::UnexpectedColon:
		return "An unexpected colon found";

		case CompileErrorCode::UnexpectedComma:
		return "An unexpected comma found";

		case CompileErrorCode::UnexpectedSemicolon:
		return "An unexpected semicolon found";

		case CompileErrorCode::UnexpectedCloseCurlyBrace:
		return "An unexpected close curly brace found";

		case CompileErrorCode::UnexpectedOpenCurlyBrace:
		return "An unexpected open curly brace found";

		case CompileErrorCode::UnexpectedCloseParenthesis:
		return "An unexpected close parenthesis found";

		case CompileErrorCode::UnexpectedOpenParenthesis:
		return "An unexpected open parenthesis found";

		case CompileErrorCode::UnmatchedCloseCurlyBrace:
		return "An unmatched close curly brace found";

		case CompileErrorCode::UnmatchedCloseParenthesis:
		return "An unmatched close parenthesis found";


		//Unit
		case CompileErrorCode::InvalidUnit:
		return "An invalid unit found";


		//UnknownSymbol
		case CompileErrorCode::UnknownSymbol:
		return "An unknown symbol found";


		default:
		return "An unknown compile error occurred";
	}
}


/*
	Validate errors
*/

const char* validate_error_category_impl::name() const noexcept
{
	return "ION script - validate error";
}

std::string validate_error_category_impl::message(int error_value) const
{
	switch (static_cast<ValidateErrorCode>(error_value))
	{
		//Class
		case ValidateErrorCode::AbstractClassInstantiated:
		return "An object of an abstract class instantiated";

		case ValidateErrorCode::AmbiguousClass:
		return "An object of an ambiguous class found";

		case ValidateErrorCode::MissingRequiredClass:
		return "An object of a required class missing";

		case ValidateErrorCode::UnexpectedClass:
		return "An object of an unexpected class found";


		//Property
		case ValidateErrorCode::InvalidPropertyArguments:
		return "An invalid set of property arguments passed";

		case ValidateErrorCode::MissingRequiredProperty:
		return "A required property missing";

		case ValidateErrorCode::UnexpectedProperty:
		return "An unexpected property found";


		default:
		return "An unknown validate error occurred";
	}
}


const std::error_category& compile_error_category() noexcept
{
	static detail::compile_error_category_impl instance;
	return instance;
}

const std::error_category& validate_error_category() noexcept
{
	static detail::validate_error_category_impl instance;
	return instance;
}

} //detail


std::error_condition make_error_condition(CompileErrorCode error) noexcept
{
	return {static_cast<int>(error), detail::compile_error_category()};
}

std::error_condition make_error_condition(ValidateErrorCode error) noexcept
{
	return {static_cast<int>(error), detail::validate_error_category()};
}

} //ion::script::script_error