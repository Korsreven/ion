/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonViewport.cpp
-------------------------------------------
*/

#include "IonViewport.h"

#include "IonRenderTarget.h"
#include "graphics/IonGraphicsAPI.h"
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/utilities/IonMatrix3.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::graphics::render
{

using namespace viewport;
using namespace types::type_literals;

namespace viewport::detail
{

std::pair<HorizontalAnchorType, VerticalAnchorType> get_anchors(AlignmentType alignment) noexcept
{
	switch (alignment)
	{
		case AlignmentType::TopLeft:
		return {HorizontalAnchorType::Left, VerticalAnchorType::Top};

		case AlignmentType::TopRight:
		return {HorizontalAnchorType::Right, VerticalAnchorType::Top};

		case AlignmentType::BottomLeft:
		return {HorizontalAnchorType::Left, VerticalAnchorType::Bottom};
		
		case AlignmentType::BottomRight:
		return {HorizontalAnchorType::Right, VerticalAnchorType::Bottom};
	}

	return {HorizontalAnchorType::Percentage, VerticalAnchorType::Percentage};
}

Aabb get_aligned_aabb(AlignmentType alignment, const Vector2 &size, const Vector2 &render_target_size) noexcept
{
	return Aabb::Size(size,
		[&]() noexcept
		{
			auto half_size = size * 0.5_r;
			auto [half_width, half_height] = half_size.XY();
			auto [render_target_width, render_target_height] = render_target_size.XY();

			switch (alignment)
			{
				case AlignmentType::TopLeft:
				return Vector2{half_width, render_target_height - half_height};

				case AlignmentType::TopRight:
				return Vector2{render_target_width - half_width, render_target_height - half_height};

				case AlignmentType::BottomRight:
				return Vector2{render_target_width - half_width, half_height};

				case AlignmentType::BottomLeft:
				default:
				return Vector2{half_width, half_height};
			}
		}());
}

Vector2 get_adjusted_position(const Vector2 &position, const Vector2 &size, const Vector2 &new_size,
	HorizontalAnchorType horizontal_anchor_type, VerticalAnchorType vertical_anchor_type) noexcept
{
	auto [x, y] = position.XY();
	auto [width, height] = size.XY();
	auto [new_width, new_height] = new_size.XY();

	return {
		[&]() noexcept
		{
			switch (horizontal_anchor_type)
			{
				case HorizontalAnchorType::Left:
				return x;

				case HorizontalAnchorType::Right:
				return new_width - (width - x);

				case HorizontalAnchorType::Percentage:
				default:
				return new_width * (x / width);
			}
		}(),
		[&]() noexcept
		{
			switch (vertical_anchor_type)
			{
				case VerticalAnchorType::Top:
				return new_height - (height - y);

				case VerticalAnchorType::Bottom:
				return y;

				case VerticalAnchorType::Percentage:
				default:
				return new_height * (y / height);
			}
		}()
	};
}


Vector2 viewport_to_ortho_ratio(const Vector2 &viewport_size, real left, real right, real bottom, real top) noexcept
{
	auto [width, height] = viewport_size.XY();
	return {(right - left) / width, (top - bottom) / height};
}

Vector2 ortho_to_viewport_ratio(const Vector2 &viewport_size, real left, real right, real bottom, real top) noexcept
{
	auto [width, height] = viewport_size.XY();
	return {width / (right - left), height / (top - bottom)};
}


Vector2 viewport_to_ortho_point(const Vector2 &viewport_size, real left, real right, real bottom, real top, const Vector2 &point) noexcept
{
	auto [width, height] = viewport_size.XY();
	auto [x, y] = point.XY();

	using namespace ion::utilities;
	return {math::Normalize(x, 0.0_r, width, left, right),
			math::Normalize(y, 0.0_r, height, bottom, top)};
}

Vector2 ortho_to_viewport_point(const Vector2 &viewport_size, real left, real right, real bottom, real top, const Vector2 &point) noexcept
{
	auto [width, height] = viewport_size.XY();
	auto [x, y] = point.XY();

	using namespace ion::utilities;
	return {math::Normalize(x, left, right, 0.0_r, width),
			math::Normalize(y, bottom, top, 0.0_r, height)};
}


Vector2 camera_to_ortho_point(scene::Camera &camera, const Vector2 &point) noexcept
{
	if (auto parent_node = camera.ParentNode(); parent_node)
		return utilities::Matrix3::Transformation(
					-(camera.Rotation() + parent_node->DerivedRotation()),
					utilities::vector2::UnitScale / parent_node->DerivedScaling(),
					-(camera.Position() + parent_node->DerivedPosition())
				).TransformPoint(point);
	else
		return utilities::Matrix3::Transformation(
					-camera.Rotation(),
					utilities::vector2::UnitScale,
					-camera.Position()
				).TransformPoint(point);
}

Vector2 ortho_to_camera_point(scene::Camera &camera, const Vector2 &point) noexcept
{
	if (auto parent_node = camera.ParentNode(); parent_node)
		return utilities::Matrix3::Transformation(
					camera.Rotation() + parent_node->DerivedRotation(),
					parent_node->DerivedScaling(),
					camera.Position() + parent_node->DerivedPosition()
				).TransformPoint(point);
	else
		return utilities::Matrix3::Transformation(
					camera.Rotation(),
					utilities::vector2::UnitScale,
					camera.Position()
				).TransformPoint(point);
}


void render_to_viewport(const Vector2 &position, const Vector2 &size, const Color &background_color) noexcept
{
	auto [x, y] = position.XY();
	auto [width, height] = size.XY();

	auto gl_x = static_cast<GLint>(x);
	auto gl_y = static_cast<GLint>(y);
	auto gl_width = static_cast<GLsizei>(width);
	auto gl_height = static_cast<GLsizei>(height);

	glViewport(gl_x, gl_y, gl_width, gl_height);
	glScissor(gl_x, gl_y, gl_width, gl_height);
	glEnable(GL_SCISSOR_TEST);

	auto [r, g, b, a] = background_color.RGBA();
	glClearDepth(1.0);
	glClearColor(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_SCISSOR_TEST);
}

} //viewport::detail


//Private

/*
	Notifying
*/

void Viewport::NotifyViewportResized() noexcept
{
	if (auto owner = Owner(); owner)
		NotifyAll(owner->ViewportEvents().Listeners(), &events::listeners::ViewportListener::ViewportResized, std::ref(*this));
}

void Viewport::NotifyViewportMoved() noexcept
{
	if (auto owner = Owner(); owner)
		NotifyAll(owner->ViewportEvents().Listeners(), &events::listeners::ViewportListener::ViewportMoved, std::ref(*this));
}


/*
	Bounds
*/

Aabb Viewport::ResizedBounds(const Vector2 &size, const Vector2 &new_size) noexcept
{
	auto [min, max] = bounds_.MinMax();
	return {detail::get_adjusted_position(min, size, new_size, left_anchor_, bottom_anchor_),
			detail::get_adjusted_position(max, size, new_size, right_anchor_, top_anchor_)};
}

void Viewport::UpdateBounds(const Aabb &bounds) noexcept
{
	auto resized = bounds_.ToSize() != bounds.ToSize();
	auto moved = bounds_.Min() != bounds.Min();		

	bounds_ = bounds;

	if (resized)
		NotifyViewportResized();
	if (moved)
		NotifyViewportMoved();
}


/*
	Events
*/

void Viewport::RenderTargetResized(Vector2 size) noexcept
{
	if (size != graphics::utilities::vector2::Zero)
	{
		UpdateBounds(ResizedBounds(render_target_size_, size));
		render_target_size_ = size;
	}
}


//Public

Viewport::Viewport(std::string name, RenderTarget &render_target) noexcept :

	managed::ManagedObject<RenderTarget>{std::move(name)},
	events::EventChannel<events::Listenable<events::listeners::RenderTargetListener>>{
		render_target, events::event_channel::SubscriptionContract::NonCancelable},

	bounds_{graphics::utilities::vector2::Zero, render_target.Size()},
	base_bounds_{bounds_},
	render_target_size_{render_target.Size()}
{
	//Empty
}

Viewport::Viewport(std::string name, RenderTarget &render_target, const Aabb &bounds) noexcept :

	managed::ManagedObject<RenderTarget>{std::move(name)},
	events::EventChannel<events::Listenable<events::listeners::RenderTargetListener>>{
		render_target, events::event_channel::SubscriptionContract::NonCancelable},

	bounds_{bounds},
	base_bounds_{bounds_},
	render_target_size_{render_target.Size()}
{
	//Empty
}

Viewport::Viewport(std::string name, RenderTarget &render_target, const Aabb &bounds,
	HorizontalAnchorType left_anchor, HorizontalAnchorType right_anchor,
	VerticalAnchorType top_anchor, VerticalAnchorType bottom_anchor) noexcept :

	managed::ManagedObject<RenderTarget>{std::move(name)},
	events::EventChannel<events::Listenable<events::listeners::RenderTargetListener>>{
		render_target, events::event_channel::SubscriptionContract::NonCancelable},

	bounds_{bounds},
	base_bounds_{bounds_},

	left_anchor_{left_anchor},
	right_anchor_{right_anchor},
	top_anchor_{top_anchor},
	bottom_anchor_{bottom_anchor},

	render_target_size_{render_target.Size()}
{
	//Empty
}


/*
	Static viewport conversions
*/

Viewport Viewport::Aligned(std::string name, RenderTarget &render_target, AlignmentType alignment, const Vector2 &size) noexcept
{
	auto [x_anchor, y_anchor] = detail::get_anchors(alignment);
	return {std::move(name), render_target, detail::get_aligned_aabb(alignment, size, render_target.Size()), x_anchor, x_anchor, y_anchor, y_anchor};
}

Viewport Viewport::Aligned(std::string name, RenderTarget &render_target, AlignmentType alignment, real width_percent, real height_percent) noexcept
{
	auto [width, height] = render_target.Size().XY();
	return {std::move(name), render_target, detail::get_aligned_aabb(alignment, {width * width_percent, height * height_percent}, render_target.Size())};
}


Viewport Viewport::LeftAligned(std::string name, RenderTarget &render_target, real width_percent) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::BottomLeft, width_percent, 1.0_r);
}

Viewport Viewport::RightAligned(std::string name, RenderTarget &render_target, real width_percent) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::BottomRight, width_percent, 1.0_r);
}

Viewport Viewport::TopAligned(std::string name, RenderTarget &render_target, real height_percent) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::TopLeft, 1.0_r, height_percent);
}

Viewport Viewport::BottomAligned(std::string name, RenderTarget &render_target, real height_percent) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::BottomLeft, 1.0_r, height_percent);
}


Viewport Viewport::TopLeftAligned(std::string name, RenderTarget &render_target, const Vector2 &size) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::TopLeft, size);
}

Viewport Viewport::TopLeftAligned(std::string name, RenderTarget &render_target, real width_percent, real height_percent) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::TopLeft, width_percent, height_percent);
}

Viewport Viewport::TopRightAligned(std::string name, RenderTarget &render_target, const Vector2 &size) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::TopRight, size);
}

Viewport Viewport::TopRightAligned(std::string name, RenderTarget &render_target, real width_percent, real height_percent) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::TopRight, width_percent, height_percent);
}

Viewport Viewport::BottomLeftAligned(std::string name, RenderTarget &render_target, const Vector2 &size) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::BottomLeft, size);
}

Viewport Viewport::BottomLeftAligned(std::string name, RenderTarget &render_target, real width_percent, real height_percent) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::BottomLeft, width_percent, height_percent);
}

Viewport Viewport::BottomRightAligned(std::string name, RenderTarget &render_target, const Vector2 &size) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::BottomRight, size);
}

Viewport Viewport::BottomRightAligned(std::string name, RenderTarget &render_target, real width_percent, real height_percent) noexcept
{
	return Aligned(std::move(name), render_target, AlignmentType::BottomRight, width_percent, height_percent);
}


/*
	Camera
*/

void Viewport::ConnectedCamera(NonOwningPtr<scene::Camera> camera) noexcept
{
	camera_ = camera;

	if (camera)
		camera->BaseViewportHeight(base_bounds_.ToSize().Y());
}


/*
	Conversions
*/

Vector2 Viewport::ViewportToOrthoRatio() const noexcept
{
	if (camera_)
	{
		auto viewport_size = BaseBounds().ToSize();
		auto [left, right, bottom, top, z_near, z_far] = camera_->ViewFrustum().ToOrthoBounds(viewport_size);
		return detail::viewport_to_ortho_ratio(viewport_size, left, right, bottom, top);
	}
	else
		return utilities::vector2::UnitScale;
}

Vector2 Viewport::OrthoToViewportRatio() const noexcept
{
	if (camera_)
	{
		auto viewport_size = BaseBounds().ToSize();
		auto [left, right, bottom, top, z_near, z_far] = camera_->ViewFrustum().ToOrthoBounds(viewport_size);
		return detail::ortho_to_viewport_ratio(viewport_size, left, right, bottom, top);
	}
	else
		return utilities::vector2::UnitScale;
}


Vector2 Viewport::ViewportToCameraPoint(const Vector2 &point) const noexcept
{
	if (camera_)
	{
		auto viewport_size = Bounds().ToSize();
		auto [left, right, bottom, top, z_near, z_far] = camera_->ViewFrustum().ToOrthoBounds(viewport_size);
		auto ortho_point = detail::viewport_to_ortho_point(viewport_size, left, right, bottom, top, point);	
		return detail::ortho_to_camera_point(*camera_, ortho_point);
	}
	else
		return point;
}

Vector2 Viewport::CameraToViewportPoint(const Vector2 &point) const noexcept
{
	if (camera_)
	{
		auto viewport_size = Bounds().ToSize();	
		auto [left, right, bottom, top, z_near, z_far] = camera_->ViewFrustum().ToOrthoBounds(viewport_size);
		auto ortho_point = detail::camera_to_ortho_point(*camera_, point);	
		return detail::ortho_to_viewport_point(viewport_size, left, right, bottom, top, ortho_point);
	}
	else
		return point;
}


/*
	Rendering
*/

void Viewport::RenderTo() noexcept
{
	detail::render_to_viewport(bounds_.Min(), bounds_.ToSize(), background_color_);
}

} //ion::graphics::render