/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system/events
File:	IonSystemInput.h
-------------------------------------------
*/

#ifndef ION_SYSTEM_INPUT_H
#define ION_SYSTEM_INPUT_H

#include <algorithm>
#include <array>
#include <optional>
#include <utility>

#include "system/IonSystemAPI.h"

#ifdef ION_WIN32
	//This value must be defined to handle the WM_MOUSEHWHEEL on 
	//Windows 2000 and Windows XP
	#ifndef WM_MOUSEWHEEL
		#define WM_MOUSEWHEEL 0x020A
	#endif
#endif

#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonMouseListener.h"

///@brief Namespace containing key and mouse input mappings that are system specific
///@details Functions, classes and class members may need different implementation based on the underlying OS.
///System specific code should have its own define directive
namespace ion::system::events
{
	using ion::events::listeners::KeyButton;
	using ion::events::listeners::MouseButton;

	#ifdef ION_WIN32
	using CodeType = WPARAM;
	#endif


	namespace detail
	{
		template <typename T, size_t Size>
		constexpr auto make_reverse_map(const std::array<T, Size> &code_map) noexcept
		{
			using U = std::pair<typename T::second_type, typename T::first_type>;
			std::array<U, Size> reverse_code_map;

			for (auto i = 0; auto &[key, value] : code_map)
			{
				reverse_code_map[i] = {value, key};
				++i;
			}

			std::sort(std::begin(reverse_code_map), std::end(reverse_code_map),
				[](const U &x, const U &y) noexcept
				{
					return x.first < y.first;
				});

			return reverse_code_map;
		}


		using key_pair_type = std::pair<CodeType, KeyButton>;
		using mouse_pair_type = std::pair<CodeType, MouseButton>;

		#ifdef ION_WIN32
		constexpr std::array key_button_map
		{
			key_pair_type{VK_CANCEL,				KeyButton::Cancel},
			key_pair_type{VK_BACK,					KeyButton::Backspace},
			key_pair_type{VK_TAB,					KeyButton::Tab},
			key_pair_type{VK_CLEAR,					KeyButton::Clear},
			key_pair_type{VK_RETURN,				KeyButton::Enter},
			key_pair_type{VK_SHIFT,					KeyButton::Shift},
			key_pair_type{VK_CONTROL,				KeyButton::Ctrl},
			key_pair_type{VK_MENU,					KeyButton::Alt},
			key_pair_type{VK_PAUSE,					KeyButton::Pause},
			key_pair_type{VK_CAPITAL,				KeyButton::CapsLock},
			key_pair_type{VK_HANGUL /*VK_KANA*/,	KeyButton::IMEHangulKana},
			key_pair_type{VK_JUNJA,					KeyButton::IMEJunja},
			key_pair_type{VK_FINAL,					KeyButton::IMEFinal},
			key_pair_type{VK_HANJA /*VK_KANJI*/,	KeyButton::IMEHanjaKanji},
			key_pair_type{VK_ESCAPE,				KeyButton::Escape},
			key_pair_type{VK_CONVERT,				KeyButton::IMEConvert},
			key_pair_type{VK_NONCONVERT,			KeyButton::IMENonConvert},
			key_pair_type{VK_ACCEPT,				KeyButton::IMEAccept},
			key_pair_type{VK_MODECHANGE,			KeyButton::IMEModeChange},
			key_pair_type{VK_SPACE,					KeyButton::Space},
			key_pair_type{VK_PRIOR,					KeyButton::PageUp},
			key_pair_type{VK_NEXT,					KeyButton::PageDown},
			key_pair_type{VK_END,					KeyButton::End},
			key_pair_type{VK_HOME,					KeyButton::Home},
			key_pair_type{VK_LEFT,					KeyButton::LeftArrow},
			key_pair_type{VK_UP,					KeyButton::UpArrow},
			key_pair_type{VK_RIGHT,					KeyButton::RightArrow},
			key_pair_type{VK_DOWN,					KeyButton::DownArrow},
			key_pair_type{VK_SELECT,				KeyButton::Select},
			key_pair_type{VK_PRINT,					KeyButton::Print},
			key_pair_type{VK_EXECUTE,				KeyButton::Execute},
			key_pair_type{VK_SNAPSHOT,				KeyButton::PrintScreen},
			key_pair_type{VK_INSERT,				KeyButton::Insert},
			key_pair_type{VK_DELETE,				KeyButton::Delete},
			key_pair_type{VK_HELP,					KeyButton::Help},
			key_pair_type{0x30,						KeyButton::Num0},
			key_pair_type{0x31,						KeyButton::Num1},
			key_pair_type{0x32,						KeyButton::Num2},
			key_pair_type{0x33,						KeyButton::Num3},
			key_pair_type{0x34,						KeyButton::Num4},
			key_pair_type{0x35,						KeyButton::Num5},
			key_pair_type{0x36,						KeyButton::Num6},
			key_pair_type{0x37,						KeyButton::Num7},
			key_pair_type{0x38,						KeyButton::Num8},
			key_pair_type{0x39,						KeyButton::Num9},
			key_pair_type{0x41,						KeyButton::A},
			key_pair_type{0x42,						KeyButton::B},
			key_pair_type{0x43,						KeyButton::C},
			key_pair_type{0x44,						KeyButton::D},
			key_pair_type{0x45,						KeyButton::E},
			key_pair_type{0x46,						KeyButton::F},
			key_pair_type{0x47,						KeyButton::G},
			key_pair_type{0x48,						KeyButton::H},
			key_pair_type{0x49,						KeyButton::I},
			key_pair_type{0x4A,						KeyButton::J},
			key_pair_type{0x4B,						KeyButton::K},
			key_pair_type{0x4C,						KeyButton::L},
			key_pair_type{0x4D,						KeyButton::M},
			key_pair_type{0x4E,						KeyButton::N},
			key_pair_type{0x4F,						KeyButton::O},
			key_pair_type{0x50,						KeyButton::P},
			key_pair_type{0x51,						KeyButton::Q},
			key_pair_type{0x52,						KeyButton::R},
			key_pair_type{0x53,						KeyButton::S},
			key_pair_type{0x54,						KeyButton::T},
			key_pair_type{0x55,						KeyButton::U},
			key_pair_type{0x56,						KeyButton::V},
			key_pair_type{0x57,						KeyButton::W},
			key_pair_type{0x58,						KeyButton::X},
			key_pair_type{0x59,						KeyButton::Y},
			key_pair_type{0x60,						KeyButton::Z},
			key_pair_type{VK_LWIN,					KeyButton::LeftWin},
			key_pair_type{VK_RWIN,					KeyButton::RightWin},
			key_pair_type{VK_APPS,					KeyButton::Apps},
			key_pair_type{VK_SLEEP,					KeyButton::Sleep},
			key_pair_type{VK_NUMPAD0,				KeyButton::Numpad0},
			key_pair_type{VK_NUMPAD1,				KeyButton::Numpad1},
			key_pair_type{VK_NUMPAD2,				KeyButton::Numpad2},
			key_pair_type{VK_NUMPAD3,				KeyButton::Numpad3},
			key_pair_type{VK_NUMPAD4,				KeyButton::Numpad4},
			key_pair_type{VK_NUMPAD5,				KeyButton::Numpad5},
			key_pair_type{VK_NUMPAD6,				KeyButton::Numpad6},
			key_pair_type{VK_NUMPAD7,				KeyButton::Numpad7},
			key_pair_type{VK_NUMPAD8,				KeyButton::Numpad8},
			key_pair_type{VK_NUMPAD9,				KeyButton::Numpad9},
			key_pair_type{VK_MULTIPLY,				KeyButton::Multiply},
			key_pair_type{VK_ADD,					KeyButton::Add},
			key_pair_type{VK_SEPARATOR,				KeyButton::Separator},
			key_pair_type{VK_SUBTRACT,				KeyButton::Subtract},
			key_pair_type{VK_DECIMAL,				KeyButton::Decimal},
			key_pair_type{VK_DIVIDE,				KeyButton::Divide},
			key_pair_type{VK_F1,					KeyButton::F1},
			key_pair_type{VK_F2,					KeyButton::F2},
			key_pair_type{VK_F3,					KeyButton::F3},
			key_pair_type{VK_F4,					KeyButton::F4},
			key_pair_type{VK_F5,					KeyButton::F5},
			key_pair_type{VK_F6,					KeyButton::F6},
			key_pair_type{VK_F7,					KeyButton::F7},
			key_pair_type{VK_F8,					KeyButton::F8},
			key_pair_type{VK_F9,					KeyButton::F9},
			key_pair_type{VK_F10,					KeyButton::F10},
			key_pair_type{VK_F11,					KeyButton::F11},
			key_pair_type{VK_F12,					KeyButton::F12},
			key_pair_type{VK_F13,					KeyButton::F13},
			key_pair_type{VK_F14,					KeyButton::F14},
			key_pair_type{VK_F15,					KeyButton::F15},
			key_pair_type{VK_F16,					KeyButton::F16},
			key_pair_type{VK_F17,					KeyButton::F17},
			key_pair_type{VK_F18,					KeyButton::F18},
			key_pair_type{VK_F19,					KeyButton::F19},
			key_pair_type{VK_F20,					KeyButton::F20},
			key_pair_type{VK_F21,					KeyButton::F21},
			key_pair_type{VK_F22,					KeyButton::F22},
			key_pair_type{VK_F23,					KeyButton::F23},
			key_pair_type{VK_F24,					KeyButton::F24},
			key_pair_type{VK_NUMLOCK,				KeyButton::NumLock},
			key_pair_type{VK_SCROLL,				KeyButton::ScrollLock},
			key_pair_type{VK_LSHIFT,				KeyButton::LeftShift},
			key_pair_type{VK_RSHIFT,				KeyButton::RightShift},
			key_pair_type{VK_LCONTROL,				KeyButton::LeftCtrl},
			key_pair_type{VK_RCONTROL,				KeyButton::RightCtrl},
			key_pair_type{VK_LMENU,					KeyButton::LeftAlt},
			key_pair_type{VK_RMENU,					KeyButton::RightAlt},
			key_pair_type{VK_BROWSER_BACK,			KeyButton::BrowserBack},
			key_pair_type{VK_BROWSER_FORWARD,		KeyButton::BrowserForward},
			key_pair_type{VK_BROWSER_REFRESH,		KeyButton::BrowserRefresh},
			key_pair_type{VK_BROWSER_STOP,			KeyButton::BrowserStop},
			key_pair_type{VK_BROWSER_SEARCH,		KeyButton::BrowserSearch},
			key_pair_type{VK_BROWSER_FAVORITES,		KeyButton::BrowserFavorites},
			key_pair_type{VK_BROWSER_HOME,			KeyButton::BrowserHome},
			key_pair_type{VK_VOLUME_MUTE,			KeyButton::VolumeMute},
			key_pair_type{VK_VOLUME_DOWN,			KeyButton::VolumeDown},
			key_pair_type{VK_VOLUME_UP,				KeyButton::VolumeUp},
			key_pair_type{VK_MEDIA_NEXT_TRACK,		KeyButton::MediaNextTrack},
			key_pair_type{VK_MEDIA_PREV_TRACK,		KeyButton::MediaPrevTrack},
			key_pair_type{VK_MEDIA_STOP,			KeyButton::MediaStop},
			key_pair_type{VK_MEDIA_PLAY_PAUSE,		KeyButton::MediaPlayPause},
			key_pair_type{VK_LAUNCH_MAIL,			KeyButton::LaunchMail},
			key_pair_type{VK_LAUNCH_MEDIA_SELECT,	KeyButton::LaunchMediaSelect},
			key_pair_type{VK_LAUNCH_APP1,			KeyButton::LaunchApp1},
			key_pair_type{VK_LAUNCH_APP2,			KeyButton::LaunchApp2},
			key_pair_type{VK_OEM_1,					KeyButton::OEM1},
			key_pair_type{VK_OEM_PLUS,				KeyButton::OEMPlus},
			key_pair_type{VK_OEM_COMMA,				KeyButton::OEMComma},
			key_pair_type{VK_OEM_MINUS,				KeyButton::OEMMinus},
			key_pair_type{VK_OEM_PERIOD,			KeyButton::OEMPeriod},
			key_pair_type{VK_OEM_2,					KeyButton::OEM2},
			key_pair_type{VK_OEM_3,					KeyButton::OEM3},
			key_pair_type{VK_OEM_4,					KeyButton::OEM4},
			key_pair_type{VK_OEM_5,					KeyButton::OEM5},
			key_pair_type{VK_OEM_6,					KeyButton::OEM6},
			key_pair_type{VK_OEM_7,					KeyButton::OEM7},
			key_pair_type{VK_OEM_8,					KeyButton::OEM8},
			key_pair_type{VK_OEM_102,				KeyButton::OEM102},
			key_pair_type{VK_PROCESSKEY,			KeyButton::IMEProcess},
			key_pair_type{VK_PACKET,				KeyButton::Packet},
			key_pair_type{VK_ATTN,					KeyButton::Attn},
			key_pair_type{VK_CRSEL,					KeyButton::CrSel},
			key_pair_type{VK_EXSEL,					KeyButton::ExSel},
			key_pair_type{VK_EREOF,					KeyButton::EraseEOF},
			key_pair_type{VK_PLAY,					KeyButton::Play},
			key_pair_type{VK_ZOOM,					KeyButton::Zoom},
			key_pair_type{VK_NONAME,				KeyButton::NoName},
			key_pair_type{VK_PA1,					KeyButton::PA1},
			key_pair_type{VK_OEM_CLEAR,				KeyButton::OEMClear}
		};

		constexpr std::array mouse_button_map
		{
			mouse_pair_type{MK_LBUTTON,				MouseButton::Left},
			mouse_pair_type{MK_RBUTTON,				MouseButton::Right},
			mouse_pair_type{MK_MBUTTON,				MouseButton::Middle},
			mouse_pair_type{MK_XBUTTON1,			MouseButton::X1},
			mouse_pair_type{MK_XBUTTON2,			MouseButton::X2}
		};
		#endif
		
		constexpr std::array reverse_key_button_map = make_reverse_map(key_button_map);
		constexpr std::array reverse_mouse_button_map = make_reverse_map(mouse_button_map);


		template <typename T, typename U>
		inline auto get_mapped_code(const T &code_map, const U &code) noexcept
		{
			auto iter = std::lower_bound(std::begin(code_map), std::end(code_map), code,
				[](const typename T::value_type &x, const U &y) noexcept
				{
					return x.first < y;
				});

			return iter != std::end(code_map) &&
				iter->first == code ?

				std::make_optional(iter->second) :
				std::nullopt;
		}
	} //detail


	/**
		@name Get key/mouse button
		@{
	*/

	///@brief Returns the key button corresponding to the system specific input code
	[[nodiscard]] inline auto GetMappedKeyButton(CodeType code) noexcept
	{
		return detail::get_mapped_code(detail::key_button_map, code);
	}

	///@brief Returns the mouse button corresponding to the system specific input code
	[[nodiscard]] inline auto GetMappedMouseButton(CodeType code) noexcept
	{
		return detail::get_mapped_code(detail::mouse_button_map, code);
	}

	///@}

	/**
		@name Get input code
		@{
	*/

	///@brief Returns the system specific input code corresponding to the key button
	[[nodiscard]] inline auto GetMappedInputCode(KeyButton button) noexcept
	{
		return detail::get_mapped_code(detail::reverse_key_button_map, button);
	}

	///@brief Returns the system specific input code corresponding to the mouse button
	[[nodiscard]] inline auto GetMappedInputCode(MouseButton button) noexcept
	{
		return detail::get_mapped_code(detail::reverse_mouse_button_map, button);
	}

	///@}
} //ion::system::events

#endif