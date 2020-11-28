/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptBuilder.cpp
-------------------------------------------
*/

#include "IonScriptBuilder.h"

#include <ctime>

#include "utilities/IonFileUtility.h"
#include "utilities/IonStringUtility.h"

namespace ion::script
{

using namespace script_builder;
using namespace std::string_view_literals;

namespace script_builder::detail
{

std::string current_date_time()
{
	std::array<char, 32> data;
	auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());	
	return std::strftime(std::data(data), std::size(data), "%F %T", std::localtime(&now)) != 0 ?
		std::data(data) : "";
}

std::string print_output(std::filesystem::path file_path, duration built_time, ScriptError error, OutputOptions output_options)
{
	std::string output;

	if (output_options == OutputOptions::Header ||
		output_options == OutputOptions::HeaderAndSummary)
	{
		if (!std::empty(output))
			output += "\n\n";

		output += ion::utilities::string::Concat(
			file_path.filename().string(), "\n",
			current_date_time());
	}

	if (output_options == OutputOptions::HeaderAndSummary)
	{
		if (!std::empty(output))
			output += "\n\n";

		output += ion::utilities::string::Concat(
			"[Build summary]\n"
			"Message - ",
				!error ?
				"Build succeeded!" :
				ion::utilities::string::Concat("Build failed. ", error.Condition.message()),
			"\n"
			"Built time - ", ion::utilities::string::Format(built_time.count(), "0.0000"sv), " seconds");
	}

	return output;
}

} //script_builder::detail


//ScriptBuilder
//Private

/*
	Helper functions
*/

bool ScriptBuilder::ValidateTree()
{
	return validator_ && tree_ && !validator_->Validate(*tree_, validate_error_) ?
		false :
		tree_.has_value();
}

void ScriptBuilder::SaveOutput() noexcept
{
	if (output_options_)
		ion::utilities::file::Save(
			compile_error_.FilePath.string() + ".output.txt", PrintOutput(*output_options_));
}

//Public

ScriptBuilder::ScriptBuilder(const assets::repositories::ScriptRepository &repository) :
	compiler_{repository}
{
	//Empty
}


/*
	Building
*/

bool ScriptBuilder::Build(std::string_view name)
{
	tree_.reset();
	compile_error_ = {}; //Reset
	validate_error_ = {}; //Reset

	tree_ = compiler_.Compile(name, compile_error_);
	auto result = ValidateTree();
	SaveOutput();
	return result;
}

bool ScriptBuilder::BuildFile(std::filesystem::path file_path)
{
	auto root_path = file_path.parent_path();
	return BuildFile(std::move(file_path), std::move(root_path));
}

bool ScriptBuilder::BuildFile(std::filesystem::path file_path, std::filesystem::path root_path)
{
	tree_.reset();
	compile_error_ = {}; //Reset
	validate_error_ = {}; //Reset

	tree_ = compiler_.CompileFile(std::move(file_path), std::move(root_path), compile_error_);
	auto result = ValidateTree();
	SaveOutput();
	return result;
}

bool ScriptBuilder::BuildString(std::string str)
{
	return BuildString(std::move(str), ".");
}

bool ScriptBuilder::BuildString(std::string str, std::filesystem::path root_path)
{
	tree_.reset();
	compile_error_ = {}; //Reset
	validate_error_ = {}; //Reset

	tree_ = compiler_.CompileString(std::move(str), std::move(root_path), compile_error_);
	auto result = ValidateTree();
	SaveOutput();
	return result;
}


/*
	Outputting
*/

std::string ScriptBuilder::PrintOutput(OutputOptions output_options) const
{
	auto error = compile_error_ ?
		ScriptError{compile_error_} :
		ScriptError{validate_error_};

	auto output = detail::print_output(compile_error_.FilePath,
		compiler_.CompileTime() + (validator_ ? validator_->ValidateTime() : duration{}),
		error, output_options);

	if (compiler_output_options_)
	{
		if (!std::empty(output))
			output += "\n\n";

		output += compiler_.PrintOutput(*compiler_output_options_);
	}

	if (!compile_error_ && validator_ && validator_output_options_)
	{
		if (!std::empty(output))
			output += "\n\n";

		output += validator_->PrintOutput(*validator_output_options_);
	}

	if (tree_ && tree_print_options_)
	{
		if (!std::empty(output))
			output += "\n\n[Tree view]";

		output += tree_->Print(*tree_print_options_);
	}

	return output;
}

} //ion::script