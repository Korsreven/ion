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

#include <optional>
#include <utility>

#include "events/listeners/IonListenerInterface.h"
#include "events/listeners/IonRenderTargetListener.h"
#include "events/listeners/IonViewportListener.h"
#include "graphics/render/IonRenderTarget.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::render
{
	using graphics::utilities::Color;
	using graphics::utilities::Vector2;

	namespace viewport
	{
		enum class AnchorType
		{
			Top,
			Left,
			Right,
			Bottom
		};

		enum class OrientationMode
		{
			Degree_0,
			Degree_90_CW,
			Degree_90_CCW,
			Degree_180
		};

		namespace detail
		{
			void change_viewport() noexcept;
		} //detail
	} //viewport


	class Viewport final :
		public events::listeners::ListenerInterface<events::listeners::ViewportListener>,
		protected events::listeners::RenderTargetListener
	{
		private:

			std::optional<real> x_;
			std::optional<real> y_;
			std::optional<real> width_;
			std::optional<real> height_;

			Color background_color_ = utilities::color::Black;
			viewport::OrientationMode orientation_ = viewport::OrientationMode::Degree_0;

		protected:

			/*
				Events
			*/

			//See RenderTarget::RenderTargetResized for more details
			void RenderTargetResized(const Vector2 &size) noexcept override;

		public:

			//Default constructor
			Viewport() = default;

			//Construct a viewport with the given extents
			Viewport(const std::optional<real> &x, const std::optional<real> &y,
				const std::optional<real> &width, const std::optional<real> &height) noexcept;

			//Construct a viewport connected to a given render target
			Viewport(RenderTarget &render_target) noexcept;

			//Construct a viewport connected to a given render target and with the given extents
			Viewport(RenderTarget &render_target,
				const std::optional<real> &x, const std::optional<real> &y,
				const std::optional<real> &width, const std::optional<real> &height) noexcept;


			/*
				Static viewport conversions
			*/

			[[nodiscard]] static Viewport Client(RenderTarget &render_target) noexcept;


			/*
				Modifiers
			*/

			//
			inline void X(const std::optional<real> &x) noexcept
			{
				x_ = x;
			}
			
			//
			inline void Y(const std::optional<real> &y) noexcept
			{
				y_ = y;
			}

			//
			inline void Width(const std::optional<real> &width) noexcept
			{
				width_ = width;
			}

			//
			inline void Height(const std::optional<real> &height) noexcept
			{
				height_ = height;
			}

			//
			inline void BackgroundColor(const Color &color) noexcept
			{
				background_color_ = color;
			}

			//
			inline void Orientation(viewport::OrientationMode orientation) noexcept
			{
				orientation_ = orientation;
			}


			//
			inline void Position(const std::optional<real> &x, const std::optional<real> &y) noexcept
			{
				X(x);
				Y(y);
			}

			//
			inline void Position(const std::optional<Vector2> &position) noexcept
			{
				if (position)
				{
					auto [x, y] = position->XY();
					Position(x, y);
				}
				else
					Position({}, {});
			}

			//
			inline void Size(const std::optional<real> &width, const std::optional<real> &height) noexcept
			{
				Width(width);
				Height(height);
			}

			//
			inline void Size(const std::optional<Vector2> &size) noexcept
			{
				if (size)
				{
					auto [width, height] = size->XY();
					Size(width, height);
				}
				else
					Size({}, {});
			}


			/*
				Observers
			*/

			//
			[[nodiscard]] inline auto& X() const noexcept
			{
				return x_;
			}

			//
			[[nodiscard]] inline auto& Y() const noexcept
			{
				return y_;
			}

			//
			[[nodiscard]] inline auto& Width() const noexcept
			{
				return width_;
			}

			//
			[[nodiscard]] inline auto& Height() const noexcept
			{
				return height_;
			}

			//
			[[nodiscard]] inline auto& BackgroundColor() const noexcept
			{
				return background_color_;
			}

			//
			[[nodiscard]] inline auto Orientation() const noexcept
			{
				return orientation_;
			}


			//
			[[nodiscard]] inline auto Position() const noexcept
			{
				return std::pair{x_, y_};
			}

			//
			[[nodiscard]] inline auto Size() const noexcept
			{
				return std::pair{width_, height_};
			}


			/*
				Viewport
			*/

			//
			[[nodiscard]] inline auto ActualPosition() const noexcept
			{
				return Vector2{};
			}

			//
			[[nodiscard]] inline auto ActualSize() const noexcept
			{
				return Vector2{};
			}
	};
} //ion::graphics

#endif