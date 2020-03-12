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
#include "system/IonSystemAPI.h"

namespace ion
{

using namespace engine;
using namespace types::type_literals;

namespace engine::detail
{

} //engine::detail


/*
	Notifying
*/

bool Engine::NotifyFrameStarted(duration time) noexcept
{
	for (auto &listener : Listeners())
	{
		if (!Notify(&events::listeners::FrameListener::FrameStarted, listener, time).value_or(true))
			return false;
	}

	return true;
}

bool Engine::NotifyFrameEnded(duration time) noexcept
{
	for (auto &listener : Listeners())
	{
		if (!Notify(&events::listeners::FrameListener::FrameEnded, listener, time).value_or(true))
			return false;
	}

	return true;
}


void Draw()
{
	/*glBegin(GL_QUADS);
	glColor3fv(graphics::utilities::color::Beige.Channels());
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, 1.0f);
	glEnd();*/

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
}

bool Engine::UpdateFrame() noexcept
{
	static bool syncronize = false; //TEMP

	if (!NotifyFrameStarted(0.0_sec))
		return false;

	viewport_->Change();
	//camera_->Update();
	glLoadIdentity();
	Draw();

	viewport2_->Change();
	//camera_->Update();
	glLoadIdentity();
	Draw();

	viewport3_->Change();
	//camera_->Update();
	glLoadIdentity();
	Draw();

	viewport4_->Change();
	//camera_->Update();
	glLoadIdentity();
	Draw();

	if (syncronize)
		glFinish();
	else
		glFlush();

	render_window_->SwapBuffers();
	return NotifyFrameEnded(0.0_sec);
}


/*
	Game loop
*/

int Engine::Start() noexcept
{
	if (!render_window_ || !render_window_->Created())
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


	#ifdef ION_WIN32
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
	#else
	return 0;
	#endif
}


/*
	Window
*/

graphics::render::RenderWindow& Engine::RenderTo(graphics::render::RenderWindow &&render_window) noexcept
{
	render_window_.emplace(std::move(render_window));

	viewport_.emplace(graphics::render::Viewport::TopLeftAligned(*render_window_, 0.5_r, 0.5_r));
	viewport_->BackgroundColor(graphics::utilities::color::Beige);

	viewport2_.emplace(graphics::render::Viewport::TopRightAligned(*render_window_, 0.5_r, 0.5_r));
	viewport2_->BackgroundColor(graphics::utilities::color::Bisque);

	viewport3_.emplace(graphics::render::Viewport::BottomLeftAligned(*render_window_, 0.5_r, 0.5_r));
	viewport3_->BackgroundColor(graphics::utilities::color::BlanchedAlmond);

	viewport4_.emplace(graphics::render::Viewport::BottomRightAligned(*render_window_, 0.5_r, 0.5_r));
	viewport4_->BackgroundColor(graphics::utilities::color::BurlyWood);

	return *render_window_;
}

} //ion