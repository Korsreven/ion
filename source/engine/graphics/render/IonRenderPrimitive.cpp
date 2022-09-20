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

#include <utility>

#include "IonRenderer.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/utilities/IonMatrix3.h"

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


void transform_positions(const vertex_metrics &metrics, const Matrix4 &model_matrix, vertex_data &data) noexcept
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

void apply_color(const vertex_metrics &metrics, const Color &color, vertex_data &data) noexcept
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

void apply_opacity(const vertex_metrics &metrics, real opacity, const vertex_data &source_data, vertex_data &data) noexcept
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


real get_position_z(const vertex_metrics &metrics, const vertex_data &data) noexcept
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

Color get_color(const vertex_metrics &metrics, const vertex_data &data) noexcept
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

	return {};
}


bool all_passes_equal(const render_passes &passes, const render_passes &passes2) noexcept
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


//Protected

void RenderPrimitive::UpdateWorldVertexData()
{
	//Local data or model matrix has changed
	if (local_data_changed_ || model_matrix_changed_)
	{
		world_vertex_data_ = local_vertex_data_;
		detail::transform_positions(vertex_metrics_, model_matrix_, world_vertex_data_);

		local_data_changed_ = false;
		world_data_changed_ = true;
		model_matrix_changed_ = false;
	}

	//Opacity has changed
	if (opacity_changed_)
	{
		detail::apply_opacity(vertex_metrics_, opacity_, local_vertex_data_, world_vertex_data_);
		world_data_changed_ = true;
		opacity_changed_ = false;
	}
}

void RenderPrimitive::UpdateWorldZ() noexcept
{
	//Local data or model matrix has changed
	if (local_data_changed_ || model_matrix_changed_)
	{
		auto z = detail::get_position_z(vertex_metrics_, local_vertex_data_);

		//Check if position z has changed for first vertex
		if (z = (model_matrix_ * Vector3{0.0_r, 0.0_r, z}).Z(); world_z_ != z)
		{
			world_z_ = z;
			need_refresh_ |= visible_;
		}
	}
}


//Public

RenderPrimitive::RenderPrimitive(vertex::vertex_batch::VertexDrawMode draw_mode, vertex::VertexDeclaration vertex_declaration) noexcept :

	draw_mode_{draw_mode},
	vertex_declaration_{std::move(vertex_declaration)},
	vertex_metrics_{detail::get_vertex_metrics(vertex_declaration_)}
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

void RenderPrimitive::LocalVertexData(render_primitive::vertex_data data) noexcept
{
	if (std::size(local_vertex_data_) != std::size(data))
		need_refresh_ |= visible_;

	local_vertex_data_ = std::move(data);

	local_data_changed_ = true;
	world_data_changed_ = false; //Discard world changes
}

void RenderPrimitive::LocalVertexData(render_primitive::vertex_data data, const Matrix4 &model_matrix) noexcept
{
	LocalVertexData(std::move(data));
	ModelMatrix(model_matrix);
}

void RenderPrimitive::WorldVertexData(render_primitive::vertex_data data, const Matrix4 &applied_model_matrix) noexcept
{
	if (std::size(world_vertex_data_) != std::size(data))
		need_refresh_ |= visible_;

	world_vertex_data_ = std::move(data);
	local_vertex_data_ = world_vertex_data_;
	model_matrix_ = applied_model_matrix;
	world_z_ = render_primitive::detail::get_position_z(vertex_metrics_, world_vertex_data_);

	local_data_changed_ = false; //Discard local changes
	world_data_changed_ = true;
	model_matrix_changed_ = false; //Discard matrix changes
}

void RenderPrimitive::ModelMatrix(const Matrix4 &model_matrix) noexcept
{
	if (std::memcmp(model_matrix_.M(), model_matrix.M(), 16 * sizeof(real)) != 0)
	{
		model_matrix_ = model_matrix;
		model_matrix_changed_ = true;
	}
}


void RenderPrimitive::BaseColor(const Color &color) noexcept
{
	if (detail::get_color(vertex_metrics_, local_vertex_data_) != color)
	{
		detail::apply_color(vertex_metrics_, color, local_vertex_data_);

		//No other local changes, apply directly to world
		if (!local_data_changed_)
		{
			detail::apply_color(vertex_metrics_, color, world_vertex_data_);

			//No other opacity changes, apply opacity again
			if (!opacity_changed_ && opacity_ != 1.0_r)
				opacity_changed_ = true;

			world_data_changed_ = true;
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
		   current_material_ == primitive.current_material_ &&
		   texture_handle_ == primitive.texture_handle_ &&
		   point_size_ == primitive.point_size_ &&
		   line_thickness_ == primitive.line_thickness_ &&
		   wire_frame_ == primitive.wire_frame_ &&

		   //Check slowest equalities last
		   vertex_declaration_ == primitive.vertex_declaration_ &&
		   render_primitive::detail::all_passes_equal(passes_, primitive.passes_);
}


/*
	Vertex batch
*/

vertex::VertexBatch RenderPrimitive::MakeVertexBatch() const noexcept
{
	auto vertex_batch = vertex::VertexBatch{draw_mode_, vertex_declaration_};

	if (current_material_)
		vertex_batch.BatchMaterial(current_material_);

	if (texture_handle_)
		vertex_batch.BatchTexture(*texture_handle_);

	return vertex_batch;
}
		
		
/*
	Preparing
*/

void RenderPrimitive::Refresh()
{
	UpdateWorldZ();

	//Check if current material has changed externally
	if (current_material_.get() != applied_material_)
	{
		applied_material_ = current_material_.get();
		need_refresh_ = visible_;
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

	auto data_changed = world_data_changed_;
	world_data_changed_ = false;
	return data_changed;
}

} //ion::graphics::render