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

#include <optional>
#include <vector>

#include "IonPass.h"
#include "graphics/textures/IonTexture.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
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
		using VertexContainer = std::vector<real>;
		using Passes = std::vector<Pass>;


		namespace detail
		{
			struct vertex_metrics
			{
				int position_components = 0;
				int position_offset = 0;
				int position_stride = 0;

				int color_components = 0;
				int color_offset = 0;
				int color_stride = 0;
			};


			int get_vertex_count(const vertex::VertexDeclaration &vertex_declaration, const vertex::VertexDataView &data_view) noexcept;
			vertex_metrics get_vertex_metrics(const vertex::VertexDeclaration &vertex_declaration) noexcept;

			void transform_positions(const vertex_metrics &metrics, const Matrix4 &model_matrix, VertexContainer &data) noexcept;
			void apply_color(const vertex_metrics &metrics, const Color &color, VertexContainer &data) noexcept;
			void apply_opacity(const vertex_metrics &metrics, real opacity, VertexContainer &data) noexcept;
			void apply_opacity(const vertex_metrics &metrics, real opacity, const VertexContainer &source_data, VertexContainer &data) noexcept;

			real get_position_z(const vertex_metrics &metrics, const VertexContainer &data) noexcept;
			Color get_color(const vertex_metrics &metrics, const VertexContainer &data) noexcept;
			real get_opacity(const vertex_metrics &metrics, const VertexContainer &data) noexcept;
			Aabb get_aabb(const vertex_metrics &metrics, const VertexContainer &data) noexcept;

			bool all_passes_equal(const Passes &passes, const Passes &passes2) noexcept;
		} //detail
	} //render_primitive


	//A class representing a primitive with some vertex data and different render properties
	//The properties determines what other primitives it can be grouped with and how it is drawn
	class RenderPrimitive
	{
		private:

			vertex::vertex_batch::VertexDrawMode draw_mode_;
			vertex::VertexDeclaration vertex_declaration_;
			render_primitive::detail::vertex_metrics vertex_metrics_;

			render_primitive::VertexContainer vertex_data_; //Local space
			render_primitive::VertexContainer world_vertex_data_; //World space
			Matrix4 model_matrix_;
			real world_z_ = 0.0_r;
			Aabb aabb_;

			render_primitive::Passes passes_;
			NonOwningPtr<materials::Material> material_;
			materials::Material *applied_material_ = nullptr;
			std::optional<textures::texture::TextureHandle> texture_handle_;

			real opacity_ = 1.0_r;
			real point_size_ = 1.0_r;
			real line_thickness_ = 1.0_r;
			bool wire_frame_ = false;
			bool point_sprite_ = false;
			bool visible_ = false;
			bool world_visible_ = false;
			Renderer *parent_renderer_ = nullptr;

			bool data_changed_ = false;
			bool world_data_changed_ = false;
			bool model_matrix_changed_ = false;
			bool opacity_changed_ = false;
			bool need_refresh_ = false;


			void UpdateWorldVertexData();
			void UpdateWorldZ() noexcept;

		protected:

			//Returns the vertex data of this render primitive
			[[nodiscard]] inline auto& VertexData() noexcept
			{
				return vertex_data_;
			}


			/*
				Events
			*/

			virtual void VertexDataChanged() noexcept;
			virtual void ModelMatrixChanged() noexcept;

			virtual void PassesChanged() noexcept;
			virtual void MaterialChanged() noexcept;
			virtual void TextureChanged() noexcept;

			virtual void BaseColorChanged() noexcept;
			virtual void BaseOpacityChanged() noexcept;
			virtual void OpacityChanged() noexcept;

			virtual void PointSizeChanged() noexcept;
			virtual void LineThicknessChanged() noexcept;
			virtual void WireFrameChanged() noexcept;
			virtual void PointSpriteChanged() noexcept;
			virtual void VisibleChanged() noexcept;
			virtual void RendererChanged() noexcept;

		public:

			//Construct a new render primitive with the given draw mode, vertex declaration and visibility
			RenderPrimitive(vertex::vertex_batch::VertexDrawMode draw_mode, vertex::VertexDeclaration vertex_declaration,
				bool visible = true) noexcept;

			//Virtual destructor
			virtual ~RenderPrimitive() noexcept;


			/*
				Modifiers
			*/

			//Sets the vertex data of this primitive to the given data
			void VertexData(render_primitive::VertexContainer data);

			//Appends the given vertex data to the vertex data of this primitive
			void AppendVertexData(const render_primitive::VertexContainer &data);

			//Sets the model matrix of this primitive to the given matrix
			void ModelMatrix(const Matrix4 &model_matrix) noexcept;


			//Sets the passes of this primitive to the given passes
			inline void RenderPasses(render_primitive::Passes passes)
			{
				if (!render_primitive::detail::all_passes_equal(passes_, passes))
				{
					passes_ = std::move(passes);
					need_refresh_ |= world_visible_;
					PassesChanged();
				}
			}
		
			//Sets the material of this primitive to the given material
			inline void RenderMaterial(NonOwningPtr<materials::Material> material) noexcept
			{
				if (material_ != material)
				{
					material_ = material;
					applied_material_ = material.get();
					need_refresh_ |= world_visible_;
					MaterialChanged();
				}
			}

			//Sets the texture of this primitive to the given texture handle
			inline void RenderTexture(std::optional<textures::texture::TextureHandle> texture_handle) noexcept
			{
				if (texture_handle_ != texture_handle)
				{
					texture_handle_ = texture_handle;
					need_refresh_ |= world_visible_;
					TextureChanged();
				}
			}


			//Sets the base color of this primitive to the given color
			void BaseColor(const Color &color) noexcept;

			//Sets the base opacity of this primitive to the given value
			void BaseOpacity(real opacity) noexcept;

			//Sets the opacity of this primitive to the given opacity
			inline void Opacity(real opacity) noexcept
			{
				if (opacity_ != opacity)
				{
					opacity_ = opacity;
					opacity_changed_ = true;
					OpacityChanged();
				}
			}


			//Sets the point size of this primitive to the given value
			inline void PointSize(real size) noexcept
			{
				if (point_size_ != size)
				{
					point_size_ = size;
					need_refresh_ |= world_visible_;
					PointSizeChanged();
				}
			}

			//Sets the line thickness of this primitive to the given value
			inline void LineThickness(real thickness) noexcept
			{
				if (line_thickness_ != thickness)
				{
					line_thickness_ = thickness;
					need_refresh_ |= world_visible_;
					LineThicknessChanged();
				}
			}

			//Sets whether or not this primitive has wire frame enabled
			inline void WireFrame(bool enable) noexcept
			{
				if (wire_frame_ != enable)
				{
					wire_frame_ = enable;
					need_refresh_ |= world_visible_;
					WireFrameChanged();
				}
			}

			//Sets whether or not this primitive has point sprite enabled
			inline void PointSprite(bool enable) noexcept
			{
				if (point_sprite_ != enable)
				{
					point_sprite_ = enable;
					need_refresh_ |= world_visible_;
					PointSpriteChanged();
				}
			}

			//Sets whether or not this primitive is visible
			inline void Visible(bool visible) noexcept
			{
				if (visible_ != visible)
				{
					visible_ = visible;
					VisibleChanged();
				}
			}

			//Sets whether or not this primitive is visible in world space
			inline void WorldVisible(bool visible) noexcept
			{
				if (world_visible_ != visible)
				{
					world_visible_ = visible;
					need_refresh_ = true;
				}
			}

			//Sets the parent renderer of this primitive to the given renderer
			inline void ParentRenderer(Renderer *parent_renderer) noexcept
			{
				if (parent_renderer_ != parent_renderer)
				{
					parent_renderer_ = parent_renderer;
					RendererChanged();
				}
			}


			/*
				Observers
			*/

			//Returns the draw mode of this render primitive
			[[nodiscard]] inline auto& DrawMode() const noexcept
			{
				return draw_mode_;
			}

			//Returns the vertex declaration of this render primitive
			[[nodiscard]] inline auto& VertexDataDeclaration() const noexcept
			{
				return vertex_declaration_;
			}


			//Returns the vertex data of this render primitive
			[[nodiscard]] inline auto& VertexData() const noexcept
			{
				return vertex_data_;
			}

			//Returns the vertex data of this render primitive in world space
			[[nodiscard]] inline auto& WorldVertexData() const noexcept
			{
				return world_vertex_data_;
			}

			//Returns the model matrix of this render primitive
			[[nodiscard]] inline auto& ModelMatrix() const noexcept
			{
				return model_matrix_;
			}


			//Returns the vertex count of this render primitive
			[[nodiscard]] inline auto VertexCount() const noexcept
			{
				return render_primitive::detail::get_vertex_count(vertex_declaration_, vertex_data_);
			}

			//Returns the vertex data size of this render primitive
			[[nodiscard]] inline auto VertexDataSize() const noexcept
			{
				return std::ssize(vertex_data_);
			}

			//Returns the z of this render primitive
			[[nodiscard]] inline auto Z() const noexcept
			{
				return render_primitive::detail::get_position_z(vertex_metrics_, vertex_data_);
			}

			//Returns the z of this render primitive in world space
			[[nodiscard]] inline auto WorldZ() const noexcept
			{
				return world_z_;
			}

			//Returns the axis aligned bounding box of this render primitive
			[[nodiscard]] inline auto& AxisAlignedBoundingBox() const noexcept
			{
				return aabb_;
			}


			//Returns the passes of this render primitive
			[[nodiscard]] inline auto& RenderPasses() const noexcept
			{
				return passes_;
			}

			//Returns the material of this render primitive
			[[nodiscard]] inline auto RenderMaterial() const noexcept
			{
				return material_;
			}

			//Returns the texture of this render primitive
			[[nodiscard]] inline auto RenderTexture() const noexcept
			{
				return texture_handle_;
			}


			//Returns the base color of this render primitive
			[[nodiscard]] inline auto BaseColor() const noexcept
			{
				return render_primitive::detail::get_color(vertex_metrics_, vertex_data_);
			}

			//Returns the base opacity of this render primitive
			[[nodiscard]] inline auto BaseOpacity() const noexcept
			{
				return render_primitive::detail::get_opacity(vertex_metrics_, vertex_data_);
			}

			//Returns the opacity of this render primitive
			[[nodiscard]] inline auto Opacity() const noexcept
			{
				return opacity_;
			}


			//Returns the point size of this render primitive
			[[nodiscard]] inline auto PointSize() const noexcept
			{
				return point_size_;
			}

			//Returns the line thickness of this render primitive
			[[nodiscard]] inline auto LineThickness() const noexcept
			{
				return line_thickness_;
			}

			//Returns whether or not this primitive has wire frame enabled
			[[nodiscard]] inline auto WireFrame() const noexcept
			{
				return wire_frame_;
			}

			//Returns whether or not this primitive has point sprite enabled
			[[nodiscard]] inline auto PointSprite() const noexcept
			{
				return point_sprite_;
			}

			//Returns whether or not this primitive is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}

			//Returns whether or not this primitive is visible in world space
			[[nodiscard]] inline auto WorldVisible() const noexcept
			{
				return world_visible_;
			}

			//Returns the parent renderer of this render primitive
			[[nodiscard]] inline auto ParentRenderer() const noexcept
			{
				return parent_renderer_;
			}


			//Returns whether or not this primitive is groupable with the given primitive
			[[nodiscard]] bool IsGroupable(const RenderPrimitive &primitive) const noexcept;


			/*
				Vertex batch
			*/

			//Returns a vertex batch that can render this primitive
			[[nodiscard]] vertex::VertexBatch MakeVertexBatch() const noexcept;
		
		
			/*
				Updating
			*/

			//Refresh render primitive, by regrouping it in the renderer
			//This function is typically called each frame
			void Refresh();

			//Prepare render primitive, by updating world vertex data
			//This function is typically called each frame
			[[nodiscard]] bool Prepare();
	};

} //ion::graphics::render

#endif