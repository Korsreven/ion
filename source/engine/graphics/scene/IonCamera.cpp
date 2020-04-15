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

#include "IonSceneManager.h"
#include "graphics/IonGraphicsAPI.h"
#include "graphics/render/IonViewport.h"

namespace ion::graphics::scene
{

using namespace camera;
using graphics::render::Frustum;

namespace camera::detail
{

void move_to(const Vector2 &position) noexcept
{
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	auto [x, y] = position.XY();
	glTranslatef(-x, -y, 0.0f);
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

void Camera::NotifyCameraMoved(const Vector2 &position) noexcept
{
	if (auto owner = Owner(); owner)
		NotifyAll(owner->CameraEvents().Listeners(), &events::listeners::CameraListener::CameraMoved, position);
}


//Public

Camera::Camera(const render::Frustum &frustum) noexcept :
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
	detail::move_to(position_);
}

} //ion::graphics::scene