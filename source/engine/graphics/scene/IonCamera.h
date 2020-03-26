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
#include "events/listeners/IonListeningChannel.h"
#include "events/listeners/IonViewportListener.h"
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
		public events::listeners::ListenerInterface<events::listeners::CameraListener>,
		protected events::listeners::ListeningChannel<events::listeners::ListenerInterface<events::listeners::ViewportListener>>
	{
		private:

			Vector2 position_;
			render::Frustum frustum_;


			/*
				Notifying
			*/

			void NotifyCameraFrustumChanged(const render::Frustum &frustum) noexcept;
			void NotifyCameraMoved(const Vector2 &position) noexcept;


			/*
				Events
			*/

			//See ViewportListener::ViewportResized for more details
			void ViewportResized(Vector2 size) noexcept override;

		public:

			//Default constructor
			Camera() = default;

			//Construct a camera with a custom frustum
			Camera(const render::Frustum &frustum) noexcept;

			//Construct a camera connected to a given viewport
			Camera(render::Viewport &viewport) noexcept;

			//Construct a camera connected to a given viewport with a custom frustum
			Camera(render::Viewport &viewport, const render::Frustum &frustum) noexcept;


			/*
				Modifiers
			*/

			//
			inline void Position(const Vector2 &position) noexcept
			{
				position_ = position;
			}

			//
			inline void ViewFrustum(const render::Frustum &frustum) noexcept
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
			[[nodiscard]] inline auto& ViewFrustum() const noexcept
			{
				return frustum_;
			}


			//Change rendering context to this camera
			void Change() noexcept;
	};
} //ion::graphics::scene

#endif