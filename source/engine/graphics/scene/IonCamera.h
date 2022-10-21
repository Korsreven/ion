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

#include "IonMovableObject.h"
#include "events/IonEventGenerator.h"
#include "events/listeners/IonCameraListener.h"
#include "graphics/render/IonFrustum.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonTypes.h"

namespace ion::graphics::render
{
	class Viewport; //Forward declaration
} //ion::graphics::render

namespace ion::graphics::scene
{
	using namespace types::type_literals;
	using utilities::Matrix4;
	using utilities::Vector2;
	using utilities::Vector3;

	namespace camera
	{
		namespace detail
		{
			void move_to(const Vector3 &position) noexcept;
			void rotate_by(real angle) noexcept;

			Matrix4 get_view_matrix(const Vector3 &position, real angle) noexcept;
		} //detail
	} //camera


	//A class representing a camera that can be attached to a scene node
	class Camera final :
		public MovableObject,
		protected events::EventGenerator<events::listeners::CameraListener>
	{
		private:

			Vector3 position_;
			real rotation_ = 0.0_r;
			render::Frustum frustum_;
			Matrix4 view_matrix_;

			Vector2 viewport_size_;
			bool update_bounding_volumes_ = true;


			void PrepareBoundingVolumes() noexcept;


			/*
				Notifying
			*/

			void NotifyCameraFrustumChanged(const render::Frustum &frustum) noexcept;
			void NotifyCameraMoved(const Vector3 &position) noexcept;

		public:

			//Construct a new camera with the given name and visibility
			explicit Camera(std::optional<std::string> name = {}, bool visible = true) noexcept;

			//Construct a new camera with the given name, a custom frustum and visibility
			Camera(std::optional<std::string> name, const render::Frustum &frustum, bool visible = true) noexcept;


			/*
				Modifiers
			*/

			//Sets the position of this camera to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					update_bounding_volumes_ = true;
					NotifyCameraMoved(position);
				}
			}

			//Sets the position of this camera to the given position
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}

			//Sets the rotation of this camera to the given angle (in radians)
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					update_bounding_volumes_ = true;
				}
			}

			//Sets the view frustum of the camera to the given frustum
			inline void ViewFrustum(const render::Frustum &frustum) noexcept
			{
				frustum_ = frustum;
				update_bounding_volumes_ = true;
				NotifyCameraFrustumChanged(frustum);
			}


			//See Frustum::BaseViewportHeight for more details
			inline void BaseViewportHeight(real height) noexcept
			{
				frustum_.BaseViewportHeight(height);
			}


			/*
				Observers
			*/

			//Returns the position of the camera
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the angle of rotation (in radians) for this camera
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}

			//Returns the view frustum of the camera
			[[nodiscard]] inline auto& ViewFrustum() const noexcept
			{
				return frustum_;
			}

			//Returns the view matrix for this camera
			[[nodiscard]] inline auto& ViewMatrix() const noexcept
			{
				return view_matrix_;
			}


			/*
				Capturing
			*/

			//Start capturing the scene from the viewpoint of this camera, with the given viewport
			void CaptureScene(const render::Viewport &viewport) noexcept;
	};
} //ion::graphics::scene

#endif