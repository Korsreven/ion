/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexBufferObject.cpp
-------------------------------------------
*/

#include "IonVertexBufferObject.h"

#include "IonVertexBufferView.h"
#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::render::vertex
{

using namespace vertex_buffer_object;

namespace vertex_buffer_object::detail
{

int vertex_buffer_usage_to_gl_buffer_usage(VertexBufferUsage buffer_usage) noexcept
{
	switch (buffer_usage)
	{
		case VertexBufferUsage::Dynamic:
		return GL_DYNAMIC_DRAW;

		case VertexBufferUsage::Stream:
		return GL_STREAM_DRAW;

		case VertexBufferUsage::Static:
		default:
		return GL_STATIC_DRAW;
	}
}

VertexBufferUsage gl_buffer_usage_to_vertex_buffer_usage(int buffer_usage) noexcept
{
	switch (buffer_usage)
	{
		case GL_DYNAMIC_DRAW:
		return VertexBufferUsage::Dynamic;

		case GL_STREAM_DRAW:
		return VertexBufferUsage::Stream;

		case GL_STATIC_DRAW:
		default:
		return VertexBufferUsage::Static;
	}
}


/*
	Graphics API
*/

std::optional<int> create_vertex_buffer_object() noexcept
{
	auto handle = 0;

	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glGenBuffers(1, reinterpret_cast<unsigned int*>(&handle));
		break;

		case gl::Extension::ARB:
		glGenBuffersARB(1, reinterpret_cast<unsigned int*>(&handle));
		break;
	}

	if (handle > 0)
		return handle;
	else
		return {};
}

void delete_vertex_buffer_object(int vbo_handle) noexcept
{
	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glDeleteBuffers(1, reinterpret_cast<unsigned int*>(&vbo_handle));
		break;

		case gl::Extension::ARB:
		glDeleteBuffersARB(1, reinterpret_cast<unsigned int*>(&vbo_handle));
		break;
	}
}

void bind_vertex_buffer_object(int vbo_handle) noexcept
{
	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glBindBuffer(GL_ARRAY_BUFFER, vbo_handle);
		break;

		case gl::Extension::ARB:
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_handle);
		break;
	}
}


void set_vertex_buffer_data(int vbo_handle, const void *data, int size, VertexBufferUsage buffer_usage) noexcept
{
	bind_vertex_buffer_object(vbo_handle);

	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glBufferData(GL_ARRAY_BUFFER, size, data,
			vertex_buffer_usage_to_gl_buffer_usage(buffer_usage));
		break;

		case gl::Extension::ARB:
		glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, data,
			vertex_buffer_usage_to_gl_buffer_usage(buffer_usage));
		break;
	}

	bind_vertex_buffer_object(0);
}

void set_vertex_buffer_sub_data(int vbo_handle, const void *data, int offset, int size) noexcept
{
	bind_vertex_buffer_object(vbo_handle);	

	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		break;

		case gl::Extension::ARB:
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, offset, size, data);
		break;
	}

	bind_vertex_buffer_object(0);
}


int get_vertex_buffer_size(int vbo_handle) noexcept
{
	auto size = 0;
	bind_vertex_buffer_object(vbo_handle);

	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		break;

		case gl::Extension::ARB:
		glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &size);
		break;
	}

	bind_vertex_buffer_object(0);
	return size;
}

VertexBufferUsage get_vertex_buffer_usage(int vbo_handle) noexcept
{
	auto usage = 0;
	bind_vertex_buffer_object(vbo_handle);

	switch (gl::VertexBufferObject_Support())
	{
		case gl::Extension::Core:
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_USAGE, &usage);
		break;

		case gl::Extension::ARB:
		glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_USAGE_ARB, &usage);
		break;
	}

	bind_vertex_buffer_object(0);
	return gl_buffer_usage_to_vertex_buffer_usage(usage);
}

} //vertex_buffer_object::detail


VertexBufferObject::VertexBufferObject() noexcept :
	handle_{detail::create_vertex_buffer_object()}
{
	//Empty
}

VertexBufferObject::VertexBufferObject(VertexBufferUsage usage) noexcept :
				
	handle_{detail::create_vertex_buffer_object()},
	usage_{usage}
{
	//Empty
}

VertexBufferObject::VertexBufferObject(int handle) noexcept :

	handle_{handle},
	usage_{detail::get_vertex_buffer_usage(handle)},
	size_{detail::get_vertex_buffer_size(handle)}
{

}

VertexBufferObject::VertexBufferObject(const VertexBufferObject &rhs) :
	usage_{rhs.usage_}
{
	//Empty
}

VertexBufferObject::~VertexBufferObject() noexcept
{
	if (handle_)
		detail::delete_vertex_buffer_object(*handle_);
}


/*
	Modifiers
*/

void VertexBufferObject::Data(const VertexData &vertex_data) noexcept
{
	Data(vertex_data, usage_);
}

void VertexBufferObject::Data(const VertexData &vertex_data, VertexBufferUsage usage) noexcept
{
	if (handle_)
	{
		//Reallocate new buffer
		if (size_ < vertex_data.Size() || usage_ != usage)
		{
			detail::set_vertex_buffer_data(*handle_, vertex_data.Pointer(), vertex_data.Size(), usage); //Copy data to VRAM
			size_ = vertex_data.Size();
			usage_ = usage;
		}
		//Reuse same buffer
		else
			detail::set_vertex_buffer_sub_data(*handle_, vertex_data.Pointer(), 0, vertex_data.Size()); //Copy data to VRAM
	}
}

void VertexBufferObject::Data(const VertexData &vertex_data, int offset) noexcept
{
	if (handle_ && offset + vertex_data.Size() <= size_)
		detail::set_vertex_buffer_sub_data(*handle_, vertex_data.Pointer(), offset, vertex_data.Size()); //Copy data to VRAM
}


void VertexBufferObject::Reserve(int size) noexcept
{
	Reserve(size, usage_);
}

void VertexBufferObject::Reserve(int size, vertex_buffer_object::VertexBufferUsage usage) noexcept
{
	//Reallocate new buffer
	if (size_ < size || usage_ != usage)
	{
		detail::set_vertex_buffer_data(*handle_, nullptr, size, usage); //Reserve space in VRAM
		size_ = size;
		usage_ = usage;
	}
}


/*
	Vertex buffer view
*/

VertexBufferView VertexBufferObject::SubBuffer(int offset, int size) const noexcept
{
	if (handle_ && offset + size <= size_)
		return {*this, offset, size};
	else
		return {};
}

} //ion::graphics::render::vertex