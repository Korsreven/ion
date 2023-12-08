/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonFrustum.h
-------------------------------------------
*/

#ifndef ION_FRUSTUM_H
#define ION_FRUSTUM_H

#include <optional>
#include <tuple>

#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector2.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::render
{
	using namespace types::type_literals;

	using graphics::utilities::Aabb;
	using graphics::utilities::Matrix4;
	using graphics::utilities::Vector2;

	namespace frustum
	{
		enum class ProjectionType : bool
		{
			Orthographic,
			Perspective
		};

		enum class AspectRatioFormat
		{
			Fill, //Fill height/width (crop)
				/*
					 ____________
					| 2  3  4  5 |
					|            |
					|            |
					|____________|
				*/

			FillHeight, //Fill height (crop width)
				/*
					 ____________
					| |12 34 56| |
					| |        | |
					| |        | |
					|_|________|_|
				*/

			FillWidth, //Fill width (crop height)
				/*
					 ____________ 
					|____________|
					|1 2 3  4 5 6|
					|____________|
					|____________|
				*/

			Letterbox, //Fill (no crop)
				/*
					 ____________    ____________
					|____________|  | |12 34 56| |
					|1 2 3  4 5 6|  | |        | |
					|____________|  | |        | |
					|____________|  |_|________|_|
				*/

			Windowbox //Don't fill
				/*
					 ____________
					|   ______   |
					|  |123456|  |
					|  |______|  |
					|____________|
				*/
		};

		namespace detail
		{
			///@brief Same function signature and result as gluPerspective provides
			inline auto perspective_to_frustum(real fov, real aspect_ratio, real z_near, real z_far) noexcept
			{
				using namespace ion::utilities;
				auto height = math::Tan(fov / 360.0_r * math::Pi) * z_near;
				auto width = height * aspect_ratio;
				return std::tuple{-width, width, -height, height, z_near, z_far};
			}

			
			Aabb keep_proportion(const Aabb &clip_plane, real aspect_ratio, AspectRatioFormat aspect_format,
				real base_viewport_height, const Vector2 &viewport_size) noexcept;

			std::tuple<real, real, real, real, real, real> to_ortho(const std::optional<Aabb> &clip_plane, real z_near, real z_far,
				const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format, real base_viewport_height, const Vector2 &viewport_size) noexcept;
			std::tuple<real, real, real, real, real, real> to_frustum(const std::optional<Aabb> &clip_plane, real z_near, real z_far, real fov,
				const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format, real base_viewport_height, const Vector2 &viewport_size) noexcept;

			Matrix4 get_projection_matrix(ProjectionType projection, const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance, real fov,
				const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format, real base_viewport_height, const Vector2 &viewport_size) noexcept;

			void project_through_frustum(ProjectionType projection, const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance, real fov,
				const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format, real base_viewport_height, const Vector2 &viewport_size) noexcept;
		} //detail
	} //frustum


	///@brief A class representing a viewing frustum which can either be an orthographic or perspective view
	///@details The scene is viewed through the camera's frustum, and the perspective matrix is calculated from it
	class Frustum final
	{
		private:

			frustum::ProjectionType projection_ = frustum::ProjectionType::Orthographic;

			std::optional<Aabb> clip_plane_ = Aabb{{-1.0_r}, {1.0_r}};
			real near_clip_distance_ = -1.0_r; //Front plane
			real far_clip_distance_ = 1.0_r; //Back plane
			real field_of_view_ = 90.0_r;

			std::optional<real> aspect_ratio_;
			frustum::AspectRatioFormat aspect_format_ = frustum::AspectRatioFormat::Fill;
			real base_viewport_height_ = 0.0_r;

			Matrix4 projection_matrix_;

		public:

			///@brief Default constructor
			Frustum() = default;

			///@brief Constructs a new frustum with the given clip plane bounds
			Frustum(const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance,
				const std::optional<real> &aspect_ratio, frustum::AspectRatioFormat aspect_format) noexcept;

			///@brief Constructs a new frustum with the given clip plane bounds, field of view and aspect ratio
			Frustum(frustum::ProjectionType projection, const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance,
				real field_of_view, const std::optional<real> &aspect_ratio, frustum::AspectRatioFormat aspect_format) noexcept;


			/**
				@name Static frustum conversions
				@{
			*/

			///@brief Returns a new orthographic frustum from the given clip plane bounds and aspect ratio
			///@details If a fixed aspect ratio is not given (nullopt), it will dynamically follow the viewport size
			[[nodiscard]] static Frustum Orthographic(const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance,
				const std::optional<real> &aspect_ratio, frustum::AspectRatioFormat aspect_format = frustum::AspectRatioFormat::Fill) noexcept;

			///@brief Returns a new perspective frustum from the given clip plane bounds
			///@details If a fixed aspect ratio is not given (nullopt), it will dynamically follow the viewport size
			[[nodiscard]] static Frustum Perspective(const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance,
				real field_of_view, const std::optional<real> &aspect_ratio, frustum::AspectRatioFormat aspect_format = frustum::AspectRatioFormat::Fill) noexcept;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the projection of the frustum to the given projection
			inline void Projection(frustum::ProjectionType projection) noexcept
			{
				projection_ = projection;
			}


			///@brief Sets the clip plane of the frustum to the given aabb
			inline void ClipPlane(const std::optional<Aabb> &clip_plane) noexcept
			{
				clip_plane_ = clip_plane;
			}

			///@brief Sets the near clip distance (z near) to the given value
			inline void NearClipDistance(real near_clip_distance) noexcept
			{
				near_clip_distance_ = near_clip_distance;
			}

			///@brief Sets the far clip distance (z far) to the given value
			inline void FarClipDistance(real far_clip_distance) noexcept
			{
				far_clip_distance_ = far_clip_distance;
			}

			///@brief Sets the field of view (fov y) to the given value
			inline void FieldOfView(real field_of_view) noexcept
			{
				field_of_view_ = field_of_view;
			}


			///@brief Sets a fixed aspect ratio to the given ratio (width/height)
			inline void AspectRatio(const std::optional<real> &aspect_ratio) noexcept
			{
				aspect_ratio_ = aspect_ratio;
			}

			///@brief Sets a fixed aspect ratio from the given width and height (width/height)
			inline void AspectRatio(real width, real height) noexcept
			{
				AspectRatio(width / height);
			}
			
			///@brief Sets the aspect ratio format to the given format
			inline void AspectFormat(frustum::AspectRatioFormat aspect_format) noexcept
			{
				aspect_format_ = aspect_format;
			}

			///@brief Sets the base viewport height to the given value
			///@details This is exclusively used for AspectRatioFormat::Windowbox
			inline void BaseViewportHeight(real height) noexcept
			{
				base_viewport_height_ = height;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the projection of the frustum
			[[nodiscard]] inline auto Projection() const noexcept
			{
				return projection_;
			}


			///@brief Returns the clip plane of the frustum
			[[nodiscard]] inline auto& ClipPlane() const noexcept
			{
				return clip_plane_;
			}

			///@brief Returns the near clip distance (z near) of the frustum
			[[nodiscard]] inline auto NearClipDistance() const noexcept
			{
				return near_clip_distance_;
			}

			///@brief Returns the far clip distance (z far) of the frustum
			[[nodiscard]] inline auto FarClipDistance() const noexcept
			{
				return far_clip_distance_;
			}

			///@brief Returns the field of view of the frustum
			[[nodiscard]] inline auto FieldOfView() const noexcept
			{
				return field_of_view_;
			}


			///@brief Returns the fixed aspect ratio of the frustum
			///@details Returns nullopt if no fixed ratio is in use
			[[nodiscard]] inline auto& AspectRatio() const noexcept
			{
				return aspect_ratio_;
			}

			///@brief Returns the aspect ratio format of the frustum
			[[nodiscard]] inline auto AspectFormat() const noexcept
			{
				return aspect_format_;
			}

			///@brief Returns the base viewport height
			///@details This is exclusively used for AspectRatioFormat::Windowbox
			[[nodiscard]] inline auto BaseViewportHeight() const noexcept
			{
				return base_viewport_height_;
			}


			///@brief Returns the projection matrix for this frustum
			[[nodiscard]] inline auto& ProjectionMatrix() const noexcept
			{
				return projection_matrix_;
			}

			///@}

			/**
				@name Projecting
				@{
			*/

			///@brief Starts projecting the scene through this frustum, with the given viewport size
			void ProjectScene(const Vector2 &viewport_size) noexcept;

			///@}

			/**
				@name Frustum conversions
				@{
			*/

			///@brief Returns the ortho bounds (left, right, bottom, top, z_near, z_far) from this frustum with the given viewport size
			///@details This returns all the values needed by glOrtho
			[[nodiscard]] std::tuple<real, real, real, real, real, real> ToOrthoBounds(const Vector2 &viewport_size) const noexcept;

			///@brief Returns the frustum bounds (left, right, bottom, top, z_near, z_far) from this frustum with the given viewport size
			///@details This returns all the values needed by glFrustum
			[[nodiscard]] std::tuple<real, real, real, real, real, real> ToFrustumBounds(const Vector2 &viewport_size) const noexcept;

			///@}
	};
} //ion::graphics::render

#endif