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

//Protected

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
	Events
*/

void Shape::BaseColorChanged() noexcept
{
	if (auto color = BaseColor(); color_ != color)
	{
		color_ = color;
		update_colors_ = false;
		update_opacity_ = false;
	}
}

void Shape::BaseOpacityChanged() noexcept
{
	if (auto opacity = BaseOpacity(); color_.A() != opacity)
	{
		color_.A(opacity);
		update_opacity_ = false;
	}
}


//Public

/*
	Preparing
*/

void Shape::Prepare()
{
	if (update_vertices_)
	{
		VertexData(GetVertices());
		update_vertices_ = false;
		update_colors_ = false;
		update_opacity_ = false;
	}
	else if (update_colors_)
	{
		BaseColor(color_);
		update_colors_ = false;
		update_opacity_ = false;
	}
	else if (update_opacity_)
	{
		BaseOpacity(color_.A());
		update_opacity_ = false;
	}


	return Mesh::Prepare();
}

} //ion::graphics::scene::shapes