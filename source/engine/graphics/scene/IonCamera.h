/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonCamera.h
-------------------------------------------
*/

#ifndef ION_CAMERA_H
#define ION_CAMERA_H

#include <optional>

#include "events/listeners/IonCameraListener.h"
#include "events/listeners/IonListenerInterface.h"
#include "events/listeners/IonViewportListener.h"
#include "graphics/render/IonViewport.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonVector2.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene
{
	using graphics::utilities::Aabb;
	using graphics::utilities::Vector2;
	using namespace types::type_literals;

	namespace camera
	{
		enum class Projection : bool
		{
			Orthographic,
			Perspective
		};

		enum class Scaling : bool
		{
			Proportional,
			Disproportional
		};

		namespace detail
		{
			void gl_perspective(real fov, real aspect_ratio, real z_near, real z_far) noexcept;
			void change_projection(const Aabb &clipping_plane, real z_near, real z_far, real fov,
				const std::optional<real> &aspect_ratio, Projection projection) noexcept;
		} //detail
	} //camera


	class Camera final :
		public events::listeners::ListenerInterface<events::listeners::CameraListener>,
		protected events::listeners::ViewportListener
	{
		private:

			Vector2 position_;
			std::optional<Aabb> clipping_plane_;
			real near_clipping_plane_ = -1.0_r; //Front plane
			real far_clipping_plane_ = 1.0_r; //Back plane
			real field_of_view_ = 60.0_r; //Fov

			std::optional<real> aspect_ratio_;
			camera::Projection projection_ = camera::Projection::Orthographic;
			camera::Scaling scaling_ = camera::Scaling::Proportional;

		protected:

			/*
				Events
			*/

			//See ViewportListener::ViewportResized for more details
			void ViewportResized(const Vector2 &size) noexcept override;

		public:

			//Default constructor
			Camera() = default;

			//Construct a camera connected to a given viewport
			Camera(render::Viewport &viewport) noexcept;


			/*
				Modifiers
			*/

			//
			inline void Position(const Vector2 &position) noexcept
			{
				position_ = position;
			}

			//
			inline void ClippingPlane(const std::optional<Aabb> &clipping_plane) noexcept
			{
				clipping_plane_ = clipping_plane;
			}

			//
			inline void NearClippingPlane(real near_clipping_plane) noexcept
			{
				near_clipping_plane_ = near_clipping_plane;
			}

			//
			inline void FarClippingPlane(real far_clipping_plane) noexcept
			{
				far_clipping_plane_ = far_clipping_plane;
			}

			//
			inline void FieldOfView(real field_of_view) noexcept
			{
				field_of_view_ = field_of_view;
			}


			//
			inline void AspectRatio(const std::optional<real> &aspect_ratio) noexcept
			{
				aspect_ratio_ = aspect_ratio;
			}

			//
			inline void AspectRatio(real width, real height) noexcept
			{
				AspectRatio(width / height);
			}

			//
			inline void Projection(camera::Projection projection) noexcept
			{
				projection_ = projection;
			}

			//
			inline void Scaling(camera::Scaling scaling) noexcept
			{
				scaling_ = scaling;
			}


			/*
				Observers
			*/

			//
			[[nodiscard]] inline const auto& Position() const noexcept
			{
				return position_;
			}

			//
			[[nodiscard]] inline const auto& ClippingPlane() const noexcept
			{
				return clipping_plane_;
			}

			//
			[[nodiscard]] inline auto NearClippingPlane() const noexcept
			{
				return near_clipping_plane_;
			}

			//
			[[nodiscard]] inline auto FarClippingPlane() const noexcept
			{
				return far_clipping_plane_;
			}

			//
			[[nodiscard]] inline auto FieldOfView() const noexcept
			{
				return field_of_view_;
			}


			//
			[[nodiscard]] inline const auto& AspectRatio() const noexcept
			{
				return aspect_ratio_;
			}

			//
			[[nodiscard]] inline auto Projection() const noexcept
			{
				return projection_;
			}

			//
			[[nodiscard]] inline auto Scaling() const noexcept
			{
				return scaling_;
			}
	};
} //ion::graphics::scene

#endif