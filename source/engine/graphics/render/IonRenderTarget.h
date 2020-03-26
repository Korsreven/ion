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

#include <vector>

#include "adaptors/ranges/IonDereferenceIterable.h"
#include "events/listeners/IonListenerInterface.h"
#include "events/listeners/IonRenderTargetListener.h"
#include "graphics/render/IonViewport.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::render
{
	class Viewport;
	using graphics::utilities::Vector2;

	namespace render_target
	{
		namespace detail
		{
			template <typename T>
			using container_type = std::vector<std::unique_ptr<T>>; //Owning
		} //detail
	} //render_target


	class RenderTarget :
		public events::listeners::ListenerInterface<events::listeners::RenderTargetListener>
	{
		private:

			render_target::detail::container_type<Viewport> viewports_;

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

			//Default copy constructor
			RenderTarget(const RenderTarget&) = default;

			//Default move constructor
			RenderTarget(RenderTarget&&) = default;

			//Default virtual destructor
			virtual ~RenderTarget() = default;


			/*
				Operators
			*/

			//Default copy assignment
			RenderTarget& operator=(const RenderTarget&) = default;

			//Default move assignment
			RenderTarget& operator=(RenderTarget&&) = default;


			/*
				Buffers
			*/

			//Exchanges the front and back buffers of the render target
			void SwapBuffers() noexcept;


			/*
				Extents
			*/

			//Returns the size of the render target
			[[nodiscard]] inline auto Size() const noexcept
			{
				return GetRenderTargetSize();
			}

			//Returns the aspect ratio of the render target
			//Returns nullopt if no window has been created
			[[nodiscard]] inline auto AspectRatio() const noexcept
			{
				auto [width, height] = Size().XY();
				return width / height;
			}


			/*
				Viewport
				Ranges
			*/

			//Returns a mutable range of all viewports in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Viewports() noexcept
			{
				return adaptors::ranges::DereferenceIterable<render_target::detail::container_type<Viewport>&>{viewports_};
			}

			//Returns an immutable range of all viewports in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Viewports() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const render_target::detail::container_type<Viewport>&>{viewports_};
			}


			/*
				Viewport
				Adding / removing
			*/

			//Add the given viewport to this render target by moving it
			//Returns a reference to the newly added viewport
			[[nodiscard]] Viewport& AddViewport(Viewport &&viewport);

			//Removes all viewports from this render target
			void ClearViewports() noexcept;

			//Remove the given viewport from this render target
			//Returns true if the viewport was found and removed
			bool RemoveViewport(const Viewport &viewport) noexcept;
	};
}

#endif