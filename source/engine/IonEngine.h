/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	engine
File:	IonEngine.h
-------------------------------------------
*/

#ifndef ION_ENGINE_H
#define ION_ENGINE_H

#include <optional>

#include "events/IonListenable.h"
#include "events/listeners/IonFrameListener.h"
#include "graphics/render/IonRenderWindow.h"
#include "graphics/scene/IonSceneManager.h"
#include "timers/IonStopwatch.h"
#include "types/IonTypes.h"

namespace ion
{
	namespace engine
	{
		namespace detail
		{
			bool init_file_system() noexcept;
			bool init_graphics() noexcept;

			void set_swap_interval(bool vsync) noexcept;
			bool get_swap_interval() noexcept;
		} //detail
	} //engine


	class Engine final :
		protected events::Listenable<events::listeners::FrameListener>
	{
		private:

			bool syncronize_ = false;		
			timers::Stopwatch frame_stopwatch_;
			timers::Stopwatch total_stopwatch_;

			std::optional<graphics::render::RenderWindow> render_window_;
			graphics::scene::SceneManager scene_manager_;


			/*
				Notifying
			*/

			bool NotifyFrameStarted(duration time) noexcept;
			bool NotifyFrameEnded(duration time) noexcept;

			bool UpdateFrame() noexcept;

		public:

			//Default constructor
			Engine() = default;

			//Deleted copy constructor
			Engine(const Engine&) = delete;

			//Default move constructor
			Engine(Engine &&rhs) = default;


			/*
				Operators
			*/

			//Deleted copy assignment
			Engine& operator=(const Engine&) = delete;

			//Default move assignment
			Engine& operator=(Engine&&) = default;


			/*
				Modifiers
			*/

			//Sets if the engine should use vertical sync or not by the given value
			void VerticalSync(bool vsync) noexcept;


			/*
				Observers
			*/

			//Returns true if the engine is using vertical sync
			[[nodiscard]] bool VerticalSync() const noexcept;


			/*
				Engine
			*/

			//Returns true if all extensions, internal and external requirements where initialized succesfully
			[[nodiscard]] bool Initialize() noexcept;

			//Start the rendering loop
			//Returns 0 if everything went fine
			[[nodiscard]] int Start() noexcept;


			/*
				Timing
			*/

			//Returns the last frame time
			[[nodiscard]] duration FrameTime() const noexcept;

			//Returns the total running time
			[[nodiscard]] duration TotalTime() const noexcept;

			//Returns the FPS
			[[nodiscard]] real FPS() const noexcept;


			//Returns true if the engine is running
			[[nodiscard]] bool Running() const noexcept;


			/*
				Window
			*/

			//
			graphics::render::RenderWindow& RenderTo(graphics::render::RenderWindow &&render_window) noexcept;
	};
} //ion
#endif