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

Shape::Shape(const mesh::Vertices &vertices, bool visible) :
	Mesh{vertices, visible}
{
	//Empty
}

Shape::Shape(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices, bool visible ) :
	Mesh{draw_mode, vertices, visible}
{
	//Empty
}


Shape::Shape(const mesh::Vertices &vertices, NonOwningPtr<materials::Material> material, bool visible) :
	Mesh{vertices, material, mesh::MeshTexCoordMode::Auto, visible}
{
	//Empty
}

Shape::Shape(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices,
	NonOwningPtr<materials::Material> material, bool visible) :

	Mesh{draw_mode, vertices, material, mesh::MeshTexCoordMode::Auto, visible}
{
	//Empty
}

} //ion::graphics::scene::shapes