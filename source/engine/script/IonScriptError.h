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

#ifndef _ION_SCRIPT_ERROR_
#define _ION_SCRIPT_ERROR_

#include <filesystem>
#include <optional>
#include <string>
#include <system_error>
#include <type_traits>

namespace ion::script
{
	namespace script_error
	{
		enum class CompileErrorCode
		{
			//File
			CircularImport = 1,
			MissingImportFile,

			//Function
			EmptyFunctionArgument,
			InvalidFunctionArgument,
			InvalidNumberOfFunctionArguments,
			UnexpectedFunction,

			//Identifier
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
			UnexpectedImportStatement,

			//Separator
			EmptyParentheses,
			MissingCloseCurlyBrace,
			MissingOpenCurlyBrace,
			MissingCloseParenthesis,
			MissingOpenParenthesis,
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


	struct ScriptError
	{
		std::error_condition Condition;

		[[nodiscard]] inline explicit operator bool() const noexcept
		{
			return !!Condition;
		}
	};
	
	struct CompileError final : ScriptError
	{	
		int LineNumber = 0;
		std::filesystem::path FilePath;
	};

	struct ValidateError final : ScriptError
	{
		std::string FullyQualifiedName;
	};
} //ion::script


//Register custom error conditions to namespace std
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