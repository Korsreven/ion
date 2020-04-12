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
	glBegin(GL_QUADS);
	glColor3fv(graphics::utilities::color::White.Channels());
	glVertex2f(-1.7778f, 1.0f);
	glVertex2f(-1.7778f, -1.0f);
	glVertex2f(1.7778f, -1.0f);
	glVertex2f(1.7778f, 1.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3fv(graphics::utilities::color::Red.Channels());
	glVertex2f(-1.7778f, 1.0f);
	glVertex2f(-1.7778f, 0.8f);
	glVertex2f(-1.57788f, 0.8f);
	glVertex2f(-1.5778f, 1.0f);
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
	glVertex2f(1.5778f, -0.8f);
	glVertex2f(1.5778f, -1.0f);
	glVertex2f(1.7778f, -1.0f);
	glVertex2f(1.7778f, -0.8f);
	glEnd();
}

bool Engine::UpdateFrame() noexcept
{
	static bool syncronize = false; //TEMP

	if (!NotifyFrameStarted(0.0_sec))
		return false;

	for (auto &viewport : render_window_->Viewports())
	{
		viewport.RenderTo();
		glLoadIdentity();
		Draw(); //RenderSystem::RenderScene()
	}

	if (syncronize)
		glFinish();
	else
		glFlush();

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

	//Depth buffer setup
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	
	glAlphaFunc(GL_GREATER, 0.0f); //Only draw pixels greater than 0% alpha
	glEnable(GL_ALPHA_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//Main loop
	while (render_window_ && render_window_->ProcessMessages() && UpdateFrame())
		render_window_->SwapBuffers();

	return 0;
}


/*
	Window
*/

graphics::render::RenderWindow& Engine::RenderTo(graphics::render::RenderWindow &&render_window) noexcept
{
	render_window_.emplace(std::move(render_window));
	auto &viewport = render_window_->CreateViewport(graphics::render::Viewport{*render_window_});

	auto frustum = graphics::render::Frustum::Orthographic(graphics::utilities::Aabb{{-1.7778_r, -1.0_r}, {1.7778_r, 1.0_r}},
														   -1.0_r, 1.0_r, 16.0_r / 9.0_r,
														   graphics::render::frustum::AspectRatioFormat::PanAndScan);
	frustum.BaseViewportHeight(viewport.Bounds().ToSize().Y());

	auto &camera = scene_manager_.CreateCamera(graphics::scene::Camera{frustum});
	viewport.ConnectCamera(camera);

	return *render_window_;
}

} //ion