/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonFrustum.cpp
-------------------------------------------
*/

#include "IonFrustum.h"

#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::render
{

using namespace frustum;

namespace frustum::detail
{



Aabb keep_proportion(const Aabb &clip_plane, real aspect_ratio, AspectRatioFormat aspect_format,
	real base_viewport_height, const Vector2 &viewport_size) noexcept
{
	auto height = clip_plane.ToSize().Y();
	auto width = height * aspect_ratio;
	
	auto [viewport_width, viewport_height] = viewport_size.XY();
	auto viewport_aspect_ratio = viewport_width / viewport_height;

	switch (aspect_format)
	{
		case AspectRatioFormat::PanAndScan:
		{
			//Crop left/right
			if (aspect_ratio > viewport_aspect_ratio)
				width *= (viewport_width / (viewport_height * aspect_ratio));
			//Crop top/bottom
			else if (aspect_ratio < viewport_aspect_ratio)
				height *= viewport_height / (viewport_width / aspect_ratio);

			break;
		}

		case AspectRatioFormat::Letterbox:
		{
			//Letterbox
			if (aspect_ratio > viewport_aspect_ratio)
				height *= viewport_height / (viewport_width / aspect_ratio);
			//Pillarbox
			else if (aspect_ratio < viewport_aspect_ratio)
				width *= (viewport_width / (viewport_height * aspect_ratio));

			break;
		}

		case AspectRatioFormat::Windowbox:
		{
			auto base_viewport_width = base_viewport_height * aspect_ratio;
			width *= (viewport_width / base_viewport_width);
			height *= (viewport_height / base_viewport_height);
			break;
		}
	}

	return Aabb::Size({width , height}, clip_plane.Center());
}

std::tuple<real, real, real, real, real, real> to_ortho(const std::optional<Aabb> &clip_plane, real z_near, real z_far,
	const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format, real base_viewport_height, const Vector2 &viewport_size) noexcept
{
	auto plane = clip_plane ?
		*clip_plane :
		Aabb{graphics::utilities::vector2::Zero, viewport_size};

	if (aspect_ratio)
		plane = keep_proportion(plane, *aspect_ratio, aspect_format, base_viewport_height, viewport_size);

	auto [left, bottom] = plane.Min().XY();
	auto [right, top] = plane.Max().XY();

	return std::tuple{left, right, bottom, top, z_near, z_far};
}

std::tuple<real, real, real, real, real, real> to_frustum(const std::optional<Aabb> &clip_plane, real z_near, real z_far, real fov,
	const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format, real base_viewport_height, const Vector2 &viewport_size) noexcept
{
	auto plane = clip_plane ?
		*clip_plane :
		Aabb{graphics::utilities::vector2::Zero, viewport_size};

	if (aspect_ratio)
		plane = keep_proportion(plane, *aspect_ratio, aspect_format, base_viewport_height, viewport_size);

	auto [left, bottom] = plane.Min().XY();
	auto [right, top] = plane.Max().XY();
	auto [width, height] = plane.ToSize().XY();

	return perspective_to_frustum(fov, width / height, z_near, z_far);
}


void change_projection(ProjectionType projection, const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance, real fov,
	const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format, real base_viewport_height, const Vector2 &viewport_size) noexcept
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	switch (projection)
	{
		case ProjectionType::Perspective:
		{
			auto [left, right, bottom, top, z_near, z_far] =
				to_frustum(clip_plane, near_clip_distance, far_clip_distance, fov,
						   aspect_ratio, aspect_format, base_viewport_height, viewport_size);
			glFrustum(left, right, bottom, top, z_near, z_far);
			break;
		}

		case ProjectionType::Orthographic:
		default:
		{
			auto [left, right, bottom, top, z_near, z_far] =
				to_ortho(clip_plane, near_clip_distance, far_clip_distance,
						 aspect_ratio, aspect_format, base_viewport_height, viewport_size);
			glOrtho(left, right, bottom, top, z_near, z_far);
			break;
		}
	}

	glMatrixMode(GL_MODELVIEW); //Switch back
}

} //frustum::detail


Frustum::Frustum(const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance,
	const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format) :

	clip_plane_{clip_plane},
	near_clip_distance_{near_clip_distance},
	far_clip_distance_{far_clip_distance},

	aspect_ratio_{aspect_ratio},
	aspect_format_{aspect_format}
{
	//Empty
}

Frustum::Frustum(ProjectionType projection, const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance,
	real field_of_view, const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format) :

	projection_{projection},

	clip_plane_{clip_plane},
	near_clip_distance_{near_clip_distance},
	far_clip_distance_{far_clip_distance},
	field_of_view_{field_of_view},

	aspect_ratio_{aspect_ratio},
	aspect_format_{aspect_format}
{
	//Empty
}


/*
	Static frustum conversions
*/

Frustum Frustum::Orthographic(const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance,
	const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format) noexcept
{
	return {clip_plane, near_clip_distance, far_clip_distance,
			aspect_ratio, aspect_format};
}

Frustum Frustum::Perspective(const std::optional<Aabb> &clip_plane, real near_clip_distance, real far_clip_distance,
	real field_of_view, const std::optional<real> &aspect_ratio, AspectRatioFormat aspect_format) noexcept
{
	return {ProjectionType::Perspective, clip_plane, near_clip_distance, far_clip_distance,
			field_of_view, aspect_ratio, aspect_format};
}


/*
	Frustum conversions
*/

std::tuple<real, real, real, real, real, real> Frustum::ToOrthoBounds(const Vector2 &viewport_size) const noexcept
{
	return detail::to_ortho(clip_plane_, near_clip_distance_, far_clip_distance_,
							aspect_ratio_, aspect_format_, base_viewport_height_, viewport_size);
}

std::tuple<real, real, real, real, real, real> Frustum::ToFrustumBounds(const Vector2 &viewport_size) const noexcept
{
	return detail::to_frustum(clip_plane_, near_clip_distance_, far_clip_distance_, field_of_view_,
							  aspect_ratio_, aspect_format_, base_viewport_height_, viewport_size);
}


} //ion::graphics::render