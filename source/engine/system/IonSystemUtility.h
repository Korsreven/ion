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
#include <vector>

#include "IonSystemAPI.h"
#include "events/listeners/IonKeyListener.h"
#include "types/IonTypes.h"

using ion::events::listeners::KeyButton;

///@brief Namespace containing commonly used utilities that are system specific
///@details Functions, classes and class members may need different implementation based on the underlying OS.
///System specific code should have its own define directive
namespace ion::system::utilities
{
	enum class DisplayDeviceState : bool
	{
		Active,
		Primary
	};

	enum class DisplaySettingModes : bool
	{
		All,
		Current
	};

	enum class DisplaySettingFrequencies
	{
		All,
		Lowest,
		Highest
	};

	enum class ProcessWindowCommand
	{
		Hidden,
		Minimized,
		Maximized,
		Normal
	};

	enum class TimeFormat
	{
		HHMMSS,
		HHMM,
		HH
	};


	struct DisplaySetting final
	{
		int Width = 0;
		int Height = 0;
		int Frequency = 0;
	};

	struct PowerStatus final
	{
		std::optional<bool> BatteryRunning;
		std::optional<bool> BatteryCharging;
		std::optional<real> BatteryPercent;
		std::optional<std::chrono::seconds> BatteryLifetime;
		std::optional<std::chrono::seconds> BatteryFullLifetime;
	};

	using DisplaySettings = std::vector<DisplaySetting>;

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
		
		
		DisplaySettings display_settings(DisplayDeviceState devices, DisplaySettingModes modes,
			DisplaySettingFrequencies frequencies) noexcept;
		std::optional<std::string> key_button_name(KeyButton button) noexcept;
		std::optional<std::string> local_time(TimeFormat format) noexcept;
		std::optional<PowerStatus> power_status() noexcept;
	} //detail


	/**
		@name Clipboard
		@{
	*/

	///@brief Copies the given text, to the system clipboard
	bool Clipboard(std::string_view text) noexcept;

	///@brief Returns the text currently stored in the system clipboard (if any)
	[[nodiscard]] std::optional<std::string> Clipboard();

	///@}

	/**
		@name Command line
		@{
	*/

	///@brief Returns the full application path, including the executable name
	[[nodiscard]] std::optional<std::filesystem::path> ApplicationPath();

	///@brief Returns the command line given to the executable at start
	[[nodiscard]] std::string_view CommandLine() noexcept;

	///@brief Returns all the command line arguments given to the executable at start
	///@details Splitting arguments by using space as the delimiter, unless double quoted
	[[nodiscard]] std::vector<std::string> CommandLineArguments();

	///@}

	/**
		@name Execution
		@{
	*/

	///@brief Ask the underlying system to execute a program or open a file/directory
	bool Execute(const std::filesystem::path &path,
		ProcessWindowCommand window_command = ProcessWindowCommand::Normal) noexcept;

	///@brief Ask the underlying system to execute a program or open a file/directory
	///@details Send in one or more parameters, or another current path to be used for the execution
	bool Execute(const std::filesystem::path &path,
		std::optional<std::string> parameters, std::optional<std::filesystem::path> current_path,
		ProcessWindowCommand window_command = ProcessWindowCommand::Normal) noexcept;

	///@}

	/**
		@name Display resolutions
		@{
	*/

	///@brief Returns all of the display resolutions supported by the connected displays
	[[nodiscard]] DisplaySettings AllDisplayResolutions(DisplayDeviceState devices = DisplayDeviceState::Active,
		DisplaySettingFrequencies frequencies = DisplaySettingFrequencies::Highest) noexcept;

	///@brief Returns the current display resolutions of the connected displays
	[[nodiscard]] DisplaySettings CurrentDisplayResolutions(DisplayDeviceState devices = DisplayDeviceState::Primary) noexcept;

	///@}

	/**
		@name Key button
		@{
	*/

	///@brief Returns the key button name, in the system language
	[[nodiscard]] std::optional<std::string> KeyButtonName(KeyButton button) noexcept;

	///@}

	/**
		@name Power
		@{
	*/

	///@brief Returns the system power status, such as battery information
	[[nodiscard]] std::optional<PowerStatus> Power() noexcept;

	///@}

	/**
		@name Time
		@{
	*/

	///@brief Returns the system time, formatted using the default locale
	[[nodiscard]] std::optional<std::string> Time(TimeFormat format = TimeFormat::HHMMSS) noexcept;

	///@}
} //ion::system::utilities

#endif