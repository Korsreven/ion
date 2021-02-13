/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexBufferView.cpp
-------------------------------------------
*/

#include "IonVertexBufferView.h"

#include "IonVertexBufferObject.h"
#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::render::vertex
{

using namespace vertex_buffer_view;

namespace vertex_buffer_view::detail
{
} //vertex_buffer_view::detail


VertexBufferView::VertexBufferView(const VertexBufferObject &vertex_buffer, int offset) noexcept :
	VertexBufferView{vertex_buffer, offset, vertex_buffer.Size()}
{
	//Empty
}

VertexBufferView::VertexBufferView(const VertexBufferObject &vertex_buffer, int offset, int size) noexcept :

	handle_{vertex_buffer.Handle()},
	offset_{offset > 0 ? offset : 0},
	size_{offset_ + size <= vertex_buffer.Size() ?
		size : vertex_buffer.Size() - offset_}
{
	//Empty
}

VertexBufferView::VertexBufferView(int handle, int offset, int size) noexcept :

	handle_{handle},
	offset_{offset > 0 ? offset : 0},
	size_{offset_ + size <= vertex_buffer_object::detail::get_vertex_buffer_size(handle) ?
		size : vertex_buffer_object::detail::get_vertex_buffer_size(handle) - offset_}
{
	//Empty
}


/*
	Modifiers
*/

void VertexBufferView::Bind() noexcept
{
	if (handle_)
		vertex_buffer_object::detail::bind_vertex_buffer_object(*handle_);
}

void VertexBufferView::Unbind() noexcept
{
	if (handle_)
		vertex_buffer_object::detail::bind_vertex_buffer_object(0);
}


void VertexBufferView::Data(const VertexData &vertex_data, int offset) noexcept
{
	if (handle_ && offset_ + offset + vertex_data.Size() <= size_)
		vertex_buffer_object::detail::set_vertex_buffer_sub_data(*handle_, vertex_data.Pointer(), offset_ + offset, vertex_data.Size()); //Copy data to VRAM
}

} //ion::graphics::render::vertex