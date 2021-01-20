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
#include "types/IonTypes.h"

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
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_SCISSOR_TEST);
}

} //viewport::detail


//Private

/*
	Notifying
*/

void Viewport::NotifyViewportResized(const Vector2 &size) noexcept
{
	if (auto owner = Owner(); owner)
		NotifyAll(owner->ViewportEvents().Listeners(), &events::listeners::ViewportListener::ViewportResized, size);
}

void Viewport::NotifyViewportMoved(const Vector2 &position) noexcept
{
	if (auto owner = Owner(); owner)
		NotifyAll(owner->ViewportEvents().Listeners(), &events::listeners::ViewportListener::ViewportMoved, position);
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
		NotifyViewportResized(bounds_.ToSize());
	if (moved)
		NotifyViewportMoved(bounds_.Min());
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
	render_target_size_{render_target.Size()}
{
	//Empty
}

Viewport::Viewport(std::string name, RenderTarget &render_target, const Aabb &bounds) noexcept :

	managed::ManagedObject<RenderTarget>{std::move(name)},
	events::EventChannel<events::Listenable<events::listeners::RenderTargetListener>>{
		render_target, events::event_channel::SubscriptionContract::NonCancelable},

	bounds_{bounds},
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
	Rendering
*/

void Viewport::RenderTo() noexcept
{
	detail::render_to_viewport(bounds_.Min(), bounds_.ToSize(), background_color_);

	if (camera_)
		camera_->CaptureScene(*this);
}

} //ion::graphics::render