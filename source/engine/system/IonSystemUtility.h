/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system
File:	IonSystemUtility.h
-------------------------------------------
*/

#ifndef ION_SYSTEM_UTILITY_H
#define ION_SYSTEM_UTILITY_H

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "system/IonSystemAPI.h"
#include "types/IonTypes.h"

namespace ion::system::utilities
{
	enum class ProcessWindowCommand
	{
		Hidden,
		Minimized,
		Maximized,
		Normal
	};

	struct PowerStatus final
	{
		std::optional<bool> BatteryRunning;
		std::optional<bool> BatteryCharging;
		std::optional<real> BatteryPercent;
		std::optional<std::chrono::seconds> BatteryLifeTime;
		std::optional<std::chrono::seconds> BatteryFullLifeTime;
	};

	namespace detail
	{
		class clipboard final
		{
			private:

				bool open_ = false;

			public:

				#ifdef ION_WIN32

				struct global_alloc_guard final
				{
					HGLOBAL buffer = nullptr;

					global_alloc_guard(std::string_view text) noexcept;
					~global_alloc_guard();
				};

				struct global_lock_guard final
				{
					HGLOBAL buffer = nullptr;

					global_lock_guard(HGLOBAL buffer) noexcept;
					~global_lock_guard();
				};

				#endif

				clipboard() noexcept;
				~clipboard();

				bool set(std::string_view text) noexcept;
				std::optional<std::string> get();
		};

		std::string_view command_line() noexcept;
		std::pair<std::optional<std::string>, std::string_view::const_iterator> next_command_line_argument(
			std::string_view cmd_line, std::string_view::const_iterator where);

		bool open_or_execute(const std::filesystem::path &path,
			std::optional<std::string> parameters, std::optional<std::filesystem::path> current_path,
			ProcessWindowCommand window_command) noexcept;

		std::optional<const PowerStatus> power_status() noexcept;
	} //detail


	/*
		Clipboard
	*/

	//Copy the given text, to the system clipboard
	bool Clipboard(std::string_view text) noexcept;

	//Returns the text currently stored in the system clipboard (if any)
	[[nodiscard]] std::optional<std::string> Clipboard();


	/*
		Command line
	*/

	//Returns the full application path, including the executable name
	[[nodiscard]] std::optional<std::filesystem::path> ApplicationPath();

	//Returns the command line given to the executable at start
	[[nodiscard]] std::string_view CommandLine() noexcept;

	//Returns all the command line arguments given to the executable at start
	//Splitting arguments by using space as the delimiter, unless double quoted
	[[nodiscard]] std::vector<std::string> CommandLineArguments();


	/*
		Execution
	*/

	//Ask the underlying system to execute a program or open a file/directory
	bool Execute(const std::filesystem::path &path,
		ProcessWindowCommand window_command = ProcessWindowCommand::Normal) noexcept;

	//Ask the underlying system to execute a program or open a file/directory
	//Send in one or more parameters, or another current path to be used for the execution
	bool Execute(const std::filesystem::path &path,
		std::optional<std::string> parameters, std::optional<std::filesystem::path> current_path,
		ProcessWindowCommand window_command = ProcessWindowCommand::Normal) noexcept;


	/*
		Power
	*/

	//Returns the system power status, such as battery information
	[[nodiscard]] std::optional<const PowerStatus> Power() noexcept;
} //ion::system::utilities

#endif