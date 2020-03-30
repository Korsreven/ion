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
#include "graphics/render/IonFrustum.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::render
{
	class Viewport; //Forward declaration
} //ion::graphics::render

namespace ion::graphics::scene
{
	using utilities::Vector2;

	namespace camera
	{
		namespace detail
		{
			void look_at() noexcept;
		} //detail
	} //camera


	class Camera final :
		public events::listeners::ListenerInterface<events::listeners::CameraListener>
	{
		private:

			Vector2 position_;
			render::Frustum frustum_;


			/*
				Notifying
			*/

			void NotifyCameraFrustumChanged(const render::Frustum &frustum) noexcept;
			void NotifyCameraMoved(const Vector2 &position) noexcept;

		public:

			//Default constructor
			Camera() = default;

			//Construct a camera with a custom frustum
			Camera(const render::Frustum &frustum) noexcept;


			/*
				Modifiers
			*/

			//
			inline void Position(const Vector2 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					NotifyCameraMoved(position);
				}
			}

			//
			inline void Lens(const render::Frustum &frustum) noexcept
			{
				frustum_ = frustum;
				NotifyCameraFrustumChanged(frustum);
			}


			/*
				Observers
			*/

			//
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//
			[[nodiscard]] inline auto& Lens() const noexcept
			{
				return frustum_;
			}


			/*
				Capturing
			*/

			//Start capturing the scene from the viewpoint of this camera, with the given viewport
			void CaptureScene(const render::Viewport &viewport) noexcept;
	};
} //ion::graphics::scene

#endif