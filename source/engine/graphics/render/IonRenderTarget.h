/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderTarget.h
-------------------------------------------
*/

#ifndef ION_RENDER_TARGET_H
#define ION_RENDER_TARGET_H

#include "events/listeners/IonListenerInterface.h"
#include "events/listeners/IonRenderTargetListener.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::render
{
	using graphics::utilities::Vector2;

	namespace render_target
	{
		namespace detail
		{
		} //detail
	} //render_target


	class RenderTarget :
		public events::listeners::ListenerInterface<events::listeners::RenderTargetListener>
	{
		protected:

			/*
				Notifying
			*/

			void NotifyRenderTargetResized(const Vector2 &size) noexcept;


			/*
				Events
			*/

			virtual void DoSwapBuffers() noexcept = 0;
			virtual Vector2 GetRenderTargetSize() const noexcept = 0;

		public:

			//Default constructor
			RenderTarget() = default;


			/*
				Events
			*/

			//Exchanges the front and back buffers of the render target
			void SwapBuffers() noexcept;


			/*
				Observers
			*/

			//Returns the size of the render target
			[[nodiscard]] inline auto Size() const noexcept
			{
				return GetRenderTargetSize();
			}


			/*
				Aspect ratio
			*/

			//Returns the aspect ratio of the render target
			//Returns nullopt if no window has been created
			[[nodiscard]] inline auto AspectRatio() const noexcept
			{
				auto [width, height] = Size().XY();
				return width / height;
			}
	};
}

#endif