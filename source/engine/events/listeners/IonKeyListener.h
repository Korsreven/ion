/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonKeyListener.h
-------------------------------------------
*/

#ifndef ION_KEY_LISTENER_H
#define ION_KEY_LISTENER_H

#include "IonListener.h"

namespace ion::events::listeners
{
	enum class KeyButton
	{
		//Alphanumeric keys
		Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		Enter,
		Space,
		Tab,

		//Special keys
		Escape,
		Pause,
		PrintScreen,

		//Modification keys
		CapsLock,
		NumLock,
		ScrollLock,
		Alt,
		LeftAlt,
		RightAlt,
		Ctrl,
		LeftCtrl,
		RightCtrl,
		Shift,
		LeftShift,
		RightShift,

		//Cursor and navigation keys
		Backspace,
		Insert,
		Delete,
		Home,
		End,
		PageUp,
		PageDown,
		UpArrow,
		DownArrow,
		LeftArrow,
		RightArrow,

		//Numeric pad keys	
		Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
		Add,
		Subtract,
		Multiply,
		Divide,
		Decimal,
		Separator,
		
		//Function keys
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
		
		//Windows keys
		LeftWin,
		RightWin,

		//Other keys
		Apps,
		Attn,
		Cancel,
		Clear,
		CrSel,
		EraseEOF,
		Execute,
		ExSel,
		Help,
		NoName,
		PA1,
		Packet,
		Play,
		Print,
		Select,
		Sleep,
		Zoom,

		//Browser keys
		BrowserBack,
		BrowserForward,
		BrowserRefresh,
		BrowserStop,
		BrowserFavorites,
		BrowserHome,
		BrowserSearch,

		//Volume keys
		VolumeUp,
		VolumeDown,
		VolumeMute,

		//Media keys
		MediaPlayPause,
		MediaStop,
		MediaNextTrack,
		MediaPrevTrack,

		//Launch keys
		LaunchApp1,
		LaunchApp2,
		LaunchMediaSelect,
		LaunchMail,

		//IME keys
		IMEAccept,
		IMEConvert,
		IMEFinal,
		IMEHangulKana,
		IMEHanjaKanji,
		IMEJunja,
		IMEModeChange,
		IMENonConvert,
		IMEProcess,

		//OEM keys
		OEMPlus,
		OEMMinus,
		OEMComma,
		OEMPeriod,
		OEM1,
		OEM2,
		OEM3,
		OEM4,
		OEM5,
		OEM6,
		OEM7,
		OEM8,
		OEM102,
		OEMClear
	};


	///@brief A class representing a listener that listens to key events
	struct KeyListener : Listener<KeyListener>
	{
		/**
			@name Events
			@{
		*/

		///@brief Called when a key button is pressed, with the button that was pressed
		virtual void KeyPressed(KeyButton button) noexcept = 0;

		///@brief Called when a key is released, with the button that was released
		virtual void KeyReleased(KeyButton button) noexcept = 0;


		///@brief Called when a character is pressed, with the character that was pressed
		///@details This function is called repeatedly if the button is held down
		virtual void CharacterPressed([[maybe_unused]] char character) noexcept
		{
			//Optional to override
		}

		///@}
	};
} //ion::events::listeners

#endif