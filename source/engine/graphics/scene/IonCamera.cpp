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

	if constexpr (std::is_same_v<real, double>)
		glTranslated(-x, -y, -z);
	else
		glTranslatef(-x, -y, -z);
}

void rotate_by(real angle) noexcept
{
	if constexpr (std::is_same_v<real, double>)
		glRotated(-math::ToDegrees(angle), 0.0_r, 0.0_r, 1.0_r); //Rotate around the z-axis
	else
		glRotatef(-math::ToDegrees(angle), 0.0_r, 0.0_r, 1.0_r); //Rotate around the z-axis
}


Matrix4 get_view_matrix(const Vector3 &position, real angle) noexcept
{
	return Matrix4::Translation(-position) * Matrix4::Rotation(angle).Transpose();
}

} //camera::detail


//Private

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

Camera::Camera(std::string name, bool visible) :
	MovableObject{std::move(name), visible}
{
	//Empty
}

Camera::Camera(std::string name, const render::Frustum &frustum, bool visible) :

	MovableObject{std::move(name), visible},
	frustum_{frustum}
{
	//Empty
}


/*
	Capturing
*/

void Camera::CaptureScene(const render::Viewport &viewport) noexcept
{
	frustum_.ProjectScene(viewport.Bounds().ToSize());
	detail::rotate_by(rotation_);
	detail::move_to(position_);

	view_matrix_ = detail::get_view_matrix(position_, rotation_);
}

} //ion::graphics::scene