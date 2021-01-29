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


	class Camera final :
		public MovableObject,
		protected events::EventGenerator<events::listeners::CameraListener>
	{
		private:

			Vector3 position_; //TEMP, should be located in parent node
			real rotation_ = 0.0_r; //TEMP, should be located in parent node
			render::Frustum frustum_;

			Matrix4 view_matrix_;


			/*
				Notifying
			*/

			void NotifyCameraFrustumChanged(const render::Frustum &frustum) noexcept;
			void NotifyCameraMoved(const Vector3 &position) noexcept;

		public:

			//Construct a new camera with the given name
			explicit Camera(std::string name);

			//Construct a new camera with the given name and a custom frustum
			Camera(std::string name, const render::Frustum &frustum);


			/*
				Modifiers
			*/

			//
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					NotifyCameraMoved(position);
				}
			}

			//
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
					rotation_ = angle;
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
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}

			//
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