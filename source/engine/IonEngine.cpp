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

#include <cmath>
#include <thread>

#include "graphics/IonGraphicsAPI.h"
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


void set_swap_interval(int interval) noexcept
{
	#ifdef ION_WIN_GLEW
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(interval);
	#else

	#endif
}

std::optional<int> get_swap_interval() noexcept
{
	#ifdef ION_WIN_GLEW
	if (wglSwapIntervalEXT)
		return wglGetSwapIntervalEXT();
	#else
	
	#endif

	return {};
}


void wait_for(duration seconds) noexcept
{
	using namespace std::chrono_literals;

	static auto estimate = 0.005_r; //5ms
	static auto mean = estimate;
	static auto m2 = 0.0_r;
	static auto count = 1LL;

	while (estimate < seconds.count())
	{
		auto start = std::chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(1ms); //Measure actual time passed
		auto stop = std::chrono::high_resolution_clock::now();

		auto observed = duration{stop - start};
		seconds -= observed;

		//Welford's algorithm
		++count;
		auto delta = observed.count() - mean;
		mean += delta / count;
		m2 += delta * (observed.count() - mean);
		auto std_dev = std::sqrt(m2 / (count - 1LL));
		estimate = mean + std_dev;
	}

	//Spin lock for the remaining time
	auto start = std::chrono::high_resolution_clock::now();
	while (std::chrono::high_resolution_clock::now() - start < seconds);
}

} //engine::detail


//Protected

/*
	Notifying
*/

bool Engine::NotifyFrameStarted(duration time) noexcept
{
	for (auto &listener : FrameEventsBase::Listeners())
	{
		if (!FrameEventsBase::Notify(&events::listeners::FrameListener::FrameStarted, listener, time).value_or(true))
			return false;
	}

	return true;
}

bool Engine::NotifyFrameEnded(duration time) noexcept
{
	for (auto &listener : FrameEventsBase::Listeners())
	{
		if (!FrameEventsBase::Notify(&events::listeners::FrameListener::FrameEnded, listener, time).value_or(true))
			return false;
	}

	return true;
}


bool Engine::UpdateFrame(duration time) noexcept
{
	timer_manager_.Elapse(time);

	if (!NotifyFrameStarted(time))
		return false;

	for (auto &viewport : render_window_->Viewports())
	{
		viewport.RenderTo();

		for (auto &scene_graph : SceneGraphs())
			scene_graph.Render(viewport, time);
	}

	return NotifyFrameEnded(time);
}


//Public

/*
	Modifiers
*/

void Engine::VerticalSync(bool vsync) noexcept
{
	VerticalSync(vsync ? VSyncMode::On : VSyncMode::Off);
}

void Engine::VerticalSync(VSyncMode mode) noexcept
{
	switch (mode)
	{
		case VSyncMode::On:
		detail::set_swap_interval(1);
		break;

		case VSyncMode::Off:
		detail::set_swap_interval(0);
		break;

		case VSyncMode::Adaptive:
		detail::set_swap_interval(-1);
		break;

		case VSyncMode::AdaptiveHalfRate:
		detail::set_swap_interval(-2);
		break;
	}
}


/*
	Observers
*/

std::optional<engine::VSyncMode> Engine::VerticalSync() const noexcept
{
	if (auto interval = detail::get_swap_interval(); interval)
	{
		switch (*interval)
		{
			case 1:
			return VSyncMode::On;

			case 0:
			return VSyncMode::Off;

			case -1:
			return VSyncMode::Adaptive;

			case -2:
			return VSyncMode::AdaptiveHalfRate;
		}
	}

	return std::nullopt;
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

	total_stopwatch_.Restart();

	//Main loop
	while (render_window_ && render_window_->ProcessMessages())
	{
		auto time = frame_stopwatch_.Restart();
			//Time should always be 0.0 sec on first frame

		if (!UpdateFrame(time))
			break;

		render_window_->SwapBuffers();
			//glFlush() or glFinish() calls are being handled by SwapBuffers

		if (target_frame_time_)
		{
			if (auto seconds = *target_frame_time_ - frame_stopwatch_.Elapsed(); seconds > 0.0_sec)
				detail::wait_for(seconds);
		}
	}

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
	if (Running())
		return *render_window_;


	if (input_controller_)
		input_controller_.reset();

	//Create render window, viewport and input controller
	render_window_.emplace(std::move(render_window));
	render_window_->CreateViewport("", *render_window_);
	input_controller_.emplace(*render_window_);

	return *render_window_;
}


/*
	Scene graphs
	Creating
*/

NonOwningPtr<graphics::scene::graph::SceneGraph> Engine::CreateSceneGraph(std::string name)
{
	return SceneGraphBase::Create(std::move(name));
}


/*
	Scene graphs
	Retrieving
*/

NonOwningPtr<graphics::scene::graph::SceneGraph> Engine::GetSceneGraph(std::string_view name) noexcept
{
	return SceneGraphBase::Get(name);
}

NonOwningPtr<const graphics::scene::graph::SceneGraph> Engine::GetSceneGraph(std::string_view name) const noexcept
{
	return SceneGraphBase::Get(name);
}


/*
	Scene graphs
	Removing
*/

void Engine::ClearSceneGraphs() noexcept
{
	SceneGraphBase::Clear();
}

bool Engine::RemoveSceneGraph(graphics::scene::graph::SceneGraph &scene_graph) noexcept
{
	return SceneGraphBase::Remove(scene_graph);
}

bool Engine::RemoveSceneGraph(std::string_view name) noexcept
{
	return SceneGraphBase::Remove(name);
}

} //ion