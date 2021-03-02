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
#include "IonVertexDataView.h"
#include "IonVertexDeclaration.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace materials
	{
		class Material;
	}

	namespace shaders
	{
		class ShaderProgram;
	}
}

namespace ion::graphics::render::vertex
{
	using namespace types::type_literals;

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
			int get_vertex_count(const VertexDeclaration &vertex_declaration, const VertexDataView &vertex_data) noexcept;


			/*
				Graphics API
			*/

			void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset, shaders::ShaderProgram &shader_program) noexcept;			
			void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const void *data, shaders::ShaderProgram &shader_program) noexcept;
			void disable_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const shaders::ShaderProgram &shader_program) noexcept;

			void set_vertex_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset) noexcept;
			void set_vertex_pointers(const VertexDeclaration &vertex_declaration, const void *data) noexcept;
			void disable_vertex_pointers(const VertexDeclaration &vertex_declaration) noexcept;

			void bind_texture(int texture_handle) noexcept;
			void bind_texture(int texture_handle, int texture_unit) noexcept;
			
			void set_has_material_uniform(materials::Material *material, shaders::ShaderProgram &shader_program) noexcept;
			void set_material_uniforms(materials::Material &material, duration time, shaders::ShaderProgram &shader_program) noexcept;
		} //detail
	} //vertex_batch


	class VertexBatch final
	{
		private:

			vertex_batch::VertexDrawMode draw_mode_;
			VertexDeclaration vertex_declaration_;
			VertexDataView vertex_data_;
			NonOwningPtr<materials::Material> material_;
			int vertex_count_ = 0;

			bool use_vao_ = true;
			std::optional<VertexArrayObject> vao_;
			std::optional<VertexBufferView> vbo_;

			duration time_ = 0.0_sec;
			bool reload_vertex_data_ = false;
			bool rebind_vertex_attributes_ = false;

		public:

			//Construct a new vertex batch with the given draw mode and vertex declaration
			VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration) noexcept;

			//Construct a new vertex batch with the given draw mode, vertex declaration, vertex data and material (optional)
			VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration, const VertexDataView &vertex_data,
				NonOwningPtr<materials::Material> material = nullptr) noexcept;


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
				rebind_vertex_attributes_ = use_vao_;
			}

			//Sets the vertex data of this vertex batch to the given data
			inline void VertexData(const VertexDataView &vertex_data, bool reload_data = true) noexcept
			{
				if (vertex_data_ != vertex_data)
				{
					vertex_data_ = vertex_data;
					vertex_count_ = vertex_batch::detail::get_vertex_count(vertex_declaration_, vertex_data);
					reload_vertex_data_ = reload_data;
				}
			}

			//Sets the material used by this vertex batch to the given material
			inline void BatchMaterial(NonOwningPtr<materials::Material> material) noexcept
			{
				material_ = material;
			}


			//Sets if this vertex batch should use vertex array or not for vertex buffer and attribute bindings
			inline void UseVertexArray(bool use) noexcept
			{
				if (use_vao_ != use)
				{
					use_vao_ = use;
					rebind_vertex_attributes_ = use_vao_;
					
					if (!use_vao_ && vao_)
						vao_.reset();
				}
			}

			//Sets the vertex buffer of this vertex batch to the given vertex buffer
			inline void VertexBuffer(std::optional<VertexBufferView> vertex_buffer, bool reload_data = true) noexcept
			{
				if (vbo_ != vertex_buffer)
				{
					vbo_ = vertex_buffer;
					reload_vertex_data_ = reload_data;
					rebind_vertex_attributes_ = use_vao_;
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
			[[nodiscard]] inline auto& Declaration() const noexcept
			{
				return vertex_declaration_;
			}

			//Returns the vertex data for this vertex batch
			[[nodiscard]] inline auto VertexData() const noexcept
			{
				return vertex_data_;
			}

			//Returns the material that this vertex batch is using
			//Returns nullptr if no material is available
			[[nodiscard]] inline auto BatchMaterial() const noexcept
			{
				return material_;
			}

			//Returns the vertex count of this vertex batch
			[[nodiscard]] inline auto VertexCount() const noexcept
			{
				return vertex_count_;
			}


			//Returns true if this vertex batch is using vertex array for vertex buffer and attribute bindings
			[[nodiscard]] inline auto UseVertexArray() const noexcept
			{
				return use_vao_;
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
				Preparing / drawing
			*/

			//Force reloading of vertex data by sending data from RAM to VRAM
			inline void ReloadData() noexcept
			{
				reload_vertex_data_ = true;
			}


			//Prepare vertex batch by passing vertices to vertex buffer and setting up attribute bindings
			//This is called once regardless of passes
			void Prepare() noexcept;

			//Draw all of the vertices in this vertex batch with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept;


			/*
				Elapse time
			*/

			//Elapse the total time for this vertex batch by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;
	};
} //ion::graphics::render::vertex

#endif