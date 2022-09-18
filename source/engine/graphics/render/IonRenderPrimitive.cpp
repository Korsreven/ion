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

int get_vertex_count(const vertex::VertexDeclaration &vertex_declaration, const vertex::VertexDataView &vertex_data) noexcept
{
	auto vertex_size = vertex_declaration.VertexSize();
	return vertex_size > 0 ? vertex_data.Size() / vertex_size : 0;
}

vertex_metrics get_vertex_metrics(const vertex::VertexDeclaration &vertex_declaration) noexcept
{
	for (auto &element : vertex_declaration.Elements())
	{
		if (element.Name == shaders::shader_layout::AttributeName::Vertex_Position)
			return {element.Components(),
					element.Offset / static_cast<int>(sizeof(real)),
					element.Stride / static_cast<int>(sizeof(real))};
	}

	return {};
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
			for (auto off = metrics.position_offset; off + 2 <= size; off += stride)
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
			for (auto off = metrics.position_offset; off + 3 <= size; off += stride)
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

real get_position_z(const vertex_metrics &metrics, const vertex_data &data) noexcept
{
	switch (metrics.position_components)
	{
		//Three-components (x, y, z)
		case 3:
		case 4:
		return metrics.position_offset + 2 < std::ssize(data) ?
			data[metrics.position_offset + 2] : 0.0_r;
	}

	return 0.0_r;
}


bool are_passes_equal(const render_passes &passes, const render_passes &passes2) noexcept
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
	if (local_data_changed_ ||
		std::memcmp(current_model_matrix_.M(), applied_model_matrix_.M(), 16 * sizeof(real)) != 0)
	{
		world_vertex_data_ = local_vertex_data_;
		detail::transform_positions(vertex_metrics_, current_model_matrix_, world_vertex_data_);
		applied_model_matrix_ = current_model_matrix_;

		local_data_changed_ = false;
		world_data_changed_ = true;
	}
}

void RenderPrimitive::UpdateWorldZ() noexcept
{
	//Local data or model matrix has changed
	if (local_data_changed_ ||
		std::memcmp(current_model_matrix_.M(), applied_model_matrix_.M(), 16 * sizeof(real)) != 0)
	{
		auto z = detail::get_position_z(vertex_metrics_, local_vertex_data_);

		//Check if position z has changed for first vertex
		if (z = (current_model_matrix_ * Vector3{0.0_r, 0.0_r, z}).Z(); z != world_z_)
		{
			world_z_ = z;
			need_refresh_ = true;
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