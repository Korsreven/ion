/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system
File:	IonGraphicsAPI.h
-------------------------------------------
*/

#ifndef ION_GRAPHICS_API_H
#define ION_GRAPHICS_API_H

#include <gl/glew.h> //OpenGL API
#define ION_GLEW

#ifdef ION_GLEW
	#ifdef _WIN32 //For both 32 and 64 bit targets
		#include <GL/wglew.h> //Windows GL extensions
		#define ION_WIN_GLEW
	#endif
#endif

#endif