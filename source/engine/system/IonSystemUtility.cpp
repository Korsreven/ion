/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system
File:	IonSystemUtility.cpp
-------------------------------------------
*/

#include "IonSystemUtility.h"

#include <algorithm>
#include <array>
#include <tuple>

#ifdef ION_WIN32
	#include <shellapi.h> //Windows shell API
#endif

#include "events/IonSystemInput.h"

namespace ion::system::utilities
{

namespace detail
{

using namespace types::type_literals;

#ifdef ION_WIN32

clipboard::global_alloc_guard::global_alloc_guard(std::string_view text) noexcept :
	buffer{GlobalAlloc(GMEM_MOVEABLE, std::size(text) + 1)}
{
	//Empty
}

clipboard::global_alloc_guard::~global_alloc_guard()
{
	if (buffer)
		GlobalFree(buffer);
}

clipboard::global_lock_guard::global_lock_guard(HGLOBAL buffer) noexcept :
	buffer{GlobalLock(buffer)}
{
	//Empty
}

clipboard::global_lock_guard::~global_lock_guard()
{
	if (buffer)
		GlobalUnlock(buffer);
}

#endif

clipboard::clipboard() noexcept :
	open_{
		#ifdef ION_WIN32
		!!OpenClipboard(nullptr)
		#else
		false
		#endif
	}
{
	//Empty
}

clipboard::~clipboard()
{
	if (open_)
	{
		#ifdef ION_WIN32
		CloseClipboard();
		#else

		#endif
	}
}

bool clipboard::set(std::string_view text) noexcept
{
	if (open_)
	{
		#ifdef ION_WIN32
		global_alloc_guard global_alloc_handle{text};

		if (global_alloc_handle.buffer)
		{
			global_lock_guard global_lock_handle{global_alloc_handle.buffer};

			if (global_lock_handle.buffer)
			{
				strcpy(static_cast<char*>(global_lock_handle.buffer), std::data(text));

				if (EmptyClipboard() &&
					SetClipboardData(CF_TEXT, global_alloc_handle.buffer))
				{
					//On success, SetClipboard takes ownership of the allocated buffer
					global_alloc_handle.buffer = nullptr;
					return true;
				}
			}
		}
		#else

		#endif
	}

	return false;
}

std::optional<std::string> clipboard::get()
{
	if (open_)
	{
		#ifdef ION_WIN32
		if (auto buffer = GetClipboardData(CF_TEXT))
		{
			global_lock_guard global_lock_handle{buffer};

			//Copy text from clipboard
			if (global_lock_handle.buffer)
				return std::string(static_cast<const char*>(global_lock_handle.buffer));
		}
		#else

		#endif
	}

	return {};
}

std::string_view command_line() noexcept
{
	#ifdef ION_WIN32
	return GetCommandLine();
	#else
	return "";
	#endif
}

std::pair<std::optional<std::string>, std::string_view::const_iterator> next_command_line_argument(
	std::string_view cmd_line, std::string_view::const_iterator where)
{
	//Skip spaces
	auto first = std::find_if_not(where, std::end(cmd_line),
		[](auto c) noexcept
		{
			return c == ' ';
		});

	std::string argument;
	auto last = [&, in_quotes = false, end = std::end(cmd_line)]() mutable
		{
			for (auto iter = first; iter != end; ++iter)
			{
				auto c = *iter;
				auto next_c = iter + 1 != end ? *(iter + 1) : '\0';

				//Space (outside quotes)
				if (c == ' ' && !in_quotes)
					return iter;

				//Backslash
				else if (c == '\\')
				{
					//Skip trailing backslashes
					auto last = std::find_if_not(iter, end,
						[](auto c) noexcept
						{
							return c == '\\';
						});

					//Trailing double quote
					if (auto count = last - iter; last != end && *last == '"')
					{
						argument.append(count / 2, '\\');
						
						//Not escaped (even count)
						if (count % 2 == 0)
							iter = last - 1;
						//Escaped (odd count)
						else
						{
							argument += *last; //Append double quote
							iter = last;
						}
					}
					else
					{
						argument.append(count, '\\');
						iter = last - 1;
					}
				}

				//Double quote
				else if (c == '"')
				{
					//Escaped by another double quote
					if (in_quotes && next_c == '"')
					{
						argument += next_c; //Append double quote
						++iter;
					}
					//Not escaped
					else
						in_quotes = !in_quotes;
				}

				//Append character
				else
					argument += c;
			}

			return end;
		}();

	return first != last ?
		std::make_pair(std::make_optional(argument), last) :
		std::make_pair(std::nullopt, last);
}

bool open_or_execute(const std::filesystem::path &path,
	std::optional<std::string> parameters, std::optional<std::filesystem::path> current_path,
	ProcessWindowCommand window_command) noexcept
{
	#ifdef ION_WIN32
	auto command = [=]() noexcept
		{
			switch (window_command)
			{
				case ProcessWindowCommand::Hidden:
				return SW_HIDE;

				case ProcessWindowCommand::Minimized:
				return SW_SHOWMINIMIZED;

				case ProcessWindowCommand::Maximized:
				return SW_SHOWMAXIMIZED;

				default:
				return SW_SHOWNORMAL;
			};
		}();

	return reinterpret_cast<int>(
		ShellExecute(
			nullptr,
			"open",
			std::data(path.string()),
			parameters ? std::data(*parameters) : nullptr,
			current_path ? std::data(current_path->string()) : nullptr,
			command)) > 32;
	#else
	return false;
	#endif
}


DisplaySettings display_settings(DisplayDeviceState devices, DisplaySettingModes modes,
	DisplaySettingFrequencies frequencies) noexcept
{
	DisplaySettings settings;

	#ifdef ION_WIN32
	DISPLAY_DEVICE device = {};
	device.cb = sizeof(DISPLAY_DEVICE);

	auto state = devices == DisplayDeviceState::Primary ?
		DISPLAY_DEVICE_PRIMARY_DEVICE :
		DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;

	for (auto i = 0;; ++i)
	{
		//Enumerate display devices
		if (!EnumDisplayDevices(NULL, i, &device, EDD_GET_DEVICE_INTERFACE_NAME))
			break;

		//Skip display devices not attached to desktop
		if (device.StateFlags & state)
		{
			DEVMODE devmode = {};
			devmode.dmSize = sizeof(DEVMODE);

			if (modes == DisplaySettingModes::Current)
			{
				if (EnumDisplaySettings(device.DeviceName, ENUM_CURRENT_SETTINGS, &devmode))
					settings.emplace_back(
						static_cast<int>(devmode.dmPelsWidth),
						static_cast<int>(devmode.dmPelsHeight),
						static_cast<int>(devmode.dmDisplayFrequency));
			}
			else //All
			{
				for (auto j = 0;; ++j)
				{
					//Enumerate display settings for each display device
					if (!EnumDisplaySettings(device.DeviceName, j, &devmode))
						break;

					settings.emplace_back(
						static_cast<int>(devmode.dmPelsWidth),
						static_cast<int>(devmode.dmPelsHeight),
						static_cast<int>(devmode.dmDisplayFrequency));
				}
			}
		}
	}
	#else

	#endif
	
	//Sort ASC
	std::sort(std::begin(settings), std::end(settings),
		[](const auto &x, const auto &y) noexcept
		{
			return std::tuple{x.Width, x.Height, x.Frequency} > std::tuple{y.Width, y.Height, y.Frequency};
		});

	//Remove duplicates
	settings.erase(
		std::unique(std::begin(settings), std::end(settings),
			[](const auto &x, const auto &y) noexcept
			{
				return x.Width == y.Width && x.Height == y.Height && x.Frequency == y.Frequency;
			}), std::end(settings));

	//Filter on frequencies
	if (frequencies != DisplaySettingFrequencies::All)
	{
		DisplaySettings custom_settings;

		for (auto &setting : settings)
		{
			//Equal size
			if (!std::empty(custom_settings) &&
				custom_settings.back().Width == setting.Width &&
				custom_settings.back().Height == setting.Height)
			{
				switch (frequencies)
				{
					case DisplaySettingFrequencies::Lowest:
					custom_settings.back().Frequency = std::min(custom_settings.back().Frequency, setting.Frequency);
					break;

					case DisplaySettingFrequencies::Highest:
					custom_settings.back().Frequency = std::max(custom_settings.back().Frequency, setting.Frequency);
					break;
				}
			}
			else
				custom_settings.push_back(setting);
		}

		settings = std::move(custom_settings);
	}

	return settings;
}

std::optional<std::string> key_button_name(KeyButton button) noexcept
{
	if (auto code = events::GetMappedInputCode(button); code)
	{
		#ifdef ION_WIN32
		if (auto scan_code = MapVirtualKey(*code, MAPVK_VK_TO_VSC); scan_code != 0)
		{
			switch (*code)
			{
				case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: //Arrow keys
				case VK_PRIOR: case VK_NEXT: //Page up and page down
				case VK_END: case VK_HOME:
				case VK_INSERT: case VK_DELETE:
				case VK_DIVIDE: //Numpad slash
				case VK_NUMLOCK:
				{
					scan_code |= 0x100; //Set extended bit
					break;
				}
			}

			std::array<char, 256> result{};
			if (auto size = GetKeyNameText(scan_code << 16, &result[0], std::size(result)); size > 0)
				return std::string(std::data(result), size);
		}
		#else
		
		#endif
	}

	return {};
}

std::optional<std::string> local_time(TimeFormat format) noexcept
{
	#ifdef ION_WIN32
	SYSTEMTIME system_time;
	GetLocalTime(&system_time);

	auto flags = 0;		
	switch (format)
	{
		case TimeFormat::HHMM:
		flags = TIME_NOSECONDS;
		break;

		case TimeFormat::HH:
		flags = TIME_NOMINUTESORSECONDS;
		break;
	}

	if (auto size = GetTimeFormat(LOCALE_USER_DEFAULT, flags, &system_time, nullptr, nullptr, 0); size > 1)
	{
		if (std::string result(size - 1, '\0');
			GetTimeFormat(LOCALE_USER_DEFAULT, flags, &system_time, nullptr, &result[0], size) != 0)
			return result;
	}
	#else

	#endif

	return {};
}

std::optional<PowerStatus> power_status() noexcept
{
	#ifdef ION_WIN32
	SYSTEM_POWER_STATUS system_power_status;

	//Power status available
	if (GetSystemPowerStatus(&system_power_status) != 0)
	{
		PowerStatus status;

		//AC line status available
		if (system_power_status.ACLineStatus != 255)
			status.BatteryRunning = (system_power_status.ACLineStatus == 0);

		//Battery life percent is known
		if (system_power_status.BatteryLifePercent != 255)
			status.BatteryPercent = system_power_status.BatteryLifePercent / 100.0_r;

		//Battery lifetime is known
		if (system_power_status.BatteryLifeTime != static_cast<DWORD>(-1))
			status.BatteryLifetime = std::chrono::seconds{system_power_status.BatteryLifeTime}; //Seconds

		//Battery full lifetime is known
		if (system_power_status.BatteryFullLifeTime != static_cast<DWORD>(-1))
			status.BatteryFullLifetime = std::chrono::seconds{system_power_status.BatteryFullLifeTime}; //Seconds

		//Battery flag is known
		if (system_power_status.BatteryFlag != 255)
			status.BatteryCharging = (system_power_status.BatteryFlag & 8); //Charging
		
		return status;
	}
	#else

	#endif

	return {};
}

} //detail


/*
	Clipboard
*/

bool Clipboard(std::string_view text) noexcept
{
	detail::clipboard clipboard;
	return clipboard.set(text);
}

std::optional<std::string> Clipboard()
{
	detail::clipboard clipboard;
	return clipboard.get();
}


/*
	Command line
*/

std::optional<std::filesystem::path> ApplicationPath()
{
	auto cmd_line = detail::command_line();
	auto [argument, iter] =
		detail::next_command_line_argument(cmd_line, std::cbegin(cmd_line)); //Extract first argument

	return argument ?
		std::make_optional<std::filesystem::path>(*argument) :
		std::nullopt;
}

std::string_view CommandLine() noexcept
{
	return detail::command_line();
}

std::vector<std::string> CommandLineArguments()
{
	auto cmd_line = detail::command_line();
	std::vector<std::string> arguments;

	for (auto iter = std::cbegin(cmd_line),
		end = std::cend(cmd_line); iter != end;)
	{
		auto [argument, next] = detail::next_command_line_argument(cmd_line, iter);
		iter = next;

		if (argument)
			arguments.push_back(std::move(*argument));
	}

	return arguments;
}


/*
	Execution
*/

bool Execute(const std::filesystem::path &path, ProcessWindowCommand window_command) noexcept
{
	return detail::open_or_execute(path, {}, {}, window_command);
}

bool Execute(const std::filesystem::path &path,
	std::optional<std::string> parameters, std::optional<std::filesystem::path> current_path,
	ProcessWindowCommand window_command) noexcept
{
	return detail::open_or_execute(path, parameters, current_path, window_command);
}


/*
	Display resolutions
*/

DisplaySettings AllDisplayResolutions(DisplayDeviceState devices, DisplaySettingFrequencies frequencies) noexcept
{
	return detail::display_settings(devices, DisplaySettingModes::All, frequencies);
}

DisplaySettings CurrentDisplayResolutions(DisplayDeviceState devices) noexcept
{
	return detail::display_settings(devices, DisplaySettingModes::Current, DisplaySettingFrequencies::Highest);
}


/*
	Key button
*/

std::optional<std::string> KeyButtonName(KeyButton button) noexcept
{
	return detail::key_button_name(button);
}


/*
	Power
*/

std::optional<PowerStatus> Power() noexcept
{
	return detail::power_status();
}


/*
	Time
*/

std::optional<std::string> Time(TimeFormat format) noexcept
{
	return detail::local_time(format);
}

} //ion::system::utilities