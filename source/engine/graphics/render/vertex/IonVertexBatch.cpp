/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexBatch.cpp
-------------------------------------------
*/

#include "IonVertexBatch.h"

#include <cstddef>
#include "graphics/IonGraphicsAPI.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/shaders/IonShaderProgramManager.h"

namespace ion::graphics::render::vertex
{

using namespace vertex_batch;

namespace vertex_batch::detail
{

int vertex_draw_mode_to_gl_draw_mode(VertexDrawMode draw_mode) noexcept
{
	switch (draw_mode)
	{
		case VertexDrawMode::Points:
		return GL_POINTS;

		case VertexDrawMode::Lines:
		return GL_LINES;

		case VertexDrawMode::LineLoop:
		return GL_LINE_LOOP;

		case VertexDrawMode::LineStrip:
		return GL_LINE_STRIP;

		case VertexDrawMode::TriangleFan:
		return GL_TRIANGLE_FAN;

		case VertexDrawMode::TriangleStrip:
		return GL_TRIANGLE_STRIP;

		case VertexDrawMode::Quads:
		return GL_QUADS;

		case VertexDrawMode::Polygon:
		return GL_POLYGON;

		case VertexDrawMode::Triangles:
		default:
		return GL_TRIANGLES;
	}
}


/*
	Graphics API
*/

void bind_vertex_attributes(const VertexDeclaration &vertex_declaration, int vao_handle, int vbo_handle, int vbo_offset) noexcept
{
	vertex_array_object::detail::bind_vertex_array_object(vao_handle);
	vertex_buffer_object::detail::bind_vertex_buffer_object(vbo_handle);
	set_vertex_attribute_pointers(vertex_declaration, vbo_offset);
	vertex_array_object::detail::bind_vertex_array_object(0);
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


void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	for (auto &vertex_element : vertex_declaration.Elements())
	{
		if (auto attribute = shader_program.GetAttribute(vertex_element.Name); attribute)
		{
			switch (vertex_element.Type)
			{
				case vertex_declaration::VertexElementType::Float1:
				attribute->Get<float>().Pointer((void*)(vbo_offset + vertex_element.Offset), vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float2:
				attribute->Get<glsl::vec2>().Pointer((void*)(vbo_offset + vertex_element.Offset), vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float3:
				attribute->Get<glsl::vec3>().Pointer((void*)(vbo_offset + vertex_element.Offset), vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float4:
				attribute->Get<glsl::vec4>().Pointer((void*)(vbo_offset + vertex_element.Offset), vertex_element.Stride);
				break;
			}
			
			glEnableVertexAttribArray(attribute->Location().value_or(-1));
		}
	}
}

void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const void *data, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	for (auto &vertex_element : vertex_declaration.Elements())
	{
		if (auto attribute = shader_program.GetAttribute(vertex_element.Name); attribute)
		{
			auto pointer = (void*)(static_cast<const std::byte*>(data) + vertex_element.Offset);

			switch (vertex_element.Type)
			{
				case vertex_declaration::VertexElementType::Float1:
				attribute->Get<float>().Pointer(pointer, vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float2:
				attribute->Get<glsl::vec2>().Pointer(pointer, vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float3:
				attribute->Get<glsl::vec3>().Pointer(pointer, vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float4:
				attribute->Get<glsl::vec4>().Pointer(pointer, vertex_element.Stride);
				break;
			}

			glEnableVertexAttribArray(attribute->Location().value_or(-1));
		}
	}
}

void disable_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const shaders::ShaderProgram &shader_program) noexcept
{
	for (auto &vertex_element : vertex_declaration.Elements())
	{
		if (auto attribute = shader_program.GetAttribute(vertex_element.Name); attribute)
			glDisableVertexAttribArray(attribute->Location().value_or(-1));
	}
}


void set_vertex_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	for (auto &vertex_element : vertex_declaration.Elements())
	{
		auto pointer = (void*)(vbo_offset + vertex_element.Offset);

		switch (vertex_element.Name)
		{
			case shaders::shader_layout::AttributeName::Vertex_Position:
			glVertexPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_VERTEX_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Normal:
			glNormalPointer(/*vertex_element.Components(),*/ type, vertex_element.Stride, pointer);
			glEnableClientState(GL_NORMAL_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Color:
			glColorPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_COLOR_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_TexCoord:
			glTexCoordPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			break;
		}
	}
}

void set_vertex_pointers(const VertexDeclaration &vertex_declaration, const void *data) noexcept
{
	constexpr auto type = std::is_same_v<real, double> ? GL_DOUBLE : GL_FLOAT;

	for (auto &vertex_element : vertex_declaration.Elements())
	{
		auto pointer = (void*)(static_cast<const std::byte*>(data) + vertex_element.Offset);

		switch (vertex_element.Name)
		{
			case shaders::shader_layout::AttributeName::Vertex_Position:
			glVertexPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_VERTEX_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Normal:
			glNormalPointer(/*vertex_element.Components(),*/ type, vertex_element.Stride, pointer);
			glEnableClientState(GL_NORMAL_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Color:
			glColorPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_COLOR_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_TexCoord:
			glTexCoordPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			break;
		}
	}
}

void disable_vertex_pointers(const VertexDeclaration &vertex_declaration) noexcept
{
	for (auto &vertex_element : vertex_declaration.Elements())
	{
		switch (vertex_element.Name)
		{
			case shaders::shader_layout::AttributeName::Vertex_Position:
			glDisableClientState(GL_VERTEX_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Normal:
			glDisableClientState(GL_NORMAL_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Color:
			glDisableClientState(GL_COLOR_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_TexCoord:
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			break;
		}
	}
}

} //vertex_batch::detail


VertexBatch::VertexBatch(VertexDrawMode draw_mode, VertexDeclaration vertex_declaration, const VertexData &vertex_data) noexcept :

	draw_mode_{draw_mode},
	vertex_declaration_{std::move(vertex_declaration)},
	vertex_data_{vertex_data},
	vertex_count_{vertex_data && std::ssize(vertex_declaration_.Elements()) > 0 ?
		vertex_data.Size() / vertex_declaration_.VertexSize() : 0}
{
	//Empty
}


/*
	Drawing
*/

void VertexBatch::Prepare() noexcept
{
	//Nothing to prepare
	if (vertex_count_ == 0)
		return;

	if (reload_vertex_data_)
	{
		//Send vertex data to VRAM
		if (vbo_ && *vbo_)
			vbo_->Data(vertex_data_);

		reload_vertex_data_ = false;
	}

	if (rebind_vertex_attributes_)
	{
		if (vbo_ && *vbo_)
		{
			if (!vao_)
				vao_.emplace();

			//Bind buffers and attributes
			if (vao_ && *vao_)
				vao_->Bind(vertex_declaration_, *vbo_);
		}

		rebind_vertex_attributes_ = false;
	}
}

void VertexBatch::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	//Nothing to draw
	if (vertex_count_ == 0)
		return;
	
	auto use_vbo = vbo_ && *vbo_;
	auto use_vao = shader_program && vao_ && *vao_ && use_vbo;
	auto has_all_attributes = true;

	//Use shaders
	if (shader_program && shader_program->Handle())
	{
		//Check if shader program has all attributes declared in vertex declaration
		for (auto i = 0; auto &vertex_element : vertex_declaration_.Elements())
		{
			if (auto attribute = shader_program->GetAttribute(vertex_element.Name))
			{
				has_all_attributes &= !!attribute;
				use_vao &= attribute->Location().value_or(-1) == i++;
			}
		}

		shaders::shader_program_manager::detail::use_shader_program(*shader_program->Handle());	

		if (!use_vao)
		{
			if (has_all_attributes)
			{
				//VRAM
				if (use_vbo)
				{
					vertex_buffer_object::detail::bind_vertex_buffer_object(*vbo_->Handle());
					detail::set_vertex_attribute_pointers(vertex_declaration_, vbo_->Offset(), *shader_program);
				}
				else //RAM
					detail::set_vertex_attribute_pointers(vertex_declaration_, vertex_data_.Pointer(), *shader_program);
			}
			else
			{
				//VRAM
				if (use_vbo)
				{
					vertex_buffer_object::detail::bind_vertex_buffer_object(*vbo_->Handle());
					detail::set_vertex_pointers(vertex_declaration_, vbo_->Offset());
				}
				else //RAM
					detail::set_vertex_pointers(vertex_declaration_, vertex_data_.Pointer());
			}

			shader_program->Owner()->SendAttributeValues(*shader_program);
		}
	}
	else //Fixed-function pipeline
	{
		if (!use_vao)
		{
			//VRAM
			if (use_vbo)
			{
				vertex_buffer_object::detail::bind_vertex_buffer_object(*vbo_->Handle());
				detail::set_vertex_pointers(vertex_declaration_, vbo_->Offset());
			}
			else //RAM
				detail::set_vertex_pointers(vertex_declaration_, vertex_data_.Pointer());
		}
	}


	if (use_vao)
		vertex_array_object::detail::bind_vertex_array_object(*vao_->Handle());

	glDrawArrays(detail::vertex_draw_mode_to_gl_draw_mode(draw_mode_), 0, vertex_count_);

	if (use_vao)
		vertex_array_object::detail::bind_vertex_array_object(0);


	//Shaders
	if (shader_program && shader_program->Handle())
	{
		if (!use_vao)
		{
			if (has_all_attributes)
				detail::disable_vertex_attribute_pointers(vertex_declaration_, *shader_program);
			else
				detail::disable_vertex_pointers(vertex_declaration_);
		}

		shaders::shader_program_manager::detail::use_shader_program(0);
	}
	else //Fixed-function pipeline
	{
		if (!use_vao)
		{
			detail::disable_vertex_pointers(vertex_declaration_);

			//VRAM
			if (use_vbo)
				vertex_buffer_object::detail::bind_vertex_buffer_object(0);
		}
	}
}

} //ion::graphics::render::vertex