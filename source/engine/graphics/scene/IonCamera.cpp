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

#include "graphics/IonGraphicsAPI.h"
#include "graphics/render/IonViewport.h"

namespace ion::graphics::scene
{

using namespace camera;
using graphics::render::Frustum;

namespace camera::detail
{

void look_at() noexcept
{
	//gluLookAt
}

} //camera::detail


//Private

/*
	Notifying
*/

void Camera::NotifyCameraFrustumChanged(const Frustum &frustum) noexcept
{
	for (auto &listener : Listeners())
		Notify(&events::listeners::CameraListener::CameraFrustumChanged, listener, frustum);
}

void Camera::NotifyCameraMoved(const Vector2 &position) noexcept
{
	for (auto &listener : Listeners())
		Notify(&events::listeners::CameraListener::CameraMoved, listener, position);
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
	detail::look_at(); //Todo
}

} //ion::graphics::scene