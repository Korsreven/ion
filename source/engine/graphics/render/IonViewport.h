/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonViewport.h
-------------------------------------------
*/

#ifndef ION_VIEWPORT_H
#define ION_VIEWPORT_H

#include <utility>

#include "events/IonEventChannel.h"
#include "events/IonEventGenerator.h"
#include "events/listeners/IonRenderTargetListener.h"
#include "events/listeners/IonViewportListener.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::scene
{
	class Camera; //Forward declaration
}

namespace ion::graphics::render
{
	class RenderTarget; //Forward declaration

	using graphics::utilities::Aabb;
	using graphics::utilities::Color;
	using graphics::utilities::Vector2;

	namespace viewport
	{
		enum class AlignmentType
		{
			TopLeft,
			TopRight,
			BottomLeft,
			BottomRight
		};

		enum class HorizontalAnchorType
		{
			Left,
			Right,
			Percentage
		};

		enum class VerticalAnchorType
		{
			Top,
			Bottom,
			Percentage
		};

		namespace detail
		{
			std::pair<HorizontalAnchorType, VerticalAnchorType> get_anchors(AlignmentType alignment) noexcept;
			Aabb get_aligned_aabb(AlignmentType alignment, const Vector2 &size, const Vector2 &render_target_size) noexcept;
			Vector2 get_adjusted_position(const Vector2 &position, const Vector2 &size, const Vector2 &new_size,
				HorizontalAnchorType horizontal_anchor_type, VerticalAnchorType vertical_anchor_type) noexcept;

			Vector2 viewport_to_ortho_ratio(const Vector2 &viewport_size, real left, real right, real bottom, real top) noexcept;
			Vector2 ortho_to_viewport_ratio(const Vector2 &viewport_size, real left, real right, real bottom, real top) noexcept;

			Vector2 viewport_to_ortho_point(const Vector2 &viewport_size, real left, real right, real bottom, real top, const Vector2 &point) noexcept;
			Vector2 ortho_to_viewport_point(const Vector2 &viewport_size, real left, real right, real bottom, real top, const Vector2 &point) noexcept;

			Vector2 camera_to_ortho_point(scene::Camera &camera, const Vector2 &point) noexcept;
			Vector2 ortho_to_camera_point(scene::Camera &camera, const Vector2 &point) noexcept;
			
			void render_to_viewport(const Vector2 &position, const Vector2 &size, const Color &background_color) noexcept;
		} //detail
	} //viewport


	//A class representing a viewport that can be aligned and anchored to a specific location
	//A viewport is connected to a camera, and displays everything being rendered through the camera
	class Viewport final :
		public managed::ManagedObject<RenderTarget>,
		protected events::EventGenerator<events::listeners::ViewportListener>,
		protected events::EventChannel<events::Listenable<events::listeners::RenderTargetListener>>
	{
		private:

			Aabb bounds_;
			Aabb base_bounds_;

			viewport::HorizontalAnchorType left_anchor_ = viewport::HorizontalAnchorType::Percentage;
			viewport::HorizontalAnchorType right_anchor_ = viewport::HorizontalAnchorType::Percentage;
			viewport::VerticalAnchorType top_anchor_ = viewport::VerticalAnchorType::Percentage;		
			viewport::VerticalAnchorType bottom_anchor_ = viewport::VerticalAnchorType::Percentage;

			Color background_color_ = utilities::color::Black;
			Vector2 render_target_size_;

			NonOwningPtr<scene::Camera> camera_;


			/*
				Notifying
			*/

			void NotifyViewportResized(const Vector2 &size) noexcept;
			void NotifyViewportMoved(const Vector2 &position) noexcept;


			/*
				Bounds
			*/

			Aabb ResizedBounds(const Vector2 &size, const Vector2 &new_size) noexcept;
			void UpdateBounds(const Aabb &bounds) noexcept;


			/*
				Events
			*/

			//See RenderTarget::RenderTargetResized for more details
			void RenderTargetResized(Vector2 size) noexcept override;

		public:

			//Constructs a new viewport with the given name and connected to a given render target
			Viewport(std::string name, RenderTarget &render_target) noexcept;

			//Constructs a new viewport with the given name, connected to a given render target and with the given bounds (region)
			Viewport(std::string name, RenderTarget &render_target, const Aabb &bounds) noexcept;

			//Constructs a new viewport with the given name, connected to a given render target and with the given bounds (region) and anchors
			Viewport(std::string name, RenderTarget &render_target, const Aabb &bounds,
				viewport::HorizontalAnchorType left_anchor, viewport::HorizontalAnchorType right_anchor,
				viewport::VerticalAnchorType top_anchor, viewport::VerticalAnchorType bottom_anchor) noexcept;


			/*
				Static viewport conversions
			*/

			//Returns a new aligned viewport with the given name, render target, alignment and size
			[[nodiscard]] static Viewport Aligned(std::string name, RenderTarget &render_target, viewport::AlignmentType alignment, const Vector2 &size) noexcept;

			//Returns a new aligned viewport with the given name, render target, alignment and width/height percent
			//Width and height should be in range [0.0, 1.0]
			[[nodiscard]] static Viewport Aligned(std::string name, RenderTarget &render_target, viewport::AlignmentType alignment, real width_percent, real height_percent) noexcept;


			//Returns a new left aligned viewport with the given name, render target and width percent
			[[nodiscard]] static Viewport LeftAligned(std::string name, RenderTarget &render_target, real width_percent) noexcept;

			//Returns a new right aligned viewport with the given name, render target and width percent
			[[nodiscard]] static Viewport RightAligned(std::string name, RenderTarget &render_target, real width_percent) noexcept;

			//Returns a new top aligned viewport with the given name, render target and height percent
			[[nodiscard]] static Viewport TopAligned(std::string name, RenderTarget &render_target, real height_percent) noexcept;

			//Returns a new bottom aligned viewport with the given name, render target and height percent
			[[nodiscard]] static Viewport BottomAligned(std::string name, RenderTarget &render_target, real height_percent) noexcept;


			//Returns a new top left aligned viewport with the given name, render target and size
			[[nodiscard]] static Viewport TopLeftAligned(std::string name, RenderTarget &render_target, const Vector2 &size) noexcept;

			//Returns a new top left aligned viewport with the given name, render target and width/height percent
			//Width and height should be in range [0.0, 1.0]
			[[nodiscard]] static Viewport TopLeftAligned(std::string name, RenderTarget &render_target, real width_percent, real height_percent) noexcept;

			//Returns a new top right aligned viewport with the given name, render target and size
			[[nodiscard]] static Viewport TopRightAligned(std::string name, RenderTarget &render_target, const Vector2 &size) noexcept;

			//Returns a new top right aligned viewport with the given name, render target and width/height percent
			//Width and height should be in range [0.0, 1.0]
			[[nodiscard]] static Viewport TopRightAligned(std::string name, RenderTarget &render_target, real width_percent, real height_percent) noexcept;

			//Returns a new bottom left aligned viewport with the given name, render target and size
			[[nodiscard]] static Viewport BottomLeftAligned(std::string name, RenderTarget &render_target, const Vector2 &size) noexcept;

			//Returns a new bottom left aligned viewport with the given name, render target and width/height percent
			//Width and height should be in range [0.0, 1.0]
			[[nodiscard]] static Viewport BottomLeftAligned(std::string name, RenderTarget &render_target, real width_percent, real height_percent) noexcept;

			//Returns a new bottom right aligned viewport with the given name, render target and size
			[[nodiscard]] static Viewport BottomRightAligned(std::string name, RenderTarget &render_target, const Vector2 &size) noexcept;

			//Returns a new bottom right aligned viewport with the given name, render target and width/height percent
			//Width and height should be in range [0.0, 1.0]
			[[nodiscard]] static Viewport BottomRightAligned(std::string name, RenderTarget &render_target, real width_percent, real height_percent) noexcept;


			/*
				Modifiers
			*/

			//Sets the viewport bounds (region)
			inline void Bounds(const Aabb &bounds) noexcept
			{
				if (bounds_ != bounds)
					UpdateBounds(bounds);
			}


			//Sets the left anchor of the viewport to the given horizontal anchor type
			inline void LeftAnchor(viewport::HorizontalAnchorType anchor_type) noexcept
			{
				left_anchor_ = anchor_type;
			}

			//Sets the right anchor of the viewport to the given horizontal anchor type
			inline void RightAnchor(viewport::HorizontalAnchorType anchor_type) noexcept
			{
				right_anchor_  = anchor_type;
			}

			//Sets the top anchor of the viewport to the given vertical anchor type
			inline void TopAnchor(viewport::VerticalAnchorType anchor_type) noexcept
			{
				top_anchor_  = anchor_type;
			}

			//Sets the bottom anchor of the viewport to the given vertical anchor type
			inline void BottomAnchor(viewport::VerticalAnchorType anchor_type) noexcept
			{
				bottom_anchor_  = anchor_type;
			}


			//Sets the background (clear) color of the viewport to the given color
			inline void BackgroundColor(const Color &color) noexcept
			{
				background_color_ = color;
			}


			/*
				Observers
			*/

			//Returns the viewport bounds (region)
			[[nodiscard]] inline auto& Bounds() const noexcept
			{
				return bounds_;
			}

			//Returns the viewport base bounds (region)
			[[nodiscard]] inline auto& BaseBounds() const noexcept
			{
				return base_bounds_;
			}


			//Returns the left anchor of the viewport
			[[nodiscard]] inline auto LeftAnchor() const noexcept
			{
				return left_anchor_;
			}

			//Returns the right anchor of the viewport
			[[nodiscard]] inline auto RightAnchor() const noexcept
			{
				return right_anchor_;
			}

			//Returns the top anchor of the viewport
			[[nodiscard]] inline auto TopAnchor() const noexcept
			{
				return top_anchor_;
			}

			//Returns the bottom anchor of the viewport
			[[nodiscard]] inline auto BottomAnchor() const noexcept
			{
				return bottom_anchor_;
			}


			//Returns the background (clear) color of the viewport
			[[nodiscard]] inline auto& BackgroundColor() const noexcept
			{
				return background_color_;
			}


			/*
				Camera
			*/

			//Sets the camera connected to this viewport to the given camera
			void ConnectedCamera(NonOwningPtr<scene::Camera> camera) noexcept;

			//Returns a pointer to the camera connected to this viewport
			//Returns nullptr if this viewport does not have a camera connected
			[[nodiscard]] inline auto ConnectedCamera() const noexcept
			{
				return camera_;
			}

			
			/*
				Conversions
			*/

			//Returns the ratio (scale factor) between this viewport and the connected cameras ortho
			[[nodiscard]] Vector2 ViewportToOrthoRatio() const noexcept;
			
			//Returns the ratio (scale factor) between the connected cameras ortho and this viewport
			[[nodiscard]] Vector2 OrthoToViewportRatio() const noexcept;


			//Returns a new point in camera space from the given point in viewport space
			[[nodiscard]] Vector2 ViewportToCameraPoint(const Vector2 &point) const noexcept;
			
			//Returns a new point in viewport space from the given point in camera space
			[[nodiscard]] Vector2 CameraToViewportPoint(const Vector2 &point) const noexcept;


			/*
				Rendering
			*/

			//Starts rendering to this viewport
			void RenderTo() noexcept;
	};
} //ion::graphics

#endif