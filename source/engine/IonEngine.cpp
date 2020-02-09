/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	engine
File:	IonEngine.cpp
-------------------------------------------
*/

#include "IonEngine.h"

#include "graphics/IonGraphicsAPI.h"
#include "graphics/utilities/IonColor.h"
#include "types/IonTypes.h"

namespace ion
{

using namespace engine;
using namespace types::type_literals;

namespace engine::detail
{

} //engine::detail


bool Engine::UpdateFrame() noexcept
{
	static bool syncronize = false; //TEMP

	for (auto &listener : Listeners())
	{
		if (!listener.FrameStarted(0.0_sec))
			return false;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glBegin(GL_QUADS);
	glColor3fv(graphics::utilities::color::Beige.Channels());
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3fv(graphics::utilities::color::Red.Channels());
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, 0.8f);
	glVertex2f(-0.8f, 0.8f);
	glVertex2f(-0.8f, 1.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3fv(graphics::utilities::color::Green.Channels());
	glVertex2f(-0.1f, 0.1f);
	glVertex2f(-0.1f, -0.1f);
	glVertex2f(0.1f, -0.1f);
	glVertex2f(0.1f, 0.1f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3fv(graphics::utilities::color::Blue.Channels());
	glVertex2f(0.8f, -0.8f);
	glVertex2f(0.8f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, -0.8f);
	glEnd();


	if (syncronize)
		glFinish();
	else
		glFlush();

	SwapBuffers(window_->DeviceContext());

	for (auto &listener : Listeners())
	{
		if (!listener.FrameEnded(0.0_sec))
			return false;
	}

	return true;
}


/*
	Game loop
*/

int Engine::Start() noexcept
{
	if (!window_ || !window_->Handle())
		return 1;

	if (glewInit() != GLEW_OK)
		return 1;

	glShadeModel(GL_SMOOTH);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glAlphaFunc(GL_GREATER, 0.0f); //Only draw pixels greater than 0% alpha
	glEnable(GL_ALPHA_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


	MSG message;

	//Main loop
	while (true)
	{
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			if (message.message == WM_QUIT)
				break;

			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else if (!UpdateFrame())
			break;
	}

	return static_cast<int>(message.wParam);
}


/*
	Window
*/

system::Window& Engine::RenderTo(system::Window &&window) noexcept
{
	return window_.emplace(std::move(window));
}

} //ion