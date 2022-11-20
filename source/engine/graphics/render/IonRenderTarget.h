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
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::render
{
	using graphics::utilities::Vector2;

	namespace render_target::detail
	{
	} //render_target::detail


	///@brief A class representing a render target that manages viewports
	class RenderTarget :
		public events::Listenable<events::listeners::RenderTargetListener>,
		public managed::ObjectManager<Viewport, RenderTarget, events::listeners::ViewportListener>
	{
		private:

			using RenderTargetEventsBase = events::Listenable<events::listeners::RenderTargetListener>;
			using ViewportEventsBase = events::Listenable<events::listeners::ViewportListener>;

		protected:

			/**
				@name Notifying
				@{
			*/

			void NotifyRenderTargetResized(const Vector2 &size) noexcept;

			///@}

			/**
				@name Events
				@{
			*/

			virtual void DoSwapBuffers() noexcept = 0;
			virtual Vector2 GetRenderTargetSize() const noexcept = 0;

			///@}

		public:

			///@brief Default constructor
			RenderTarget() = default;

			///@brief Deleted copy constructor
			RenderTarget(const RenderTarget&) = delete;

			///@brief Default move constructor
			RenderTarget(RenderTarget&&) = default;

			///@brief Default virtual destructor
			virtual ~RenderTarget() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			RenderTarget& operator=(const RenderTarget&) = delete;

			///@brief Default move assignment
			RenderTarget& operator=(RenderTarget&&) = default;

			///@}

			/**
				@name Events
				@{
			*/

			///@brief Returns a mutable reference to the render target events of this render target
			[[nodiscard]] inline auto& Events() noexcept
			{
				return static_cast<RenderTargetEventsBase&>(*this);
			}

			///@brief Returns an immutable reference to the render target events of this render target
			[[nodiscard]] inline auto& Events() const noexcept
			{
				return static_cast<const RenderTargetEventsBase&>(*this);
			}


			///@brief Returns a mutable reference to the viewport events of this render target
			[[nodiscard]] inline auto& ViewportEvents() noexcept
			{
				return static_cast<ViewportEventsBase&>(*this);
			}

			///@brief Returns an immutable reference to the viewport events of this render target
			[[nodiscard]] inline auto& ViewportEvents() const noexcept
			{
				return static_cast<const ViewportEventsBase&>(*this);
			}

			///@}

			/**
				@name Buffers
				@{
			*/

			///@brief Exchanges the front and back buffers of the render target
			void SwapBuffers() noexcept;

			///@}

			/**
				@name Extents
				@{
			*/

			///@brief Returns the size of the render target
			[[nodiscard]] inline auto Size() const noexcept
			{
				return GetRenderTargetSize();
			}

			///@brief Returns the aspect ratio of the render target
			///@details Returns nullopt if no window has been created
			[[nodiscard]] inline auto AspectRatio() const noexcept
			{
				auto [width, height] = Size().XY();
				return width / height;
			}

			///@}

			/**
				@name Viewports - Ranges
				@{
			*/

			///@brief Returns a mutable range of all viewports in this render target
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Viewports() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all viewports in this render target
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Viewports() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Viewports - Creating
				@{
			*/

			///@brief Creates a viewport with the given name and connected to a given render target
			NonOwningPtr<Viewport> CreateViewport(std::string name, RenderTarget &render_target);

			///@brief Creates a viewport with the given name, connected to a given render target and with the given bounds (region)
			NonOwningPtr<Viewport> CreateViewport(std::string name, RenderTarget &render_target, const Aabb &bounds);

			///@brief Creates a viewport with the given name, connected to a given render target and with the given bounds (region) and anchors
			NonOwningPtr<Viewport> CreateViewport(std::string name, RenderTarget &render_target, const Aabb &bounds,
				viewport::HorizontalAnchorType left_anchor, viewport::HorizontalAnchorType right_anchor,
				viewport::VerticalAnchorType top_anchor, viewport::VerticalAnchorType bottom_anchor);


			///@brief Creates a viewport as a copy of the given viewport
			NonOwningPtr<Viewport> CreateViewport(const Viewport &viewport);

			///@brief Creates a viewport by moving the given viewport
			NonOwningPtr<Viewport> CreateViewport(Viewport &&viewport);

			///@}

			/**
				@name Viewports - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable viewport with the given name
			///@details Returns nullptr if viewport could not be found
			[[nodiscard]] NonOwningPtr<Viewport> GetViewport(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable viewport with the given name
			///@details Returns nullptr if viewport could not be found
			[[nodiscard]] NonOwningPtr<const Viewport> GetViewport(std::string_view name) const noexcept;
			

			///@brief Gets a pointer to a mutable viewport at a given position
			///@details Returns nullptr if viewport could not be found
			[[nodiscard]] NonOwningPtr<Viewport> GetViewport(const Vector2 &position) noexcept;

			///@brief Gets a pointer to an immutable viewport at a given position
			///@details Returns nullptr if viewport could not be found
			[[nodiscard]] NonOwningPtr<const Viewport> GetViewport(const Vector2 &position) const noexcept;

			///@}

			/**
				@name Viewports - Removing
				@{
			*/

			///@brief Clears all removable viewports from this manager
			void ClearViewports() noexcept;

			///@brief Removes a removable viewport from this manager
			bool RemoveViewport(Viewport &viewport) noexcept;

			///@brief Removes a removable viewport with the given name from this manager
			bool RemoveViewport(std::string_view name) noexcept;

			///@}
	};
}

#endif