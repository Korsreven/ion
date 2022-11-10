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
#include <tuple>
#include <variant>

#include "IonVertexArrayObject.h"
#include "IonVertexBufferObject.h"
#include "IonVertexDataView.h"
#include "IonVertexDeclaration.h"
#include "graphics/textures/IonTexture.h"
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

	namespace textures
	{
		class Animation;
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
			using texture_type = std::variant<
				std::monostate,
				NonOwningPtr<textures::Animation>,
				NonOwningPtr<textures::Texture>,
				textures::texture::TextureHandle>;


			int vertex_draw_mode_to_gl_draw_mode(VertexDrawMode draw_mode) noexcept;
			int get_vertex_count(const VertexDeclaration &vertex_declaration, const VertexDataView &vertex_data) noexcept;

			std::tuple<NonOwningPtr<textures::Animation>, NonOwningPtr<textures::Texture>, std::optional<textures::texture::TextureHandle>>
				get_textures(const texture_type &some_texture) noexcept;
			std::optional<textures::texture::TextureHandle> get_texture_handle(const texture_type &some_texture, duration time) noexcept;


			/**
				@name Graphics API
				@{
			*/

			void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset, shaders::ShaderProgram &shader_program) noexcept;
			void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const void *data, shaders::ShaderProgram &shader_program) noexcept;
			void disable_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const shaders::ShaderProgram &shader_program) noexcept;

			void set_vertex_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset) noexcept;
			void set_vertex_pointers(const VertexDeclaration &vertex_declaration, const void *data) noexcept;
			void disable_vertex_pointers(const VertexDeclaration &vertex_declaration) noexcept;

			void bind_texture(textures::texture::TextureHandle texture_handle) noexcept;
			void bind_texture(textures::texture::TextureHandle texture_handle, int texture_unit) noexcept;
			
			void set_light_uniforms(shaders::ShaderProgram &shader_program) noexcept;
			void set_material_uniforms(materials::Material *material, duration time, shaders::ShaderProgram &shader_program) noexcept;
			void set_texture_uniforms(texture_type &some_texture, duration time, shaders::ShaderProgram &shader_program) noexcept;

			///@}
		} //detail
	} //vertex_batch


	///@brief A class representing a batch of vertices that can be drawn in a single call
	///@details Vertices are drawn either from a VBO or directly from memory with or without a shader program
	class VertexBatch final
	{
		private:

			vertex_batch::VertexDrawMode draw_mode_;
			VertexDeclaration vertex_declaration_;
			VertexDataView vertex_data_;
			NonOwningPtr<materials::Material> material_;
			vertex_batch::detail::texture_type texture_;
			int vertex_count_ = 0;

			bool use_vao_ = true;
			std::optional<VertexArrayObject> vao_;
			std::optional<VertexBufferView> vbo_;

			duration time_ = 0.0_sec;
			bool reload_vertex_data_ = false;
			bool rebind_vertex_attributes_ = false;

		public:

			///@brief Constructs a new vertex batch with the given draw mode and vertex declaration
			VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration) noexcept;

			///@brief Constructs a new vertex batch with the given draw mode, vertex declaration, vertex data and material (optional)
			VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration,
				const VertexDataView &vertex_data, NonOwningPtr<materials::Material> material = nullptr) noexcept;

			///@brief Constructs a new vertex batch with the given draw mode, vertex declaration, vertex data and animation
			VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration,
				const VertexDataView &vertex_data, NonOwningPtr<textures::Animation> animation) noexcept;

			///@brief Constructs a new vertex batch with the given draw mode, vertex declaration, vertex data and texture
			VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration,
				const VertexDataView &vertex_data, NonOwningPtr<textures::Texture> texture) noexcept;

			///@brief Constructs a new vertex batch with the given draw mode, vertex declaration, vertex data and a texture handle
			VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration,
				const VertexDataView &vertex_data, textures::texture::TextureHandle texture_handle) noexcept;


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the draw mode of this vertex batch to the given mode
			inline void DrawMode(vertex_batch::VertexDrawMode draw_mode) noexcept
			{
				draw_mode_ = draw_mode;
			}

			///@brief Sets the vertex declaration of this vertex batch to the given declaration
			inline void Declaration(VertexDeclaration vertex_declaration) noexcept
			{
				vertex_declaration_ = std::move(vertex_declaration);
				vertex_count_ = vertex_batch::detail::get_vertex_count(vertex_declaration_, vertex_data_);
				rebind_vertex_attributes_ = use_vao_;
			}

			///@brief Sets the vertex data of this vertex batch to the given data
			inline void VertexData(const VertexDataView &vertex_data, bool reload_data = true) noexcept
			{
				if (vertex_data_ != vertex_data)
				{
					vertex_data_ = vertex_data;
					vertex_count_ = vertex_batch::detail::get_vertex_count(vertex_declaration_, vertex_data);
				}

				reload_vertex_data_ |= reload_data;
			}

			///@brief Sets the material used by this vertex batch to the given material
			inline void BatchMaterial(NonOwningPtr<materials::Material> material) noexcept
			{
				material_ = material;
			}

			///@brief Sets the texture used by this vertex batch to the given animation
			inline void BatchTexture(NonOwningPtr<textures::Animation> animation) noexcept
			{
				texture_ = animation;
			}

			///@brief Sets the texture used by this vertex batch to the given texture
			inline void BatchTexture(NonOwningPtr<textures::Texture> texture) noexcept
			{
				texture_ = texture;
			}

			///@brief Sets the texture used by this vertex batch to the given texture handle
			inline void BatchTexture(textures::texture::TextureHandle texture_handle) noexcept
			{
				texture_ = texture_handle;
			}

			///@brief Detaches the texture used by this vertex batch
			inline void BatchTexture(std::nullptr_t) noexcept
			{
				texture_ = std::monostate{};
			}

			///@brief Detaches the texture used by this vertex batch
			inline void BatchTexture(std::nullopt_t) noexcept
			{
				BatchTexture(nullptr);
			}


			///@brief Sets if this vertex batch should use vertex array or not for vertex buffer and attribute bindings
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

			///@brief Sets the vertex buffer of this vertex batch to the given vertex buffer
			inline void VertexBuffer(std::optional<VertexBufferView> vertex_buffer, bool reload_data = true) noexcept
			{
				if (vbo_ != vertex_buffer)
				{
					vbo_ = vertex_buffer;
					rebind_vertex_attributes_ = use_vao_;
				}

				reload_vertex_data_ |= reload_data;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the draw mode for this vertex batch
			[[nodiscard]] inline auto DrawMode() const noexcept
			{
				return draw_mode_;
			}

			///@brief Returns the vertex declaration for this vertex batch
			///@details Returns nullptr if no vertex data is available
			[[nodiscard]] inline auto& Declaration() const noexcept
			{
				return vertex_declaration_;
			}

			///@brief Returns the vertex data for this vertex batch
			[[nodiscard]] inline auto VertexData() const noexcept
			{
				return vertex_data_;
			}

			///@brief Returns the material that this vertex batch is using
			///@details Returns nullptr if no material is available
			[[nodiscard]] inline auto BatchMaterial() const noexcept
			{
				return material_;
			}

			///@brief Returns the attached texture as a tuple of either animation, texture or a texture handle that this vertex batch is using
			///@details Returns {nullptr, nullptr, nullopt} if no texture is available
			[[nodiscard]] inline auto BatchTexture() const noexcept
			{
				return vertex_batch::detail::get_textures(texture_);
			}

			///@brief Returns the vertex count of this vertex batch
			[[nodiscard]] inline auto VertexCount() const noexcept
			{
				return vertex_count_;
			}


			///@brief Returns true if this vertex batch is using vertex array for vertex buffer and attribute bindings
			[[nodiscard]] inline auto UseVertexArray() const noexcept
			{
				return use_vao_;
			}

			///@brief Returns the vertex array that this vertex batch is using for buffer and attribute bindings
			///@details Returns nullopt if no vertex array is available
			[[nodiscard]] inline auto& VertexArray() const noexcept
			{
				return vao_;
			}

			///@brief Returns the vertex buffer that this vertex batch is using
			///@details Returns nullopt if no vertex buffer is available
			[[nodiscard]] inline auto VertexBuffer() const noexcept
			{
				return vbo_;
			}

			///@}

			/**
				@name Preparing/drawing
				@{
			*/

			///@brief Force reloading of vertex data by sending data from RAM to VRAM
			inline void ReloadData() noexcept
			{
				reload_vertex_data_ = true;
			}


			///@brief Prepares vertex batch by passing vertices to vertex buffer and setting up attribute bindings
			///@details This function is typically called each frame
			void Prepare() noexcept;

			///@brief Draws all of the vertices in this vertex batch with the given shader program (optional)
			///@details This function is typically called each frame, once for each render pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this vertex batch by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;

			///@}
	};
} //ion::graphics::render::vertex

#endif