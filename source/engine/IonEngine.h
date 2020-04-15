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

			bool running_ = false;
			bool syncronize_ = false;

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

			//Returns true if the engine is running
			[[nodiscard]] inline auto Running() const noexcept
			{
				return running_;
			}

			//Returns true if the engine is using vertical sync
			[[nodiscard]] bool VerticalSync() const noexcept;


			/*
				Game loop
			*/

			//
			[[nodiscard]] int Start() noexcept;


			/*
				Window
			*/

			//
			graphics::render::RenderWindow& RenderTo(graphics::render::RenderWindow &&render_window) noexcept;
	};
} //ion
#endif