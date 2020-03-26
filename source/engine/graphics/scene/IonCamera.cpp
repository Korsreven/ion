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


/*
	Events
*/

void Camera::ViewportResized(Vector2 size) noexcept
{
	//Todo
}


//Public

Camera::Camera(const render::Frustum &frustum) noexcept :

	ListeningChannel{events::listeners::listening_channel::SubscriptionContract::NonCancelable},
	frustum_{frustum}
{
	//Empty
}

Camera::Camera(render::Viewport &viewport) noexcept :
	ListeningChannel{viewport, events::listeners::listening_channel::SubscriptionContract::NonCancelable}
{
	//Empty
}

Camera::Camera(render::Viewport &viewport, const render::Frustum &frustum) noexcept :

	ListeningChannel{viewport, events::listeners::listening_channel::SubscriptionContract::NonCancelable},
	frustum_{frustum}
{
	//Empty
}


void Camera::Change() noexcept
{
	if (auto publisher = Publisher(); publisher)
		frustum_.Change(static_cast<const render::Viewport&>(*publisher).Bounds().ToSize());
}

} //ion::graphics::scene