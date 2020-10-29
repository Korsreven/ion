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

#include <string>
#include <string_view>

#include "adaptors/ranges/IonDereferenceIterable.h"
#include "events/IonListenable.h"
#include "events/listeners/IonRenderTargetListener.h"
#include "events/listeners/IonViewportListener.h"
#include "graphics/render/IonViewport.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonObjectManager.h"

namespace ion::graphics::render
{
	using graphics::utilities::Vector2;

	namespace render_target::detail
	{
	} //render_target::detail


	class RenderTarget :
		public events::Listenable<events::listeners::RenderTargetListener>,
		public managed::ObjectManager<Viewport, RenderTarget, events::listeners::ViewportListener>
	{
		private:

			using RenderTargetEventsBase = events::Listenable<events::listeners::RenderTargetListener>;
			using ViewportEventsBase = events::Listenable<events::listeners::ViewportListener>;

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
				Events
			*/

			//Return a mutable reference to the render target events of this render target
			[[nodiscard]] inline auto& Events() noexcept
			{
				return static_cast<RenderTargetEventsBase&>(*this);
			}

			//Return a immutable reference to the render target events of this render target
			[[nodiscard]] inline auto& Events() const noexcept
			{
				return static_cast<const RenderTargetEventsBase&>(*this);
			}


			//Return a mutable reference to the viewport events of this render target
			[[nodiscard]] inline auto& ViewportEvents() noexcept
			{
				return static_cast<ViewportEventsBase&>(*this);
			}

			//Return a immutable reference to the viewport events of this render target
			[[nodiscard]] inline auto& ViewportEvents() const noexcept
			{
				return static_cast<const ViewportEventsBase&>(*this);
			}


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
				Viewports
				Ranges
			*/

			//Returns a mutable range of all viewports in this render target
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Viewports() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all viewports in this render target
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Viewports() const noexcept
			{
				return Objects();
			}


			/*
				Viewports
				Creating
			*/

			//Create a viewport as a copy of the given viewport
			Viewport& CreateViewport(const Viewport &viewport);

			//Create a viewport by moving the given viewport
			Viewport& CreateViewport(Viewport &&viewport);


			/*
				Viewports
				Retrieving
			*/

			//Gets a pointer to a mutable viewport with the given name
			//Returns nullptr if viewport could not be found
			[[nodiscard]] Viewport* GetViewport(std::string_view name) noexcept;

			//Gets a pointer to an immutable viewport with the given name
			//Returns nullptr if viewport could not be found
			[[nodiscard]] const Viewport* GetViewport(std::string_view name) const noexcept;


			/*
				Viewports
				Removing
			*/

			//Clear all removable viewports from this manager
			void ClearViewports() noexcept;

			//Remove a removable viewport from this manager
			bool RemoveViewport(Viewport &viewport) noexcept;

			//Remove a removable viewport with the given name from this manager
			bool RemoveViewport(std::string_view name) noexcept;
	};
}

#endif