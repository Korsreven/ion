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
#include <utility>

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
		bind_vbo_to_vao(handle, vbo_handle);
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

void bind_vbo_to_vao(int vao_handle, int vbo_handle) noexcept
{
	glBindVertexArray(vao_handle);
	bind_vertex_buffer_object(vbo_handle);
	glBindVertexArray(0);
}


void set_vertex_buffer_data(int vbo_handle, int vbo_offset, const vertex_storage_type &vertex_data) noexcept
{
	bind_vertex_buffer_object(vbo_handle);
	glBufferSubData(GL_ARRAY_BUFFER, vbo_offset * sizeof(real),
		std::size(vertex_data) * sizeof(real), std::data(vertex_data));
	bind_vertex_buffer_object(0);
}

void set_vertex_attribute_pointers(int vertex_count, int vbo_offset) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	glVertexAttribPointer(0, position_components, type, GL_FALSE, 0,
		(void*)(vbo_offset * sizeof(real)));
	glVertexAttribPointer(1, normal_components, type, GL_FALSE, 0,
		(void*)((vbo_offset + normal_data_offset(vertex_count)) * sizeof(real)));
	glVertexAttribPointer(2, color_components, type, GL_FALSE, 0,
		(void*)((vbo_offset + color_data_offset(vertex_count)) * sizeof(real)));
	glVertexAttribPointer(3, tex_coord_components, type, GL_FALSE, 0,
		(void*)((vbo_offset + tex_coord_data_offset(vertex_count)) * sizeof(real)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}

void set_vertex_attribute_pointers(int vertex_count, const vertex_storage_type &vertex_data) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	glVertexAttribPointer(0, position_components, type, GL_FALSE, 0,
		std::data(vertex_data));
	glVertexAttribPointer(1, normal_components, type, GL_FALSE, 0,
		std::data(vertex_data) + normal_data_offset(vertex_count));
	glVertexAttribPointer(2, color_components, type, GL_FALSE, 0,
		std::data(vertex_data) + color_data_offset(vertex_count));
	glVertexAttribPointer(3, tex_coord_components, type, GL_FALSE, 0,
		std::data(vertex_data) + tex_coord_data_offset(vertex_count));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}

void set_vertex_pointers(int vertex_count, int vbo_offset) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	glVertexPointer(position_components, type, 0,
		(void*)(vbo_offset * sizeof(real)));
	glNormalPointer(/*normal_components,*/ type, 0,
		(void*)((vbo_offset + normal_data_offset(vertex_count)) * sizeof(real)));
	glColorPointer(color_components, type, 0,
		(void*)((vbo_offset + color_data_offset(vertex_count)) * sizeof(real)));
	glTexCoordPointer(tex_coord_components, type, 0,
		(void*)((vbo_offset + tex_coord_data_offset(vertex_count)) * sizeof(real)));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void set_vertex_pointers(int vertex_count, const vertex_storage_type &vertex_data) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	glVertexPointer(position_components, type, 0,
		std::data(vertex_data));
	glNormalPointer(/*normal_components,*/ type, 0,
		std::data(vertex_data) + normal_data_offset(vertex_count));
	glColorPointer(color_components, type, 0,
		std::data(vertex_data) + color_data_offset(vertex_count));
	glTexCoordPointer(tex_coord_components, type, 0,
		std::data(vertex_data) + tex_coord_data_offset(vertex_count));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
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


vertex_storage_type vertices_to_vertex_data(const Vertices &vertices)
{
	vertex_storage_type vertex_data;
	vertex_data.reserve(std::ssize(vertices) * vertex_components);

	//Insert positions
	for (auto &vertex : vertices)
	{
		vertex_data.insert(std::end(vertex_data), vertex.position.Components(), vertex.position.Components() + 2);
		vertex_data.push_back(-1.0_r); //z
	}

	//Insert normals
	for (auto &vertex : vertices)
	{
		vertex_data.insert(std::end(vertex_data), vertex.normal.Components(), vertex.normal.Components() + 2);
		vertex_data.push_back(1.0_r); //z
	}

	//Insert colors
	for (auto &vertex : vertices)
		vertex_data.insert(std::end(vertex_data), vertex.color.Channels(), vertex.color.Channels() + 4);

	//insert tex coords
	for (auto &vertex : vertices)
		vertex_data.insert(std::end(vertex_data), vertex.tex_coord.Components(), vertex.tex_coord.Components() + 2);

	return vertex_data;
}

} //mesh::detail


Mesh::Mesh(std::optional<int> vbo_handle, int vbo_offset) noexcept :

	vao_handle_{vbo_handle ? detail::create_vertex_array_object(*vbo_handle) : std::nullopt},
	vertex_buffer_offset_{vbo_handle ? vbo_offset : 0}
{
	//Empty
}

Mesh::Mesh(std::optional<int> vbo_handle, int vbo_offset, const mesh::Vertices &vertices) :

	vertex_count_{std::ssize(vertices)},

	vao_handle_{vbo_handle ? detail::create_vertex_array_object(*vbo_handle) : std::nullopt},
	vertex_buffer_offset_{vbo_handle ? vbo_offset : 0},

	vertex_data_{detail::vertices_to_vertex_data(vertices)},
	reload_vertex_array_{vertex_count_ > 0 && vao_handle_}
{
	//Empty
}

Mesh::Mesh(std::optional<int> vbo_handle, int vbo_offset, detail::vertex_storage_type vertex_data) :

	vertex_count_{std::ssize(vertex_data) % detail::vertex_components == 0 ?
		std::ssize(vertex_data) / detail::vertex_components : 0},

	vao_handle_{vbo_handle ? detail::create_vertex_array_object(*vbo_handle) : std::nullopt},
	vertex_buffer_offset_{vbo_handle ? vbo_offset : 0},

	vertex_data_{vertex_count_ > 0 ? std::move(vertex_data) : decltype(vertex_data){}},
	reload_vertex_array_{vertex_count_ > 0 && vao_handle_}
{
	//Empty
}

Mesh::~Mesh() noexcept
{
	if (vao_handle_)
		detail::delete_vertex_array_object(*vao_handle_);
}


/*
	Vertices
*/

void Mesh::VertexColor(const Color &color) noexcept
{
	if (!material_)
	{
		for (auto i = 0; i < vertex_count_; ++i)
			std::copy(color.Channels(), color.Channels() + 4,
				std::begin(vertex_data_) + detail::color_data_offset(vertex_count_) + (i * detail::color_components));

		reload_vertex_data_ = vertex_count_ > 0;
	}
}


/*
	Drawing
*/

void Mesh::Prepare() noexcept
{
	if (reload_vertex_data_)
	{
		if (vbo_handle_)
			detail::set_vertex_buffer_data(*vbo_handle_, vertex_buffer_offset_, vertex_data_);

		reload_vertex_data_ = false;
	}

	if (reload_vertex_array_)
	{
		if (vao_handle_)
		{
			detail::bind_vertex_array_object(*vao_handle_);
			detail::set_vertex_attribute_pointers(vertex_count_, vertex_buffer_offset_);
			detail::bind_vertex_array_object(0);
		}

		reload_vertex_array_ = false;
	}
}

void Mesh::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	//Nothing to draw
	if (vertex_count_ == 0)
		return;

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
		detail::bind_vertex_array_object(*vao_handle_);

	glDrawArrays(detail::mesh_draw_mode_to_gl_draw_mode(draw_mode_), 0, vertex_count_);

	if (vao_handle_)
		detail::bind_vertex_array_object(0);


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