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
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"

namespace ion::graphics::scene
{

using namespace model;

namespace model::detail
{
} //model::detail


//Private

void Model::Created(shapes::Mesh &mesh)
{
	reload_vertex_buffer_ |= mesh.VertexCount() > 0;
	update_bounding_volumes_ |= reload_vertex_buffer_;
}


//Public

Model::Model(bool visible) noexcept :
	MovableObject{visible}
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
			auto size = 0;
			for (auto &mesh : Meshes())
				size += std::ssize(mesh.VertexData());

			vbo_->Reserve(size * sizeof(real));

			for (auto offset = 0; auto &mesh : Meshes())
			{
				size = std::ssize(mesh.VertexData());
				mesh.VertexBuffer(vbo_->SubBuffer(offset * sizeof(real), size * sizeof(real)));
				offset += size;
			}
		}

		reload_vertex_buffer_ = false;
	}


	//Prepare all meshes
	for (auto &mesh : Meshes())
		update_bounding_volumes_ |=
			mesh.Prepare() == shapes::mesh::MeshBoundingVolumeStatus::Changed;

	if (update_bounding_volumes_)
	{
		aabb_ = {};

		for (auto &mesh : Meshes())
			aabb_.Merge(mesh.AxisAlignedBoundingBox());

		obb_ = aabb_;
		sphere_ = {aabb_.ToHalfSize().Length(), aabb_.Center()};

		update_bounding_volumes_ = false;
		need_bounding_update_ = true;
	}
}

void Model::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	if (auto meshes = Meshes(); visible_ && !std::empty(meshes))
	{
		auto use_shader = shader_program && shader_program->Owner() && shader_program->Handle();

		if (use_shader)
			shader_program->Owner()->ActivateShaderProgram(*shader_program);

		//Draw all meshes
		for (auto &mesh : meshes)
			mesh.Draw(shader_program);

		if (use_shader)
			shader_program->Owner()->DeactivateShaderProgram(*shader_program);
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

NonOwningPtr<shapes::Mesh> Model::CreateMesh(const shapes::mesh::Vertices &vertices, bool visible)
{
	auto ptr = Create(vertices, visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(const shapes::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
	shapes::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	auto ptr = Create(vertices, material, tex_coord_mode, visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const shapes::mesh::Vertices &vertices, bool visible)
{
	auto ptr = Create(draw_mode, vertices, visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const shapes::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
	shapes::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	auto ptr = Create(draw_mode, vertices, material, tex_coord_mode, visible);
	Created(*ptr);
	return ptr;
}


NonOwningPtr<shapes::Mesh> Model::CreateMesh(shapes::mesh::VertexContainer vertex_data, bool visible)
{
	auto ptr = Create(std::move(vertex_data), visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(shapes::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	shapes::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	auto ptr = Create(std::move(vertex_data), material, tex_coord_mode, visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, shapes::mesh::VertexContainer vertex_data, bool visible)
{
	auto ptr = Create(draw_mode, std::move(vertex_data), visible);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, shapes::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
	shapes::mesh::MeshTexCoordMode tex_coord_mode, bool visible)
{
	auto ptr = Create(draw_mode, std::move(vertex_data), material, tex_coord_mode, visible);
	Created(*ptr);
	return ptr;
}


NonOwningPtr<shapes::Mesh> Model::CreateMesh(const shapes::Mesh &mesh)
{
	auto ptr = Create(mesh);
	Created(*ptr);
	return ptr;
}

NonOwningPtr<shapes::Mesh> Model::CreateMesh(shapes::Mesh &&mesh)
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

bool Model::RemoveMesh(shapes::Mesh &mesh) noexcept
{
	auto removed = Remove(mesh);
	update_bounding_volumes_ |= removed;
	return removed;
}

} //ion::graphics::scene