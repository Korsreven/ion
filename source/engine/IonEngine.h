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

#include "events/IonInputController.h"
#include "events/IonListenable.h"
#include "events/listeners/IonFrameListener.h"
#include "graphics/render/IonFrustum.h"
#include "graphics/render/IonRenderWindow.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "graphics/utilities/IonAabb.h"
#include "timers/IonStopwatch.h"
#include "timers/IonTimerManager.h"
#include "types/IonTypes.h"

namespace ion
{
	using namespace types::type_literals;

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
		public events::Listenable<events::listeners::FrameListener>
	{
		private:

			bool initialized_ = false;
			bool syncronize_ = false;		
			timers::Stopwatch frame_stopwatch_;
			timers::Stopwatch total_stopwatch_;

			std::optional<graphics::render::RenderWindow> render_window_;
			std::optional<events::InputController> input_controller_;
			graphics::scene::graph::SceneGraph scene_graph_;
			timers::TimerManager timer_manager_;


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


			//Returns a pointer to a mutable render window
			//Returns nullptr if the engine is not rendering to a target
			[[nodiscard]] inline auto Target() noexcept
			{
				return render_window_ ? &*render_window_ : nullptr;
			}

			//Returns a pointer to an immutable render window
			//Returns nullptr if the engine is not rendering to a target
			[[nodiscard]] inline auto Target() const noexcept
			{
				return render_window_ ? &*render_window_ : nullptr;
			}


			//Returns a pointer to a mutable input controller
			//Returns nullptr if the engine has no input controller (missing rendering target)
			[[nodiscard]] inline auto Input() noexcept
			{
				return input_controller_ ? &*input_controller_ : nullptr;
			}

			//Returns a pointer to an immutable input controller
			//Returns nullptr if the engine has no input controller (missing rendering target)
			[[nodiscard]] inline auto Input() const noexcept
			{
				return input_controller_ ? &*input_controller_ : nullptr;
			}


			//Returns a mutable reference to a scene graph
			[[nodiscard]] inline auto& Scene() noexcept
			{
				return scene_graph_;
			}

			//Returns an immutable reference to a scene graph
			[[nodiscard]] inline auto& Scene() const noexcept
			{
				return scene_graph_;
			}


			//Returns a mutable reference to a timer manager
			[[nodiscard]] inline auto& SyncedTimers() noexcept
			{
				return timer_manager_;
			}

			//Returns an immutable reference to a timer manager
			[[nodiscard]] inline auto& SyncedTimers() const noexcept
			{
				return timer_manager_;
			}


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
				Rendering target
			*/

			//Render to the given render window, and create a default viewport
			graphics::render::RenderWindow& RenderTo(graphics::render::RenderWindow &&render_window) noexcept;
	};
} //ion
#endif