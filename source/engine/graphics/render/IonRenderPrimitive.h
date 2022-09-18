/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderPrimitive.h
-------------------------------------------
*/

#ifndef ION_RENDER_PRIMITIVE_H
#define ION_RENDER_PRIMITIVE_H

#include <cstring>
#include <optional>
#include <vector>

#include "IonPass.h"
#include "graphics/textures/IonTexture.h"
#include "graphics/utilities/IonMatrix4.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"
#include "vertex/IonVertexBatch.h"
#include "vertex/IonVertexDataView.h"
#include "vertex/IonVertexDeclaration.h"

namespace ion::graphics::materials
{
	class Material; //Forward declaration
}

namespace ion::graphics::render
{
	class Renderer; //Forward declaration

	using namespace graphics::utilities;
	using namespace types::type_literals;

	namespace render_primitive
	{
		using vertex_data = std::vector<real>;
		using render_passes = std::vector<Pass>;

		namespace detail
		{
			struct vertex_metrics
			{
				int position_components = 0;
				int position_offset = 0;
				int position_stride = 0;
			};


			int get_vertex_count(const vertex::VertexDeclaration &vertex_declaration, const vertex::VertexDataView &vertex_data) noexcept;
			vertex_metrics get_vertex_metrics(const vertex::VertexDeclaration &vertex_declaration) noexcept;

			void transform_positions(const vertex_metrics &metrics, const Matrix4 &model_matrix, vertex_data &data) noexcept;
			real get_position_z(const vertex_metrics &metrics, const vertex_data &data) noexcept;

			bool are_passes_equal(const render_passes &passes, const render_passes &passes2) noexcept;
		} //detail
	} //render_primitive


	class RenderPrimitive
	{
		private:

			vertex::vertex_batch::VertexDrawMode draw_mode_;
			vertex::VertexDeclaration vertex_declaration_;
			render_primitive::detail::vertex_metrics vertex_metrics_;

			render_primitive::vertex_data local_vertex_data_; //Local space
			render_primitive::vertex_data world_vertex_data_; //World space
			Matrix4 current_model_matrix_;
			Matrix4 applied_model_matrix_;
			real world_z_ = 0.0_r;

			render_primitive::render_passes passes_;
			NonOwningPtr<materials::Material> current_material_;
			materials::Material *applied_material_ = nullptr;
			std::optional<textures::texture::TextureHandle> texture_handle_;
			real point_size_ = 1.0_r;
			real line_thickness_ = 1.0_r;
			bool wire_frame_ = false;
			bool visible_ = false;

			bool local_data_changed_ = false;
			bool world_data_changed_ = false;
			bool need_refresh_ = false;
			Renderer *parent_renderer_ = nullptr;

		protected:

			void UpdateWorldVertexData();
			void UpdateWorldZ() noexcept;

		public:

			//Construct a new render primitive with the given draw mode and vertex declaration
			RenderPrimitive(vertex::vertex_batch::VertexDrawMode draw_mode, vertex::VertexDeclaration vertex_declaration) noexcept;

			//Virtual destructor
			virtual ~RenderPrimitive() noexcept;


			/*
				Modifiers
			*/

			//
			inline void LocalVertexData(render_primitive::vertex_data data) noexcept
			{
				if (std::size(local_vertex_data_) != std::size(data))
					need_refresh_ = visible_;

				local_vertex_data_ = std::move(data);
				local_data_changed_ = true;
				world_data_changed_ = false; //Discard world changes
			}

			//
			inline void LocalVertexData(render_primitive::vertex_data data, const Matrix4 &model_matrix) noexcept
			{
				LocalVertexData(std::move(data));
				ModelMatrix(model_matrix);
			}

			//
			inline void WorldVertexData(render_primitive::vertex_data data, const Matrix4 &applied_model_matrix) noexcept
			{
				if (std::size(world_vertex_data_) != std::size(data))
					need_refresh_ = visible_;

				world_vertex_data_ = std::move(data);
				local_vertex_data_ = world_vertex_data_;
				ModelMatrix(applied_model_matrix);
				applied_model_matrix_ = applied_model_matrix;
				world_z_ = render_primitive::detail::get_position_z(vertex_metrics_, world_vertex_data_);
				local_data_changed_ = false; //Discard local changes
				world_data_changed_ = true;
			}
		
			//
			inline void ModelMatrix(const Matrix4 &model_matrix) noexcept
			{
				current_model_matrix_ = model_matrix;
			}


			//
			inline void RenderPasses(render_primitive::render_passes passes) noexcept
			{
				if (!render_primitive::detail::are_passes_equal(passes_, passes))
				{
					passes_ = std::move(passes);
					need_refresh_ = visible_;
				}
			}
		
			//
			inline void RenderMaterial(NonOwningPtr<materials::Material> material) noexcept
			{
				if (current_material_ != material)
				{
					current_material_ = material;
					applied_material_ = material.get();
					need_refresh_ = visible_;
				}
			}

			//
			inline void RenderTexture(std::optional<textures::texture::TextureHandle> texture_handle) noexcept
			{
				if (texture_handle_ != texture_handle)
				{
					texture_handle_ = texture_handle;
					need_refresh_ = visible_;
				}
			}

			//
			inline void PointSize(real size) noexcept
			{
				if (point_size_ != size)
				{
					point_size_ = size;
					need_refresh_ = visible_;
				}
			}

			//
			inline void LineThickness(real thickness) noexcept
			{
				if (line_thickness_ != thickness)
				{
					line_thickness_ = thickness;
					need_refresh_ = visible_;
				}
			}

			//
			inline void WireFrame(bool enable) noexcept
			{
				if (wire_frame_ != enable)
				{
					wire_frame_ = enable;
					need_refresh_ = visible_;
				}
			}

			//
			inline void Visible(bool visible) noexcept
			{
				if (visible_ != visible)
				{
					visible_ = visible;
					need_refresh_ = true;
				}
			}

			//
			inline void ParentRenderer(Renderer *parent_renderer) noexcept
			{
				parent_renderer_ = parent_renderer;
			}


			/*
				Observers
			*/

			//
			[[nodiscard]] inline auto& DrawMode() const noexcept
			{
				return draw_mode_;
			}

			//
			[[nodiscard]] inline auto& VertexDataDeclaration() const noexcept
			{
				return vertex_declaration_;
			}


			//
			[[nodiscard]] inline auto& LocalVertexData() const noexcept
			{
				return local_vertex_data_;
			}

			//
			[[nodiscard]] inline auto& WorldVertexData() const noexcept
			{
				return world_vertex_data_;
			}

			//
			[[nodiscard]] inline auto& CurrentModelMatrix() const noexcept
			{
				return current_model_matrix_;
			}

			//
			[[nodiscard]] inline auto& AppliedModelMatrix() const noexcept
			{
				return applied_model_matrix_;
			}

			//
			[[nodiscard]] inline auto WorldZ() const noexcept
			{
				return world_z_;
			}

			//
			[[nodiscard]] inline auto VertexCount() const noexcept
			{
				return render_primitive::detail::get_vertex_count(vertex_declaration_, local_vertex_data_);
			}

			//
			[[nodiscard]] inline auto VertexDataSize() const noexcept
			{
				return std::ssize(local_vertex_data_);
			}


			//
			[[nodiscard]] inline auto& RenderPasses() const noexcept
			{
				return passes_;
			}

			//
			[[nodiscard]] inline auto RenderMaterial() const noexcept
			{
				return current_material_;
			}

			//
			[[nodiscard]] inline auto RenderTexture() const noexcept
			{
				return texture_handle_;
			}

			//
			[[nodiscard]] inline auto PointSize() const noexcept
			{
				return point_size_;
			}

			//
			[[nodiscard]] inline auto LineThickness() const noexcept
			{
				return line_thickness_;
			}

			//
			[[nodiscard]] inline auto WireFrame() const noexcept
			{
				return wire_frame_;
			}

			//
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}

			//
			[[nodiscard]] inline auto ParentRenderer() const noexcept
			{
				return parent_renderer_;
			}


			//		
			[[nodiscard]] inline auto IsGroupable(const RenderPrimitive &primitive) const noexcept
			{
				return draw_mode_ == primitive.draw_mode_ && world_z_ == primitive.world_z_ &&
					   current_material_ == primitive.current_material_ && texture_handle_ == primitive.texture_handle_ &&
					   line_thickness_ == primitive.line_thickness_ && wire_frame_ == primitive.wire_frame_ &&
					   //Check slowest equalities last
					   vertex_declaration_ == primitive.vertex_declaration_ &&
					   render_primitive::detail::are_passes_equal(passes_, primitive.passes_);
			}


			/*
				Vertex batch
			*/

			//
			[[nodiscard]] vertex::VertexBatch MakeVertexBatch() const noexcept;
		
		
			/*
				Refreshing / Preparing
			*/

			//
			void Refresh();

			//
			[[nodiscard]] bool Prepare();
	};

} //ion::graphics::render

#endif