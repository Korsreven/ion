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
#include "graphics/utilities/IonAabb.h"
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
	using namespace graphics::utilities;
	using namespace types::type_literals;

	namespace camera
	{
		namespace detail
		{
			void move_to(const Vector3 &position) noexcept;
			void rotate_by(real angle) noexcept;

			Matrix4 get_view_matrix(const Vector3 &position, real angle) noexcept;
		} //detail
	} //camera


	///@brief A class representing a camera that can be attached to a scene node
	class Camera final :
		public MovableObject,
		protected events::EventGenerator<events::listeners::CameraListener>
	{
		private:

			Vector3 position_;
			real rotation_ = 0.0_r;
			Vector2 scaling_ = vector2::UnitScale;
			render::Frustum frustum_;
			Matrix4 view_matrix_;

			Vector2 viewport_size_;
			std::optional<Aabb> frustum_clip_plane_;
			bool update_bounding_volumes_ = true;

			Vector3 derived_position_;
			real derived_rotation_ = 0.0_r;
			Vector2 derived_scaling_ = vector2::UnitScale;


			void ScaleFrustum(const Vector2 &scaling) noexcept;
			void PrepareBoundingVolumes() noexcept;


			/**
				@name Notifying
				@{
			*/

			void NotifyCameraFrustumChanged() noexcept;
			void NotifyCameraMoved() noexcept;
			void NotifyCameraRotated() noexcept;
			void NotifyCameraScaled() noexcept;

			///@}

		public:

			///@brief Constructs a new camera with the given name and visibility
			explicit Camera(std::optional<std::string> name = {}, bool visible = true) noexcept;

			///@brief Constructs a new camera with the given name, a custom frustum and visibility
			Camera(std::optional<std::string> name, const render::Frustum &frustum, bool visible = true) noexcept;


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the position of this camera to the given position
			inline void Position(const Vector3 &position) noexcept
			{
				if (position_ != position)
				{
					position_ = position;
					update_bounding_volumes_ = true;
				}
			}

			///@brief Sets the position of this camera to the given position
			inline void Position(const Vector2 &position) noexcept
			{
				Position({position.X(), position.Y(), position_.Z()});
			}

			///@brief Sets the rotation of this camera to the given angle in radians
			inline void Rotation(real angle) noexcept
			{
				if (rotation_ != angle)
				{
					rotation_ = angle;
					update_bounding_volumes_ = true;
				}
			}

			///@brief Sets the scaling of this camera to the given scaling
			inline void Scaling(const Vector2 &scaling) noexcept
			{
				if (scaling_ != scaling)
				{
					scaling_ = scaling;
					ScaleFrustum(DerivedScaling());
					update_bounding_volumes_ = true;
				}
			}

			///@brief Sets the view frustum of this camera to the given frustum
			inline void ViewFrustum(const render::Frustum &frustum) noexcept
			{
				frustum_ = frustum;
				frustum_clip_plane_ = frustum.ClipPlane();
				ScaleFrustum(DerivedScaling());
				update_bounding_volumes_ = true;
				NotifyCameraFrustumChanged();
			}


			///@brief See Frustum::BaseViewportHeight for more details
			inline void BaseViewportHeight(real height) noexcept
			{
				frustum_.BaseViewportHeight(height);
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the position of this camera
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			///@brief Returns the rotation of this camera in radians
			[[nodiscard]] inline auto Rotation() const noexcept
			{
				return rotation_;
			}

			///@brief Returns the scaling of this camera
			[[nodiscard]] inline auto& Scaling() const noexcept
			{
				return scaling_;
			}

			///@brief Returns the derived position of this camera
			[[nodiscard]] Vector3 DerivedPosition() const noexcept;

			///@brief Returns the derived rotation of this camera in radians
			[[nodiscard]] real DerivedRotation() const noexcept;

			///@brief Returns the derived scaling of this camera
			[[nodiscard]] Vector2 DerivedScaling() const noexcept;


			///@brief Returns the view frustum of this camera
			[[nodiscard]] inline auto& ViewFrustum() const noexcept
			{
				return frustum_;
			}

			///@brief Returns the view frustum of this camera with the given scaling applied
			[[nodiscard]] inline auto ViewFrustum(const Vector2 &scaling) const noexcept
			{
				auto frustum = frustum_;

				if (frustum_clip_plane_)
					frustum.ClipPlane(frustum_clip_plane_->ScaleCopy(scaling));
				else if (auto &clip_plane = frustum.ClipPlane(); clip_plane)
					frustum.ClipPlane(clip_plane->ScaleCopy(scaling));

				return frustum;
			}

			///@brief Returns the view matrix for this camera
			[[nodiscard]] inline auto& ViewMatrix() const noexcept
			{
				return view_matrix_;
			}

			///@}

			/**
				@name Capturing
				@{
			*/

			///@brief Starts capturing the scene from the viewpoint of this camera, with the given viewport
			void CaptureScene(const render::Viewport &viewport) noexcept;

			///@}
	};
} //ion::graphics::scene

#endif