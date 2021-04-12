/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonMesh.cpp
-------------------------------------------
*/

#include "IonMesh.h"

#include <algorithm>
#include <type_traits>
#include <utility>

#include "graphics/IonGraphicsAPI.h"
#include "graphics/materials/IonMaterial.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"

namespace ion::graphics::scene::shapes
{

using namespace mesh;

namespace mesh
{

Vertex::Vertex(const Vector3 &position) noexcept :
	Position{position}
{
	//Empty
}

Vertex::Vertex(const Vector3 &position, const Vector3 &normal, const Color &base_color) noexcept :
	Position{position},
	Normal{normal},
	BaseColor{base_color}
{
	//Empty
}

Vertex::Vertex(const Vector3 &position, const Vector3 &normal, const Vector2 &tex_coord, const Color &base_color) noexcept :
	Position{position},
	Normal{normal},
	TexCoord{tex_coord},
	BaseColor{base_color}
{
	//Empty
}

namespace detail
{

VertexContainer vertices_to_vertex_data(const Vertices &vertices)
{
	VertexContainer vertex_data;
	vertex_data.reserve(std::ssize(vertices) * vertex_components);

	for (auto &vertex : vertices)
	{
		vertex_data.insert(std::end(vertex_data), vertex.Position.Components(), vertex.Position.Components() + position_components);
		vertex_data.insert(std::end(vertex_data), vertex.Normal.Components(), vertex.Normal.Components() + normal_components);
		vertex_data.insert(std::end(vertex_data), vertex.BaseColor.Channels(), vertex.BaseColor.Channels() + color_components);
		vertex_data.insert(std::end(vertex_data), vertex.TexCoord.Components(), vertex.TexCoord.Components() + tex_coord_components);
	}

	return vertex_data;
}


std::tuple<Aabb, Obb, Sphere> generate_bounding_volumes(const VertexContainer &vertex_data)
{
	auto min = std::ssize(vertex_data) > 1 ?
		Vector2{vertex_data[detail::position_offset], vertex_data[detail::position_offset + 1]} :
		vector2::Zero;
	auto max = min;

	//Find min/max for each vertex position (x,y)
	for (auto i = detail::position_offset + vertex_components; i < std::ssize(vertex_data);
		i += detail::vertex_components)
	{
		auto position = Vector2{vertex_data[i], vertex_data[i + 1]};

		min = std::min(min, position);
		max = std::max(max, position);
	}

	Aabb aabb{min, max};
	return {aabb, aabb, {aabb.ToHalfSize().Length(), aabb.Center()}};
}

void generate_tex_coords(VertexContainer &vertex_data, const Aabb &aabb) noexcept
{
	//Generate each vertex tex coords (s,t) from position (x,y) in range [0, 1]
	for (auto i = detail::position_offset, j = detail::tex_coord_offset; i < std::ssize(vertex_data);
		i += detail::vertex_components, j += detail::vertex_components)
	{
		auto tex_coord =
			materials::material::detail::get_normalized_tex_coord(
				{vertex_data[i], vertex_data[i + 1]},
				aabb.Min(), aabb.Max(), vector2::Zero, vector2::UnitScale);

		auto [s, t] = tex_coord.XY();
		vertex_data[j] = s;
		vertex_data[j + 1] = t;
	}
}

void normalize_tex_coords(VertexContainer &vertex_data, const materials::Material *material) noexcept
{
	//Clamp each vertex tex coords (s,t) to range [0, 1]
	for (auto i = detail::tex_coord_offset; i < std::ssize(vertex_data);
		i += detail::vertex_components)
	{
		vertex_data[i] = std::clamp(vertex_data[i], 0.0_r, 1.0_r);
		vertex_data[i + 1] = std::clamp(vertex_data[i + 1], 0.0_r, 1.0_r);
	}

	auto [world_lower_left_tex_coord, world_upper_right_tex_coord] = material ?
		material->WorldTexCoords() :
		std::pair{vector2::Zero, vector2::UnitScale};
	auto [world_lower_left, world_upper_right] =
		materials::material::detail::get_unflipped_tex_coords(world_lower_left_tex_coord, world_upper_right_tex_coord);

	auto [mid_s, mid_t] = world_lower_left.Midpoint(world_upper_right).XY();
	auto flip_s = materials::material::detail::is_flipped_horizontally(world_lower_left_tex_coord, world_upper_right_tex_coord);
	auto flip_t = materials::material::detail::is_flipped_vertically(world_lower_left_tex_coord, world_upper_right_tex_coord);

	//Normalize each vertex tex coords (s,t)
	for (auto i = detail::tex_coord_offset; i < std::ssize(vertex_data);
		i += detail::vertex_components)
	{
		auto norm_tex_coord =
			materials::material::detail::get_normalized_tex_coord(
				{vertex_data[i], vertex_data[i + 1]},
				vector2::Zero, vector2::UnitScale,
				world_lower_left,  world_upper_right);

		auto [s, t] = norm_tex_coord.XY();

		//Make sure mesh texture is flipped the same way as material texture
		if (flip_s)
			s = 2.0_r * mid_s - s; //Reflect s across middle point
		if (flip_t)
			t = 2.0_r * mid_t - t; //Reflect t across middle point

		vertex_data[i] = s;
		vertex_data[i + 1] = t;
	}
}


/*
	Graphics API
*/

void enable_wire_frames() noexcept
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void disable_wire_frames() noexcept
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

} //detail
} //mesh


Mesh::Mesh(const Vertices &vertices, bool visible) :
	Mesh{vertex::vertex_batch::VertexDrawMode::Triangles, vertices, visible}
{
	//Empty
}

Mesh::Mesh(const Vertices &vertices, NonOwningPtr<materials::Material> material,
	MeshTexCoordMode tex_coord_mode, bool visible) :

	Mesh{vertex::vertex_batch::VertexDrawMode::Triangles, vertices, material, tex_coord_mode, visible}
{
	//Empty
}

Mesh::Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, const Vertices &vertices, bool visible) :

	vertex_data_{detail::vertices_to_vertex_data(vertices)},
	visible_{visible},

	vertex_batch_{draw_mode, detail::get_vertex_declaration(), vertex_data_},
	update_bounding_volumes_{true},
	update_tex_coords_{true}
{
	//Empty
}

Mesh::Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, const Vertices &vertices, NonOwningPtr<materials::Material> material,
	MeshTexCoordMode tex_coord_mode, bool visible) :
	
	vertex_data_{detail::vertices_to_vertex_data(vertices)},
	tex_coord_mode_{tex_coord_mode},
	visible_{visible},

	vertex_batch_{draw_mode, detail::get_vertex_declaration(), vertex_data_, material},
	update_bounding_volumes_{true},
	update_tex_coords_{true}
{
	//Empty
}


Mesh::Mesh(VertexContainer vertex_data, bool visible) :
	Mesh{vertex::vertex_batch::VertexDrawMode::Triangles, std::move(vertex_data), visible}
{
	//Empty
}

Mesh::Mesh(VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	MeshTexCoordMode tex_coord_mode, bool visible) :

	Mesh{vertex::vertex_batch::VertexDrawMode::Triangles, std::move(vertex_data), material, tex_coord_mode, visible}
{
	//Empty
}

Mesh::Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, VertexContainer vertex_data, bool visible) :

	vertex_data_{std::move(vertex_data)},
	visible_{visible},

	vertex_batch_{draw_mode, detail::get_vertex_declaration(), vertex_data_},
	update_bounding_volumes_{true},
	update_tex_coords_{true}
{
	//Empty
}

Mesh::Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	MeshTexCoordMode tex_coord_mode, bool visible) :

	vertex_data_{std::move(vertex_data)},
	tex_coord_mode_{tex_coord_mode},
	visible_{visible},

	vertex_batch_{draw_mode, detail::get_vertex_declaration(), vertex_data_, material},
	update_bounding_volumes_{true},
	update_tex_coords_{true}
{
	//Empty
}


/*
	Modifiers
*/

void Mesh::SurfaceColor(const Color &color) noexcept
{
	for (auto i = detail::color_offset; i < std::ssize(vertex_data_); i += detail::vertex_components)
		std::copy(color.Channels(), color.Channels() + detail::color_components, std::begin(vertex_data_) + i);

	vertex_batch_.ReloadData();
}


/*
	Observers
*/

Color Mesh::SurfaceColor() const noexcept
{
	if (auto i = detail::color_offset; i < std::ssize(vertex_data_))
		return Color{vertex_data_[i], vertex_data_[i + 1], vertex_data_[i + 2], vertex_data_[i + 3]};
	else
		return color::Transparent;
}


/*
	Preparing / drawing
*/

MeshBoundingVolumeStatus Mesh::Prepare() noexcept
{
	//Make sure, if vertex data view has been initialized, that it is viewing the correct vertex data
	//Could happen if vertex data has been reallocated post init
	if (vertex_batch_.VertexData() && vertex_batch_.VertexData() != vertex_data_)
		vertex_batch_.VertexData(vertex_data_);

	auto bounding_volume_status = MeshBoundingVolumeStatus::Unchanged;

	if (vertex_batch_.VertexCount() > 0)
	{
		if (update_bounding_volumes_)
		{
			auto [aabb, obb, sphere] = detail::generate_bounding_volumes(vertex_data_);
			aabb_ = aabb;
			obb_ = obb;
			sphere_ = sphere;

			update_bounding_volumes_ = false;
			bounding_volume_status = MeshBoundingVolumeStatus::Changed;
		}

		if (update_tex_coords_)
		{
			//Auto generate tex coords
			if (tex_coord_mode_ == mesh::MeshTexCoordMode::Auto)
				detail::generate_tex_coords(vertex_data_, aabb_);

			//Normalize tex coords
			if (tex_coord_mode_ == mesh::MeshTexCoordMode::Manual || vertex_batch_.BatchMaterial())
				detail::normalize_tex_coords(vertex_data_, vertex_batch_.BatchMaterial().get());

			update_tex_coords_ = false;
		}
	}

	vertex_batch_.Prepare();
	return bounding_volume_status;
}

void Mesh::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (visible_)
	{
		if (show_wireframe_)
			detail::enable_wire_frames();

		vertex_batch_.Draw(shader_program);

		if (show_wireframe_)
			detail::disable_wire_frames();
	}
}


/*
	Elapse time
*/

void Mesh::Elapse(duration time) noexcept
{
	vertex_batch_.Elapse(time);
}

} //ion::graphics::scene::shapes