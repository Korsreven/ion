/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonMesh.cpp
-------------------------------------------
*/

#include "IonMesh.h"

#include <type_traits>

#include "graphics/IonGraphicsAPI.h"
#include "graphics/materials/IonMaterial.h"
#include "graphics/shaders/IonShaderProgram.h"

namespace ion::graphics::render
{

using namespace mesh;

namespace mesh::detail
{

int mesh_draw_mode_to_gl_draw_mode(MeshDrawMode draw_mode) noexcept
{
	switch (draw_mode)
	{
		case MeshDrawMode::Points:
		return GL_POINTS;

		case MeshDrawMode::Lines:
		return GL_LINES;

		case MeshDrawMode::LineLoop:
		return GL_LINE_LOOP;

		case MeshDrawMode::LineStrip:
		return GL_LINE_STRIP;

		case MeshDrawMode::TriangleFan:
		return GL_TRIANGLE_FAN;

		case MeshDrawMode::TriangleStrip:
		return GL_TRIANGLE_STRIP;

		case MeshDrawMode::Quads:
		return GL_QUADS;

		case MeshDrawMode::Polygon:
		return GL_POLYGON;

		case MeshDrawMode::Triangles:
		default:
		return GL_TRIANGLES;
	}
}


std::optional<int> create_vertex_array_object(int vbo_handle) noexcept
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
	{
		glBindVertexArray(handle);
		detail::bind_vertex_buffer_object(vbo_handle);
		glBindVertexArray(0);

		return handle;
	}
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


void use_shader_program(int program_handle) noexcept
{
	switch (gl::Shader_Support())
	{
		case gl::Extension::Core:
		glUseProgram(program_handle);
		break;

		case gl::Extension::ARB:
		glUseProgramObjectARB(program_handle);
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


void set_vertex_attribute_pointers(int vertex_count, int vbo_offset) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	glVertexAttribPointer(0, vertex_components_, type, GL_FALSE, 0,
		(void*)vbo_offset);
	glVertexAttribPointer(1, normal_components_, type, GL_FALSE, 0,
		(void*)(vbo_offset + normal_data_offset(vertex_count) * sizeof(real)));
	glVertexAttribPointer(2, color_components_, type, GL_FALSE, 0,
		(void*)(vbo_offset + color_data_offset(vertex_count) * sizeof(real)));
	glVertexAttribPointer(3, tex_coord_components_, type, GL_FALSE, 0,
		(void*)(vbo_offset + tex_coord_data_offset(vertex_count) * sizeof(real)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}

void set_vertex_attribute_pointers(int vertex_count, const vertex_storage_type &vertex_data) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	glVertexAttribPointer(0, vertex_components_, type, GL_FALSE, 0,
		std::data(vertex_data));
	glVertexAttribPointer(1, normal_components_, type, GL_FALSE, 0,
		std::data(vertex_data) + normal_data_offset(vertex_count));
	glVertexAttribPointer(2, color_components_, type, GL_FALSE, 0,
		std::data(vertex_data) + color_data_offset(vertex_count));
	glVertexAttribPointer(3, tex_coord_components_, type, GL_FALSE, 0,
		std::data(vertex_data) + tex_coord_data_offset(vertex_count));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}


void set_vertex_pointers(int vertex_count, int vbo_offset) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	glVertexPointer(vertex_components_, type, 0,
		(void*)vbo_offset);
	glNormalPointer(/*normal_components_,*/ type, 0,
		(void*)(vbo_offset + normal_data_offset(vertex_count) * sizeof(real)));
	glColorPointer(color_components_, type, 0,
		(void*)(vbo_offset + color_data_offset(vertex_count) * sizeof(real)));
	glTexCoordPointer(tex_coord_components_, type, 0,
		(void*)(vbo_offset + tex_coord_data_offset(vertex_count) * sizeof(real)));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void set_vertex_pointers(int vertex_count, const vertex_storage_type &vertex_data) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	glVertexPointer(vertex_components_, type, 0,
		std::data(vertex_data));
	glNormalPointer(/*normal_components_,*/ type, 0,
		std::data(vertex_data) + normal_data_offset(vertex_count));
	glColorPointer(color_components_, type, 0,
		std::data(vertex_data) + color_data_offset(vertex_count));
	glTexCoordPointer(tex_coord_components_, type, 0,
		std::data(vertex_data) + tex_coord_data_offset(vertex_count));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

} //mesh::detail


Mesh::Mesh(std::optional<int> vbo_handle) :
	vao_handle_{vbo_handle ? detail::create_vertex_array_object(*vbo_handle) : std::nullopt}
{
	//Empty
}

Mesh::~Mesh()
{
	if (vao_handle_)
		detail::delete_vertex_array_object(*vao_handle_);
}


/*
	Modifiers
*/




/*
	Observers
*/




/*
	Drawing
*/

void Mesh::Draw(shaders::ShaderProgram *shader_program)
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	if (reload_vertex_array_ && vao_handle_)
	{
		glBindVertexArray(*vao_handle_);
		detail::set_vertex_attribute_pointers(vertex_count_, vertex_buffer_offset_);
		glBindVertexArray(0);

		reload_vertex_array_ = false;
	}

	auto has_supported_attributes = false;

	//Use shaders
	if (shader_program && shader_program->Handle())
	{
		detail::use_shader_program(*shader_program->Handle());	

		if (!vao_handle_)
		{
			has_supported_attributes = true;

			if (has_supported_attributes)
			{
				//VRAM
				if (vbo_handle_)
				{
					if (!vao_handle_)
						detail::bind_vertex_buffer_object(*vbo_handle_);

					detail::set_vertex_attribute_pointers(vertex_count_, vertex_buffer_offset_);
				}
				else //RAM
					detail::set_vertex_attribute_pointers(vertex_count_, vertex_data_);
			}
			else
			{
				//VRAM
				if (vbo_handle_)
				{
					detail::bind_vertex_buffer_object(*vbo_handle_);
					detail::set_vertex_pointers(vertex_count_, vertex_buffer_offset_);
				}
				else //RAM
					detail::set_vertex_pointers(vertex_count_, vertex_data_);
			}
		}
	}
	else //Fixed-function pipeline
	{
		if (!vao_handle_)
		{
			//VRAM
			if (vbo_handle_)
			{
				detail::bind_vertex_buffer_object(*vbo_handle_);
				detail::set_vertex_pointers(vertex_count_, vertex_buffer_offset_);
			}
			else //RAM
				detail::set_vertex_pointers(vertex_count_, vertex_data_);
		}
	}


	if (vao_handle_)
		glBindVertexArray(*vao_handle_);

	glDrawArrays(detail::mesh_draw_mode_to_gl_draw_mode(draw_mode_), 0, vertex_count_);

	if (vao_handle_)
		glBindVertexArray(0);


	//Shaders
	if (shader_program && shader_program->Handle())
	{
		if (!vao_handle_)
		{
			if (has_supported_attributes)
			{
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);
				glDisableVertexAttribArray(3);
			}
			else
			{
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
		}

		detail::use_shader_program(0);
	}
	else //Fixed-function pipeline
	{
		if (!vao_handle_)
		{
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			//VRAM
			if (vbo_handle_)
				detail::bind_vertex_buffer_object(0);
		}
	}
}

} //ion::graphics::render