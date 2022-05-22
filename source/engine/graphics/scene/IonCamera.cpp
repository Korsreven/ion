/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonCamera.cpp
-------------------------------------------
*/

#include "IonCamera.h"

#include <type_traits>

#include "graphics/IonGraphicsAPI.h"
#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "query/IonSceneQuery.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::scene
{

using namespace camera;
using namespace types::type_literals;
using namespace ion::utilities;
using graphics::render::Frustum;

namespace camera::detail
{

void move_to(const Vector3 &position) noexcept
{
	auto [x, y, z] = position.XYZ();

	if constexpr (std::is_same_v<real, float>)
		glTranslatef(static_cast<float>(-x), static_cast<float>(-y), static_cast<float>(-z));
	else
		glTranslated(static_cast<double>(-x), static_cast<double>(-y), static_cast<double>(-z));
}

void rotate_by(real angle) noexcept
{
	#ifdef ION_LEFT_HANDED
	//Left-hand rotation CW
	if constexpr (std::is_same_v<real, float>)
		glRotatef(static_cast<float>(math::ToDegrees(angle)), 0.0f, 0.0f, 1.0f); //Rotate around the z-axis
	else
		glRotated(static_cast<double>(math::ToDegrees(angle)), 0.0, 0.0, 1.0); //Rotate around the z-axis
	#else
	//Right-hand rotation CCW
	if constexpr (std::is_same_v<real, float>)
		glRotatef(static_cast<float>(-math::ToDegrees(angle)), 0.0f, 0.0f, 1.0f); //Rotate around the z-axis
	else
		glRotated(static_cast<double>(-math::ToDegrees(angle)), 0.0, 0.0, 1.0); //Rotate around the z-axis
	#endif
}


Matrix4 get_view_matrix(const Vector3 &position, real angle) noexcept
{
	return Matrix4::Rotation(angle).Transpose() * Matrix4::Translation(-position);
}

} //camera::detail


//Private

void Camera::PrepareBoundingVolumes() noexcept
{
	auto [left, right, bottom, top, z_near, z_far] =
		frustum_.ToOrthoBounds(viewport_size_);

	aabb_ = Aabb::Size({right - left, top - bottom}, position_).RotateCopy(rotation_);
	obb_ = aabb_;
	sphere_ = {aabb_.ToHalfSize().Max(), aabb_.Center()};
}


/*
	Notifying
*/

void Camera::NotifyCameraFrustumChanged(const Frustum &frustum) noexcept
{
	if (auto owner = Owner(); owner)
		NotifyAll(owner->CameraEvents().Listeners(), &events::listeners::CameraListener::CameraFrustumChanged, frustum);
}

void Camera::NotifyCameraMoved(const Vector3 &position) noexcept
{
	if (auto owner = Owner(); owner)
		NotifyAll(owner->CameraEvents().Listeners(), &events::listeners::CameraListener::CameraMoved, position);
}


//Public

Camera::Camera(std::optional<std::string> name, bool visible) :
	MovableObject{std::move(name), visible}
{
	query_type_flags_ |= query::scene_query::QueryType::Camera;
}

Camera::Camera(std::optional<std::string> name, const render::Frustum &frustum, bool visible) :

	MovableObject{std::move(name), visible},
	frustum_{frustum}
{
	query_type_flags_ |= query::scene_query::QueryType::Camera;
}


/*
	Capturing
*/

void Camera::CaptureScene(const render::Viewport &viewport) noexcept
{
	if (auto viewport_size = viewport.Bounds().ToSize();
		viewport_size_ != viewport_size)
	{
		viewport_size_ = viewport_size;
		update_bounding_volumes_ = true;
	}
	
	frustum_.ProjectScene(viewport_size_);

	auto position = position_;
	auto rotation = rotation_;

	if (auto parent_node = ParentNode(); parent_node)
	{		
		position += parent_node->DerivedPosition();
		rotation += parent_node->DerivedRotation();
	}

	detail::rotate_by(rotation);
	detail::move_to(position);
	view_matrix_ = detail::get_view_matrix(position, rotation);

	if (update_bounding_volumes_)
	{
		PrepareBoundingVolumes();
		update_bounding_volumes_ = false;
	}
}

} //ion::graphics::scene