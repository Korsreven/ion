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
#include "system/IonSystemUtility.h"
#include "utilities/IonFileUtility.h"

namespace ion
{

using namespace engine;
using namespace types::type_literals;

namespace engine::detail
{

bool init_file_system() noexcept
{
	if (auto application_path = system::utilities::ApplicationPath(); application_path)
	{
		application_path->remove_filename();

		if (utilities::file::CurrentPath(*application_path))
			return true;
	}
	
	return false;
}

bool init_graphics() noexcept
{
	#ifdef ION_GLEW
	if (glewInit() != GLEW_OK)
		return false;
	#endif

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
	return true;
}


void set_swap_interval(bool vsync) noexcept
{
	#ifdef ION_WIN_GLEW
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT((int)vsync);
	#else

	#endif
}

bool get_swap_interval() noexcept
{
	#ifdef ION_WIN_GLEW
	return wglGetSwapIntervalEXT && wglGetSwapIntervalEXT() != 0;
	#else
	return false;
	#endif
}

} //engine::detail


//Protected

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
	auto time = frame_stopwatch_.Restart();
		//Time should always be 0.0 sec on first frame

	if (!NotifyFrameStarted(time))
		return false;

	for (auto &viewport : render_window_->Viewports())
	{
		viewport.RenderTo();
		Draw(); //RenderSystem::RenderScene()
	}

	if (syncronize_)
		glFinish();
	else
		glFlush();

	return NotifyFrameEnded(time);
}


//Public

/*
	Modifiers
*/

void Engine::VerticalSync(bool vsync) noexcept
{
	detail::set_swap_interval(vsync);
}


/*
	Observers
*/

bool Engine::VerticalSync() const noexcept
{
	return detail::get_swap_interval();
}


/*
	Engine
*/

bool Engine::Initialize() noexcept
{
	//No window to render to
	if (!render_window_)
		return false;

	//Create render window (render target)
	if (!render_window_->Created() && !render_window_->Create())
		return false;

	//Initialize file system and graphics
	return detail::init_file_system() && detail::init_graphics();
}

int Engine::Start() noexcept
{
	//Not successfully initialized
	if (!Initialize())
		return 1;

	//Already running
	if (Running())
		return 1;


	//Show window
	if (render_window_)
		render_window_->Show();

	syncronize_ = VerticalSync();
	total_stopwatch_.Restart();

	//Main loop
	while (render_window_ && render_window_->ProcessMessages() && UpdateFrame())
		render_window_->SwapBuffers();

	frame_stopwatch_.Reset();
	total_stopwatch_.Stop(); //Total time could be retrieved later

	//Hide window
	if (render_window_)
		render_window_->Hide();

	return 0;
}


/*
	Timing
*/

duration Engine::FrameTime() const noexcept
{
	return frame_stopwatch_.Elapsed();
}

duration Engine::TotalTime() const noexcept
{
	return total_stopwatch_.Elapsed();
}

real Engine::FPS() const noexcept
{
	auto time = FrameTime();
	return time > 0.0_sec ?
		1.0_r / time.count() :
		0.0_r;
}


bool Engine::Running() const noexcept
{
	return total_stopwatch_.IsRunning();
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
	camera.Position({0.0_r, 0.0_r});

	viewport.ConnectCamera(camera);

	return *render_window_;
}

} //ion