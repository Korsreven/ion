/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system
File:	IonSystemAPI.h
-------------------------------------------
*/

#ifndef ION_SYSTEM_API_H
#define ION_SYSTEM_API_H

#ifdef _WIN32 //Defined for both 32 and 64 bit targets
	#include <windows.h> //Windows API
	#define ION_WIN32

	#ifdef _WIN64 //Defined for 64 bit target only
		#define ION_WIN64
	#endif
#endif

#endif