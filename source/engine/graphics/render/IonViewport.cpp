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

namespace ion::graphics::render
{

namespace viewport::detail
{

void change_viewport() noexcept
{

}

} //viewport::detail


//Protected

/*
	Events
*/

void Viewport::RenderTargetResized(const Vector2 &size) noexcept
{

}


//Public

Viewport::Viewport(const std::optional<real> &x, const std::optional<real> &y,
	const std::optional<real> &width, const std::optional<real> &height) noexcept :
	
	x_{x},
	y_{y},
	width_{width},
	height_{height}
{
	//Empty
}

Viewport::Viewport(RenderTarget &render_target) noexcept
{
	render_target.Subscribe(*this);
}

Viewport::Viewport(RenderTarget &render_target, 
	const std::optional<real> &x, const std::optional<real> &y,
	const std::optional<real> &width, const std::optional<real> &height) noexcept :

	x_{x},
	y_{y},
	width_{width},
	height_{height}
{
	render_target.Subscribe(*this);
}


/*
	Static viewport conversions
*/

Viewport Viewport::Client(RenderTarget &render_target) noexcept
{
	return {render_target, {}, {}, {}, {}};
}

} //ion::graphics::render