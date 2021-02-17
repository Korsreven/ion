/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexArrayObject.cpp
-------------------------------------------
*/

#include "IonVertexArrayObject.h"

#include "graphics/IonGraphicsAPI.h"
#include "graphics/render/vertex/IonVertexBufferObject.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "types/IonTypes.h"

namespace ion::graphics::render::vertex
{

using namespace vertex_array_object;

namespace vertex_array_object::detail
{

/*
	Graphics API
*/

std::optional<int> create_vertex_array_object() noexcept
{
	auto handle = 0;

	switch (gl::VertexArrayObject_Support())
	{
		case gl::Extension::Core:
		case gl::Extension::ARB:
		glGenVertexArrays(1, reinterpret_cast<unsigned int*>(&handle));
		break;
	}

	if (handle > 0)
		return handle;
	else
		return {};
}

void delete_vertex_array_object(int vao_handle) noexcept
{
	switch (gl::VertexArrayObject_Support())
	{
		case gl::Extension::Core:
		case gl::Extension::ARB:
		glDeleteVertexArrays(1, reinterpret_cast<unsigned int*>(&vao_handle));
		break;
	}
}

void bind_vertex_array_object(int vao_handle) noexcept
{
	switch (gl::VertexArrayObject_Support())
	{
		case gl::Extension::Core:
		case gl::Extension::ARB:
		glBindVertexArray(vao_handle);
		break;
	}
}


void bind_vertex_attributes(const VertexDeclaration &vertex_declaration, int vao_handle, int vbo_handle, int vbo_offset) noexcept
{
	bind_vertex_array_object(vao_handle);
	vertex_buffer_object::detail::bind_vertex_buffer_object(vbo_handle);
	set_vertex_attribute_pointers(vertex_declaration, vbo_offset);
	bind_vertex_array_object(0);
}


void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset) noexcept
{
	//For fixed location attributes

	for (auto i = 0; auto &vertex_element : vertex_declaration.Elements())
	{
		shaders::shader_program_manager::detail::set_attribute_value{i}.
			set_vertex_pointer(i, vertex_element.Components(), false, vertex_element.Stride,
							   (void*)(vbo_offset + vertex_element.Offset), real{});
		glEnableVertexAttribArray(i++);
	}
}

void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const void *data) noexcept
{
	//For fixed location attributes

	for (auto i = 0; auto &vertex_element : vertex_declaration.Elements())
	{
		shaders::shader_program_manager::detail::set_attribute_value{i}.
			set_vertex_pointer(i, vertex_element.Components(), false, vertex_element.Stride,
							   static_cast<const std::byte*>(data) + vertex_element.Offset, real{});
		glEnableVertexAttribArray(i++);
	}
}

} //vertex_array_object::detail


VertexArrayObject::VertexArrayObject() noexcept :
	handle_{detail::create_vertex_array_object()}
{
	//Empty
}

VertexArrayObject::VertexArrayObject(const VertexArrayObject&) noexcept
{
	//Do nothing
}

VertexArrayObject::~VertexArrayObject() noexcept
{
	if (handle_)
		detail::delete_vertex_array_object(*handle_);
}


/*
	Modifiers
*/

void VertexArrayObject::Bind() noexcept
{
	if (handle_)
		detail::bind_vertex_array_object(*handle_);
}

void VertexArrayObject::Bind(const VertexDeclaration &vertex_declaration, const VertexBufferView &vertex_buffer) noexcept
{
	if (handle_ && vertex_buffer)
		detail::bind_vertex_attributes(vertex_declaration, *handle_, *vertex_buffer.Handle(), vertex_buffer.Offset());
}

void VertexArrayObject::Unbind() noexcept
{
	if (handle_)
		detail::bind_vertex_array_object(0);
}

} //ion::graphics::render::vertex