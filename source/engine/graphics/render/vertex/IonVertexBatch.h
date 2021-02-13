/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexBatch.h
-------------------------------------------
*/

#ifndef ION_VERTEX_BATCH_H
#define ION_VERTEX_BATCH_H

#include <optional>

#include "IonVertexArrayObject.h"
#include "IonVertexBufferObject.h"
#include "IonVertexData.h"
#include "IonVertexDeclaration.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "types/IonTypes.h"

namespace ion::graphics::render::vertex
{
	namespace vertex_batch
	{
		enum class VertexDrawMode
		{
			Points,
			Lines,
			LineLoop,
			LineStrip,
			Triangles,
			TriangleFan,
			TriangleStrip,
			Quads,
			Polygon
		};


		namespace detail
		{
			int vertex_draw_mode_to_gl_draw_mode(VertexDrawMode draw_mode) noexcept;
			int get_vertex_count(const VertexDeclaration &vertex_declaration, const VertexData &vertex_data) noexcept;

			/*
				Graphics API
			*/

			void bind_vertex_attributes(const VertexDeclaration &vertex_declaration, int vao_handle, int vbo_handle, int vbo_offset) noexcept;

			void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset) noexcept;
			void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const void *data) noexcept;

			void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset, shaders::ShaderProgram &shader_program) noexcept;			
			void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const void *data, shaders::ShaderProgram &shader_program) noexcept;
			void disable_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const shaders::ShaderProgram &shader_program) noexcept;

			void set_vertex_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset) noexcept;
			void set_vertex_pointers(const VertexDeclaration &vertex_declaration, const void *data) noexcept;
			void disable_vertex_pointers(const VertexDeclaration &vertex_declaration) noexcept;
		} //detail
	} //vertex_batch


	class VertexBatch final
	{
		private:

			vertex_batch::VertexDrawMode draw_mode_;
			VertexDeclaration vertex_declaration_;
			VertexData vertex_data_;
			int vertex_count_ = 0;

			std::optional<VertexArrayObject> vao_;
			std::optional<VertexBufferView> vbo_;

			bool reload_vertex_data_ = false;
			bool rebind_vertex_attributes_ = false;

		public:

			//Construct a new vertex batch with the given draw mode and vertex declaration
			VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration) noexcept;

			//Construct a new vertex batch with the given draw mode, vertex declaration and vertex data
			VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration, const VertexData &vertex_data) noexcept;


			/*
				Modifiers
			*/

			//Sets the draw mode of this vertex batch to the given mode
			inline void DrawMode(vertex_batch::VertexDrawMode draw_mode) noexcept
			{
				draw_mode_ = draw_mode;
			}

			//Sets the vertex declaration of this vertex batch to the given declaration
			inline void Declaration(VertexDeclaration vertex_declaration) noexcept
			{
				vertex_declaration_ = std::move(vertex_declaration);
				vertex_count_ = vertex_batch::detail::get_vertex_count(vertex_declaration, vertex_data_);
				rebind_vertex_attributes_ = vertex_count_ > 0;
			}

			//Sets the vertex data of this vertex batch to the given data
			inline void Data(const VertexData &vertex_data) noexcept
			{
				if (vertex_data_ != vertex_data)
				{
					vertex_data_ = vertex_data;
					vertex_count_ = vertex_batch::detail::get_vertex_count(vertex_declaration_, vertex_data);
					reload_vertex_data_ = vertex_count_ > 0;
				}
			}


			//Sets the vertex buffer of this vertex batch to the given VBO
			inline void VertexBuffer(std::optional<VertexBufferView> vertex_buffer) noexcept
			{
				if (vbo_ != vertex_buffer)
				{
					vbo_ = vertex_buffer;
					reload_vertex_data_ = vertex_count_ > 0;
					rebind_vertex_attributes_ = reload_vertex_data_;
				}
			}


			/*
				Observers
			*/

			//Returns the draw mode for this vertex batch
			[[nodiscard]] inline auto DrawMode() const noexcept
			{
				return draw_mode_;
			}

			//Returns the vertex declaration for this vertex batch
			//Returns nullptr if no vertex data is available
			[[nodiscard]] inline auto Declaration() const noexcept
			{
				return vertex_declaration_;
			}

			//Returns the vertex data for this vertex batch
			[[nodiscard]] inline auto Data() const noexcept
			{
				return vertex_data_;
			}

			//Returns the vertex count of this vertex batch
			[[nodiscard]] inline auto VertexCount() const noexcept
			{
				return vertex_count_;
			}


			//Returns the vertex array that this vertex batch is using for buffer and attribute bindings
			//Returns nullopt if no vertex array is available
			[[nodiscard]] inline auto& VertexArray() const noexcept
			{
				return vao_;
			}

			//Returns the vertex buffer that this vertex batch is using
			//Returns nullopt if no vertex buffer is available
			[[nodiscard]] inline auto VertexBuffer() const noexcept
			{
				return vbo_;
			}


			/*
				Drawing
			*/

			//Prepare vertex batch by passing vertices to vertex buffer and setting up attribute bindings
			//This is called once regardless of passes
			void Prepare() noexcept;

			//Draw all of the vertices in this vertex batch with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept;
	};
} //ion::graphics::render::vertex

#endif