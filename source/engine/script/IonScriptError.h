/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptError.h
-------------------------------------------
*/

#ifndef ION_SCRIPT_ERROR_H
#define ION_SCRIPT_ERROR_H

#include <filesystem>
#include <string>
#include <system_error>
#include <type_traits>

namespace ion::script
{
	namespace script_error
	{
		enum class CompileErrorCode
		{
			//File/resource
			CircularImport = 1,
			InvalidFilePath,
			InvalidResourceName,

			//Function
			EmptyFunctionArgument,
			InvalidFunctionArgument,
			InvalidNumberOfFunctionArguments,
			UnexpectedFunction,

			//Identifier
			MissingIdentifier,
			UnexpectedIdentifier,

			//Literal	
			InvalidBooleanLiteral,
			InvalidHexLiteral,
			InvalidNumericLiteral,
			InvalidStringLiteral,
			UnexpectedLiteral,

			//Operator
			InvalidLeftOperand,
			InvalidRightOperand,
			UnexpectedOperator,
			UnexpectedBinaryOperator,
			UnexpectedUnaryOperator,

			//Rule
			InvalidImportStatement,
			InvalidRule,
			UnexpectedImportStatement,

			//Selector
			UnexpectedSelector,

			//Separator
			EmptyParentheses,
			MissingCloseCurlyBrace,
			MissingOpenCurlyBrace,
			MissingCloseParenthesis,
			MissingOpenParenthesis,
			MissingSemicolon,
			UnexpectedColon,
			UnexpectedComma,
			UnexpectedSemicolon,
			UnexpectedCloseCurlyBrace,
			UnexpectedOpenCurlyBrace,
			UnexpectedCloseParenthesis,
			UnexpectedOpenParenthesis,
			UnmatchedCloseCurlyBrace,
			UnmatchedCloseParenthesis,

			//Unit
			InvalidUnit,

			//Variable
			UndeclaredVariable,
			UnexpectedVariableDeclaration,

			//Unknown symbol
			UnknownSymbol
		};

		enum class ValidateErrorCode
		{
			//Class
			AbstractClassInstantiated = 1,
			AmbiguousClass,
			MissingRequiredClass,
			UnexpectedClass,

			//Property
			InvalidPropertyArguments,
			MissingRequiredProperty,
			UnexpectedProperty
		};


		namespace detail
		{
			struct compile_error_category_impl final : std::error_category
			{
				virtual const char* name() const noexcept override;
				virtual std::string message(int error_value) const override;
			};

			struct validate_error_category_impl final : std::error_category
			{
				virtual const char* name() const noexcept override;
				virtual std::string message(int error_value) const override;
			};


			const std::error_category& compile_error_category() noexcept;
			const std::error_category& validate_error_category() noexcept;
		} //detail


		std::error_condition make_error_condition(CompileErrorCode error) noexcept;
		std::error_condition make_error_condition(ValidateErrorCode error) noexcept;
	} //script_error


	///@brief A base class representing a general scripting error with the error condition
	struct ScriptError
	{
		std::error_condition Condition;

		[[nodiscard]] inline explicit operator bool() const noexcept
		{
			return !!Condition;
		}
	};
	
	///@brief A class representing a script compilation error with the file path and line number
	struct CompileError final : ScriptError
	{
		std::filesystem::path FilePath;
		int LineNumber = 0;
	};

	///@brief A class representing a script validation error with the fully qualified name
	struct ValidateError final : ScriptError
	{
		std::string FullyQualifiedName;
	};
} //ion::script


//Registers custom error conditions to namespace std
namespace std
{
	template<> struct is_error_condition_enum<ion::script::script_error::CompileErrorCode> : std::true_type
	{
	};

	template<> struct is_error_condition_enum<ion::script::script_error::ValidateErrorCode> : std::true_type
	{
	};
} //std

#endif