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
#include <utility>

#include "graphics/materials/IonMaterial.h"
#include "graphics/scene/IonModel.h"

namespace ion::graphics::scene::shapes
{

using namespace mesh;

namespace mesh
{

Vertex::Vertex(const Vector3 &position, const Color &base_color) noexcept :

	Position{position},
	BaseColor{base_color}
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

render_primitive::VertexContainer vertices_to_vertex_data(const Vertices &vertices)
{
	render_primitive::VertexContainer vertex_data;
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


void generate_tex_coords(render_primitive::VertexContainer &vertex_data, const Aabb &aabb) noexcept
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

void normalize_tex_coords(render_primitive::VertexContainer &vertex_data, const materials::Material *material) noexcept
{
	auto [s_repeatable, t_repeatable] = material ?
		material->IsRepeatable() : std::pair{false, false};

	//Clamp each vertex tex coords (s,t) to range [0, 1]
	for (auto i = detail::tex_coord_offset; i < std::ssize(vertex_data);
		i += detail::vertex_components)
	{
		if (!s_repeatable)
			vertex_data[i] = std::clamp(vertex_data[i], 0.0_r, 1.0_r);

		if (!t_repeatable)
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

} //detail
} //mesh


//Protected

/*
	Events
*/

void Mesh::VertexDataChanged() noexcept
{
	if (auto owner = Owner(); owner)
		owner->NotifyVertexDataChanged(*this);
}

void Mesh::MaterialChanged() noexcept
{
	update_tex_coords_ = true;

	if (auto owner = Owner(); owner)
		owner->NotifyMaterialChanged(*this);
}


//Public

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
	RenderPrimitive{draw_mode, detail::get_vertex_declaration(), visible}
{
	VertexData(detail::vertices_to_vertex_data(vertices));
}

Mesh::Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, const Vertices &vertices, NonOwningPtr<materials::Material> material,
	MeshTexCoordMode tex_coord_mode, bool visible) :
	
	RenderPrimitive{draw_mode, detail::get_vertex_declaration(), visible},
	tex_coord_mode_{tex_coord_mode}
{
	VertexData(detail::vertices_to_vertex_data(vertices));
	RenderMaterial(material);
}


Mesh::Mesh(render_primitive::VertexContainer vertex_data, bool visible) :
	Mesh{vertex::vertex_batch::VertexDrawMode::Triangles, std::move(vertex_data), visible}
{
	//Empty
}

Mesh::Mesh(render_primitive::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	MeshTexCoordMode tex_coord_mode, bool visible) :

	Mesh{vertex::vertex_batch::VertexDrawMode::Triangles, std::move(vertex_data), material, tex_coord_mode, visible}
{
	//Empty
}

Mesh::Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, render_primitive::VertexContainer vertex_data, bool visible) :
	RenderPrimitive{draw_mode, detail::get_vertex_declaration(), visible}
{
	VertexData(std::move(vertex_data));
}

Mesh::Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, render_primitive::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	MeshTexCoordMode tex_coord_mode, bool visible) :

	RenderPrimitive{draw_mode, detail::get_vertex_declaration(), visible},
	tex_coord_mode_{tex_coord_mode}
{
	VertexData(std::move(vertex_data));
	RenderMaterial(material);
}


/*
	Preparing
*/

void Mesh::Prepare()
{
	if (update_tex_coords_)
	{
		//Auto generate tex coords
		if (tex_coord_mode_ == mesh::MeshTexCoordMode::Auto)
			detail::generate_tex_coords(VertexData(), AxisAlignedBoundingBox());

		//Normalize tex coords
		if (tex_coord_mode_ == mesh::MeshTexCoordMode::Manual || RenderMaterial())
			detail::normalize_tex_coords(VertexData(), RenderMaterial().get());

		update_tex_coords_ = false;
	}

	RenderPrimitive::Prepare();
}


/*
	Elapse time
*/

void Mesh::Elapse([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}

} //ion::graphics::scene::shapes