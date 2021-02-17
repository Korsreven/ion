/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonModel.cpp
-------------------------------------------
*/

#include "IonModel.h"

#include "graphics/IonGraphicsAPI.h"

namespace ion::graphics::scene
{

using namespace model;

namespace model::detail
{
} //model::detail


//Private

void Model::Created(render::Mesh &mesh)
{
	reload_vertex_buffer_ |= mesh.VertexCount() > 0;
	update_bounding_volumes_ |= reload_vertex_buffer_;
}

render::mesh::VertexContainer Model::MeshVertexData() const
{
	auto size = 0;
	for (auto &mesh : Meshes())
		size += std::ssize(mesh.VertexData());

	render::mesh::VertexContainer vertex_data;
	vertex_data.reserve(size);

	//Copy and append vertex data from each mesh
	for (auto &mesh : Meshes())
		vertex_data.insert(std::end(vertex_data), std::begin(mesh.VertexData()), std::end(mesh.VertexData()));

	return vertex_data;
}


//Public

Model::Model(bool visible) noexcept :
	visible_{visible}
{
	//Empty
}


/*
	Preparing / drawing
*/

void Model::Prepare() noexcept
{
	if (reload_vertex_buffer_)
	{
		if (!vbo_)
			vbo_.emplace(render::vertex::vertex_buffer_object::VertexBufferUsage::Static);

		if (vbo_ && *vbo_)
		{
			if (auto vertex_data = MeshVertexData(); !std::empty(vertex_data))
			{
				vbo_->Data(vertex_data);

				for (auto offset = 0; auto &mesh : Meshes())
				{
					auto size = std::ssize(mesh.VertexData());
					mesh.VertexBuffer(vbo_->SubBuffer(offset * sizeof(real), size * sizeof(real)), false);
					offset += size;
				}
			}
		}

		reload_vertex_buffer_ = false;
	}


	//Prepare all meshes
	for (auto &mesh : Meshes())
		mesh.Prepare();


	if (update_bounding_volumes_)
	{
		aabb_ = {};

		for (auto &mesh : Meshes())
			aabb_.Merge(mesh.AxisAlignedBoundingBox());

		obb_ = aabb_;
		sphere_ = {aabb_.ToHalfSize().Length(), aabb_.Center()};

		update_bounding_volumes_ = false;
	}
}

void Model::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (visible_)
	{
		//Draw all meshes
		for (auto &mesh : Meshes())
			mesh.Draw(shader_program);
	}
}


/*
	Elapse time
*/

void Model::Elapse(duration time) noexcept
{
	//Elapse all meshes
	for (auto &mesh : Meshes())
		mesh.Elapse(time);
}


/*
	Meshes
	Creating
*/

NonOwningPtr<render::Mesh> Model::CreateMesh(const render::mesh::Vertices &vertices, bool visible)
{
	auto ptr = Create(vertices, visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<render::Mesh> Model::CreateMesh(const render::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
	render::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	auto ptr = Create(vertices, material, tex_coord_mode, visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<render::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const render::mesh::Vertices &vertices, bool visible)
{
	auto ptr = Create(draw_mode, vertices, visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<render::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const render::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
	render::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	auto ptr = Create(draw_mode, vertices, material, tex_coord_mode, visible);
	Created(*ptr);
	return ptr;
}


NonOwningPtr<render::Mesh> Model::CreateMesh(render::mesh::VertexContainer vertex_data, bool visible)
{
	auto ptr = Create(std::move(vertex_data), visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<render::Mesh> Model::CreateMesh(render::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	render::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	auto ptr = Create(std::move(vertex_data), material, tex_coord_mode, visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<render::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, render::mesh::VertexContainer vertex_data, bool visible)
{
	auto ptr = Create(draw_mode, std::move(vertex_data), visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<render::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, render::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	render::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	auto ptr = Create(draw_mode, std::move(vertex_data), material, tex_coord_mode, visible);
	Created(*ptr);
	return ptr;
}


NonOwningPtr<render::Mesh> Model::CreateMesh(const render::Mesh &mesh)
{
	auto ptr = Create(mesh);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<render::Mesh> Model::CreateMesh(render::Mesh &&mesh)
{
	auto ptr = Create(std::move(mesh));
	Created(*ptr);
	return ptr;
}


/*
	Meshes
	Removing
*/

void Model::ClearMeshes() noexcept
{
	update_bounding_volumes_ |= !std::empty(Meshes());
	Clear();
}

bool Model::RemoveMesh(render::Mesh &mesh) noexcept
{
	auto removed = Remove(mesh);
	update_bounding_volumes_ |= removed;
	return removed;
}

} //ion::graphics::scene