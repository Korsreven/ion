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
#include "utilities/IonMath.h"

namespace ion::graphics::scene
{

using namespace camera;
using namespace ion::utilities;

namespace camera::detail
{

void gl_perspective(real fov, real aspect_ratio, real z_near, real z_far) noexcept
{
    auto height = math::Tan(fov / 360.0_r * math::Pi) * z_near;
    auto width = height * aspect_ratio;
    glFrustum(-width, width, -height, height, z_near, z_far);
}

void change_projection(const Aabb &clipping_plane, real z_near, real z_far, real fov,
	const std::optional<real> &aspect_ratio, Projection projection) noexcept
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	auto [left, bottom] = clipping_plane.Min().XY();
	auto [right, top] = clipping_plane.Max().XY();
	auto width = right - left;
	auto height = bottom - top;

	switch (projection)
	{
		case Projection::Orthographic:
		{
			if (aspect_ratio)
				glOrtho(left, right, bottom, top / *aspect_ratio, z_near, z_far);
			else
				glOrtho(left, right, bottom, top, z_near, z_far);

			if (aspect_ratio)
				glOrtho(left, right, bottom / *aspect_ratio, top / *aspect_ratio, z_near, z_far);
			else
				glOrtho(left, right, bottom / (width / height), top / (width / height), z_near, z_far);

			break;
		}

		case Projection::Perspective:
		{
			if (aspect_ratio)
				gl_perspective(fov, *aspect_ratio * width / height, z_near, z_far);
			else
				gl_perspective(fov, width / height, z_near, z_far);

			break;
		}
	}

	glMatrixMode(GL_MODELVIEW); //Switch back
}

} //camera::detail


//Protected

/*
	Events
*/

void Camera::ViewportResized(Vector2 size) noexcept
{

}


//Public

Camera::Camera(render::Viewport &viewport) noexcept
{
	viewport.Subscribe(*this);
}

} //ion::graphics::scene