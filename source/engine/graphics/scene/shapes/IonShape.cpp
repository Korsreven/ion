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

#include "graphics/scene/IonModel.h"

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

void Shape::VertexColorChanged() noexcept
{
	FillColor(VertexColor());
}

void Shape::VertexOpacityChanged() noexcept
{
	FillOpacity(VertexOpacity());
}


//Public

/*
	Preparing
*/

mesh::MeshBoundingVolumeStatus Shape::Prepare() noexcept
{
	if (update_vertices_)
	{
		auto vertices = GetVertices();

		if (auto model = Owner(); model)
		{
			//Adjust alpha with model opacity
			if (auto opacity = model->Opacity(); opacity != 1.0_r)
			{
				for (auto &vertex : vertices)
					vertex.BaseColor.A(vertex.BaseColor.A() * opacity);
			}
		}

		Mesh::VertexData(std::move(vertices));
		update_vertices_ = false;
	}

	return Mesh::Prepare();
}

} //ion::graphics::scene::shapes