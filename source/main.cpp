/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	engine
File:	main.cpp
-------------------------------------------
*/

#include "IonEngine.h"

using namespace ion::graphics::render;
using namespace ion::types::type_literals;

#ifdef ION_WIN32
//Entry point for windows 32/64 bit
int WINAPI WinMain([[maybe_unused]] _In_ HINSTANCE instance,
				   [[maybe_unused]] _In_opt_ HINSTANCE prev_instance,
				   [[maybe_unused]] _In_ LPSTR cmd_line,
				   [[maybe_unused]] _In_ int cmd_show)
#else
//Entry point for non windows systems
int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
#endif
{
	ion::Engine engine;

	//Create render window
	engine.RenderTo(
		RenderWindow::Resizable("ION engine", {1280.0_r, 720.0_r})
	);

	//Initialize engine
	if (!engine.Initialize())
		return 1;

	//Start rendering
	return engine.Start();
}