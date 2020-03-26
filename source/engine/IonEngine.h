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

#include "events/listeners/IonListenerInterface.h"
#include "events/listeners/IonFrameListener.h"
#include "graphics/IonGraphicsAPI.h"
#include "graphics/render/IonRenderWindow.h"
#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonCamera.h"
#include "types/IonTypes.h"

namespace ion
{
	namespace engine
	{
		namespace detail
		{

		} //detail
	} //engine


	class Engine final :
		protected events::listeners::ListenerInterface<events::listeners::FrameListener>
	{
		private:

			std::optional<graphics::render::RenderWindow> render_window_;
			std::optional<graphics::scene::Camera> camera_;


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


			/*
				Observers
			*/


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