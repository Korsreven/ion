/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderPrimitive.cpp
-------------------------------------------
*/

#include "IonRenderPrimitive.h"

#include <cstring>
#include <utility>

#include "IonRenderer.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/utilities/IonMatrix3.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::graphics::render
{

using namespace render_primitive;

namespace render_primitive::detail
{

int get_vertex_count(const vertex::VertexDeclaration &vertex_declaration, const vertex::VertexDataView &data_view) noexcept
{
	auto vertex_size = vertex_declaration.VertexSize();
	return vertex_size > 0 ? data_view.Size() / vertex_size : 0;
}

vertex_metrics get_vertex_metrics(const vertex::VertexDeclaration &vertex_declaration) noexcept
{
	auto metrics = vertex_metrics{};

	for (auto &element : vertex_declaration.Elements())
	{
		if (element.Name == shaders::shader_layout::AttributeName::Vertex_Position)
		{
			metrics.position_components = element.Components();
			metrics.position_offset = element.Offset / static_cast<int>(sizeof(real));
			metrics.position_stride = element.Stride / static_cast<int>(sizeof(real));
		}
		else if (element.Name == shaders::shader_layout::AttributeName::Vertex_Color)
		{
			metrics.color_components = element.Components();
			metrics.color_offset = element.Offset / static_cast<int>(sizeof(real));
			metrics.color_stride = element.Stride / static_cast<int>(sizeof(real));
		}
	}

	return metrics;
}


void transform_positions(const vertex_metrics &metrics, const Matrix4 &model_matrix, VertexContainer &data) noexcept
{
	auto size = std::ssize(data);
	auto stride = std::max(metrics.position_components, metrics.position_stride);

	switch (metrics.position_components)
	{
		//Two-components (x, y)
		case 2:
		{
			for (auto off = metrics.position_offset; off + 1 < size; off += stride)
			{
				auto [x, y] = (Matrix3::Transformation(model_matrix) * Vector2{data[off + 0], data[off + 1]}).XY();
				data[off + 0] = x;
				data[off + 1] = y;
			}

			break;
		}

		//Three-components (x, y, z)
		case 3:
		case 4: //w is left as is
		{
			for (auto off = metrics.position_offset; off + 2 < size; off += stride)
			{
				auto [x, y, z] = (model_matrix * Vector3{data[off + 0], data[off + 1], data[off + 2]}).XYZ();
				data[off + 0] = x;
				data[off + 1] = y;
				data[off + 2] = z;
			}

			break;
		}
	}
}

void apply_color(const vertex_metrics &metrics, const Color &color, VertexContainer &data) noexcept
{
	auto size = std::ssize(data);
	auto stride = std::max(metrics.color_components, metrics.color_stride);

	switch (metrics.color_components)
	{
		//Three-channels (r, g, b)
		case 3:
		{
			for (auto off = metrics.color_offset; off + 2 < size; off += stride)
			{
				auto [r, g, b] = color.RGB();
				data[off + 0] = r;
				data[off + 1] = g;
				data[off + 2] = b;
			}

			break;
		}

		//Four-channels (r, g, b, a)
		case 4:
		{
			for (auto off = metrics.color_offset; off + 3 < size; off += stride)
			{
				auto [r, g, b, a] = color.RGBA();
				data[off + 0] = r;
				data[off + 1] = g;
				data[off + 2] = b;
				data[off + 3] = a;
			}

			break;
		}
	}
}

void apply_opacity(const vertex_metrics &metrics, real opacity, VertexContainer &data) noexcept
{
	auto size = std::ssize(data);
	auto stride = std::max(metrics.color_components, metrics.color_stride);

	switch (metrics.color_components)
	{
		//Four-channels (r, g, b, a)
		case 4: //Need alpha channel
		{
			for (auto off = metrics.color_offset; off + 3 < size; off += stride)
				data[off + 3] = opacity;

			break;
		}
	}
}

void apply_opacity(const vertex_metrics &metrics, real opacity, const VertexContainer &source_data, VertexContainer &data) noexcept
{
	auto size = std::ssize(data);
	auto stride = std::max(metrics.color_components, metrics.color_stride);

	switch (metrics.color_components)
	{
		//Four-channels (r, g, b, a)
		case 4: //Need alpha channel
		{
			for (auto off = metrics.color_offset; off + 3 < size; off += stride)
				data[off + 3] = source_data[off + 3] * opacity;

			break;
		}
	}
}


real get_position_z(const vertex_metrics &metrics, const VertexContainer &data) noexcept
{
	switch (metrics.position_components)
	{
		//Three-components (x, y, z)
		case 3:
		case 4:
		{
			if (metrics.position_offset + 2 < std::ssize(data))
				return data[metrics.position_offset + 2];
		}
	}

	return 0.0_r;
}

Color get_color(const vertex_metrics &metrics, const VertexContainer &data) noexcept
{
	switch (metrics.color_components)
	{
		//Three-channels (r, g, b)
		case 3:
		{
			if (metrics.color_offset + 2 < std::ssize(data))
				return Color{
					data[metrics.color_offset + 0],
					data[metrics.color_offset + 1],
					data[metrics.color_offset + 2]
				};

			break;
		}

		//Four-channels (r, g, b, a)
		case 4:
		{
			if (metrics.color_offset + 3 < std::ssize(data))
				return Color{
					data[metrics.color_offset + 0],
					data[metrics.color_offset + 1],
					data[metrics.color_offset + 2],
					data[metrics.color_offset + 3]
				};

			break;
		}
	}

	return color::Transparent;
}

real get_opacity(const vertex_metrics &metrics, const VertexContainer &data) noexcept
{
	switch (metrics.color_components)
	{
		//Four-channels (r, g, b, a)
		case 4: //Need alpha channel
		{
			if (metrics.color_offset + 3 < std::ssize(data))
				return data[metrics.color_offset + 3];

			break;
		}
	}

	return 0.0_r;
}

Aabb get_aabb(const vertex_metrics &metrics, const VertexContainer &data) noexcept
{
	auto size = std::ssize(data);
	auto stride = std::max(metrics.position_components, metrics.position_stride);

	auto min = vector2::Zero;
	auto max = vector2::Zero;

	switch (metrics.position_components)
	{
		case 2: //Two-components (x, y)
		case 3: //z is left as is
		case 4: //w is left as is
		{
			//Initial min/max
			if (metrics.position_offset + 1 < size)
			{
				min = max =
					Vector2{data[metrics.position_offset], data[metrics.position_offset + 1]};

				//Find min/max for each vertex position
				for (auto off = metrics.position_offset + stride; off + 1 < size; off += stride)
				{
					auto x = data[off];
					auto y = data[off + 1];
					auto [min_x, min_y] = min.XY();
					auto [max_x, max_y] = max.XY();

					min.X(std::min(min_x, x));
					min.Y(std::min(min_y, y));
					max.X(std::max(max_x, x));
					max.Y(std::max(max_y, y));
				}
			}
		}
	}

	return {min, max};
}


bool all_passes_equal(const Passes &passes, const Passes &passes2) noexcept
{
	if (std::size(passes) != std::size(passes2))
		return false;

	for (auto iter = std::begin(passes), iter2 = std::begin(passes2),
		end = std::end(passes); iter != end; ++iter, ++iter2)
	{
		if (*iter != *iter2)
			return false;
	}

	return true;
}

} //render_primitive::detail


//Private

void RenderPrimitive::UpdateWorldVertexData()
{
	//Vertex data or model matrix has changed
	if (data_changed_ || model_matrix_changed_)
	{
		world_vertex_data_ = vertex_data_;
		detail::transform_positions(vertex_metrics_, model_matrix_, world_vertex_data_);

		data_changed_ = false;
		world_data_changed_ = true;
		model_matrix_changed_ = false;
	}

	//Opacity has changed
	if (opacity_changed_)
	{
		detail::apply_opacity(vertex_metrics_, opacity_, vertex_data_, world_vertex_data_);
		world_data_changed_ = true;
		opacity_changed_ = false;
	}
}

void RenderPrimitive::UpdateWorldZ() noexcept
{
	//Vertex data or model matrix has changed
	if (data_changed_ || model_matrix_changed_)
	{
		auto z = detail::get_position_z(vertex_metrics_, vertex_data_);

		//Check if position z has changed for first vertex
		if (z = (model_matrix_ * Vector3{0.0_r, 0.0_r, z}).Z(); world_z_ != z)
		{
			world_z_ = z;
			need_refresh_ |= world_visible_;
		}
	}
}


//Protected

/*
	Events
*/

void RenderPrimitive::VertexDataChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::ModelMatrixChanged() noexcept
{
	//Optional to override
}


void RenderPrimitive::PassesChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::MaterialChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::TextureChanged() noexcept
{
	//Optional to override
}


void RenderPrimitive::BaseColorChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::BaseOpacityChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::OpacityChanged() noexcept
{
	//Optional to override
}


void RenderPrimitive::PointSizeChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::LineThicknessChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::WireFrameChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::PointSpriteChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::VisibleChanged() noexcept
{
	//Optional to override
}

void RenderPrimitive::RendererChanged() noexcept
{
	//Optional to override
}


//Public

RenderPrimitive::RenderPrimitive(vertex::vertex_batch::VertexDrawMode draw_mode, vertex::VertexDeclaration vertex_declaration,
	bool visible) noexcept :

	draw_mode_{draw_mode},
	vertex_declaration_{std::move(vertex_declaration)},
	vertex_metrics_{detail::get_vertex_metrics(vertex_declaration_)},
	visible_{visible}
{
	//Empty
}

RenderPrimitive::~RenderPrimitive() noexcept
{
	if (parent_renderer_)
		parent_renderer_->RemovePrimitive(*this);
}

/*
	Modifiers
*/

void RenderPrimitive::VertexData(VertexContainer data) noexcept
{
	if (!std::empty(vertex_data_) || !std::empty(data))
	{
		if (std::size(vertex_data_) != std::size(data))
			need_refresh_ |= world_visible_;

		vertex_data_ = std::move(data);
		aabb_ = detail::get_aabb(vertex_metrics_, vertex_data_);

		data_changed_ = true;
		world_data_changed_ = false; //Discard world changes
		VertexDataChanged();
	}
}

void RenderPrimitive::VertexData(VertexContainer data, const Matrix4 &model_matrix) noexcept
{
	VertexData(std::move(data));
	ModelMatrix(model_matrix);
}

void RenderPrimitive::ModelMatrix(const Matrix4 &model_matrix) noexcept
{
	if (std::memcmp(model_matrix_.M(), model_matrix.M(), 16 * sizeof(real)) != 0)
	{
		model_matrix_ = model_matrix;
		model_matrix_changed_ = true;
		ModelMatrixChanged();
	}
}


void RenderPrimitive::BaseColor(const Color &color) noexcept
{
	if (VertexCount() > 0 &&
		detail::get_color(vertex_metrics_, vertex_data_) != color)
	{
		detail::apply_color(vertex_metrics_, color, vertex_data_);

		//No other data changes, apply directly to world
		if (!data_changed_)
		{
			detail::apply_color(vertex_metrics_, color, world_vertex_data_);

			//No other opacity changes, apply opacity again
			if (!opacity_changed_ && opacity_ != 1.0_r)
				opacity_changed_ = true;

			world_data_changed_ = true;
			BaseColorChanged();
		}
	}
}

void RenderPrimitive::BaseOpacity(real opacity) noexcept
{
	if (VertexCount() > 0 &&
		detail::get_opacity(vertex_metrics_, vertex_data_) != opacity)
	{
		detail::apply_opacity(vertex_metrics_, opacity, vertex_data_);

		//No other data changes, apply directly to world
		if (!data_changed_)
		{
			detail::apply_opacity(vertex_metrics_, opacity, world_vertex_data_);

			//No other opacity changes, apply opacity again
			if (!opacity_changed_ && opacity_ != 1.0_r)
				opacity_changed_ = true;

			world_data_changed_ = true;
			BaseOpacityChanged();
		}
	}
}


/*
	Observers
*/

bool RenderPrimitive::IsGroupable(const RenderPrimitive &primitive) const noexcept
{
	return draw_mode_ == primitive.draw_mode_ &&
		   world_z_ == primitive.world_z_ &&
		   material_ == primitive.material_ &&
		   texture_handle_ == primitive.texture_handle_ &&
		   point_size_ == primitive.point_size_ &&
		   line_thickness_ == primitive.line_thickness_ &&
		   wire_frame_ == primitive.wire_frame_ &&
		   point_sprite_ == primitive.point_sprite_ &&

		   //Check slowest equalities last
		   vertex_declaration_ == primitive.vertex_declaration_ &&
		   detail::all_passes_equal(passes_, primitive.passes_);
}


/*
	Vertex batch
*/

vertex::VertexBatch RenderPrimitive::MakeVertexBatch() const noexcept
{
	auto vertex_batch = vertex::VertexBatch{draw_mode_, vertex_declaration_};

	if (material_)
		vertex_batch.BatchMaterial(material_);

	if (texture_handle_)
		vertex_batch.BatchTexture(*texture_handle_);

	return vertex_batch;
}


/*
	Updating
*/

void RenderPrimitive::Refresh()
{
	UpdateWorldZ();

	//Check if material has changed externally
	if (material_.get() != applied_material_)
	{
		applied_material_ = material_.get();
		need_refresh_ = world_visible_;
		MaterialChanged();
	}

	if (need_refresh_)
	{
		if (parent_renderer_)
			parent_renderer_->RefreshPrimitive(*this);

		need_refresh_ = false;
	}
}

bool RenderPrimitive::Prepare()
{
	UpdateWorldVertexData();

	auto world_data_changed = world_data_changed_;
	world_data_changed_ = false;
	return world_data_changed;
}

} //ion::graphics::render