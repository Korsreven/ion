/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonShape.cpp
-------------------------------------------
*/

#include "IonShape.h"

namespace ion::graphics::scene::shapes
{

using namespace shape;

namespace shape::detail
{
} //shape::detail


Shape::Shape(const mesh::Vertices &vertices, const Color &color, bool visible) :

	Mesh{vertices, visible},
	color_{color}
{
	//Empty
}

Shape::Shape(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices,
	const Color &color, bool visible ) :

	Mesh{draw_mode, vertices, visible},
	color_{color}
{
	//Empty
}


Shape::Shape(const mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
	const Color &color, bool visible) :

	Mesh{vertices, material, mesh::MeshTexCoordMode::Auto, visible},
	color_{color}
{
	//Empty
}

Shape::Shape(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices,
	NonOwningPtr<materials::Material> material, const Color &color, bool visible) :

	Mesh{draw_mode, vertices, material, mesh::MeshTexCoordMode::Auto, visible},
	color_{color}
{
	//Empty
}


/*
	Preparing
*/

mesh::MeshBoundingVolumeStatus Shape::Prepare() noexcept
{
	if (update_vertices_)
	{
		Mesh::VertexData(GetVertices());
		update_vertices_ = false;
	}

	return Mesh::Prepare();
}

} //ion::graphics::scene::shapes