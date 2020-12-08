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
	static std::vector vertices{
		//vertex
		-0.375f, 0.75f, -1.0f,
		-0.375f, -0.75f, -1.0f,
		0.375f, -0.75f, -1.0f,
		0.375f, -0.75f, -1.0f,
		0.375f, 0.75f, -1.0f,
		-0.375f, 0.75f, -1.0f,
		//normal
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		//color
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		//texcoord
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	static auto vao = 0u;
	static auto vbo = 0u;

	if (!std::empty(vertices))
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, std::size(vertices) * sizeof(float), std::data(vertices), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(18 * sizeof(float)));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)(36 * sizeof(float)));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)(60 * sizeof(float)));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glBindVertexArray(0);

		vertices.clear();
		vertices.shrink_to_fit();
	}
	

	glColor3fv(graphics::utilities::color::LightGray.Channels());

	glBegin(GL_QUADS);
	glVertex3f(-1.7778f, 1.0f, -4.0f);
	glVertex3f(-1.7778f, -1.0f, -4.0f);
	glVertex3f(1.7778f, -1.0f, -4.0f);
	glVertex3f(1.7778f, 1.0f, -4.0f);
	glEnd();

	glColor3fv(graphics::utilities::color::Red.Channels());

	glBegin(GL_QUADS);
	glVertex3f(-1.7778f, 1.0f, -2.5f);
	glVertex3f(-1.7778f, 0.8f, -2.5f);
	glVertex3f(-1.57788f, 0.8f, -2.5f);
	glVertex3f(-1.5778f, 1.0f, -2.5f);
	glEnd();

	glColor3fv(graphics::utilities::color::Green.Channels());

	glBegin(GL_QUADS);	
	glVertex3f(-0.1f, 0.1f, -1.5f);
	glVertex3f(-0.1f, -0.1f, -1.5f);
	glVertex3f(0.1f, -0.1f, -1.5f);
	glVertex3f(0.1f, 0.1f, -1.5f);
	glEnd();

	glColor3fv(graphics::utilities::color::Blue.Channels());

	glBegin(GL_QUADS);	
	glVertex3f(1.5778f, -0.8f, -1.25f);
	glVertex3f(1.5778f, -1.0f, -1.25f);
	glVertex3f(1.7778f, -1.0f, -1.25f);
	glVertex3f(1.7778f, -0.8f, -1.25f);
	glEnd();


	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	/*glVertexPointer(3, GL_FLOAT, 0, nullptr);
	glNormalPointer(GL_FLOAT, 0, (void*)(18 * sizeof(float)));
	glColorPointer(4, GL_FLOAT, 0, (void*)(36 * sizeof(float)));
	glTexCoordPointer(2, GL_FLOAT, 0, (void*)(60 * sizeof(float)));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);*/

	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(18 * sizeof(float)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)(36 * sizeof(float)));
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)(60 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);*/
	
	
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, 1);
	
	glBindVertexArray(vao);
	glUseProgram(5);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUseProgram(0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);	
	

	/*glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);*/

	/*glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool Engine::UpdateFrame() noexcept
{
	auto time = frame_stopwatch_.Restart();
		//Time should always be 0.0 sec on first frame

	timer_manager_.Elapse(time);

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
	//Already initialized
	if (initialized_)
		return true;

	//No window to render to
	if (!render_window_)
		return false;

	//Create render window (render target)
	if (!render_window_->Created() && !render_window_->Create())
		return false;

	//Initialize file system and graphics
	initialized_ = detail::init_file_system() && detail::init_graphics();
	return initialized_;
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

graphics::render::RenderWindow& Engine::RenderTo(graphics::render::RenderWindow &&render_window,
	std::optional<real> aspect_ratio, graphics::render::frustum::AspectRatioFormat aspect_format) noexcept
{
	return RenderTo(std::move(render_window), graphics::utilities::Aabb{-1.0_r, 1.0_r}, -1.0_r, 1.0_r, aspect_ratio, aspect_format);
}

graphics::render::RenderWindow& Engine::RenderTo(graphics::render::RenderWindow &&render_window,
	std::optional<graphics::utilities::Aabb> clipping_plane, real near_clip_distance, real far_clip_distance,
	std::optional<real> aspect_ratio, graphics::render::frustum::AspectRatioFormat aspect_format) noexcept
{
	if (Running())
		return *render_window_;


	if (input_controller_)
		input_controller_.reset();

	//Create render window and input controller
	render_window_.emplace(std::move(render_window));
	input_controller_.emplace(*render_window_);

	//Create a default viewport and connect to input
	auto &viewport = render_window_->CreateViewport(graphics::render::Viewport{"", *render_window_});
	input_controller_->ConnectViewport(viewport);
	
	//Create a default ortographic frustum
	auto frustum = graphics::render::Frustum::Orthographic(
		clipping_plane, near_clip_distance, far_clip_distance, aspect_ratio, aspect_format);
	//auto frustum = graphics::render::Frustum::Perspective(
	//	clipping_plane, near_clip_distance, far_clip_distance, 90.0, aspect_ratio, aspect_format);
	frustum.BaseViewportHeight(viewport.Bounds().ToSize().Y());

	//Create a default camera with frustum and connect to viewport
	auto &camera = scene_manager_.CreateCamera(graphics::scene::Camera{"", frustum});
	viewport.ConnectCamera(camera);

	return *render_window_;
}

} //ion